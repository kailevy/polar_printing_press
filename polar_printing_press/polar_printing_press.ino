
#define BAUD (9600) //Define the serial communication

// constants
const int STEPS_PER_ROTATION = 200;
const int SMALL_ROTS = 130; // number of stepper motor rotations for the width of a smaller paper
const int BIG_ROTS = 200; // number of stepper motor rotations for the width of the whole platter
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;
const int NUM_MARKER_PINS = 1;
const int IN = LOW;
const int OUT = !IN;
const int UP = LOW;
const int DOWN = HIGH;

// ir sensor constants
const int IR_SENSOR_THRESHOLD = 100;
const long NUM_BW_PER_ROTATION = 360;

// pins
const int rotationLimInputPin = 7; //input pin for limit switch
const int barLimInputPin = 11; //input pin for limit switch
const int stepperEnablePin= 6;//Enable value of stepper motor. if it's not low it won't work
const int stepperStepPin = 5;
const int stepperDirectionPin = 4; //high is ?clockwise? and low is ?counter?
const int markerPins[NUM_MARKER_PINS] = {10}; // the pins that the marker solenoids are on
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

// limit switch on bar vars
int barLimitSwitchState;
int barLimitSwitchVal;
int barLimitSwitchPresses = 0;
long barLimitSwitchLastToggleTime = 0;
long barLimitSwitchDebounceTime = 50;

// limit switch rotation vars
int rotationLimitSwitchState; //state for limit switch
int rotationLimitSwitchVal; //val for limit switch
long rotationLimitSwitchTime = 0; // time for limit switch
long rotationLimitSwitchLastToggleTime = 0;
long rotationLimitSwitchDebounceTime = 200; //debounce for limit switch

// ir sensor vars
int irSensorNumSteps = 0;
int irSensorValue = 0;
int irSensorState = 0; // 0 for black, 1 for white
int irPrevSensorState = 0;
long irSensorLastDebounceTime = 0;

int currentCommand = 0;
int numCommands = 18;
long angles[18] = {0, 360*3, 30*360+210, 30*360+330, 31*360+60, 31*360+70, 31*360+120, 31*360+130, 32*360+60, 32*360+70, 32*360+120, 32*360+130, 33*360+60, 33*360+70, 33*360+120, 33*360+130,  50*360, 51*360};
int upDown[18] = {DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP, DOWN, UP};
int solenoidState = LOW;

// NUM_BW_PER_ROTATION
long STEPS_PER_LEAD_SCREW_ROTATION = 80;
long platterRotationsNeededPerImage = 130;
long LEAD_SCREW_ROTATIONS_PER_IMAGE = 200;
long SCALE_VALUE = 1000;

long stepperStepsPerDegree = SCALE_VALUE * STEPS_PER_LEAD_SCREW_ROTATION * LEAD_SCREW_ROTATIONS_PER_IMAGE / (platterRotationsNeededPerImage * NUM_BW_PER_ROTATION);
long stepperStepsDone = 0;
long stepsQueue = 0;
long lastStepTime = 0;

int stepperState = HIGH;

boolean executing = 1;
int lastRotationSteps = 0;
int lastRotations = 0;

void setup() {
  Serial.begin(BAUD);

// setup solenoids
  setupSolenoids();

  // setup the stepper motor driver
  setupStepper();

  // setup limit input switches
  setupRotationLimInputSwitch();
  setupBarLimInputSwitch();

  // currently don't need ir sensor setup

  // move pens to beginning for setup
  moveToBeginning(barLimInputPin); // center the pens

  numRotations = 0;
  rotationSteps = 0;
  stepsQueue = 0;

  digitalWrite(stepperDirectionPin, OUT);
}

void setupSolenoids() {
  for (int i=0;i<NUM_MARKER_PINS;i++) {
    Serial.println(markerPins[i]);
    pinMode(markerPins[i], OUTPUT);
  }
}

