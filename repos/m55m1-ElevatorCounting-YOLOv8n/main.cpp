/**************************************************************************//**
 * @file     main.cpp
 * @version  V1.00
 * @brief    Object detection network sample. Demonstrate object detectioin.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "BoardInit.hpp"      /* Board initialisation */
#include "log_macros.h"      /* Logging macros (optional) */

#include "BufAttributes.hpp" /* Buffer attributes to be applied */
#include "YOLOv8nODModel.hpp"       /* Model API */
#include "YOLOv8nODPostProcessing.hpp"
#include "Labels.hpp"

#include "imlib.h"          /* Image processing */
#include "framebuffer.h"
#include "ModelFileReader.h"
#include "ff.h"

#undef PI /* PI macro conflict with CMSIS/DSP */
#include "NuMicro.h"

//#define __PROFILE__
#define __USE_CCAP__
#define __USE_DISPLAY__
//#define __USE_UVC__

#include "Profiler.hpp"

#if defined (__USE_CCAP__)
#include "ImageSensor.h"
#endif

#if !defined (__USE_CCAP__)
#include "InputFiles.hpp"             /* Baked-in input (not needed for live data) */
#endif

#if defined (__USE_DISPLAY__)
    #include "Display.h"
#endif

#if defined (__USE_UVC__)
    #include "UVC.h"
#endif

#define NUM_FRAMEBUF 2  //1 or 2

#define MODEL_AT_HYPERRAM_ADDR (0x82400000)

#define OD_PRESENCE_THRESHOLD  				(0.4)

typedef enum
{
    eFRAMEBUF_EMPTY,
    eFRAMEBUF_FULL,
    eFRAMEBUF_INF
} E_FRAMEBUF_STATE;

typedef struct
{
    E_FRAMEBUF_STATE eState;
    image_t frameImage;
    std::vector<arm::app::yolov8n_od::DetectionResult> results;
} S_FRAMEBUF;


S_FRAMEBUF s_asFramebuf[NUM_FRAMEBUF];

namespace arm
{
namespace app
{
/* Tensor arena buffer */
static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

} /* namespace app */
} /* namespace arm */

//frame buffer managemnet function
static S_FRAMEBUF *get_empty_framebuf()
{
    int i;

    for (i = 0; i < NUM_FRAMEBUF; i ++)
    {
        if (s_asFramebuf[i].eState == eFRAMEBUF_EMPTY)
            return &s_asFramebuf[i];
    }

    return NULL;
}

static S_FRAMEBUF *get_full_framebuf()
{
    int i;

    for (i = 0; i < NUM_FRAMEBUF; i ++)
    {
        if (s_asFramebuf[i].eState == eFRAMEBUF_FULL)
            return &s_asFramebuf[i];
    }

    return NULL;
}

static S_FRAMEBUF *get_inf_framebuf()
{
    int i;

    for (i = 0; i < NUM_FRAMEBUF; i ++)
    {
        if (s_asFramebuf[i].eState == eFRAMEBUF_INF)
            return &s_asFramebuf[i];
    }

    return NULL;
}

#define IMAGE_DISP_UPSCALE_FACTOR 2
#if defined(LT7381_LCD_PANEL)
#define FONT_DISP_UPSCALE_FACTOR 2
#else
#define FONT_DISP_UPSCALE_FACTOR 1
#endif

/* Image processing initiate function */
//Used by omv library
#if defined(__USE_UVC__)
//UVC only support QVGA, QQVGA
#define GLCD_WIDTH	320
#define GLCD_HEIGHT	240
#elif !defined(__USE_CCAP__)
#define GLCD_WIDTH		IMAGE_WIDTH
#define GLCD_HEIGHT		IMAGE_HEIGHT
#else
#define GLCD_WIDTH		320
#define GLCD_HEIGHT		240
#endif

//RGB565
#define IMAGE_FB_SIZE	(GLCD_WIDTH * GLCD_HEIGHT * 2)

