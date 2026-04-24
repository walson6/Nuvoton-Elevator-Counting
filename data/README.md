# Data

This folder stores dataset structure and dataset metadata for training.

## Expected structure

```text
data/
├── dataset.yaml
├── images/
│   ├── train/
│   └── val/
└── labels/
    ├── train/
    └── val/
```

---

## Label format

YOLO object detection format:
```text
class x_center y_center width height
```

---

## Notes

- One class: `person`
- Each image must have a matching `.txt` label file
- Full image/label data should not be committed to Git unless explicitly agreed by the team
- `dataset.yaml` defines the training/validation paths used by the ML pipeline