void setupStepper() {
  pinMode(stepperEnablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(stepperStepPin, OUTPUT); // Step of stepper motor
  pinMode(stepperDirectionPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(stepperEnablePin,LOW);//set Enable of stepper low
  digitalWrite(stepperDirectionPin, IN);
}

void setupRotationLimInputSwitch() {
  pinMode(rotationLimInputPin, INPUT);
  digitalWrite(rotationLimInputPin, HIGH);
}

void setupBarLimInputSwitch() {
  pinMode(barLimInputPin, INPUT);
  digitalWrite(barLimInputPin, HIGH);
}

void loop() {
  // read the encoder, this will update current state values
  readRotationSwitch();
  readIrSensor();
  if (executing) {

    if (currentCommand > numCommands) {
      Serial.println("Done with current commands");
      executing = 0;
      lastRotations = numRotations; // maybe don't need this?
      lastRotationSteps = rotationSteps;
    } else if (360*numRotations + rotationSteps >= angles[currentCommand]) {
      actuatePen(0, upDown[currentCommand]);
      currentCommand++;
    }

    long degreesDone = NUM_BW_PER_ROTATION * numRotations + rotationSteps;
    long stepsNeeded = degreesDone * stepperStepsPerDegree / SCALE_VALUE;

    if (stepsNeeded > stepperStepsDone) {
      stepsQueue += stepsNeeded - stepperStepsDone;
      stepperStepsDone += stepsNeeded - stepperStepsDone;
    }

    stepIfNeeded();
  } else {
    // TODO: read waiting commands from serial and reset command counter
    if (currentCommand == 0){
      // we have read the commands and reset the command counter, so we wait to
      // hit the limit switch to start again
      if (rotationSteps == lastRotationSteps) {
        // only start executing when we have rotated to the same point we were at when we stopped
        executing = 1;
      }
    }
  }
}

int count = 0;

void stepIfNeeded() {
  if (stepperState == HIGH && millis() - lastStepTime >= 4) {
    stepperState = LOW;
    digitalWrite(stepperStepPin,stepperState);
    lastStepTime = millis();
    stepsQueue--;
    count++;
  } else if (stepperState == LOW && stepsQueue > 0 && millis() - lastStepTime >= 4) {
    stepperState = HIGH;
    digitalWrite(stepperStepPin,stepperState);
    lastStepTime = millis();
  }
}

void stepMotor(int in, int steps) {
  digitalWrite(stepperDirectionPin,in); // Set Dir high
  for (int i=0; i<steps; i++) {
    stepperState = !stepperState;
    digitalWrite(stepperStepPin,HIGH); // Output high
    delay(1); // Wait
    digitalWrite(stepperStepPin,LOW); // Output low
    delay(1); // Wait
  }
}

void readRotationSwitch(){
  rotationLimitSwitchVal = digitalRead(rotationLimInputPin);

  if (rotationLimitSwitchVal != rotationLimitSwitchState && millis() - rotationLimitSwitchLastToggleTime > rotationLimitSwitchDebounceTime) {
    rotationLimitSwitchState = rotationLimitSwitchVal;
    rotationLimitSwitchLastToggleTime = millis();
    if (rotationLimitSwitchState == HIGH) {
      rotationSteps = 0;
      if (executing) {
        numRotations++;
      }
    }
  }
}

void readBarSwitch(){
  barLimitSwitchVal = digitalRead(barLimInputPin);

  if (barLimitSwitchVal != barLimitSwitchState && millis() - barLimitSwitchLastToggleTime > barLimitSwitchDebounceTime) {
    barLimitSwitchPresses = barLimitSwitchPresses + 1;
    barLimitSwitchState = barLimitSwitchVal;
    barLimitSwitchLastToggleTime = millis();
  }
}

void moveToBeginning(int inputPin) {
  // reset the pen cars in their central position
  readBarSwitch();
  while (barLimitSwitchState == LOW) {
    stepMotor(IN, 1); // move toward center 1 rotation
    readBarSwitch();
  }
}

void readIrSensor() {
  irSensorValue = analogRead(irSensorPin);
  //Serial.println(irSensorValue);

  int currentState;
  if (irSensorValue >= IR_SENSOR_THRESHOLD) {
    currentState = 1;
  } else {
    currentState = 0;
  }

  if (currentState != irSensorState) {
    irSensorState = currentState;
    rotationSteps++;

    // DO SOMETHING
  }
}

void actuatePen(int marker, int state){
  // state is either 0 or 1
  digitalWrite(markerPins[marker], state);
  solenoidState = state;
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
  // TODO update this with correct version
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