#undef OMV_FB_SIZE
#define OMV_FB_SIZE (IMAGE_FB_SIZE + 1024)

#undef OMV_FB_ALLOC_SIZE
#define OMV_FB_ALLOC_SIZE	(1*1024)

__attribute__((section(".bss.vram.data"), aligned(32))) static char fb_array[OMV_FB_SIZE + OMV_FB_ALLOC_SIZE];
__attribute__((section(".bss.vram.data"), aligned(32))) static char jpeg_array[OMV_JPEG_BUF_SIZE];

#if (NUM_FRAMEBUF == 2)
    __attribute__((section(".bss.vram.data"), aligned(32))) static char frame_buf1[OMV_FB_SIZE];
#endif

char *_fb_base = NULL;
char *_fb_end = NULL;
char *_jpeg_buf = NULL;
char *_fballoc = NULL;

static void omv_init()
{
    image_t frameBuffer;
    int i;

    frameBuffer.w = GLCD_WIDTH;
    frameBuffer.h = GLCD_HEIGHT;
    frameBuffer.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
    frameBuffer.pixfmt = PIXFORMAT_RGB565;

    _fb_base = fb_array;
    _fb_end =  fb_array + OMV_FB_SIZE - 1;
    _fballoc = _fb_base + OMV_FB_SIZE + OMV_FB_ALLOC_SIZE;
    _jpeg_buf = jpeg_array;

    fb_alloc_init0();

    framebuffer_init0();
    framebuffer_init_from_image(&frameBuffer);

    for (i = 0 ; i < NUM_FRAMEBUF; i++)
    {
        s_asFramebuf[i].eState = eFRAMEBUF_EMPTY;
    }

    framebuffer_init_image(&s_asFramebuf[0].frameImage);

#if (NUM_FRAMEBUF == 2)
    s_asFramebuf[1].frameImage.w = GLCD_WIDTH;
    s_asFramebuf[1].frameImage.h = GLCD_HEIGHT;
    s_asFramebuf[1].frameImage.size = GLCD_WIDTH * GLCD_HEIGHT * 2;
    s_asFramebuf[1].frameImage.pixfmt = PIXFORMAT_RGB565;
    s_asFramebuf[1].frameImage.data = (uint8_t *)frame_buf1;
#endif
}

static int DrawDetectBox(
    const std::vector<arm::app::yolov8n_od::DetectionResult> &results,
    image_t *drawImg,
    std::vector<std::string> &labels
)
{
	arm::app::yolov8n_od::DetectionResult detectBox;
	int boxSize = results.size();
    int personCount = 0;

	for(int p = 0; p < boxSize; p ++)
	{
		detectBox = results[p];
        if (detectBox.m_detectBox.cls == 0) {
            personCount++;
		    imlib_draw_rectangle(drawImg, detectBox.m_detectBox.x, detectBox.m_detectBox.y, detectBox.m_detectBox.w, detectBox.m_detectBox.h, COLOR_B5_MAX, 2, false);

            imlib_draw_string(drawImg, detectBox.m_detectBox.x, detectBox.m_detectBox.y - 16, labels[detectBox.m_detectBox.cls].c_str(), COLOR_B5_MAX, 2, 0, 0, false,
                              false, false, false, 0, false, false);
        }
	}

    
    return personCount;
}

