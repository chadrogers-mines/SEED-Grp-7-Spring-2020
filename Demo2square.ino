// Project Demo 1 for SEED Lasb EENG350
// Arduno reads motor encoders, commands it to the position chosen by the RPi running openCV
// Pins 9 & 10 are ON/OFF controls. Pins 7 & 8 are direction controls. 
// Encoder pins are attached to pins 2 & 4, 3 & 5, and have interrupts attached to them.

//NOTES: POSITIVE ANGLES ARE CLOCKWISE ROTATION OF THE ROBOT, SEEN FROM ABOVE.
//left encoder read: negative if robot is going forward


//Include necessary libraries
#include <Wire.h>
#include <Encoder.h>

//Setup encoders
Encoder leftEnc(2, 6);
Encoder rightEnc(3, 5);

//Global costant declarations
#define clockFrequency 400000
#define MOTORPOWERLEFT 10
#define MOTORPOWERRIGHT 9
#define MOTORDIRECTIONLEFT 8
#define MOTORDIRECTIONRIGHT 7
#define MOTORGLOBALENABLE 4
#define ENCL1 2
#define ENCL2 6
#define ENCR1 3
#define ENCR2 5
#define CIRCUMFERENCE 0.769         //Wheel circumference in feet
#define COUNTSPERDEGREE 8.72       //originally 23.72
#define COUNTSPERFOOT 1730        // was 4161
#define FEETPERDEGREE 0.0057

                                    //These will be given by TA's
#define DESIREDFEET 8.4               //This will be TA assigned for task 2, 1ft for task 3
#define DESIREDANGLE 210            //This will be 0 for task 2, TA assigned for task 3

//Global variable declarations
long positionCommandLeft = 0;
long positionCommandRight = 0;
long errorLeft = 0;
long errorRight = 0;
long startingCountsLeft = 0;
long startingCountsRight = 0;
int straightDriveLevel = 130;
float controlLeft = 0;
float controlRight = 0;
float proportionalLeft = 0;
float proportionalRight = 0;
float derivative = 0;
float timeLoop = 0;
float startTime= 0;
float outputLeft = 0;
float outputRight = 0;
long encRightPrev = 0;
long encLeftPrev = 0;


float Ki = (0.01);
float Kp = (0.000001);
float KpDrive = (0.1);
float Ka = (0.0); // angular velocity proportional coefficient
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int holdOn = 0;
volatile byte positionCommand = 0;

void receiveCommand(){
  while(Wire.available() > 0){
    positionCommand = Wire.read(); 
    holdOn = 1;
    Serial.println(positionCommand);
    if(positionCommand <= 90){
      turn(positionCommand);
      holdOn = 0;
    }
    else if(positionCommand > 90){
      positionCommand = positionCommand - 90;
      drive(positionCommand);
      holdOn = 0;
    }
    else if(positionCommand = 0){
      boxTurn(1);
      holdOn = 0;
    }
    else{
      holdOn = 0;
      break;
    }
  }
}

void sendCommand(){
  Wire.write(holdOn);
}


