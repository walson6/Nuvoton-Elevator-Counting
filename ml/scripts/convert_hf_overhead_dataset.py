from pathlib import Path
from datasets import load_dataset
import random

PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
OUT_ROOT = PROJECT_ROOT / "data"
TRAIN_RATIO = 0.9
SEED = 42

def ensure_dirs() -> None:
    for split in ["train", "val"]:
        (OUT_ROOT / "images" / split).mkdir(parents=True, exist_ok=True)
        (OUT_ROOT / "labels" / split).mkdir(parents=True, exist_ok=True)

def xywh_abs_to_yolo(box, img_w: int, img_h: int):
    x, y, w, h = box
    x_center = (x + w / 2.0) / img_w
    y_center = (y + h / 2.0) / img_h
    w_norm = w / img_w
    h_norm = h / img_h
    return x_center, y_center, w_norm, h_norm

def main() -> None:
    ensure_dirs()

    ds = load_dataset("bdanko/overhead-person-detection", split="train")
    indices = list(range(len(ds)))
    random.Random(SEED).shuffle(indices)

    train_cutoff = int(len(indices) * TRAIN_RATIO)
    train_indices = set(indices[:train_cutoff])

    for i, row in enumerate(ds):
        split = "train" if i in train_indices else "val"

        image = row["image"]
        img_w, img_h = image.size
        objects = row["objects"]

        image_name = f"img_{i:05d}.png"
        label_name = f"img_{i:05d}.txt"

        image_path = OUT_ROOT / "images" / split / image_name
        label_path = OUT_ROOT / "labels" / split / label_name

        image.convert("RGB").save(image_path)

        bboxes = objects["bbox"]
        categories = objects["category"]

        lines = []
        for bbox, category in zip(bboxes, categories):
            # Expect class 0 = person
            cls_id = int(category)
            x_center, y_center, w_norm, h_norm = xywh_abs_to_yolo(bbox, img_w, img_h)
            lines.append(
                f"{cls_id} {x_center:.6f} {y_center:.6f} {w_norm:.6f} {h_norm:.6f}"
            )

        label_path.write_text("\n".join(lines) + ("\n" if lines else ""), encoding="utf-8")

    print("Done.")
    print(f"Saved dataset to: {OUT_ROOT.resolve()}")

if __name__ == "__main__":
    main()