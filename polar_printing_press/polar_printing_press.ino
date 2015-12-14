
#define BAUD (9600) //Define the serial communication

// constants
const unsigned long STEPS_PER_ROTATION = 200;
const unsigned long SMALL_ROTS = 130; // number of stepper motor rotations for the width of a smaller paper
const unsigned long BIG_ROTS = 200; // number of stepper motor rotations for the width of the whole platter
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int NUM_MARKER_PINS = 1;
const int NUM_COMMANDS = 10; // number of commands to process at once
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
byte readyForCommand = 1;
unsigned long currentAngle = 0;
boolean forwardMotor= 1;
unsigned long numRotations = 0;
unsigned long rotationSteps = 0;

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
int irSensorValue = 0;
int irSensorState = 0; // 0 for black, 1 for white
int irPrevSensorState = 0;

#define NUM_COMMANDS 50
int currentCommand = NUM_COMMANDS;
int numCommands = NUM_COMMANDS;
unsigned long angles[NUM_COMMANDS];
int upDown[NUM_COMMANDS];
int solenoidState[NUM_MARKER_PINS] = {LOW};

// NUM_BW_PER_ROTATION
unsigned long STEPS_PER_LEAD_SCREW_ROTATION = 80;
unsigned long platterRotationsNeededPerImage;
unsigned long LEAD_SCREW_ROTATIONS_PER_IMAGE = 200;
unsigned long SCALE_VALUE = 1000;

unsigned long stepperStepsPerDegree;
unsigned long stepperStepsDone = 0;
unsigned long stepsQueue = 0;
unsigned long lastStepTime = 0;

int stepperState = HIGH;

int serialIndex = 0;

boolean executing = 0;
boolean initialized = 0;
boolean rotationsZeroed = 0;
boolean done = 0;
unsigned long lastRotationSteps = 0;
unsigned long lastRotations = 0;

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
  
  // if we don't have the totalRotations, get it now
  if (!initialized) {
    platterRotationsNeededPerImage = readSerialString().toInt();
    if (platterRotationsNeededPerImage != 0) {
      stepperStepsPerDegree = SCALE_VALUE * STEPS_PER_LEAD_SCREW_ROTATION * LEAD_SCREW_ROTATIONS_PER_IMAGE / (platterRotationsNeededPerImage * NUM_BW_PER_ROTATION);
      initialized = 1;
      Serial.print("Initialized");
      Serial.println(platterRotationsNeededPerImage);
    }
  }

  // read the encoder, this will update current state values
  readRotationSwitch();
  readIrSensor();
  if (executing) {

    if (currentCommand >= numCommands) {
      Serial.println("a"); 
      executing = 0;
      rotationsZeroed = 0;
      for (int i=0; i<NUM_MARKER_PINS; i++) {
        delayPen(markerPins[i]);
      }
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
  } else if (initialized) {

    if (currentCommand == 0) { // if have commands, wait for the press to re-center itself
      // we have read the commands and reset the command counter, so we wait to
      // hit the limit switch to start again
      if (rotationLimitSwitchState == HIGH) {
        rotationsZeroed = 1;
      }
      if (rotationSteps == lastRotationSteps && rotationsZeroed) {
        // only start executing when we have rotated to the same point we were at when we stopped
        executing = 1;
        for (int i=0; i<NUM_MARKER_PINS; i++) {
          restorePen(markerPins[i]);
        }
      }
    } else { // if no commands, need to get commands
      readSerialCommands();
    }
  }
}

int count = 0;

void stepIfNeeded() {
  if (stepperState == HIGH && millis() - lastStepTime >= 3) {
    stepperState = LOW;
    digitalWrite(stepperStepPin,stepperState);
    lastStepTime = millis();
    stepsQueue--;
    count++;
  } else if (stepperState == LOW && stepsQueue > 0 && millis() - lastStepTime >= 3 && !done) {
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
  solenoidState[marker] = state;
}

void delayPen(int marker) {
  digitalWrite(markerPins[marker], UP);
}

void restorePen(int marker) {
  digitalWrite(markerPins[marker], solenoidState[marker]);
}

String readSerialString() {
  String serialString = Serial.readStringUntil(';');
  serialString.trim();
  return serialString;
}

void readSerialCommands() {
  String serialString = readSerialString();

  if (serialString.length() > 0) {
    if (serialString == "d") {
      //for (int i=0; i<NUM_COMMANDS; i++) {
        // Serial.println(angles[i]);
//        Serial.println(markers[i]);
//        Serial.println(ups[i]);
      //}
      currentCommand = 0;
      //Serial.println("a");
    } else {
      // check for done
      if (serialString == "done") {
        done = 1;
      }
      int commaIndex = serialString.indexOf(",");
      String angleString = serialString.substring(0, commaIndex);
      unsigned long angle = stringToLong(angleString);
      String rest = serialString.substring(commaIndex+1);
      commaIndex = rest.indexOf(",");
      String upString = rest.substring(0,commaIndex);
      int up = upString.toInt();
      String markerString = rest.substring(commaIndex+1);
      int marker = markerString.toInt();

      angles[serialIndex] = angle;
      //markers[serialIndex] = marker;
      upDown[serialIndex] = up;
      Serial.println(angle);
      serialIndex++;
      Serial.println(serialIndex);
      if (serialIndex == NUM_COMMANDS) {
        serialIndex = 0;
      }
    }
  }
}

long stringToLong(String s) {
  int len = s.length();
  char charArray[len+1];
  s.toCharArray(charArray, len+1);
  long res = atol(charArray);
  return res;
}

