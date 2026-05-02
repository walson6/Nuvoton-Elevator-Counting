# SJSU AI & ML Club - Nuvoton

**Edge AI People Counting on the Nuvoton M55M1 EVB**

Deploy a real-time headcount system using a top-down camera and a model accelerated by the Ethos-U55 NPU. All inference runs locally on-device. No cloud or external storage.

## Goals

- ≥95% headcount accuracy
- ≥15 FPS inference
- Zero-Cloud: all processing on-device, no data transmitted externally

---

## Repo Structure

```text
Nuvoton-Team/
├── data/         # dataset structure, metadata, and data instructions
├── deployment/   # export / Vela / board integration notes and scripts
├── ml/           # training, local inference, and conversion scripts
├── models/       # trained checkpoints and compiled deployment models
│   ├── EmKnightBest.pt
│   ├── EmKnightBest_full_integer_quant_vela.tflite
│   ├── YOLOv8n-elevator-od.tflite
│   ├── best_full_integer_quant.tflite
│   └── TristaOverhead200epBest_full_integer_quant_vela.tflite
├── repos/
│   ├── m55m1-ElevatorCounting-YOLOv8n/
└── README.md
```

> `Library/` and `ThirdParty/` are not committed. See setup steps below to get them.

---

## Resources

- [ML_YOLO Repo](https://github.com/OpenNuvoton/ML_YOLO) - training, export, and quantization pipeline
- [ML_M55M1_SampleCode](https://github.com/OpenNuvoton/ML_M55M1_SampleCode) - sample M55M1 ML application reference
- [M55M1 BSP](https://github.com/OpenNuvoton/M55M1BSP) - board support package for lower-level firmware integration
- [NuEdgeWise](https://github.com/OpenNuvoton/NuEdgeWise)

---

## Dataset & Performance

**Dataset Setup Guide & Information**

For full instructions on downloading, structuring, and configuring the dataset for this project, see:
https://github.com/harshakrishna15/Nuvoton_Model/blob/main/docs/DATASETS.md


### Performance

| Metric        | Value   | Description                                        |
| ------------- | ------- | -------------------------------------------------- |
| Precision (P) | `94.9%` | Positive prediction accuracy (low false positives) |
| Recall (R)    | `94.1%` | Sensitivity / detection coverage                   |
| mAP50         | `97.8%` | Mean Average Precision at IoU=0.50                 |
| mAP50-95      | `68.6%` | Average mAP across thresholds                      |
| FPS           | `22`    | Average FPS on M55M1                               |

---

## Team Evaluation - Installation & Test

> This section is for reviewers evaluating the project. `EmKnightBest.pt` and all compiled model files are already included in the `models/` folder. No training or data download needed.

### 1. Clone the repo

```bash
git clone https://github.com/walson6/Nuvoton-Elevator-Counting.git
cd Nuvoton-Elevator-Counting
```

### 2. Clone ML_YOLO and set up the conda environment

```bash
mkdir -p repos
cd repos
git clone https://github.com/OpenNuvoton/ML_YOLO.git
cd ..
```

See the [`yolov8_DG` Environment Setup](#yolov8_dg-environment-setup) section below for the full conda environment and package installation steps.

### 3. Run validation

> ⚠️ Before running, open `data/dataset.yaml` and update the `path` field to point to your local copy of the dataset images and labels:
> ```yaml
> path: C:\path\to\your\local\data  # update this per machine
> ```

From inside `repos/ML_YOLO/yolov8_ultralytics/`:

```bash
python dg_val.py \
  --weights "../../../models/EmKnightBest.pt" \
  --data "../../../data/dataset.yaml" \
  --img 192 \
  --device cpu
```

Expected output: see [Performance](#performance) table above.

### 4. Run visual inference (optional)

```bash
python -c "
from ultralytics import YOLO
model = YOLO('../../../models/EmKnightBest.pt')
model.predict(source='../../../data/images/val', imgsz=192, save=True, conf=0.25)
"
```

Results are saved to `runs/detect/predict/`.

---

## Export & Deployment Pipeline

This section covers the full workflow from a trained `.pt` checkpoint to a Vela-compiled `.tflite` file running on the M55M1 board.

> **Prerequisites:** Miniforge installed, `yolov8_DG` conda environment set up (see below), ML_YOLO cloned to `repos/ML_YOLO/`.

> **Note:** Calibration data generation and INT8 accuracy validation are checkpoint- and dataset-specific. Refer to your checkpoint's training notes for those steps.

### Step 1 - Place your `.pt` weights

Copy your trained weights file into `models/` and also into the working directory:

```
models\YourCheckpoint.pt
repos\ML_YOLO\yolov8_ultralytics\YourCheckpoint.pt
```

Then open Miniforge and navigate to the working directory:

```bash
cd path/to/Nuvoton-Team/repos/ML_YOLO/yolov8_ultralytics
conda activate yolov8_DG
```

---

### Step 2 - Export to ONNX

```bash
python nu_export_tflite_int8.py \
  --format onnx \
  --weights "YourCheckpoint.pt" \
  --img 192
```

---

### Step 3 - Quantize (ONNX → TFLite INT8)

```bash
onnx2tf \
  -i YourCheckpoint.onnx \
  -oiqt \
  -cind images calib_data.npy "[[[[0,0,0]]]]" "[[[[1,1,1]]]]"
```

Output: `saved_model\YourCheckpoint_full_integer_quant.tflite`

---

### Step 4 - Run Vela compilation

1. Move the quantized model into the Vela input directory:

```
saved_model\YourCheckpoint_full_integer_quant.tflite
→ vela\generated\
```

2. Update `vela\variables.bat` with the correct filenames:

```bat
set MODEL_SRC_FILE=YourCheckpoint_full_integer_quant.tflite
set MODEL_OPTIMISE_FILE=YourCheckpoint_full_integer_quant_vela.tflite
```

3. Run the Vela batch script:

```bash
cd "path/to/Nuvoton-Team/repos/ML_YOLO/yolov8_ultralytics/vela"
.\gen_model_cpp.bat
```

Output: `vela\generated\YourCheckpoint_full_integer_quant_vela.tflite.cc`

---

### Step 5 - Deploy to the M55M1 board

You now have a `*_vela.tflite` file. Do all three of the following:

**A. Save to `models/`**

Copy the Vela `.tflite` into `models/` for version tracking:

```
vela\generated\YourCheckpoint_full_integer_quant_vela.tflite
→ models\YourCheckpoint_full_integer_quant_vela.tflite
```

**B. Add to the firmware repo (for Keil to build)**

Copy and rename the Vela `.tflite` to the expected filename:

```
vela\generated\YourCheckpoint_full_integer_quant_vela.tflite
→ repos\m55m1-ElevatorCounting-YOLOv8n\Model\YOLOv8n-elevator-od.tflite
```

**C. Copy to the physical SD card**

Copy that same Vela `.tflite` to the **root of the microSD card**, named:

```
YOLOv8n-elevator-od.tflite
```

The board reads the model from the SD card at runtime at path `"0:\\YOLOv8n-elevator-od.tflite"`.

**D. Build and flash**

Open the Keil project from (`repos/m55m1-ElevatorCounting-YOLOv8n/KEIL`) and run build + flash.

---

## Running Inference (Keil / Board Setup)

> **Warning:** Additional libraries are required before building.

Run the install script to fetch `Library` and `ThirdParty`:

```bash
cd repos
git clone https://github.com/OpenNuvoton/ML_M55M1_SampleCode.git
cd ML_M55M1_SampleCode
python install.py
```

Once installed, configure the library paths in `repos/m55m1-ElevatorCounting-YOLOv8n/KEIL/ObjectDetection.csolution.yml`:

```yaml
- BSP_PATH: "C:/Library"
- TP_PATH: "C:/ThirdParty"
```

Update these paths to match where your `Library` and `ThirdParty` folders were installed if they differ from the defaults.

---

## `yolov8_DG` Environment Setup

This environment is required for all export, quantization, and Vela steps.

Source: [OpenNuvoton/ML_YOLO - yolov8_ultralytics](https://github.com/OpenNuvoton/ML_YOLO)

### 1. Create and activate the environment

```bash
conda create --name yolov8_DG python=3.10
conda activate yolov8_DG
```

### 2. Upgrade pip

```bash
python -m pip install --upgrade pip setuptools
```

### 3. Install PyTorch (CUDA 11.8)

```bash
python -m pip install torch==2.5.1 torchvision==0.20.1 torchaudio==2.5.1 --index-url https://download.pytorch.org/whl/cu118
```

> If you are on a CPU-only machine, check the [PyTorch install page](https://pytorch.org/get-started/locally/) for the correct command.

### 4. Install the Ultralytics YOLOv8 package

From inside `repos/ML_YOLO/yolov8_ultralytics/`:

```bash
cd repos/ML_YOLO/yolov8_ultralytics
python -m pip install .[export]
```

This installs the full export-capable build of the DeGirum-modified Ultralytics package, including ONNX and TFLite dependencies.

---

## Dataset Format

The `yolov8_DG` training scripts expect datasets in Ultralytics YOLO format with this structure:

```text
<dataset_name>/
├── train.txt        # paths to training images
├── val.txt          # paths to validation images
├── train/
│   ├── image1.jpg
│   └── image1.txt   # YOLO label: <class_id> <cx> <cy> <w> <h>
└── val/
    ├── image2.jpg
    └── image2.txt
```

The `dataset.yaml` should look like:

```yaml
path: C:\path\to\your_dataset
train: train.txt
val: val.txt
test: val.txt

names:
  0: person
```

And update `ultralytics/cfg/models/v8/relu6-yolov8.yaml` to match your class count:

```yaml
nc: 1  # number of classes
```

---

## Training

```bash
python dg_train.py \
  --model-cfg relu6-yolov8.yaml \
  --data dataset.yaml \
  --imgsz 192 \
  --weights yolov8n.pt \
  --batch 64 \
  --epochs 300
```

### Evaluate `.pt` model

```bash
python dg_val.py \
  --weights "../../../models/best.pt" \
  --data "../../../data/dataset.yaml" \
  --img 192
```

## Contributors
- Bence Danko
- Harsha Krishnaswamy
- Jimmy Vu
- Shrivaikunth Krishnakumar
- Trista Chen
