import numpy as np
import cv2
import matplotlib.pyplot as plt
import spiral
import csv
import math

# TODO: Potentially remove precision from the coordinates so that arduino doesn't overflow?

def cart2pol(x,y):
    r = np.sqrt(x**2 + y**2)
    theta = np.arctan2(y,x) + np.pi
    return (r,theta)

class PolarImageConverter(object):

    def __init__(self, imageArray):
        imageArray[imageArray == 255] = 1
        self.imageArray = imageArray
        self.imageSize = imageArray.shape
        self.largestRadius = min(self.imageSize)/2
        self.origin = None
        self.cartesianList = []
        self.polarList = []
        self.spiralList = []

    def cropImage(self):
        #delete odd sized columns and rows
        if self.imageSize[0] % 2 == 0:
            self.imageArray = np.delete(self.imageArray, 0, axis=0)
        if self.imageSize[1] % 2 == 0:
            self.imageArray = np.delete(self.imageArray, 0, axis=1)
        self.imageSize = self.imageArray.shape

        # Crop not square images
        if self.imageSize[0] > self.imageSize[1]:
            largerSide = 0
        else:
            largerSide = 1
        sideDifference = self.imageSize[largerSide] - self.imageSize[1-largerSide]
        self.imageArray = np.delete(self.imageArray, range(sideDifference/2), axis=largerSide)
        self.imageArray = np.delete(self.imageArray, range(len(self.imageArray)-sideDifference/2,len(self.imageArray)), axis=largerSide)

        # save variables for drawing
        self.imageSize = self.imageArray.shape
        self.largestRadius = min(self.imageSize) / 2


    def constructCartesianList(self):
        self.cartesianList = []
        for (x,y),value in np.ndenumerate(self.imageArray):
            l = [x-self.imageSize[0]/2,y-self.imageSize[1]/2,value]
            self.cartesianList.append(l)

    def constructPolarFromCartesian(self):
        self.polarList = []
        for x,y,value in self.cartesianList:
            polar = cart2pol(x,y)
            l = [polar[0],polar[1]]
            if l[0] <= self.largestRadius:
                l = np.around([100*l[0]/self.largestRadius,l[1]], decimals=2)
                # convert radius to percentage of total radius
                l = l.tolist()
                l.append(value)
                self.polarList.append(l)
            else:
                # cutting corners and making it a circle image
                pass
        self.polarList.sort(key=lambda x : (x[0], x[1]))

    def constructTraversalDirections(self):
        """
        Constructs list with traversal directions in form of
        (down point, up point)
        """
        self.polarTraversal = []
        tmp = []
        for coord in self.polarList:
            if coord[2] == 1:
                tmp.append(coord)
            if coord[2] == 0 and len(tmp) > 0:
                self.polarTraversal.append((tmp[0][0],tmp[0][1],coord[0],coord[1]))
                tmp = []

    def constructSpiralCommands(self, totalRotations, stepsPerRotation, marker):
        """
        Markers:
        0 = black (driving)
        1 = darkgrey (+120 degrees)
        2 = lightgrey (+240 degrees)
        """
        self.spiralList = [[100.0/totalRotations]]
        steps = 0.0
        angle = 0
        radius = 0

        for i in range(totalRotations*stepsPerRotation):
            # constructs all of the angle, rotation, and radius variables for this step
            steps += 1.0
            rotations = steps / stepsPerRotation
            angle = (360.0) * steps / stepsPerRotation + 120 * marker
            radiansAngle = angle * math.pi / 180.0
            radius = rotations / totalRotations * self.largestRadius

            # gets the predicted x and y coordinates and gets the image data nearest to that
            # angle will be from right spiraling counter clockwise
            xDistance = int(round(radius * math.cos(radiansAngle))) + self.largestRadius
            yDistance = self.largestRadius - int(round(radius * math.sin(radiansAngle)))

            self.spiralList.append([int(round(angle)), self.imageArray[xDistance][yDistance], marker])

    def constructSpiralTraversalDirections(self):
        """
        Constructs list with traversal directions in form of
        (down point, up point)
        """
        self.spiralTraversal = [self.spiralList[1]] # self.spiralList[0] is a command
        prevCommand = self.spiralList[1][1]
        for coord in self.spiralList[2:]:
            if coord[1] != prevCommand:
                self.spiralTraversal.append(coord)
            prevCommand = coord[1]
        return self.spiralTraversal

    def saveSpiralCSV(self, filename):
        with open(filename, 'wb') as f:
            writer = csv.writer(f)
            writer.writerows(self.spiralTraversal)

    def savePolarCSV(self, filename):
        with open(filename, 'wb') as f:
            writer = csv.writer(f)
            writer.writerows(self.polarTraversal)

def combineLists( l1, l2, l3=[]):
    return sorted(l1+l2+l3)


def saveCSV(size, l, filename):
    with open(filename, 'wb') as f:
        writer = csv.writer(f)
        writer.writerows([size])
        writer.writerows(l)


if __name__=="__main__":
    puppy = spiral.Spiral('images/puppy.jpg')
    puppy.binary4_split()
    totalRotations = 200
    stepsPerRotation = 200
    blackConverter = PolarImageConverter(puppy.black)
    blackConverter.cropImage()
    blackConverter.constructCartesianList()
    blackConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 0)
    black = blackConverter.constructSpiralTraversalDirections()
    darkgreyConverter = PolarImageConverter(puppy.darkgrey)
    darkgreyConverter.cropImage()
    darkgreyConverter.constructCartesianList()
    darkgreyConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 1)
    darkgrey = darkgreyConverter.constructSpiralTraversalDirections()
    lightgreyConverter = PolarImageConverter(puppy.lightgrey)
    lightgreyConverter.cropImage()
    lightgreyConverter.constructCartesianList()
    lightgreyConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 2)
    lightgrey = lightgreyConverter.constructSpiralTraversalDirections()
    saveCSV([stepsPerRotation,100.0/totalRotations],combineLists(black, darkgrey, lightgrey), 'puppy3way.csv')
