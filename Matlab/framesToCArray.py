from PIL import Image
import glob
import numpy as np

NUM_FRAMES = 8


## Find files
frames = glob.glob('VideoFrames/*')



def createHeaderFileArray(frames):
	processedFrames = []
	for frame in frames:
		imageArr = np.array(Image.open(frame).convert('RGB'))
		processedFrames.append(formatArray(imageArr))
	return processedFrames

def formatArray(npArray):
	## Convert from img[row][col][channel] into img[channel][row][col]
	red = []
	green = []
	blue = []
	for row in npArray:
		for pixel in row:
			red.append(pixel[0])
			green.append(pixel[1])
			blue.append(pixel[2])
	
	return (red+green+blue)

processedFrames = createHeaderFileArray(frames[0:NUM_FRAMES])
for n, frame in enumerate(processedFrames):
	with open('outputFrames\\frame{}.bin'.format(n), 'wb+') as file:
		file.write(bytes(frame))

