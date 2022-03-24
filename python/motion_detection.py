from dataclasses import dataclass
import cv2
from cvzone.PoseModule import PoseDetector
import json

#cap = cv2.VideoCapture(0)
cap = cv2.VideoCapture("assets/short_2.mp4")
detector = PoseDetector()

collectedData = {"data": []}
frame = 0

maxFrames = 15000

while True:
  success, img = cap.read()
  if img is None: 
    break

  img = detector.findPose(img)
  lmList, bboxInfo = detector.findPosition(img)

  if bboxInfo:
    collectedData["data"].append({
      "bbox": bboxInfo,
      "lmList": [[item[0], item[1], img.shape[0] - item[2], item[3]] for item in lmList]
    })
  else:
    collectedData["data"].append(None)

  cv2.imshow("Image", img)
  cv2.waitKey(1)

  frame += 1
  if frame > maxFrames:
    break;


# Write collected data to output file
with open('C:/Projects/UE4VideoFeatures/shared/motion_data.json', 'w') as f:
 f.write(json.dumps(collectedData))