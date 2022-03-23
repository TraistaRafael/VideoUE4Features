from dataclasses import dataclass
import cv2
from cvzone.PoseModule import PoseDetector
import json

# cap = cv2.VideoCapture(0)
cap = cv2.VideoCapture("assets/short_1.mp4")
detector = PoseDetector()

collectedData = []
frame = 0

while True:
  success, img = cap.read()
  if img is None: 
    break

  img = detector.findPose(img)
  lmList, bboxInfo = detector.findPosition(img)
  collectedData.append({})
  if bboxInfo:
    collectedData.append({
      "bbox": bboxInfo,
      "lmList": lmList
    })
  else:
    collectedData.append({})

  cv2.imshow("Image", img)
  cv2.waitKey(1)


# Write collected data to output file
with open('C:/Projects/UE4VideoFeatures/shared/motion_data.json', 'w') as f:
 f.write(json.dumps(collectedData))