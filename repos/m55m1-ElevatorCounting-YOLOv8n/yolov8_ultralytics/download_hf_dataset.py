"""Download from HuggingFace and structure for local YOLOv8 training.
Converts absolute [xmin, ymin, w, h] boxes from HF into normalized [x_center, y_center, w, h] YOLO format.
Split: 90/10 train/val.
"""
import os
import argparse
from datasets import load_dataset
from tqdm import tqdm

DATASET_NAME = "bdanko/overhead-person-detection"
TARGET_DIR = "datasets/overhead_monochrome"
IMG_SIZE = 192.0  # target dims are 192x192

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dataset', type=str, default=DATASET_NAME, help='HF Dataset name')
    parser.add_argument('--out-dir', type=str, default=TARGET_DIR, help='Target directory')
    return parser.parse_args()

def main():
    args = parse_args()
    
    # Load dataset
    print(f"Loading HuggingFace dataset: {args.dataset}")
    ds = load_dataset(args.dataset)
    
    # Isolate training split to create custom 90/10 layout
    # HF usually wraps it as "train" layout
    main_split = ds["train"]
    print(f"Total items: {len(main_split)}")
    
    # 90/10 Split
    train_test_split = main_split.train_test_split(test_size=0.1, seed=42)
    splits = {
        "train": train_test_split["train"],
        "val": train_test_split["test"]
    }

    # Setup directories
    for split in ["train", "val"]:
        os.makedirs(os.path.join(args.out_dir, split, "images"), exist_ok=True)
        os.makedirs(os.path.join(args.out_dir, split, "labels"), exist_ok=True)

    print("Populating local environment folder splits...")
    for split_name, data_split in splits.items():
        print(f"Processing split: {split_name} ({len(data_split)} items)")
        
        for idx, item in enumerate(tqdm(data_split)):
            pil_img = item["image"]
            objects = item["objects"]
            
            img_filename = f"{idx:05d}.png"
            lbl_filename = f"{idx:05d}.txt"
            
            img_path = os.path.join(args.out_dir, split_name, "images", img_filename)
            lbl_path = os.path.join(args.out_dir, split_name, "labels", lbl_filename)
            
            # Save Image (grayscale .png)
            pil_img.save(img_path)
            
            # Save Label in YOLO format
            with open(lbl_path, "w") as f_lbl:
                for bbox, category in zip(objects["bbox"], objects["category"]):
                    xmin, ymin, w, h = bbox
                    
                    # Convert absolute (192x192) to normalized coordinates
                    x_center = (xmin + w / 2.0) / IMG_SIZE
                    y_center = (ymin + h / 2.0) / IMG_SIZE
                    w_norm = w / IMG_SIZE
                    h_norm = h / IMG_SIZE
                    
                    # Ensure positive normalized range
                    x_center = max(0.0, min(1.0, x_center))
                    y_center = max(0.0, min(1.0, y_center))
                    w_norm = max(0.0, min(1.0, w_norm))
                    h_norm = max(0.0, min(1.0, h_norm))
                    
                    f_lbl.write(f"{category} {x_center:.6f} {y_center:.6f} {w_norm:.6f} {h_norm:.6f}\n")

    # Generate data.yaml
    yaml_path = os.path.join(args.out_dir, "data.yaml")
    abs_out_dir = os.path.abspath(args.out_dir)
    
    yaml_content = f"""path: {abs_out_dir}
train: train/images
val: val/images

nc: 1
names: ['person']
"""
    with open(yaml_path, "w") as f_yaml:
        f_yaml.write(yaml_content)

    print(f"\nDone! Dataset structure created at: {abs_out_dir}")
    print(f"You can train with: --data {os.path.join(args.out_dir, 'data.yaml')}")

if __name__ == "__main__":
    main()
