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
#define COUNTSPERFOOT 4161

                                    //These will be given by TA's
#define DESIREDFEET 1               //This will be TA assigned for task 2, 1ft for task 3
#define DESIREDANGLE 90            //This will be 0 for task 2, TA assigned for task 3

//Global variable declarations
int positionCommandLeft = 0;
int positionCommandRight = 0;
int errorLeft = 0;
int errorRight = 0;
int startingCountsLeft = 0;
int startingCountsRight = 0;
int straightDriveLevel = 130;
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
  //turn(DESIREDANGLE);
  drive(DESIREDFEET);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void turn(int angle){
  startingCountsLeft = leftEnc.read();
  startingCountsRight = rightEnc.read();
  if(angle == 0){
    return;
  }
  else if(angle > 0){
    positionCommandLeft = angle*COUNTSPERDEGREE;    //
    positionCommandRight = angle*COUNTSPERDEGREE;   //
    digitalWrite(MOTORDIRECTIONLEFT, LOW);
    digitalWrite(MOTORDIRECTIONRIGHT, LOW);
    analogWrite(MOTORPOWERLEFT, 130);
    analogWrite(MOTORPOWERRIGHT, 130);
  }
  else if(angle < 0){
    positionCommandLeft = angle*COUNTSPERDEGREE;
    positionCommandRight = angle*COUNTSPERDEGREE;
    digitalWrite(MOTORDIRECTIONLEFT, HIGH);
    digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
    analogWrite(MOTORPOWERLEFT, 130);
    analogWrite(MOTORPOWERRIGHT, 130);
  }
  errorLeft = positionCommandLeft-startingCountsLeft;
  errorRight = positionCommandRight-startingCountsRight;
  while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){
    Serial.print(errorLeft);
    Serial.print("  ");
    Serial.println(errorRight);
    if((abs(errorLeft)<50) || (abs(errorLeft)<50)){
      digitalWrite(MOTORPOWERLEFT, 0);
      digitalWrite(MOTORPOWERRIGHT, 0);
    }
//    if(abs(errorLeft)<50){
//      digitalWrite(MOTORPOWERLEFT, 0);
//    }
//    if(abs(errorRight)<50){
//      digitalWrite(MOTORPOWERRIGHT, 0);
//    }
    //if(angle > 0){
      errorLeft = positionCommandLeft+leftEnc.read();
      errorRight = positionCommandRight+rightEnc.read();
    //}
    //else if(angle < 0){
    //  errorLeft = positionCommandLeft-leftEnc.read();
    //  errorRight = positionCommandRight-rightEnc.read();
    //}
  }
  Serial.println("Completed turn");
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void drive (int distance){
//  positionCommandLeft = 0;
//positionCommandRight = 0;
//errorLeft = 0;
//errorRight = 0;
//startingCountsLeft = 0;
//startingCountsRight = 0;
  Serial.println("driving");
  startingCountsLeft = leftEnc.read();
  startingCountsRight = rightEnc.read();
  positionCommandLeft = DESIREDFEET*COUNTSPERFOOT;
  positionCommandRight = DESIREDFEET*COUNTSPERFOOT;
  digitalWrite(MOTORDIRECTIONLEFT, LOW);
  digitalWrite(MOTORDIRECTIONRIGHT, HIGH);
  analogWrite(MOTORPOWERLEFT, 110);
  analogWrite(MOTORPOWERRIGHT, 110);
   errorLeft = (positionCommandLeft)-(leftEnc.read() + startingCountsLeft);
   errorRight = (positionCommandRight)-(rightEnc.read() - startingCountsRight);
  while(abs(errorLeft) > 25 || abs(errorRight) > 25 ){
    Serial.println(straightDriveLevel);
    analogWrite(MOTORPOWERLEFT, straightDriveLevel);
    analogWrite(MOTORPOWERRIGHT, straightDriveLevel);
    if((abs(leftEnc.read())-abs(startingCountsLeft)) < 1040){
      straightDriveLevel = constrain(((abs(leftEnc.read())/1040)*255), 110, 255);
    Serial.println(" first if ");
    }
    if(((positionCommandLeft)-3121) < abs(leftEnc.read())){
      straightDriveLevel = constrain((((abs(leftEnc.read())-(DESIREDFEET*4161))/3121)*255), 0, 255);
        Serial.println(" second if ");
    }
    errorLeft = (DESIREDFEET*COUNTSPERFOOT)-(leftEnc.read()-startingCountsLeft);
    errorRight = (DESIREDFEET*COUNTSPERFOOT)-(rightEnc.read()-startingCountsRight);
    Serial.print(errorLeft);
    Serial.print("  ");
    Serial.println(errorRight);
  }  
  Serial.println("done driving");
}