static int32_t PrepareModelToHyperRAM(void)
{
#define MODEL_FILE "0:\\YOLOv8n-elevator-od.tflite"
#define EACH_READ_SIZE 512
	
    TCHAR sd_path[] = { '0', ':', 0 };    /* SD drive started from 0 */	
    f_chdrive(sd_path);          /* set default path */

	int32_t i32FileSize;
	int32_t i32FileReadIndex = 0;
	int32_t i32Read;
	
	if(!ModelFileReader_Initialize(MODEL_FILE))
	{
        printf_err("Unable open model %s\n", MODEL_FILE);		
		return -1;
	}
	
	i32FileSize = ModelFileReader_FileSize();
    info("Model file size %i \n", i32FileSize);

	while(i32FileReadIndex < i32FileSize)
	{
		i32Read = ModelFileReader_ReadData((BYTE *)(MODEL_AT_HYPERRAM_ADDR + i32FileReadIndex), EACH_READ_SIZE);
		if(i32Read < 0)
			break;
		i32FileReadIndex += i32Read;
	}
	
	if(i32FileReadIndex < i32FileSize)
	{
        printf_err("Read Model file size is not enough\n");		
		return -2;
	}
	
#if 0
	/* verify */
	i32FileReadIndex = 0;
	ModelFileReader_Rewind();
	BYTE au8TempBuf[EACH_READ_SIZE];
	
	while(i32FileReadIndex < i32FileSize)
	{
		i32Read = ModelFileReader_ReadData((BYTE *)au8TempBuf, EACH_READ_SIZE);
		if(i32Read < 0)
			break;
		
		if(std::memcmp(au8TempBuf, (void *)(MODEL_AT_HYPERRAM_ADDR + i32FileReadIndex), i32Read)!= 0)
		{
			printf_err("verify the model file content is incorrect at %i \n", i32FileReadIndex);		
			return -3;
		}
		i32FileReadIndex += i32Read;
	}
	
#endif	
	ModelFileReader_Finish();
	
	return i32FileSize;
}	

int main()
{
    /* Initialise the UART module to allow printf related functions (if using retarget) */
    BoardInit();
		Display_Init();
		printf("LCD: %lu x %lu\n", Disaplay_GetLCDWidth(), Disaplay_GetLCDHeight());


	/* Copy model file from SD to HyperRAM*/
	int32_t i32ModelSize;
		
	i32ModelSize = PrepareModelToHyperRAM();

	if(i32ModelSize <= 0 )
	{
        printf_err("Failed to prepare model\n");
        return 1;
	}

    /* Model object creation and initialisation. */
    arm::app::YOLOv8nODModel model;

    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    (unsigned char *)MODEL_AT_HYPERRAM_ADDR,
                    i32ModelSize))
    {
        printf_err("Failed to initialise model\n");
        return 1;
    }

    /* Setup cache poicy of tensor arean buffer */
    info("Set tesnor arena cache policy to WTRA \n");
    const std::vector<ARM_MPU_Region_t> mpuConfig =
    {
        {
            // SRAM for tensor arena
            ARM_MPU_RBAR(((unsigned int)arm::app::tensorArena),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)arm::app::tensorArena) + ACTIVATION_BUF_SZ - 1),        // Limit
                         eMPU_ATTR_CACHEABLE_WTRA) // Attribute index - Write-Through, Read-allocate
        },
        {
            // Image data from CCAP DMA, so must set frame buffer to Non-cache attribute
            ARM_MPU_RBAR(((unsigned int)fb_array),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)fb_array) + OMV_FB_SIZE - 1),        // Limit
                         eMPU_ATTR_NON_CACHEABLE) // NonCache
        },
#if (NUM_FRAMEBUF == 2)
        {
            // Image data from CCAP DMA, so must set frame buffer to Non-cache attribute
            ARM_MPU_RBAR(((unsigned int)frame_buf1),        // Base
                         ARM_MPU_SH_NON,    // Non-shareable
                         0,                 // Read-only
                         1,                 // Non-Privileged
                         1),                // eXecute Never enabled
            ARM_MPU_RLAR((((unsigned int)frame_buf1) + OMV_FB_SIZE - 1),        // Limit
                         eMPU_ATTR_NON_CACHEABLE) // NonCache
        },
#endif
    };

    // Setup MPU configuration
    InitPreDefMPURegion(&mpuConfig[0], mpuConfig.size());

