
#define BAUD (9600) //Define the serial communication

// constants
const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;
const int NUM_MARKER_PINS = 1;
const boolean IN = true;
const boolean OUT = !IN;

// ir sensor constants
const int IR_SENSOR_THRESHOLD = 100;
const int NUM_BW_PER_ROTATION = 360;

// pins
const int rotationLimInputPin = 8; //input pin for limit switch
const int stepperEnablePin= 6;//Enable value of stepper motor. if it's not low it won't work
const int stepperStepPin = 5;
const int stepperDirectionPin = 4; //high is ?clockwise? and low is ?counter?
const int markerPins[NUM_MARKER_PINS] = {9}; // the pins that the marker solenoids are on
const int irSensorPin = A5;

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

// ir sensor vars
int irSensorNumSteps = 0;
int irSensorValue = 0;
int irSensorState = 0; // 0 for black, 1 for white
int irPrevSensorState = 0;
long irSensorLastDebounceTime = 0;

int stateMachine = 0;
int halfCircle = 120; // calibrate this!
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
  pinMode(stepperEnablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(stepperStepPin, OUTPUT); // Step of stepper motor
  pinMode(stepperDirectionPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(stepperEnablePin,LOW);//set Enable of stepper low

  pinMode(rotationLimInputPin, INPUT);
  digitalWrite(rotationLimInputPin, HIGH);
  // move pens to beginning for setup
  moveToSide(rotationLimInputPin, IN); // center the pens

  // ir sensor setup
  irSensorLastDebounceTime = millis();

  // setup rotary encoder
  pinMode(rotaryEncoderA, INPUT);
  pinMode(rotaryEncoderB, INPUT);
  currentTime = millis();
  loopTime = currentTime;
}

void loop() {
  // read the encoder, this will update current state values

  readRotationSwitch();
  readEncoder();
  Serial.println(rotationSteps);

  if (rotationSteps > halfCircle || rotationSteps < -halfCircle) {
    Serial.println("hello");
    stateMachine = (stateMachine + 1)%4;
    rotationSteps = 0;
  }
  
  switch(stateMachine){
    case 0:
      if ((rotationSteps / 5) % 2 == 1){
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
  stepMotor(OUT, 1);
  

}

void stepMotor(boolean clockwise, int steps) {
  if (clockwise) {
    digitalWrite(stepperDirectionPin,HIGH); // Set Dir high
  } else {
    digitalWrite(stepperDirectionPin,LOW); // Set Dir low
  }
  for (int i=0; i<steps; i++) {
    digitalWrite(stepperStepPin,HIGH); // Output high
    delay(1); // Wait
    digitalWrite(stepperStepPin,LOW); // Output low
    delay(1); // Wait
  }
}

void readRotationSwitch(){
  // do a bunch of limit switch things
  int val = digitalRead(rotationLimInputPin);

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

void readIrSensor() {
  irSensorValue = analogRead(irSensorPin);

  int currentState;
  if (irSensorValue >= IR_SENSOR_THRESHOLD) {
    currentState = 1;
  } else {
    currentState = 0;
  }

  if (currentState != irSensorState) {
    irSensorState = currentState;
    numSteps++;
    if (numSteps % NUM_BW_PER_ROTATION == 0) {
      Serial.println("ONE FULL ROTATION");
    }
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
