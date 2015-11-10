#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int CENTER_MOTOR_SPEED = 10;

const int MARKER_PINS[1] = { 0 }; // the pins that the marker solenoids are on

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *centerMotor = AFMS.getMotor(1);
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 2);

int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1; 

void setup() {
  Serial.begin(9600);

  AFMS.begin();

  moveToBeginning(); // center the pens

  centerMotor->setSpeed(CENTER_MOTOR_SPEED);
  motor1->setSpeed(500);  // 10 rpm   
  
}

void loop() {
//  if (numsteps == 1000){
//    forwardMotor = !forwardMotor;
//    numsteps = 0;
//  }
  if (forwardMotor){
    motor1->step(5, FORWARD); 
    numsteps += 1;
  }
//  else{
//      motor1->step(5, BACKWARD);
//      numsteps += 1;
//    }
  }

void moveToBeginning() {
  // reset the pen cars in their central position
}

void penUp(int marker) {
	// do the control to put the pen up here
}

void penDown(int marker) {
	// do pen down here
}

void moveToAngle(unsigned long angle) {
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
  } 
}
