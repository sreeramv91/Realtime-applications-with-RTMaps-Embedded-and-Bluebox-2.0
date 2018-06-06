# ---------------- SAMPLE -------------------
# This sample draws a cursor on the center of an image

import rtmaps.types
import numpy as np
import sys
import cv2
import os
import imutils
import time
from imutils.object_detection import non_max_suppression
from rtmaps.base_component import BaseComponent
import copy
#import tensorflow as tf
#import tflearn

# Python class that will be called from RTMaps.
class rtmaps_python(BaseComponent):
    def __init__(self):
        BaseComponent.__init__(self)
        # call base class constructor
        #Define Inputs and Outputs in RTMaps
        self.add_input("in", rtmaps.types.ANY)
        self.add_output("out", rtmaps.types.IPL_IMAGE) 

# Birth() will be called once at diagram execution startup
    def Birth(self):
        pass

# Core() is called every time you have a new input
    def Core(self):
        out = self.inputs["in"].ioelt.data
        #Initialize HOG descriptor/person detector
        hog = cv2.HOGDescriptor()
        hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())   
        #Resize the image to reduce the time it takes to do detection
        #improve detection time
        out.image_data = imutils.resize(out.image_data, width=min(400, out.image_data.shape[1]))
        
        orig = out.image_data.copy()
        
        #detect people in images
        (rects, weights) = hog.detectMultiScale(out.image_data, winStride=(4, 4),
                                            padding=(8, 8), scale=1.05)
        for(x,y,w,h) in rects:
             cv2.rectangle(orig, (x, y), (x + w, y + h), (0, 0, 255), 2)
        
        #Apply non-maxima suppression to the bounding boxes
        rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
        pick = non_max_suppression(rects, probs=None, overlapThresh=0.65)
        
        #Draw the final bounding boxes
        for (xA, yA, xB, yB) in pick:
             cv2.rectangle(out.image_data, (xA, yA), (xB, yB), (0, 255, 0), 2)   
        
        #Display Outputs in console
        filename = "testing"
        print("[INFO] {}: {} original boxes, {} after suppression".format(filename, len(rects), len(pick)))     
        
        print(len(pick)) 
        
        print(out.image_data.shape)
        
        print("The number of pedestrians detected in this image is {} ".format (len(pick)))
                                                    
        # Then we write it on the output
        self.outputs["out"].write(out)


# Death() will be called once at diagram execution shutdown
    def Death(self):
        pass
