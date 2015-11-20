
// constants
const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;
const int NUM_MARKER_PINS = 1;
const boolean IN = true;
const boolean OUT = !IN;

// pins
const int motorLimInputPin = 8; //input pin for limit switch
const int enablePin= 6;//Enable value of stepper motor. if it's not low it won't work
const int stepPin = 5;
const int dirPin = 4; //high is ?clockwise? and low is ?counter?
const int markerPins[NUM_MARKER_PINS] = {9}; // the pins that the marker solenoids are on
const int rotaryEncoderA = 12;  // pin 12
const int rotaryEncoderB = 11;  // pin 11

#define BAUD (9600) //Define the serial communication

// current state variables
int x; //variable for stepper driver looping
unsigned long currentTime;
unsigned long loopTime;
unsigned char encoderA;
unsigned char encoderB;
unsigned char encoderAprev=0;

byte readyForCommand = 1;
unsigned long currentAngle = 0;
boolean forwardMotor= 1;
int numRotations = 0;
int rotationSteps = 0;

// limit switch vars
int buttonState; //state for limit switch
int val; //val for limit switch
long time = 0; // time for limit switch
long debounce = 200; //debounce for limit switch

int stateMachine = 0;
int halfCircle = 20*60; // calibrate this!
// 1: alternate, 2: down, 3 & 4: up, increments every half circle
boolean penState = 0;


void setup() {
  Serial.begin(BAUD);

// setup solenoids
  for (int i=0;i<NUM_MARKER_PINS;i++) {
    Serial.println(markerPins[i]);
    pinMode(markerPins[i], OUTPUT);
  }

  // setup the stepper motor driver
  pinMode(enablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(stepPin, OUTPUT); // Step of stepper motor
  pinMode(dirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(enablePin,LOW);//set Enable of stepper low

  // move pens to beginning for setup
  moveToSide(motorLimInputPin, IN); // center the pens

  // setup rotary encoder
  pinMode(rotaryEncoderA, INPUT);
  pinMode(rotaryEncoderB, INPUT);
  currentTime = millis();
  loopTime = currentTime;
  // setup limit switch
  pinMode(motorLimInputPin, INPUT);
  digitalWrite(motorLimInputPin, HIGH);
  buttonState = digitalRead(motorLimInputPin); // store initial button state (should be high)
}

void loop() {
  // read the encoder, this will update current state values

  readRotationSwitch();
  readEncoder();
  Serial.println(rotationSteps);
  if (rotationSteps >= halfCircle){
    stateMachine = (stateMachine + 1)%4;
    rotationSteps = 0;
  }
  switch(stateMachine){
    case 0:
      if ((rotationSteps / 20) % 2 == 1){
        actuatePen(0,!penState);
      }
      break;
    case 1:
      actuatePen(0,1);
      break;
    default:
      actuatePen(0,0);
      break;
  }
        
  // step the motor
  stepMotor(true, 1);
  

}

void stepMotor(boolean clockwise, int steps) {
  if (clockwise) {
    digitalWrite(dirPin,HIGH); // Set Dir high
  } else {
    digitalWrite(dirPin,LOW); // Set Dir low
  }
  for (int i=0; i<steps; i++) {
    Serial.println("Step");
    digitalWrite(stepPin,HIGH); // Output high
    delay(1); // Wait
    digitalWrite(stepPin,LOW); // Output low
    delay(1); // Wait
  }
}

void readRotationSwitch(){
  // do a bunch of limit switch things
  int val = digitalRead(motorLimInputPin);

  //debounce the limit switch first
  if (val != buttonState && millis() - time > debounce) {
    if (buttonState == HIGH){
      numRotations++;
      time = millis();
     }
     buttonState = val;
   }
}

void moveToSide(int inputPin, bool direction) {
  // reset the pen cars in their central position
  while (digitalRead(inputPin) == LOW) {
    stepMotor(direction, 1); // move toward center 1 rotation
  }
  Serial.println("Done");
}

void readEncoder() {
  // get the current elapsed time
  currentTime = millis();
  if(currentTime >= (loopTime + ROTARY_ENCODER_READ_DELAY)){ // some delay
    // 5ms since last check of encoder = 200Hz
    encoderA = digitalRead(rotaryEncoderA);    // Read encoder pins
    encoderB = digitalRead(rotaryEncoderB);
    if((!encoderA) && (encoderAprev)){
      // A has gone from high to low
      if(encoderB) {
        // B is high so clockwise
        rotationSteps++;
      } else {
        // B is low so counter-clockwise
        rotationSteps--;
      }
    }
    encoderAprev = encoderA;     // Store value of A for next time
    loopTime = currentTime;  // Updates loopTime
  }
}

void actuatePen(int marker, int state){
  // state is either 0 or 1
  digitalWrite(markerPins[marker], state);
  penState = state;
}

void penDown(int marker) {
  //needs to take python command input
  Serial.println(markerPins[marker]);
	digitalWrite(markerPins[marker], HIGH);
}

void penUp(int marker) {
	// do the control to put the pen up here
  digitalWrite(markerPins[marker], LOW);
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
