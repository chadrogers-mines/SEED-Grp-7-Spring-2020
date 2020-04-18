from picamera.array import PiRGBArray
from picamera import PiCamera
import cv2, PIL, time
import cv2.aruco as aruco #Without this package, detecting the aruco markers would be impossible
import numpy as np
import time
import board
import smbus
import busio
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

bus = smbus.SMBus(1)

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 2

# Initialise I2C bus.
i2c = busio.I2C(board.SCL, board.SDA)

# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)

lcd.clear()
# Set LCD color to red
lcd.color = [0, 0,100]

# This is the address we setup in the Arduino Program
address = 0x04

#The following section automatically calibrates the camera...
frame_width = 1080
frame_height = 860
camera = PiCamera() #Initiated the camera
camera.resolution = (frame_width, frame_height) #Sets the resolution
camera.iso = 800 #Sets the iso so it can see in low light (400-800)
time.sleep(1) #Allows the camera to warm up
red_blue = camera.awb_gains
f = 1073.45 #focal length of camera, the average of 3, 4, 5, & 7 foot distances and calculating.


camera.awb_mode = 'off' #Stops the camera from adjusting the awb mode so we get consistent images from here on out
camera.awb_gains = red_blue
#End of camera calibration.

dictionary = aruco.getPredefinedDictionary(aruco.DICT_5X5_100) #Sets the list of possible aruco markers to check for
parameters = aruco.DetectorParameters_create() #Creates a parameters object for the proper identification of the potential markers
#cv2.namedWindow("Image with Marker Identified", cv2.WINDOW_NORMAL) 


def find_distance(): #takes a picture and returns the distance away from the camera an aruco marker is
    global frame_width, frame_height
    camera.capture("working_img.jpg") #Captures and saves the image to the working directory under the user given file name
    img = cv2.imread("working_img.jpg", 1) #Reads in the image that was just saved
      
    grayTestImage = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #Converts the input image to gray for aruco marker identification purposes
        
    corners, ids, rejectedImgPoints = aruco.detectMarkers( grayTestImage, dictionary, parameters = parameters) #This magical function finds all markers in the frame and fills the associated arrays with all the pertinent data
    
    detected_img = aruco.drawDetectedMarkers(img, corners) #This image is simply a color version of the input image with the outlines of the markers drawn on top.   
    #cv2.imshow("Image with Markers Identified", detected_img)
    
    if cv2.waitKey(20) & 0xFF == 27: #cv2.imshow only works with a waitkey called somewhere after it, so calling it in an if works to show the image without actually waiting.
        print("Something is broken")
        
    if(len(corners) > 0):
        #print(corners)
        y_top = (corners[0][0][2][1] + corners[0][0][3][1] ) / 2
        y_bottom = (corners[0][0][0][1] + corners[0][0][1][1] ) / 2
        
        height = (y_top - y_bottom) # in pixels
        
        
        distance_away = f * ( 2.75 / height )
        
        return(distance_away)
        
    else:

        print("No Markers Detected")
         
def find_aruco_x_center(corners): # A helper function for xpixel_to_angle()
    global frame_x_middle
    x_middle = 0
    for i in range(0,4):
        x_middle = corners[0][0][i][0] + x_middle       
    x_middle = x_middle / 4
        
    return (x_middle)
           
def xpixel_to_angle(xpixel_val): #this is a helper function for find_aruco_angle()
    global frame_width
   
    angle = -1 * ( (xpixel_val * (53.00 / frame_width)- 27.00 ) )
    
    if(angle < 0):
        angle = angle * 1.000
    if(angle > 0):
        angle = angle * 1.090

        
    return angle
           
def find_aruco_angle(): #takes a picture and returns the angle from center of an aruco marker that is in the image.
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

# Use find_aruco_angle() to find the angle from center an aruco marker is, returns a float
# Use find_distance() to find the distance of an aruco marker from the camera, returns a float.
print(find_aruco_angle())
print(find_distance())



while True:
    angle = find_aruco_angle()
    distance = find_distance()
    lcd.clear()
    lcdstring = str(angle) + "\n" + str(distance)
    lcd.message = lcdstring
    
    #holdOn = bus.read_byte_data(address, 0)
        
    #if holdOn == 0:    
    if angle == 69:
        lcd.color=[100,0,0]
        angle = 45
        bus.write_byte_data(address, 0, int(angle))  #pivot 45 degrees
            
    elif angle >= 2:
        lcd.color=[100,100,0]
        bus.write_byte_data(address, 0, int(angle))  #pivot angle
            
    else:
        if distance > 8:
            bus.write_byte_data(address, 0, (int(distance) + 90))#travel distance
                
        else:
            lcd.color=[0,100,0]
            #box turn
            break
    time.sleep(20)
