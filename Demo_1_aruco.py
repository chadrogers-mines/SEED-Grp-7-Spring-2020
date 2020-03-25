from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2, PIL, time
import cv2.aruco as aruco #Without this package, detecting the aruco markers would be impossible
import numpy as np
import board
import busio
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd
import smbus
import time

#i2c setup
bus = smbus.SMBus(1)
i2c = busio.I2C(board.SCL, board.SDA)
lcd_columns = 16
lcd_rows = 2
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)
lcd.clear()
lcd.color = [100, 0, 0]

# This is the address we setup in the Arduino Program
address = 0x04

#The following section automatically calibrates the camera...
frame_width = 1080
frame_height = 860
frame_x_middle = frame_width / 2
frame_y_middle = frame_height / 2
camera = PiCamera() #Initiated the camera
camera.resolution = (frame_width, frame_height) #Sets the resolution
camera.iso = 800 #Sets the iso so it can see in low light (400-800)
time.sleep(1) #Allows the camera to warm up
red_blue = camera.awb_gains

#print(red_blue)

camera.awb_mode = 'off' #Stops the camera from adjusting the awb mode so we get consistent images from here on out
camera.awb_gains = red_blue
#End of camera calibration.

dictionary = aruco.getPredefinedDictionary(aruco.DICT_5X5_100) #Sets the list of possible aruco markers to check for
parameters = aruco.DetectorParameters_create() #Creates a parameters object for the proper identification of the potential markers



def find_aruco_quad():
    global frame_width, frame_height
    camera.capture("working_img.jpg") #Captures and saves the image to the working directory under the user given file name
    img = cv2.imread("working_img.jpg", 1) #Reads in the image that was just saved
      
    grayTestImage = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #Converts the input image to gray for aruco marker identification purposes
        
    corners, ids, rejectedImgPoints = aruco.detectMarkers( grayTestImage, dictionary, parameters = parameters) #This magical function finds all markers in the frame and fills the associated arrays with all the pertinent data
    
    detected_img = aruco.drawDetectedMarkers(img, corners) #This image is simply a color version of the input image with the outlines of the markers drawn on top.   
    cv2.namedWindow("Image with Markers Identified", cv2.WINDOW_NORMAL) 
    cv2.imshow("Image with Markers Identified", detected_img)
    
    if cv2.waitKey(20) & 0xFF == 27: #cv2.imshow only works with a waitkey called somewhere after it, so calling it in an if works to show the image without actually waiting.
        print("Something is broken")
        
#    if len(corners) != 0: #If there are markers detected, their id numbers are displayed
#        print("Id numbers detected: ")
#        print(ids)
#    else:  #If no markers are detected, then the below statement is printed
#        print("No markers detected.")
        
    
        
    if(len(corners) > 0):
        #print(corners)
        x_middle = 0
        y_middle = 0
        for i in range(0,4):
            x_middle = corners[0][0][i][0] + x_middle
            #print(corners[0][0][i])
            y_middle = corners[0][0][i][1] + y_middle
                
        x_middle = x_middle / 4
        y_middle = y_middle / 4
#            print("X middle", x_middle)
#            print("Y middle %d", y_middle)
        frame_x_middle = frame_width / 2
        frame_y_middle = frame_height / 2
        if (x_middle > frame_x_middle): #This condition checks if the center of the aruco marker is in the left half of the frame
            if(y_middle < frame_y_middle): #Checks if the center is in the upper half of the frame
                return 0
            if(y_middle > frame_y_middle):
                return 3
        if (x_middle < frame_x_middle):
            if(y_middle < frame_y_middle):
                return 1
            if(y_middle > frame_y_middle):
                return 2
    else:
        return 4
    
def find_aruco_x_center(corners):
    global frame_x_middle
    x_middle = 0
    for i in range(0,4):
        x_middle = corners[0][0][i][0] + x_middle       
    x_middle = x_middle / 4
        
    return (x_middle)
    
def xpixel_to_angle(xpixel_val):
    global frame_width
    
    
        
    angle = -1 * ( (xpixel_val * (53.00 / frame_width)- 27.00 ) )
    
    if(angle < 0):
        angle = angle * 1.000
    if(angle > 0):
        angle = angle * 1.090

        
    return angle
    
def find_aruco_angle():
    camera.capture("working_img.jpg") #Captures and saves the image to the working directory under the user given file name
    img = cv2.imread("working_img.jpg", 1) #Reads in the image that was just saved  
    grayTestImage = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #Converts the input image to gray for aruco marker identification purposes
    corners, ids, rejectedImgPoints = aruco.detectMarkers( grayTestImage, dictionary, parameters = parameters) #This magical function finds all markers in the frame and fills the associated arrays with all the pertinent data
        
    if(len(corners) > 0):
        x_center = find_aruco_x_center(corners)
        return (xpixel_to_angle(x_center))
    else:
        return 69
            
            
#Start main code here###

while(True):
    
    angle_found = find_aruco_angle()
    print("Angle Found: " + str(angle_found) )
    if(angle_found == 69):
        lcd.color = [100, 0, 0]   #red for no angle
        lcd.message = ("No angle\ndetected")
    elif(angle_found <= 0):
        lcd.color = [0, 0, 100]   #green for negative angles
        lcd.message = ("Angle = " + str(angle_found) )
    else:
        lcd.color = [0, 100, 0]   #blue for positive angles
        lcd.message = ("Angle = " + str(angle_found) )
        
    


