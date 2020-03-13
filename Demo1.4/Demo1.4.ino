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
Encoder leftEnc(2, 4);
Encoder rightEnc(3, 5);

//Global costant declarations
#define MOTORPOWERLEFT 10
#define MOTORPOWERRIGHT 9
#define MOTORDIRECTIONLEFT 8
#define MOTORDIRECTIONRIGHT 7
#define MOTORGLOBALENABLE 4
#define ENCL1 2
#define ENCL2 4
#define ENCR1 3
#define ENCR2 5
#define CIRCUMFERENCE 0.769         //Wheel circumference in feet
#define COUNTSPERDEGREE 10.3       //originally 23.72
#define COUNTSPERFOOT 1161        // was 4161
#define FEETPERDEGREE 0.0057

                                    //These will be given by TA's
#define DESIREDFEET 1               //This will be TA assigned for task 2, 1ft for task 3
#define DESIREDANGLE 90            //This will be 0 for task 2, TA assigned for task 3

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
float Kp = (0.4);
float KpDrive = (0.07);
float Ka = (0.1); // angular velocity proportional coefficient
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);               //Start serial debug
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
   
  drive(DESIREDFEET);
 delay(1000);
  return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//void turn(int angle){
//  startingCountsLeft = leftEnc.read();
//  startingCountsRight = rightEnc.read();
//  if(angle == 0){
//    return;
//  }
//  else if(angle > 0){
//    positionCommandLeft = angle*COUNTSPERDEGREE;    //
//    positionCommandRight = angle*COUNTSPERDEGREE;   //
//    digitalWrite(MOTORDIRECTIONLEFT, LOW);
//    digitalWrite(MOTORDIRECTIONRIGHT, LOW);
//    analogWrite(MOTORPOWERLEFT, 130);
//    analogWrite(MOTORPOWERRIGHT, 130);
//  }
//  else if(angle < 0){
//    positionCommandLeft = angle*COUNTSPERDEGREE;
//    positionCommandRight = angle*COUNTSPERDEGREE;
//    digitalWrite(MOTORDIRECTIONLEFT, HIGH);
//    digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
//    analogWrite(MOTORPOWERLEFT, 130);
//    analogWrite(MOTORPOWERRIGHT, 130);
//  }
//  errorLeft = positionCommandLeft-startingCountsLeft;
//  errorRight = positionCommandRight-startingCountsRight;
//  while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){
//    Serial.print(errorLeft);
//    Serial.print("  ");
//    Serial.println(errorRight);
//    if((abs(errorLeft)<50) || (abs(errorLeft)<50)){
//      digitalWrite(MOTORPOWERLEFT, 0);
//      digitalWrite(MOTORPOWERRIGHT, 0);
//      return;
//    }
////    if(abs(errorLeft)<50){
////      digitalWrite(MOTORPOWERLEFT, 0);
////    }
////    if(abs(errorRight)<50){
////      digitalWrite(MOTORPOWERRIGHT, 0);
////    }
//    //if(angle > 0){
//      errorLeft = positionCommandLeft+leftEnc.read();
//      errorRight = positionCommandRight+rightEnc.read();
//    //}
//    //else if(angle < 0){
//    //  errorLeft = positionCommandLeft-leftEnc.read();
//    //  errorRight = positionCommandRight-rightEnc.read();
//    //}
//  }
//  Serial.println("Completed turn");
//}
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

  Serial.println("turning");
  startingCountsLeft = leftEnc.read();
  startingCountsRight = rightEnc.read();
  positionCommandLeft = startingCountsLeft - angle*COUNTSPERDEGREE;
  positionCommandRight = startingCountsRight - angle*COUNTSPERDEGREE;
  errorLeft = positionCommandLeft - leftEnc.read(); 
  errorRight = positionCommandRight - rightEnc.read();
  Serial.print("errorRight  ");
  Serial.println(errorRight);

  

  while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){  

velocityLeft = leftEnc.read() - encLeftPrev;
velocityRight = rightEnc.read() - encRightPrev;
angularVelocity = velocityLeft - velocityRight;
Serial.print("ang vel  ");
Serial.println(angularVelocity*Ka);

  
integralLeft = integralLeft + timeLoop * errorLeft;
integralRight = integralRight + timeLoop * errorRight;

outputLeft = constrain((abs( errorLeft * Kp + integralLeft * Ki + Ka*angularVelocity)), 0, 210);
outputRight = constrain((abs(errorRight * Kp + integralRight * Ki - Ka*angularVelocity)), 0, 210);
//Serial.print("integral rRight  ");
//Serial.println(integralRight*Ki);

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
}
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

  Serial.println("driving");
  startingCountsLeft = leftEnc.read();
