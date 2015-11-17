#include <Wire.h>
#include "utility/Adafruit_PWMServoDriver.h"
const int EnablePin= 6;
const int StepPin = 5;
const int DirPin = 4;

int x; 
#define BAUD (9600) //Define the serial communication

int numsteps = 0;
bool forwardMotor= 1; 

void setup() {
  Serial.begin(BAUD);
  pinMode(EnablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(StepPin, OUTPUT); // Step of stepper motor
  pinMode(DirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(EnablePin,LOW);//set Enable of stepper low
  
}

void loop() {
  digitalWrite(6,LOW); // Set Enable low
  digitalWrite(4,HIGH); // Set Dir high
  Serial.println("Loop 200 steps (1 rev)");
  for(x = 0; x < 200; x++) // Loop 200 times
  {
    digitalWrite(5,HIGH); // Output high
    delay(10); // Wait
    digitalWrite(5,LOW); // Output low
    delay(100); // Wait
  }
  Serial.println("Pause");
  delay(1000); // pause one second
  }

void penUp() {
	// do the control to put the pen up here
}

void penDown() {
	// do pen down here
}

void moveFromPointToPoint(float startX, float startY, float endX, float endY) {
	// do things to move pen from point to point
}

void readSerialCommand() {
  // we can only get one byte at a time, so we have to fill a string, and then parse it
  String serialString = "";
  int count = 0;
  bool done = false;
  while (Serial.available() && !done) {
    delay(3);  // delay is apparently necessary
    if (Serial.available() > 0) {
      char c = Serial.read();
      serialString += c;
      count++;
      if (c=='\n') { // Don't overflow serial string
        serialString[count] = '\0'; // null terminate
        done = true;
      }
    }
  }

  
  if (serialString.length() > 0) {
    String color; // color is 0 for black, 1 for dark gray, 2 for light gray
    String value;  
    color = serialString.substring(0,5); // read first 5 chars
    value = serialString.substring(6,9);

    int v = value.toInt();
  } 
}
