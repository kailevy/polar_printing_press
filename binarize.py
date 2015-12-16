import numpy as np
import cv2
import matplotlib.pyplot as plt

IMG_NAME = 'poehler'

class Binarize(object):

    def __init__(self, image):
        """
        Takes in file path to image
        """
        self.image = cv2.imread(image,0) # grey-scaled
        self.im2 = cv2.imread(image) #non grey-scaled
        self.black = None
        self.darkgrey = None
        self.lightgrey = None
        self.white = None

    def binary4(self):
        """
        Splits image into 4 binarized images with different light thresholds and displays them together
        """
        black = cv2.inRange(self.image, 0, 40)
        darkgrey = cv2.inRange(self.image, 41, 131)
        lightgrey = cv2.inRange(self.image, 132, 220)
        white = cv2.inRange(self.image, 221, 255)
        black[black == 255] = 1
        darkgrey[darkgrey == 255] = 65
        lightgrey[lightgrey == 255] = 196
        tmp = np.add(black,darkgrey)
        tmp = np.add(tmp,lightgrey)
        tmp[tmp == 0] = 255
        cv2.imwrite('images/binarized/'+IMG_NAME+'/3shades.png', tmp)

    def binary_split(self):
        """
        Binarizes an image, with adaptive thresholds based on Otsu's method
        """
        thresh, self.black = cv2.threshold(self.image, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        self.black = cv2.inRange(self.image,0,thresh)

    def binary4_split(self):
        """
        Splits image into 4 binarized images with different light thresholds and displays them separate
        """
        self.black = cv2.inRange(self.image, 0, 40)
        self.darkgrey = cv2.inRange(self.image, 41, 131)
        self.lightgrey = cv2.inRange(self.image, 132, 220)
        self.white = cv2.inRange(self.image, 221, 255)

    def display_image(self):
        """
        Displays a given image
        """
        cv2.imshow("image", self.image)
        cv2.waitKey(0)

    def display_black(self):
        """
        Displays black
        """
        cv2.imshow("image", self.black)
        cv2.waitKey(0)

if __name__ == '__main__':
    im = Binarize('images/Square.png')
    im.binary_split()
    im.display_black()
