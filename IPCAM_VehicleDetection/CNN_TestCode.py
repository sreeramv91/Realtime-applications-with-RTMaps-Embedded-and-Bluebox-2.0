import rtmaps.types
from rtmaps.base_component import BaseComponent
import tensorflow as tf
import numpy as np
import os
from matplotlib.pyplot import imsave
import matplotlib.image as pyimg
import tflearn
from tflearn.layers.conv import conv_2d, max_pool_2d
from tflearn.layers.core import input_data,dropout, fully_connected
from tflearn.layers.estimator import regression
from PIL import Image

# Python class that will be called from RTMaps.

class rtmaps_python(BaseComponent):

    def __init__(self):
        BaseComponent.__init__(self) # call base class constructor
        self.add_input("in", rtmaps.types.ANY) # define input
        self.add_output("out", rtmaps.types.AUTO) # define output

# Birth() will be called once at diagram execution startup

    def Birth(self):
        print("Python Birth")
        #i =0

       
# Core() is called every time you have a new input

    def Core(self):
        print("Inside core")
        TEST_DIR = 'test2'
        IMG_SIZE = 50
        LR = 1e-4
        MODEL_NAME = 'VehicleConvent'
        #if (i==11):
        i=1
        #i=i+1

#--------------------------------------------------------------------------------#
#------------Convolution layer to be changed to function inside class------------#
#--------------------------------------------------------------------------------#
        tf.reset_default_graph()
        convnet = input_data(shape = [None,IMG_SIZE,IMG_SIZE, 1],name='input')
        convnet = conv_2d(convnet, 32, 5, activation ='relu')
        convnet = max_pool_2d(convnet, 5,5)
        
        convnet = conv_2d(convnet, 64, 5, activation ='relu')
        convnet = max_pool_2d(convnet, 5,5)
        
        convnet = conv_2d(convnet, 128, 5, activation ='relu')
        convnet = max_pool_2d(convnet, 5,5)
        
        convnet = conv_2d(convnet, 64, 5, activation ='relu')
        convnet = max_pool_2d(convnet, 5,5)
        
        convnet = conv_2d(convnet, 32, 5, activation ='relu')
        convnet = max_pool_2d(convnet, 5,5)
        
        convnet = fully_connected(convnet, 1024, activation = 'softmax')
        convnet = dropout(convnet,0.8)
        
        convnet = fully_connected(convnet, 2, activation = 'softmax')
        convnet = regression(convnet, optimizer = 'adam', learning_rate=LR, loss='categorical_crossentropy', name = 'targets')
        model = tflearn.DNN (convnet)
#--------------------------------------------------------------------------------#
#------------------------------loading the model----------------------------------#
#--------------------------------------------------------------------------------#

        model.load("/home/bluebox/.RTMaps-4.0/model_car.tfl")
        print("completed the model")

        def rgb2gray(rgb):
            return np.dot(rgb[...,:3], [0.299, 0.587, 0.114])

        for i in range (1,11):
            path = "/home/bluebox/.RTMaps-4.0/test2/"+str(i)+".jpeg"
            img = pyimg.imread(path)     
            print(path)
            gray = rgb2gray(img) 
            #print(gray)
            imsave("Gray.jpg",gray,cmap="gray")
            test_img = Image.open("Gray.jpg")
            rsize = test_img.resize((IMG_SIZE,IMG_SIZE))
            pix = np.array(rsize)
            data = pix[:,:,1].reshape(IMG_SIZE, IMG_SIZE, 1)
            #print(data)

            model_out = model.predict([data])[0]
            print(model_out)
            if np.argmax(model_out) == 1:
                str_label = 'VehiclePresent'
            else:
                str_label = 'VehicleAbsent'
            print(str_label)

    def Death(self):
        pass