#if !defined (__USE_CCAP__)
    uint8_t u8ImgIdx = 0;
    char chStdIn;
#endif

    TfLiteTensor *inputTensor   = model.GetInputTensor(0);

    if (!inputTensor->dims)
    {
        printf_err("Invalid input tensor dims\n");
        return 2;
    }
    else if (inputTensor->dims->size < 3)
    {
        printf_err("Input tensor dimension should be >= 3\n");
        return 3;
    }

    TfLiteIntArray *inputShape = model.GetInputShape(0);

    const int inputImgCols = inputShape->data[arm::app::YOLOv8nODModel::ms_inputColsIdx];
    const int inputImgRows = inputShape->data[arm::app::YOLOv8nODModel::ms_inputRowsIdx];
    const uint32_t nChannels = inputShape->data[arm::app::YOLOv8nODModel::ms_inputChannelsIdx];

    /* Get input quantization params information. */
    arm::app::QuantParams inQuantParams = arm::app::GetTensorQuantParams(inputTensor);
	
    // postProcess
    arm::app::yolov8n_od::YOLOv8nODPostProcessing postProcess(&model, OD_PRESENCE_THRESHOLD);

    //label information
    std::vector<std::string> labels;
    GetLabelsVector(labels);
	
    //display framebuffer
    image_t frameBuffer;
    rectangle_t roi;

    //omv library init
    omv_init();
    framebuffer_init_image(&frameBuffer);

#if defined(__PROFILE__)

    arm::app::Profiler profiler;
    uint64_t u64StartCycle;
    uint64_t u64EndCycle;
    uint64_t u64CCAPStartCycle;
    uint64_t u64CCAPEndCycle;
#else
    pmu_reset_counters();
#endif

#define EACH_PERF_SEC 1
int g_currentPersonCount = 0; // persistent state between frames
    uint64_t u64PerfCycle;
    uint64_t u64PerfFrames = 0;

    u64PerfCycle = pmu_get_systick_Count();
    u64PerfCycle += (SystemCoreClock * EACH_PERF_SEC);

    S_FRAMEBUF *infFramebuf;
    S_FRAMEBUF *fullFramebuf;
    S_FRAMEBUF *emptyFramebuf;

#if defined (__USE_CCAP__)
    //Setup image senosr
    ImageSensor_Init();
    ImageSensor_Config(eIMAGE_FMT_RGB565, frameBuffer.w, frameBuffer.h, true);
#endif

#if defined (__USE_DISPLAY__)
    char szDisplayText[100];
    S_DISP_RECT sDispRect;

    Display_Init();
    Display_ClearLCD(C_WHITE);
#endif

#if defined (__USE_UVC__)
	UVC_Init();
    HSUSBD_Start();
