import tensorflow as tf
import sys

model_path = "/home/bence/m55m1-ElevatorCounting-YOLOv8n/Model/YOLOv8n-od.tflite"

try:
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()
except Exception as e:
    print(f"Error loading model: {e}")
    sys.exit(1)

print("--- Output Tensor Details ---")
output_details = interpreter.get_output_details()
for i, details in enumerate(output_details):
    print(f"Index {i}: name='{details['name']}'")
    print(f"  Shape: {details['shape']}")
    print(f"  DType: {details['dtype']}")
    print(f"  TensorIndex: {details['index']}")
