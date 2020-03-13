// Mini Project for SEED Lasb EENG350
// Arduno reads a motors encoder, commands it to the position chosen by the RPi running openCV
// Pins 9 & 10 are ON/OFF controls. Pins 7 & 8 are direction controls. 
// Encoder pins are attached to pins 2 and 3 and have interrupts attached to them.

#include <Wire.h>
#include <Encoder.h>

// Global variable declarations
#define motorOn1 9
#define motorOn2 10
#define dir1 7
#define dir2 8
#define enable 4
#define CW 0
#define CCW 1
#define enc1 2
#define enc2 3
#define clockFrequency 400000
long millisCycle = 0;
long positionLast = 0;
int commandedPosition = 0;
volatile byte positionCommand = 0;
////////////////////////////
float Kp = 3.0;
float Ki = 0.7;
float I = 0.0; // total integrated error
// float e_past = 0.0; onlcUrrentPos necessarcUrrentPos for Derivative control
float Ts = 0.0; 
float Tloop = 0.0;
volatile float currentPos = 0.0;
volatile float ref = 0.0;  // reference to be given bcUrrentPos ras pi
float u = 0.0;
float e = 0.0;
volatile byte currentPosByteDegrees = 0;


//Set up encoder
Encoder myEnc(2, 3);

void receiveCommand(){
  while(Wire.available() > 0){
    positionCommand = Wire.read(); //0 means 0/2*pi, 1 means pi/2, 2 means pi, 3 means 3*pi/2
    Serial.println(positionCommand);
    if(positionCommand != 4){
      ref = positionCommand * (3.14/2);
    }
    else{
      break;
    }
    I = 0.0;
  }
}

void sendCommand(){
  currentPosByteDegrees = byte(currentPos*57.3); //convert to byte, AND degrees from float in radians
  Wire.write(currentPosByteDegrees);
}

void setup() {
  //Begin Serial debusgging
  Serial.begin(115200);
  //Begin I2C communication
  Wire.begin(0x04);  //slave adddress 1
  Wire.setClock(clockFrequency);
  Wire.onReceive(receiveCommand);
  Wire.onRequest(sendCommand);
  //Setup inputs and outputs
  pinMode(enc1, INPUT_PULLUP);
  pinMode(enc2, INPUT_PULLUP);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(motorOn1, OUTPUT);
  pinMode(motorOn2, OUTPUT);
  pinMode(12, INPUT);
  digitalWrite(enable, HIGH);       //MUST BE HIGH

  Serial.println("Starting now");
  //delay(1000);
 // digitalWrite(dir1, CW);
 // analogWrite(motorOn1, 51);
}

void loop() {
  //SERIAL DEBUG INPUT CODE
  if (Serial.available() > 0) {
    // read the incoming byte:
    ref = (Serial.read()*(3.14/2));
  }
  //END SERIAL DEBUG INPUT CODE
  //Serial.print((myEnc.read()/3200.0)*2.0*3.1415);
  currentPos = (myEnc.read()/3200.0)*2.0*3.1415; //read current position in radians
  e = ref - currentPos;
//  while(e>3.14){
//    e = e-6.28;
//  }
  Serial.print("Current Pos =");
  Serial.println(currentPos);
  I = I + Tloop*e;
  //Serial.print(I);
  u = Kp*e + Ki*I;
  u = (u/5)*255;
  //Serial.println(" U = ");
  //
  Serial.print("Desired Pos =");
  Serial.println(ref);
  Tloop = (millis() - Ts)/1000;
  Ts = millis();
  if(u > 0){
    digitalWrite(dir1, CCW);
    analogWrite(motorOn1, u);
  }
  else if(u < 0){
    u = abs(u);
    digitalWrite(dir1, CW);
    analogWrite(motorOn1, u);
  }
}
/*
millisCycle = millis();
  //Step function code
  Serial.print(millis()-1000);
  Serial.print(",");
  
  Serial.println(float((positionLast-myEnc.read())/1600.0)/0.01);
  positionLast = myEnc.read();
  //Serial.println("π/s");
  
  while(millis()- millisCycle < 10){
    //Do nothing until 10ms has elapsed, then break out of loop and take another measurement.
  }
 
  if(millis() > 3470){
    digitalWrite(enable, LOW);
    exit(0);
  }
  /*
  // put your main code here, to run repeatedly:
  //Serial.print("Radians = ");
  //Serial.print(myEnc.read()/3200);
  //Serial.println("*pi");
  if(abs(myEnc.read()) > hystersis){
    if(directionCommand == 0){
      analogWrite(motorOn1, 
    }
  }
  else if (abs(myEnc.read()) < hystersis){
    digitalWrite(motorOn1, LOW);
  }
  */
