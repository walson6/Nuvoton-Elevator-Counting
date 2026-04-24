# ML

This folder contains ML-side scripts for:
- local inference demos
- dataset conversion
- training workflow support

## Contents

- `demos/people_count_demo.py`: local baseline/custom inference testing
- `scripts/convert_hf_overhead_dataset.py`: convert Hugging Face dataset into local YOLO format

---

## Dataset conversion

```bash
python scripts/convert_hf_overhead_dataset.py
```

---

## Training Command

Run from the project root:

```bash
python repos/ML_YOLO/yolov8_ultralytics/dg_train.py \
  --model-cfg repos/ML_YOLO/yolov8_ultralytics/ultralytics/cfg/models/v8/relu6-yolov8.yaml \
  --weights yolov8n.pt \
  --data data/dataset.yaml \
  --imgsz 192 \
  --epochs 20 \
  --batch 8 \
  --device cpu \
  --project runs/train \
  --name overhead_20ep
```

---

## Notes

- Using YOLOv8 ReLU6 for better Ethos-U55 compatibility
- Counting is done by the number of detected person bounding boxes
- `relu6-yolov8.yaml` should use `nc: 1`
- Local training at 192x192 completed successfully and produced a usable `best.pt` checkpoint