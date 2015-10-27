import numpy as np
import cv2
import matplotlib.pyplot as plt
import spiral
import csv

# TODO: Potentially remove precision from the coordinates so that arduino doesn't overflow?

def cart2pol(x,y):
    r = np.sqrt(x**2 + y**2)
    theta = np.arctan2(y,x)
    return (r,theta)

class PolarImageConverter(object):

    def __init__(self, imageArray):
        imageArray[imageArray == 255] = 1
        self.imageArray = imageArray
        self.imageSize = imageArray.shape
        self.origin = None
        self.cartesianList = []
        self.polarList = []

    def cropImage(self):
        #delete odd sized columns and rows
        if self.imageSize[0] % 2 == 0:
            self.imageArray = np.delete(self.imageArray, 0, axis=0)
        if self.imageSize[1] % 2 == 0:
            self.imageArray = np.delete(self.imageArray, 0, axis=1)
        self.imageSize = self.imageArray.shape


    def constructCartesianList(self):
        self.cartesianList = []
        for (x,y),value in np.ndenumerate(self.imageArray):
            l = [x-self.imageSize[0]/2,y-self.imageSize[1]/2,value]
            self.cartesianList.append(l)

    def constructPolarFromCartesian(self):
        self.polarList = []
        for x,y,value in self.cartesianList:
            polar = cart2pol(x,y)
            l = [polar[0],polar[1],value]
            self.polarList.append(l)
        self.polarList.sort()
        for i in range(len(self.polarList)):
            l = self.polarList[i]
            r = np.around(l[0], decimals=1)
            t = np.around(l[1], decimals=1)
            v = np.around(l[2], decimals=0)
            tmp = [r,t,v]
            self.polarList[i] = tmp


    def savePolarCSV(self, filename):
        with open(filename, 'wb') as f:
            writer = csv.writer(f)
            writer.writerows(self.polarList)

if __name__=="__main__":
    puppy = spiral.Spiral('images/puppy.jpg')
    puppy.binary4_split()
    converter = PolarImageConverter(puppy.black)
    converter.cropImage()
    converter.constructCartesianList()
    converter.constructPolarFromCartesian()
    converter.savePolarCSV('puppy.csv')
