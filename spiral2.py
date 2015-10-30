import numpy as np
import cv2
import matplotlib.pyplot as plt
import spiral
import csv

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

    def savePolarCSV(self, filename):
        with open(filename, 'wb') as f:
            writer = csv.writer(f)
            writer.writerows(self.polarTraversal)

if __name__=="__main__":
    puppy = spiral.Spiral('images/puppy.jpg')
    puppy.binary4_split()
    converter = PolarImageConverter(puppy.black)
    converter.cropImage()
    converter.constructCartesianList()
    converter.constructPolarFromCartesian()
    converter.constructTraversalDirections()
    converter.savePolarCSV('puppypolartraversal.csv')
