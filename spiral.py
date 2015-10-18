import numpy as np
import cv2
import matplotlib.pyplot as plt

class Spiral(object):

    def __init__(self, image):
        """
        Takes in file path to image
        """
        self.image = cv2.imread(image,0) # grey-scaled
        self.im2 = cv2.imread(image) #non grey-scaled
        self.window = cv2.namedWindow("image")
        self.thresh, self.image_bw = cv2.threshold(self.image, 128, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)

    def binary4(self):
        """
        Splits image into 4 binarized images with different light thresholds and displays them together
        """
        black = cv2.inRange(self.image, 0, 30)
        darkgrey = cv2.inRange(self.image, 31, 91)
        midgrey = cv2.inRange(self.image, 92, 152)
        lightgrey = cv2.inRange(self.image, 153, 220)
        white = cv2.inRange(self.image, 221, 255)
        black[black == 255] = 1
        darkgrey[darkgrey == 255] = 65
        midgrey[midgrey == 255] = 132
        lightgrey[lightgrey == 255] = 196
        tmp = np.add(black,darkgrey)
        tmp = np.add(tmp,midgrey)
        tmp = np.add(tmp,lightgrey)
        tmp[tmp == 0] = 255
        cv2.imshow("image",tmp)
        cv2.waitKey(0)

    def binary4_split(self):
        """
        Splits image into 4 binarized images with different light thresholds and displays them separate
        """
        black = cv2.inRange(self.image, 0, 30)
        darkgrey = cv2.inRange(self.image, 31, 91)
        midgrey = cv2.inRange(self.image, 92, 152)
        lightgrey = cv2.inRange(self.image, 153, 220)
        white = cv2.inRange(self.image, 221, 255)
        cv2.imshow("draw black", black)
        cv2.imshow("draw dark grey", darkgrey)
        cv2.imshow("draw mid grey", midgrey)
        cv2.imshow("draw light grey", lightgrey)
        cv2.imshow("leave white", white)
        cv2.waitKey(0)


    def display_binary(self):
        cv2.imshow("image", self.image_bw)
        cv2.waitKey(0)

    def display_image(self):
        cv2.imshow("image", self.image)
        cv2.waitKey(0)

if __name__ == '__main__':
    puppy = Spiral('images/puppy.jpg')
    # puppy.binary4()
    # puppy.binary4_split()
    # puppy.display_binary()
