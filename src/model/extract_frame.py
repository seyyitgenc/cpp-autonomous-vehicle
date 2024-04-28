import cv2
import csv
import os
import numpy as np

def preprocess_sign(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # Lower red range
    lower_red1 = np.array([0, 120, 70])
    upper_red1 = np.array([10, 255, 255])

    # Upper red range
    lower_red2 = np.array([160, 120, 70])
    upper_red2 = np.array([180, 255, 255])

    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)

    mask = cv2.bitwise_or(mask1, mask2)

    return mask
def findContour(img):
	img = img.astype('uint8')
	contours, hierarchy = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
	return contours

def findBiggestContour(contours):
	m = 0
	c = [cv2.contourArea(i) for i in contours]
	return contours[c.index(max(c))]

def boundaryBox(img,contours):
	x, y, w, h = cv2.boundingRect(contours)
	# img = cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 2)
	sign = img[y:(y+h) , x:(x+w)]
	return img, sign

# Open the video file
cap = cv2.VideoCapture('/home/seyyit/projects/cpp-autonomous-vehicle/DataSetVideos/stopSign.mp4')

# Check if video opened successfully
if not cap.isOpened(): 
	print("Error opening video file")

# Specify the directory where the images are saved
image_dir = '/home/seyyit/dataset/0/'

# Read until video is completed
frame_count = 0
while(cap.isOpened()):
	# Capture frame-by-frame
	ret, frame = cap.read()
	if ret:
		redness = (frame)
		contours = findContour(redness)
		big = findBiggestContour(contours)
		if big <= 0:
			continue
		if cv2.contourArea(big) > 3000:
			img,sign = boundaryBox(frame,big)
			cv2.imwrite(f'{image_dir}frame{frame_count}.png', sign)
			frame_count += 1
	else: 
		break

# When everything done, release the video capture object
cap.release()

# Close all the frames
cv2.destroyAllWindows()

# Get a list of all image file names in the directory
image_files = os.listdir(image_dir)

# # Open a new CSV file in write mode
# with open('image_data.csv', 'w', newline='') as file:
# 	writer = csv.writer(file)
# 	# Write the header row
# 	writer.writerow(["FileName", "Label"])
# 	# Write a row for each image file
# 	for image_file in image_files:
# 		# Use the file name as the label for now
# 		label = "turn_left"
# 		writer.writerow([image_file, label])