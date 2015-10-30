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