//  startingCountsRight = rightEnc.read();
  positionCommandLeft = startingCountsLeft - DESIREDFEET*COUNTSPERFOOT;
//  positionCommandRight = startingCountsRight + DESIREDFEET*COUNTSPERFOOT;
  errorLeft = positionCommandLeft - leftEnc.read(); 
//  errorRight = positionCommandRight - rightEnc.read();
  
  

  while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){  

velocityLeft = leftEnc.read() - encLeftPrev;
velocityRight = rightEnc.read() - encRightPrev;
angularVelocity = velocityLeft - velocityRight;
Serial.print("ang vel  ");
Serial.println(angularVelocity*Ka);

  
integralLeft = integralLeft + timeLoop * errorLeft;
//integralRight = integralRight + timeLoop * errorRight;

outputLeft = constrain(( errorLeft * KpDrive + integralLeft * Ki - Ka*angularVelocity ), -255, 255);
outputRight = constrain((-outputLeft + Ka*angularVelocity), -255, 255);
Serial.print("integral rRight  ");
Serial.println(integralRight*Ki);

timeLoop = (millis() - startTime)/1000;
startTime = millis();


  if(outputRight > 0){
      digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
  }
  else {
          digitalWrite(MOTORDIRECTIONRIGHT, LOW);
  }
  
   if(outputLeft > 0){
      digitalWrite(MOTORDIRECTIONLEFT, HIGH);
  }
  else {
          digitalWrite(MOTORDIRECTIONLEFT, LOW);
  }
  analogWrite(MOTORPOWERRIGHT, abs(outputRight));
  analogWrite(MOTORPOWERLEFT, abs(outputLeft));
Serial.print("outputLEFT  ");
  Serial.print(outputLeft);
  Serial.print("outputRight  ");
  Serial.println(outputRight);

  
  errorLeft = positionCommandLeft - leftEnc.read(); 
  errorRight = positionCommandRight - rightEnc.read();
  Serial.print("errorLeft  ");
  Serial.println(errorLeft);

  if(i%20 ==0){
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
}



//Serial.println(straightDriveLevel);
//    analogWrite(MOTORPOWERLEFT, straightDriveLevel);
//    analogWrite(MOTORPOWERRIGHT, straightDriveLevel);
//    if((abs(leftEnc.read())-abs(startingCountsLeft)) < 1040){
//      straightDriveLevel = constrain(((abs(leftEnc.read())/1040)*255), 110, 255);
//    Serial.println(" first if ");
//    }
//    if(((positionCommandLeft)-3121) < abs(leftEnc.read())){
//      straightDriveLevel = constrain((((abs(leftEnc.read())-(DESIREDFEET*4161))/3121)*255), 0, 255);
//        Serial.println(" second if ");
//    }
//    errorLeft = (DESIREDFEET*COUNTSPERFOOT)-(leftEnc.read()-startingCountsLeft);
//    errorRight = (DESIREDFEET*COUNTSPERFOOT)-(rightEnc.read()-startingCountsRight);
//    Serial.print(errorLeft);
//    Serial.print("  ");
//    Serial.println(errorRight);
//  } 
