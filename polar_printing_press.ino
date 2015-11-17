#define BAUD (9600) //Define the serial communication

// constants
const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;
const int NUM_MARKER_PINS = 1;

// pins
const int enablePin= 6;
const int stepPin = 5;
const int dirPin = 4;
const int markerPins[NUM_MARKER_PINS] = {0}; // the pins that the marker solenoids are on
const int rotaryEncoderA = 12;  // pin 12
const int rotaryEncoderB = 11;  // pin 11

// current state variables
unsigned long currentTime;
unsigned long loopTime;
unsigned char encoderA;
unsigned char encoderB;
unsigned char encoderAprev=0;

int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1; 

void setup() {
  Serial.begin(BAUD);

  // setup solenoids
  for (int i=0;i<NUM_MARKER_PINS;i++) {
    pinMode(markerPins[i], OUTPUT);
  }

  // setup the stepper motor driver
  pinMode(enablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(stepPin, OUTPUT); // Step of stepper motor
  pinMode(dirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(enablePin,LOW);//set Enable of stepper low

  // move pens to beginning for setup
  moveToBeginning(); // center the pens

  // setup rotary encoder
  pinMode(rotaryEncoderA, INPUT);
  pinMode(rotaryEncoderB, INPUT);
  currentTime = millis();
  loopTime = currentTime;
}

void loop() {
  // read the encoder, this will update current state values
  readEncoder();

  // step the motor
  stepMotor(true);

  // pen down, wait, pen up
  penDown(0);
  delay(5);
  penUp(0);
}

void stepMotor(bool clockwise) {
  if (clockwise) {
    digitalWrite(dirPin,HIGH); // Set Dir high
  } else {
    digitalWrite(dirPin,LOW); // Set Dir high
  }
  digitalWrite(stepPin,HIGH); // Output high
  delay(1); // Wait
  digitalWrite(stepPin,LOW); // Output low
  delay(1); // Wait
}

void moveToBeginning() {
  // reset the pen cars in their central position
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
        // TODO: Update current value
      } else {
        // B is low so counter-clockwise      
        // TODO: Update current value      
      }
    }
    encoderAprev = encoderA;     // Store value of A for next time    
    loopTime = currentTime;  // Updates loopTime
  }
}

void penUp(int marker) {
	// do the control to put the pen up here
  digitalWrite(markerPins[marker], LOW);
}

void penDown(int marker) {
	// do pen down here
  digitalWrite(markerPins[marker], HIGH);
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
