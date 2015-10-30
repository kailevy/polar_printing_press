#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 1);

int numsteps = 0;

void setup() {
  Serial.begin(9600);

  AFMS.begin();
  motor1->setSpeed(10);  // 10 rpm   
}

void loop() {
  motor1->step(100, FORWARD, SINGLE); 
  numsteps += 1;
  Serial.println(numsteps);
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
    command = serialString.substring(0,5); // read first 5 chars
    value = serialString.substring(6,9);

    int v = value.toInt();

    if (v==0 && value!="000") { // check if conversion was actually made
      Serial.print("Invalid value ");
      Serial.println(value);
      serialString = "";
      return;
    }

    if (command == "avgws") {
        Serial.print("Set averageWheelSpeed to ");
        Serial.println(v);
        averageWheelSpeed = v;
    } else if (command == "cpwsc") {
        Serial.print("Set controlPerWheelSpeedChange to ");
        Serial.println(v);
        controlPerWheelSpeedChange = v;
    } else if (command == "mxerr") {
        Serial.print("Set maxError to ");
        Serial.println(v);
        maxError = v;
    } else if (command == "setkp") {
        Serial.print("Set kP to ");
        Serial.println(v);
        kP = v;
    } else if (command == "setki") {
        Serial.print("Set kI to ");
        Serial.println(v);
        kI = v;
    } else if (command == "setkd") {
        Serial.print("Set kD to ");
        Serial.println(v);
        kD = v;
    } else {
      Serial.print("Invalid command "); 
      Serial.println(command);
    }

    serialString = "";
  } 
}