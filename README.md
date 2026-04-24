# SJSU AI & ML Club — Nuvoton Team 2
**Edge AI People Counting on the Nuvoton M55M1 EVB**

Deploy a real-time headcount system using a top-down camera and a model accelerated by the Ethos-U55 NPU. All inference runs locally on-device — no cloud or external storage.

## Goals

- ≥95% headcount accuracy
- ≥15 FPS inference
- Zero-Cloud: all processing on-device, no data transmitted externally

---

## Repo Structure

```text
Nuvoton-Team-2/
├── data/         # dataset structure, metadata, and data instructions
├── ml/           # training, local inference, and conversion scripts
└── deployment/   # export / Vela / board integration notes and scripts
```

---

## Resources

- [ML_YOLO Repo](https://github.com/OpenNuvoton/ML_YOLO) — training, export, and quantization pipeline
- [ML_M55M1_SampleCode](https://github.com/OpenNuvoton/ML_M55M1_SampleCode) — sample M55M1 ML application reference
- [M55M1 BSP](https://github.com/OpenNuvoton/M55M1BSP) — board support package for lower-level firmware integration
- [NuEdgeWise](https://github.com/OpenNuvoton/NuEdgeWise)
- [bdanko/overhead-person-detection](https://huggingface.co/datasets/bdanko/overhead-person-detection) — overhead/top-down detection dataset used for initial training tests

---

## Quick Start

### 1. Create and activate virtual environment

```bash
python3.11 -m venv .venv
source .venv/bin/activate
```

### 2. Install dependencies

```bash
pip install --upgrade pip setuptools
pip install torch==2.5.1 torchvision==0.20.1 torchaudio==2.5.1
pip install ultralytics opencv-python matplotlib pyyaml datasets pillow
pip install numpy==1.26.4 py-cpuinfo seaborn
```

### 3. Clone required repositories

```bash
mkdir -p repos
cd repos
git clone https://github.com/OpenNuvoton/ML_YOLO.git
git clone https://github.com/OpenNuvoton/ML_M55M1_SampleCode.git
cd ..
```

### 4. Download baseline YOLOv8 weights

```bash
python -c "from ultralytics import YOLO; YOLO('yolov8n.pt')"
```

---

## See also

- `data/README.md` for dataset format and labeling expectations
- `ml/README.md` for dataset conversion, training, and local inference
- `deployment/README.md` for export / deployment notes

---

## Current Status

- Local YOLOv8 ReLU6 training pipeline is working
- Initial training on overhead-person dataset at 192×192 completed successfully
- Local inference demo supports image/video testing with baseline vs custom checkpoints