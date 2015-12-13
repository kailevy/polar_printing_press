import numpy as np
import cv2
import matplotlib.pyplot as plt
from binarize import Binarize
import csv
import math, serial, time, sys

port = 'COM16'
frequency = 9600

def cart2pol(x,y):
    """
    Converts cartesian x y to polar r theta
    """
    r = np.sqrt(x**2 + y**2)
    theta = np.arctan2(y,x) + np.pi
    return (r,theta)

class PolarImageConverter(object):
    """
    This object takes a binarized numpy image array and deconstructs
    it into spiral commands in the form of [angle, command, marker]
    """
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
        """
        Crops irregularly sized images to be square, and have an odd number
        or rows and columns
        """
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
        """
        Creates an array of x,y coordinates from the original image array
        """
        self.cartesianList = []
        for (x,y),value in np.ndenumerate(self.imageArray):
            l = [x-self.imageSize[0]/2,y-self.imageSize[1]/2,value]
            self.cartesianList.append(l)

    def constructPolarFromCartesian(self):
        """
        Converts the x,y cartesian array to an r, theta polar array, where r
        is between 0-100
        """
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

    def constructSpiralCommands(self, totalRotations, stepsPerRotation, marker):
        """
        Creates spiral traversal commands, where the drawing goes in an archimedian spiral
        determined by totalRotations and stepsPerRotation, and notates marker down and
        ups in the form of [angle, command, marker]
        """
        self.spiralList = [[100.0/totalRotations]]
        steps = 0.0
        angle = 0
        radius = 0

        for i in range(totalRotations*stepsPerRotation):
            # constructs all of the angle, rotation, and radius variables for this step
            steps += 1.0
            rotations = steps / stepsPerRotation
            angle = (360.0) * steps / stepsPerRotation
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
        """
        Writes the spiral traversal direction to a CSV file
        """
        with open(filename, 'wb') as f:
            writer = csv.writer(f)
            writer.writerows(self.spiralTraversal)

def combineLists(l1, l2=[], l3=[]):
    """
    Combines up to 3 lists, sorted by the first element of each
    """
    return sorted(l1+l2+l3)


def saveCSV(l, filename):
    """
    Final output like:
    line 1: steps per rotation, radius size per rotation
    rest: theta (degrees), up or down (0 or 1), which marker (0-2)
    """
    with open(filename, 'wb') as f:
        writer = csv.writer(f)
        writer.writerows(l)

def sendSerial(l, send_step=10):
    """
    Method to send commands to the arduino across serial, in set increments,
    so that the controller doesn't run out of memory
    """
    ser = serial.Serial(port, frequency, timeout=2)
    tot_num = len(l)
    time.sleep(2)
    ser.readline()
    send = False
    # Send first command, which is for setting up the drawing
    send_string = ",".join("{0}".format(n) for n in l[0])
    ser.write(send_string+';')
    num_sent = 1

    while True:
        if num_sent >= tot_num:
            # We've finished commands, we are done
            ser.write("done;")
            print("done")
            break

        if ser.inWaiting():
            send = False
            reading = ser.readline()

            if (reading):
                print(reading.strip())

            if "a" in reading:
                # Arduino is ready for more commands
                send = True

        if send and num_sent < tot_num:
            for command in l[num_sent:num_sent+send_step]:
                send_string = ",".join("{0}".format(n) for n in command)
                print send_string
                ser.write(send_string+';')
            num_sent += send_step
            print 'sent'
            ser.write("d"+';') # Character to tell arduino we're done sending commands
            send = False


if __name__=="__main__":
    image_path = sys.argv[1]
    image = Binarize(image_path)
    image.binary_split()
    totalRotations = 160
    stepsPerRotation = 360
    blackConverter = PolarImageConverter(image.black)
    blackConverter.cropImage()
    blackConverter.constructCartesianList()
    blackConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 0)
    black = blackConverter.constructSpiralTraversalDirections()
    # darkgreyConverter = PolarImageConverter(image.darkgrey)
    # darkgreyConverter.cropImage()
    # darkgreyConverter.constructCartesianList()
    # darkgreyConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 1)
    # darkgrey = darkgreyConverter.constructSpiralTraversalDirections()
    # lightgreyConverter = PolarImageConverter(image.lightgrey)
    # lightgreyConverter.cropImage()
    # lightgreyConverter.constructCartesianList()
    # lightgreyConverter.constructSpiralCommands(totalRotations, stepsPerRotation, 2)
    # lightgrey = lightgreyConverter.constructSpiralTraversalDirections()
    directionsList = [[totalRotations]]+combineLists(black)
    print("Starting serial")
    sendSerial(directionsList, 50)