void setup() {
  Serial.begin(115200);               //Start serial debug
    //Begin I2C communication
  Wire.begin(0x04);  //slave adddress 1
  Wire.setClock(clockFrequency);
  Wire.onReceive(receiveCommand);
  Wire.onRequest(sendCommand);
  
  pinMode(MOTORPOWERLEFT, OUTPUT);  //Declare outputs as outputs
  pinMode(MOTORPOWERRIGHT, OUTPUT);
  pinMode(MOTORDIRECTIONLEFT, OUTPUT);
  pinMode(MOTORDIRECTIONRIGHT, OUTPUT);
  // pinMode(MOTORPOWERLEFT, OUTPUT); //duplicate
  pinMode(MOTORGLOBALENABLE, OUTPUT);
  pinMode(ENCL1, INPUT);            //Declare inputs as inputs
  pinMode(ENCL2, INPUT);
  pinMode(ENCR1, INPUT);
  pinMode(ENCR2, INPUT);

  digitalWrite(MOTORGLOBALENABLE, HIGH); //Must be high
  
  delay(500);                      //Wait 2 seconds before beginning movement
  Serial.println("Exiting setup()///////////////////////////////////////////////////////////////////");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // turn(DESIREDANGLE);
  //Serial.println("Escaped turn, ready for drive.");
  //delay(1000);
  //drive(DESIREDFEET);
  //boxTurn(1);
  //exit(1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void turn (int angle){
positionCommandLeft = 0;
positionCommandRight = 0;
errorLeft = 0;
errorRight = 0;
startingCountsLeft = 0;
startingCountsRight = 0;
straightDriveLevel = 130;
controlLeft = 0;
controlRight = 0;
proportionalLeft = 0;
proportionalRight = 0;
derivative = 0;
timeLoop = 0;
startTime= 0;
outputLeft = 0;
outputRight = 0;
float integralLeft = 0;
float integralRight = 0;
int i;
float velocityLeft = 0;
float velocityRight = 0;
float angularVelocity = 0; // positive = clockwise
startTime = millis();

  Serial.println("turning");
  startingCountsLeft = leftEnc.read();
  startingCountsRight = rightEnc.read();
  positionCommandLeft = startingCountsLeft - angle*COUNTSPERDEGREE;
  positionCommandRight = startingCountsRight - angle*COUNTSPERDEGREE;
  errorLeft = positionCommandLeft - leftEnc.read(); 
  errorRight = positionCommandRight - rightEnc.read();
  Serial.print("errorRight  ");
  Serial.println(errorRight);

  

  do{  

//    velocityLeft = leftEnc.read() - encLeftPrev;
//    velocityRight = rightEnc.read() - encRightPrev;
//    angularVelocity = velocityLeft - velocityRight;
//    Serial.print("ang vel  ");
//    Serial.println(angularVelocity*Ka);

  
    integralLeft = integralLeft + timeLoop * errorLeft;
    integralRight = integralRight + timeLoop * errorRight;
    
    outputLeft = constrain((abs( (errorLeft * Kp) + (integralLeft * Ki) + (Ka*angularVelocity))), 0, 90);    //was 210
    outputRight = constrain((abs((errorRight * Kp) + (integralRight * Ki) - (Ka*angularVelocity))), 0, 90);  //was 210
    //Serial.print("integral rRight  ");
    //Serial.println(integralRight*Ki);
    
    timeLoop = (millis() - startTime)/1000;
    //startTime = millis();


  if(errorRight > 0){
      digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
  }
  else {
          digitalWrite(MOTORDIRECTIONRIGHT, LOW);
  }
  
   if(errorLeft > 0){
      digitalWrite(MOTORDIRECTIONLEFT, HIGH);
  }
  else {
          digitalWrite(MOTORDIRECTIONLEFT, LOW);
  }
  analogWrite(MOTORPOWERRIGHT, outputRight);
  analogWrite(MOTORPOWERLEFT, outputLeft);
Serial.print("outputLEFT  ");
  Serial.print(outputLeft);
  Serial.print("outputRight  ");
  Serial.println(outputRight);

  
  errorLeft = positionCommandLeft - leftEnc.read(); 
  errorRight = positionCommandRight - rightEnc.read();
  
  if(i%15 ==0){
  encLeftPrev = leftEnc.read();
  encRightPrev = rightEnc.read();
    }
  i ++;
// Serial.print("errorRight  ");
//  Serial.print(errorRight);
//   Serial.print("errorLEFT  ");
//  Serial.println(errorRight);
  if(millis() > startTime+5000){
    return;
  }
 } while(abs(errorLeft) > 25 || abs(errorRight) > 25 );

  analogWrite(MOTORPOWERRIGHT, 0);
  analogWrite(MOTORPOWERLEFT, 0);
  return;

  Serial.println("done driving");
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drive (int distance){
positionCommandLeft = 0;
 positionCommandRight = 0;
errorLeft = 0;
errorRight = 0;
startingCountsLeft = 0;
startingCountsRight = 0;
straightDriveLevel = 130;
controlLeft = 0;
controlRight = 0;
proportionalLeft = 0;
proportionalRight = 0;
derivative = 0;
timeLoop = 0;
startTime= 0;
outputLeft = 0;
outputRight = 0;
float integralLeft = 0;
float integralRight = 0;
int i;
float velocityLeft = 0;
float velocityRight = 0;
float angularVelocity = 0; // positive = clockwise
//digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
//digitalWrite(MOTORDIRECTIONLEFT, LOW);
//analogWrite(MOTORPOWERRIGHT, 200);
//analogWrite(MOTORPOWERLEFT, 200);
//delay(100);
//analogWrite(MOTORPOWERRIGHT, 0);
//analogWrite(MOTORPOWERLEFT, 0);

  Serial.println("driving");
  startingCountsLeft = leftEnc.read();
  startingCountsRight = rightEnc.read();
  positionCommandLeft = startingCountsLeft - distance*COUNTSPERFOOT;
  positionCommandRight = startingCountsRight + distance*COUNTSPERFOOT;
  errorLeft = positionCommandLeft - leftEnc.read(); 
  errorRight = positionCommandRight - rightEnc.read();
  Serial.print("errorRight  ");
  Serial.println(errorRight);

errorLeft ++;
errorRight ++;
  

 while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){  

    velocityLeft = leftEnc.read() - encLeftPrev;
    velocityRight = rightEnc.read() - encRightPrev;
    angularVelocity = velocityLeft - velocityRight;
  
   
      
    integralLeft = integralLeft + timeLoop * errorLeft;
    integralRight = integralRight + timeLoop * errorRight;
    
    outputLeft = constrain((abs( (errorLeft * Kp) + (integralLeft * Ki) - (Ka*angularVelocity))), 0, 110);          //was 255
    outputRight = constrain((abs((errorRight * Kp) + (integralRight * Ki) + (Ka*angularVelocity))), 0, 110);        //was 255
    Serial.print("error*kp =  ");
    Serial.println(errorLeft * Kp);
//    
    timeLoop = (millis() - startTime)/1000;
    startTime = millis();
    
    
      if(errorRight > 0){
          digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
      }
      else {
              digitalWrite(MOTORDIRECTIONRIGHT, LOW);
      }
      
       if(errorLeft > 0){
          digitalWrite(MOTORDIRECTIONLEFT, HIGH);
      }
      else {
              digitalWrite(MOTORDIRECTIONLEFT, LOW);
      }
      analogWrite(MOTORPOWERRIGHT, outputRight);
      analogWrite(MOTORPOWERLEFT, outputLeft);
      Serial.print("outputLEFT  ");
      Serial.print(outputLeft);
      Serial.print("outputRight  ");
      Serial.println(outputRight);
    
      
      errorLeft = positionCommandLeft - leftEnc.read(); 
      errorRight = positionCommandRight - rightEnc.read();
      
      if(i%15 ==0){
      encLeftPrev = leftEnc.read();
      encRightPrev = rightEnc.read();
        }
      i ++;
// Serial.print("errorRight  ");
//  Serial.print(errorRight);
//   Serial.print("errorLEFT  ");
//  Serial.println(errorRight);
 }

    analogWrite(MOTORPOWERRIGHT, 0);
  analogWrite(MOTORPOWERLEFT, 0);

  Serial.println("done driving");
  return;
}

void boxTurn (bool lOrR){
  // Left is defined as 0, Right is defined as 1
  if(lOrR == 0) {
    turn(-90);
    delay(200);
    drive(1.1);
    delay(200);
    turn(-90);
    delay(200);
    drive(1.1);
  }
  else if(lOrR == 1){
    turn(90);
    delay(200);
    drive(1.2);
    delay(200);
    turn(-90);
    delay(200);
    drive(2.2);
    delay(200);
    turn(-90);
    delay(200);
    drive(2.2);
    delay(200);
    turn(-90);
    delay(200);
    drive(2.2);
    delay(200);
    turn(-90);
    delay(200);
    drive(1.2);    
  }
}