#endif
	
	while(1)
	{
        emptyFramebuf = get_empty_framebuf();

        if (emptyFramebuf)
        {
#if defined (__USE_CCAP__)
            //capture frame from CCAP
#if defined(__PROFILE__)
            u64CCAPStartCycle = pmu_get_systick_Count();
#endif

            ImageSensor_TriggerCapture((uint32_t)(emptyFramebuf->frameImage.data));
#endif
		}

        fullFramebuf = get_full_framebuf();

        if (fullFramebuf)
        {
            //resize full image to input tensor
            // Letterbox logic
            image_t resizeImg;
            roi.x = 0;
            roi.y = 0;
            roi.w = fullFramebuf->frameImage.w; // 320
            roi.h = fullFramebuf->frameImage.h; // 240

            resizeImg.w = 192;
            resizeImg.h = 144; // 240 * 192 / 320 = 144
            
            // Offset the resize buffer by 24 rows
            uint32_t offset = 24 * 192 * 3; 
            resizeImg.data = (uint8_t *)inputTensor->data.data + offset;
            resizeImg.pixfmt = PIXFORMAT_RGB888;

#if defined(__PROFILE__)
            u64StartCycle = pmu_get_systick_Count();
#endif
            imlib_nvt_scale(&fullFramebuf->frameImage, &resizeImg, &roi);

            // Pad top and bottom with zero (Black)
            uint8_t *req_data_pad = static_cast<uint8_t *>(inputTensor->data.data);
            memset(req_data_pad, 0, offset); // Top pad
            memset(req_data_pad + (168 * 192 * 3), 0, offset); // Bottom pad (24+144 = 168 rows)

#if defined(__PROFILE__)
            u64EndCycle = pmu_get_systick_Count();
            info("resize cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif

			//Quantize input tensor data (and convert to monochrome in-place)
			auto *req_data = static_cast<uint8_t *>(inputTensor->data.data);
			auto *signed_req_data = static_cast<int8_t *>(inputTensor->data.data);

			for (size_t i = 0; i < inputTensor->bytes; i += 3)
			{
			    uint8_t r = req_data[i];
			    uint8_t g = req_data[i + 1];
			    uint8_t b = req_data[i + 2];

                // FAST APPROX FOR DIV 3: sum * 85 >> 8
			    uint8_t gray_uint8 = (r + g + b) * 85 >> 8;
			    int8_t gray_int8 = static_cast<int8_t>(gray_uint8) - 128;

			    signed_req_data[i]     = gray_int8;
			    signed_req_data[i + 1] = gray_int8;
			    signed_req_data[i + 2] = gray_int8;
			}

#if defined(__PROFILE__)
            u64EndCycle = pmu_get_systick_Count();
            info("quantize cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif

#if defined(__PROFILE__)
			profiler.StartProfiling("Inference");
#endif

			model.RunInference();

#if defined(__PROFILE__)
			profiler.StopProfiling();
			profiler.PrintProfilingResult();
#endif

            fullFramebuf->eState = eFRAMEBUF_INF;
        }
		
        infFramebuf = get_inf_framebuf();

		if (infFramebuf)
		{
			//post process

		#if defined(__PROFILE__)
					u64StartCycle = pmu_get_systick_Count();
		#endif
		postProcess.RunPostProcessing(
				inputImgCols,
				inputImgRows,
				inputImgRows, // 1:1 scale so output is in 192px canvas coords
				inputImgCols,
				infFramebuf->results);

		// Undo Letterbox: Scale back to 320x240 and remove pad
		for (auto &det : infFramebuf->results) {
				det.m_detectBox.y -= 24; 
				det.m_detectBox.x = (det.m_detectBox.x * 5) / 3; // 320/192
				det.m_detectBox.y = (det.m_detectBox.y * 5) / 3;
				det.m_detectBox.w = (det.m_detectBox.w * 5) / 3;
				det.m_detectBox.h = (det.m_detectBox.h * 5) / 3;
		}

		#if defined(__PROFILE__)
					u64EndCycle = pmu_get_systick_Count();
					info("post processing cycles %llu \n", (u64EndCycle - u64StartCycle));
		#endif

            //draw bbox and render
            /* Draw boxes. */
			if(infFramebuf->results.size())
			{
					#if defined(__PROFILE__)
									u64StartCycle = pmu_get_systick_Count();
					#endif
									g_currentPersonCount = DrawDetectBox(infFramebuf->results, &infFramebuf->frameImage, labels);
					#if defined(__PROFILE__)
									u64EndCycle = pmu_get_systick_Count();
									info("draw box cycles %llu \n", (u64EndCycle - u64StartCycle));
					#endif
			}
      else 
      {
					g_currentPersonCount = 0; // reset for empty frames
      }

            // Process display image: grayscale center crop and black sides
            /*
						uint16_t *pixel_data = (uint16_t *)infFramebuf->frameImage.data;
            
            for (int y = 0; y < 240; y++) {
                int row_idx = y * 320;
                
                // 1. Black out left side (x = 0 to 39)
                for (int x = 0; x < 40; x++) {
                    pixel_data[row_idx + x] = 0;
                }
                
                // 2. Grayscale center region (x = 40 to 279)
                for (int x = 40; x < 280; x++) {
                    int idx = row_idx + x;
                    uint16_t p = pixel_data[idx];
                    
                    uint8_t r = (p >> 11) & 0x1F;  // 5 bits
                    uint8_t g = (p >> 5) & 0x3F;   // 6 bits
                    uint8_t b = p & 0x1F;         // 5 bits

                    // Fast Grayscale scaling for 6-bit levels without division
                    // (r<<1) and (b<<1) promote 5-bit to 6-bit depth
                    uint8_t gray6 = ((r << 1) + g + (b << 1)) * 85 >> 8;

                    pixel_data[idx] = ((gray6 >> 1) << 11) | (gray6 << 5) | (gray6 >> 1);
                }
                
                // 3. Black out right side (x = 280 to 319)
                for (int x = 280; x < 320; x++) {
                    pixel_data[row_idx + x] = 0;
                }
            }
						*/

            //display result image
#if defined (__USE_DISPLAY__)
            //Display image on LCD
            const int X_OFFSET = 80;
						const int Y_OFFSET = 0;

						sDispRect.u32TopLeftX = X_OFFSET;
						sDispRect.u32TopLeftY = Y_OFFSET;

						sDispRect.u32BottonRightX = X_OFFSET + (320 * IMAGE_DISP_UPSCALE_FACTOR) - 1;
						sDispRect.u32BottonRightY = Y_OFFSET + (240 * IMAGE_DISP_UPSCALE_FACTOR) - 1;

#if defined(__PROFILE__)
            u64StartCycle = pmu_get_systick_Count();
#endif

            Display_FillRect((uint16_t *)infFramebuf->frameImage.data, &sDispRect, IMAGE_DISP_UPSCALE_FACTOR);

#if defined(__PROFILE__)
            u64EndCycle = pmu_get_systick_Count();
            info("display image cycles %llu \n", (u64EndCycle - u64StartCycle));
#endif

#endif

#if defined (__USE_UVC__)
			if(UVC_IsConnect())
			{
#if (UVC_Color_Format == UVC_Format_YUY2)
				image_t RGB565Img;
				image_t YUV422Img;

				RGB565Img.w = infFramebuf->frameImage.w;
				RGB565Img.h = infFramebuf->frameImage.h;
				RGB565Img.data = (uint8_t *)infFramebuf->frameImage.data;
				RGB565Img.pixfmt = PIXFORMAT_RGB565;

				YUV422Img.w = RGB565Img.w;
				YUV422Img.h = RGB565Img.h;
				YUV422Img.data = (uint8_t *)infFramebuf->frameImage.data;
				YUV422Img.pixfmt = PIXFORMAT_YUV422;
				
				roi.x = 0;
				roi.y = 0;
				roi.w = RGB565Img.w;
				roi.h = RGB565Img.h;
				imlib_nvt_scale(&RGB565Img, &YUV422Img, &roi);
				
#else
				image_t origImg;
				image_t vflipImg;

				origImg.w = infFramebuf->frameImage.w;
				origImg.h = infFramebuf->frameImage.h;
				origImg.data = (uint8_t *)infFramebuf->frameImage.data;
				origImg.pixfmt = PIXFORMAT_RGB565;

				vflipImg.w = origImg.w;
				vflipImg.h = origImg.h;
				vflipImg.data = (uint8_t *)infFramebuf->frameImage.data;
				vflipImg.pixfmt = PIXFORMAT_RGB565;

				imlib_nvt_vflip(&origImg, &vflipImg);
#endif
				UVC_SendImage((uint32_t)infFramebuf->frameImage.data, IMAGE_FB_SIZE, uvcStatus.StillImage);				

			}

#endif

      u64PerfFrames ++;
			
					static uint64_t last_cycle = pmu_get_systick_Count();

					uint64_t now = pmu_get_systick_Count();
					uint64_t delta = now - last_cycle;
					last_cycle = now;

					double fps = 0.0;
					if (delta > 0)
							fps = (double)SystemCoreClock / delta;

					info("Total inference rate: %f\n", fps);
					#if defined (__USE_DISPLAY__)
							sprintf(szDisplayText, "FPS: %.2f | People: %d", fps, g_currentPersonCount);
							//sprintf(szDisplayText,"Time %llu",(uint64_t) pmu_get_systick_Count() / (uint64_t)SystemCoreClock);
							//info("Running %s sec \n", szDisplayText);

							sDispRect.u32TopLeftX = 0;
							sDispRect.u32TopLeftY = frameBuffer.h * IMAGE_DISP_UPSCALE_FACTOR;
							sDispRect.u32BottonRightX = (frameBuffer.w);
							sDispRect.u32BottonRightY = ((frameBuffer.h * IMAGE_DISP_UPSCALE_FACTOR) + (FONT_DISP_UPSCALE_FACTOR * FONT_HTIGHT) - 1);

							Display_ClearRect(C_WHITE, &sDispRect);
													
												
							int text_len = strlen(szDisplayText);
							int char_width = 8 * FONT_DISP_UPSCALE_FACTOR;   // approximate font width
							int text_width = text_len * char_width;

							int x = (frameBuffer.w * IMAGE_DISP_UPSCALE_FACTOR - text_width) / 2;
							int y = 5; // small padding from top
												
												
							Display_PutText(
									szDisplayText,
									strlen(szDisplayText),
									x,
									y,
									C_BLUE,
									C_WHITE,
									false,
									FONT_DISP_UPSCALE_FACTOR
							);
					#endif
					u64PerfCycle = (uint64_t)pmu_get_systick_Count() + (uint64_t)(SystemCoreClock * EACH_PERF_SEC);
					u64PerfFrames = 0;
			
      infFramebuf->eState = eFRAMEBUF_EMPTY;
		}
		
		//Wait CCAP ready
		if (emptyFramebuf)
		{
#if !defined (__USE_CCAP__)
            info("Press 'n' to run next image inference \n");
            info("Press 'q' to exit program \n");

            while ((chStdIn = getchar()))
            {
                if (chStdIn == 'q')
                {
                    return 0;
                }
                else if (chStdIn != 'n')
                {
                    break;
                }
            }

            const uint8_t *pu8ImgSrc = get_img_array(u8ImgIdx);

            if (nullptr == pu8ImgSrc)
            {
                printf_err("Failed to get image index %" PRIu32 " (max: %u)\n", u8ImgIdx,
                           NUMBER_OF_FILES - 1);
                return -1;
            }

            u8ImgIdx ++;

            if (u8ImgIdx >= NUMBER_OF_FILES)
                u8ImgIdx = 0;

#endif

#if defined (__USE_CCAP__)
			//Capture new image

			ImageSensor_WaitCaptureDone();
#if defined(__PROFILE__)
			u64CCAPEndCycle = pmu_get_systick_Count();
			info("ccap capture cycles %llu \n", (u64CCAPEndCycle - u64CCAPStartCycle));
#endif
#else
            //copy source image to frame buffer
            image_t srcImg;

            srcImg.w = IMAGE_WIDTH;
            srcImg.h = IMAGE_HEIGHT;
            srcImg.data = (uint8_t *)pu8ImgSrc;
            srcImg.pixfmt = PIXFORMAT_RGB888;

            roi.x = 0;
            roi.y = 0;
            roi.w = IMAGE_WIDTH;
            roi.h = IMAGE_HEIGHT;

            imlib_nvt_scale(&srcImg, &emptyFramebuf->frameImage, &roi);

#endif
			emptyFramebuf->eState = eFRAMEBUF_FULL;		
		}

	}
		
    return 0;
}



