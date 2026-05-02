# groups.cmake

# group CMSIS
add_library(Group_CMSIS OBJECT
  "${SOLUTION_ROOT}/../../../../Library/Device/Nuvoton/M55M1/Source/startup_M55M1.c"
  "${SOLUTION_ROOT}/../../../../Library/Device/Nuvoton/M55M1/Source/system_M55M1.c"
)
target_include_directories(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_CMSIS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_CMSIS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_CMSIS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_CMSIS PUBLIC
  Group_CMSIS_ABSTRACTIONS
)

# group Driver
add_library(Group_Driver OBJECT
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/uart.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/retarget.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/ccap.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/clk.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/sys.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/gpio.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/npu/ethosu_device_u55_u65.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/npu/ethosu_driver.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/npu/ethosu_pmu.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/ebi.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/spim_hyper.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/sdh.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/spi.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/hsusbd.c"
  "${SOLUTION_ROOT}/../../../../Library/StdDriver/src/pdma.c"
)
target_include_directories(Group_Driver PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Driver PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Driver_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Driver_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Driver PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Driver PUBLIC
  Group_Driver_ABSTRACTIONS
)

# group Application
add_library(Group_Application OBJECT
  "${SOLUTION_ROOT}/../ModelFileReader.c"
  "${SOLUTION_ROOT}/../BoardInit.cpp"
  "${SOLUTION_ROOT}/../main.cpp"
  "${SOLUTION_ROOT}/../YOLOv8nODPostProcessing.cpp"
)
target_include_directories(Group_Application PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  "${SOLUTION_ROOT}/.."
)
target_compile_definitions(Group_Application PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Application_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Application_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Application PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Application PUBLIC
  Group_Application_ABSTRACTIONS
)

# group Lib
add_library(Group_Lib INTERFACE)
target_include_directories(Group_Lib INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Lib INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Lib_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Lib_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_link_libraries(Group_Lib INTERFACE
  ${SOLUTION_ROOT}/../../../../Library/CMSIS/Lib/KEIL/cmsis_dsp.lib
  ${SOLUTION_ROOT}/../../../../ThirdParty/tflite_micro/Lib/tflu.lib
  ${SOLUTION_ROOT}/../../../../Library/CMSIS/Lib/KEIL/cmsis_nn.lib
  ${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/Lib/omv.lib
)

# group Model
add_library(Group_Model OBJECT
  "${SOLUTION_ROOT}/../Model/YOLOv8nODModel.cpp"
  "${SOLUTION_ROOT}/../Model/Labels.cpp"
)
target_include_directories(Group_Model PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Model PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Model_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Model_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Model PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Model PUBLIC
  Group_Model_ABSTRACTIONS
)

# group ArmMLApi
add_library(Group_ArmMLApi OBJECT
  "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/application/api/common/source/Model.cc"
  "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/application/api/common/source/TensorFlowLiteMicro.cc"
  "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/math/PlatformMath.cc"
  "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/profiler/Profiler.cc"
)
target_include_directories(Group_ArmMLApi PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ArmMLApi PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ArmMLApi_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ArmMLApi_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ArmMLApi PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_ArmMLApi PUBLIC
  Group_ArmMLApi_ABSTRACTIONS
)

# group NPU
add_library(Group_NPU OBJECT
  "${SOLUTION_ROOT}/../NPU/ethosu_cpu_cache.c"
  "${SOLUTION_ROOT}/../NPU/ethosu_npu_init.c"
  "${SOLUTION_ROOT}/../NPU/ethosu_profiler.c"
)
target_include_directories(Group_NPU PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_NPU PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_NPU_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_NPU_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_NPU PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_NPU PUBLIC
  Group_NPU_ABSTRACTIONS
)

# group ProfilerCounter
add_library(Group_ProfilerCounter OBJECT
  "${SOLUTION_ROOT}/../ProfilerCounter/pmu_counter.c"
)
target_include_directories(Group_ProfilerCounter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_ProfilerCounter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_ProfilerCounter_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_ProfilerCounter_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_ProfilerCounter PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_ProfilerCounter PUBLIC
  Group_ProfilerCounter_ABSTRACTIONS
)

# group Device
add_library(Group_Device OBJECT
  "${SOLUTION_ROOT}/../Device/ImageSensor/ImageSensor.c"
  "${SOLUTION_ROOT}/../Device/ImageSensor/Sensor/Sensor_HM1055.c"
  "${SOLUTION_ROOT}/../Device/ImageSensor/Sensor/SWI2C.c"
  "${SOLUTION_ROOT}/../Device/Display/Display.c"
  "${SOLUTION_ROOT}/../Device/Display/LCD/LCD_FSA506.c"
  "${SOLUTION_ROOT}/../Device/Display/Font8_16.c"
  "${SOLUTION_ROOT}/../Device/HyperRAM/hyperram_code.c"
  "${SOLUTION_ROOT}/../Device/SDCard/sdglue.c"
  "${SOLUTION_ROOT}/../Device/Display/LCD/LCD_ILI9341.c"
  "${SOLUTION_ROOT}/../Device/UVC/descriptors.c"
  "${SOLUTION_ROOT}/../Device/UVC/UVC.c"
  "${SOLUTION_ROOT}/../Device/Display/LCD/LCD_LT7381.c"
  "${SOLUTION_ROOT}/../Device/Display/drv_pdma.c"
  "${SOLUTION_ROOT}/../../../../Library/Storage/diskio_SDH.c"
)
target_include_directories(Group_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Device_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Device_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Device PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Device PUBLIC
  Group_Device_ABSTRACTIONS
)

# group FatFS
add_library(Group_FatFS OBJECT
  "${SOLUTION_ROOT}/../../../../ThirdParty/FatFs/source/ff.c"
  "${SOLUTION_ROOT}/../../../../ThirdParty/FatFs/source/ffunicode.c"
)
target_include_directories(Group_FatFS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_FatFS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_FatFS_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_FatFS_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_FatFS PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_FatFS PUBLIC
  Group_FatFS_ABSTRACTIONS
)

# group Pattern
add_library(Group_Pattern OBJECT
  "${SOLUTION_ROOT}/../Pattern/car.cpp"
  "${SOLUTION_ROOT}/../Pattern/dinner.cpp"
  "${SOLUTION_ROOT}/../Pattern/InputFiles.cpp"
)
target_include_directories(Group_Pattern PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_Pattern PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_Pattern_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_Pattern_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_Pattern PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_Pattern PUBLIC
  Group_Pattern_ABSTRACTIONS
)
