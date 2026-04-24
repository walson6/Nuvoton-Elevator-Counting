from ultralytics import YOLO
import cv2
import torch
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent

# ===== Choose input =====
# INPUT_PATH = PROJECT_ROOT / "local_inputs/test.png"
INPUT_PATH = PROJECT_ROOT / "local_inputs/test.mp4"

# ===== Choose model =====
USE_CUSTOM_MODEL = False

if USE_CUSTOM_MODEL:
    MODEL_PATH = PROJECT_ROOT / "saved_models/overhead_20ep_best.pt"
    MODEL_TAG = "custom"
else:
    MODEL_PATH = PROJECT_ROOT / "yolov8n.pt"
    MODEL_TAG = "baseline"

PERSON_CLASS_ID = 0
CONF_THRESHOLD = 0.35

IMAGE_EXTS = {".jpg", ".jpeg", ".png", ".bmp", ".webp"}
VIDEO_EXTS = {".mp4", ".mov", ".avi", ".mkv", ".m4v"}

device = "mps" if torch.backends.mps.is_available() else "cpu"
print("Using device:", device)

model = YOLO(str(MODEL_PATH))


def detect_and_draw(frame):
    results = model.predict(frame, device=device, verbose=False)[0]

    count = 0

    for box in results.boxes:
        cls = int(box.cls[0])
        conf = float(box.conf[0])

        if cls == PERSON_CLASS_ID and conf > CONF_THRESHOLD:
            count += 1
            x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(
                frame,
                f"{conf:.2f}",
                (x1, max(y1 - 8, 20)),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,
                (0, 255, 0),
                1
            )

    cv2.putText(
        frame,
        f"Count: {count}",
        (20, 40),
        cv2.FONT_HERSHEY_SIMPLEX,
        1,
        (0, 0, 255),
        2
    )
    return frame, count


def process_image(input_path: Path):
    output_dir = PROJECT_ROOT / "outputs/photos"
    output_dir.mkdir(parents=True, exist_ok=True)

    image = cv2.imread(str(input_path))
    if image is None:
        raise RuntimeError(f"Could not open image: {input_path}")

    result_image, count = detect_and_draw(image)

    output_path = output_dir / f"{input_path.stem}_{MODEL_TAG}.png"
    success = cv2.imwrite(str(output_path), result_image)
    if not success:
        raise RuntimeError(f"Could not save image: {output_path}")

    print(f"Saved image: {output_path}")
    print(f"Detected count: {count}")


def process_video(input_path: Path):
    output_dir = PROJECT_ROOT / "outputs/videos"
    output_dir.mkdir(parents=True, exist_ok=True)

    cap = cv2.VideoCapture(str(input_path))
    if not cap.isOpened():
        raise RuntimeError(f"Could not open video: {input_path}")

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = cap.get(cv2.CAP_PROP_FPS)
    if fps <= 0:
        fps = 20

    output_path = output_dir / f"{input_path.stem}_{MODEL_TAG}.mp4"

    writer = cv2.VideoWriter(
        str(output_path),
        cv2.VideoWriter_fourcc(*"mp4v"),
        fps,
        (width, height)
    )

    if not writer.isOpened():
        raise RuntimeError(f"Could not open video writer for: {output_path}")

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        result_frame, _ = detect_and_draw(frame)
        writer.write(result_frame)

    cap.release()
    writer.release()

    print(f"Saved video: {output_path}")


def main():
    if not INPUT_PATH.exists():
        raise FileNotFoundError(f"Input file not found: {INPUT_PATH}")

    suffix = INPUT_PATH.suffix.lower()

    if suffix in IMAGE_EXTS:
        process_image(INPUT_PATH)
    elif suffix in VIDEO_EXTS:
        process_video(INPUT_PATH)
    else:
        raise ValueError(f"Unsupported file type: {suffix}")


if __name__ == "__main__":
    main()
