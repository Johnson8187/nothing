from ultralytics import YOLO

model = YOLO("yolov8n.pt")

results = model("input.jpg", save=True)

# 印出所有偵測到的物件類別
for r in results:
    for box in r.boxes:
        class_name = model.names[int(box.cls[0])]
        print(f"辨識到： {class_name}")
