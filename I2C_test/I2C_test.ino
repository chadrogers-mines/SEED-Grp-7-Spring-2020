// I2C test for SEED Lab EENG350
// Arduno reads motor encoders, commands it to the position chosen by the RPi running openCV
// Pins 9 & 10 are ON/OFF controls. Pins 7 & 8 are direction controls. 
// Encoder pins are attached to pins 2 & 4, 3 & 5, and have interrupts attached to them.

//NOTES: POSITIVE ANGLES ARE CLOCKWISE ROTATION OF THE ROBOT, SEEN FROM ABOVE.
//left encoder read: negative if robot is going forward


//Include necessary libraries
#include <Wire.h>

byte behaviorByte = 0;
byte positionCommand = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void receiveCommand(){
  while(Wire.available() > 0){
    behaviorByte = Wire.read();
    //positionCommand = Wire.read();
    if(behaviorByte == 128){         //If behaviorByte is 1, we are requesting a turn
      digitalWrite(13, HIGH);
    }
    else if (behaviorByte == 129){
      digitalWrite(13, LOW);
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendCommand(){
  //prepare data into a byte to be sent to the pi
  //Wire.write(dataByte);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  //Serial.begin(115200);                  //Start serial debug
  Wire.begin(0x04);                      //Join I2C interface as a aslave with address 0x01
  Wire.setClock(400000);                 //Set I2C interface to correct speed
  Wire.onReceive(receiveCommand);
  Wire.onRequest(sendCommand);
  pinMode(13, OUTPUT);
  
  delay(500);                            //Wait 2 seconds before beginning movement
  //Serial.println("Exiting setup()///////////////////////////////////////////////////////////////////");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // turn(DESIREDANGLE);
  //Serial.println("Escaped turn, ready for drive.");
  //delay(1000);
  //drive(DESIREDFEET);
 
  //exit(1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
