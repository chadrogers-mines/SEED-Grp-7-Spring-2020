/* another version of Digital Read
*  jsteele 201301220
*/

#define pushButton 2
#define motorApwm  3
#define motorAdir  12
#define ledPin     13

boolean buttonPushed = false;

int buttonState = 0;
int onoffcycles = 0;
long starttime, stoptime, deltatime;


// the setup routine runs once when you press reset:
void setup() {
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // indicates whether the button is pressed
  //make the motor ports outputs
  pinMode(motorApwm, OUTPUT);
  pinMode(motorAdir, OUTPUT);
  digitalWrite(motorAdir, HIGH);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);  // set the serial comm to 9600 BAUD
}

// the loop routine runs over and over again forever:
void loop() {
    
  // read the input pin:
  buttonState = digitalRead(pushButton);
  if(buttonState == 0){
    analogWrite(motorApwm, 255); // turns the motor On
    digitalWrite(ledPin, LOW);
    if (buttonPushed == false){
      buttonPushed = true;
      starttime = millis();
      Serial.println(starttime);
    }
  }
  else {
    analogWrite(motorApwm,0);
    digitalWrite(ledPin, HIGH);
    if (buttonPushed == true) { // we are coming out of the "ON" state
      stoptime = millis();
      Serial.println(stoptime);
      buttonPushed = false;
      deltatime = stoptime - starttime;
      onoffcycles++;
      Serial.print(onoffcycles);
      Serial.print("\t");
      Serial.print(stoptime);
      Serial.print(" ");
      Serial.print(starttime);
      Serial.print("\t");
      Serial.println(deltatime);
      
    }
  } 
  // print out the state of the button:
  //Serial.println(buttonState);
  delay(10);        // delay in between reads for stability
}
