
// constants
const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;
const int NUM_MARKER_PINS = 1;
const boolean IN = true;
const boolean OUT = !IN;

// pins
const int motorLimInputPin = 9; //input pin for limit switch
const int enablePin= 6;//Enable value of stepper motor. if it's not low it won't work
const int stepPin = 5;
const int dirPin = 4; //high is ?clockwise? and low is ?counter?
const int markerPins[NUM_MARKER_PINS] = {9}; // the pins that the marker solenoids are on
const int rotaryEncoderA = 12;  // pin 12
const int rotaryEncoderB = 11;  // pin 11

// current state variables
int x; //variable for stepper driver looping
unsigned long currentTime;
unsigned long loopTime;
unsigned char encoderA;
unsigned char encoderB;
unsigned char encoderAprev=0;

// limit switch vars
int buttonState; //state for limit switch
int val; //val for limit switch
int presses = 0; //times limit switch is pressed
long time = 0; // time for limit switch
long debounce = 200; //debounce for limit switch

#define BAUD (9600) //Define the serial communication

int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1;

void setup() {
  Serial.begin(BAUD);

  // setup solenoids
//  for (int i=0;i<NUM_MARKER_PINS;i++) {
//    Serial.println(markerPins[i]);
//    pinMode(markerPins[i], OUTPUT);
//  }

  // setup the stepper motor driver
  pinMode(enablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(stepPin, OUTPUT); // Step of stepper motor
  pinMode(dirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(enablePin,LOW);//set Enable of stepper low
//
//  // move pens to beginning for setup
  moveToSide(motorLimInputPin, IN); // center the pens
//
//  // setup rotary encoder
//  pinMode(rotaryEncoderA, INPUT);
//  pinMode(rotaryEncoderB, INPUT);
//  currentTime = millis();
//  loopTime = currentTime;
//
//  // setup limit switch
  pinMode(motorLimInputPin, INPUT);
  digitalWrite(motorLimInputPin, HIGH);
  buttonState = digitalRead(motorLimInputPin); // store initial button state (should be high)
}

void loop() {
  // read the encoder, this will update current state values


//  readEncoder();
//
//  // step the motor
//  stepMotor(true, 1);
//
//  // pen down, wait, pen up
//  penDown(0);
//  delay(500);
//  penUp(0);
//  delay(500);
//
//  // do a bunch of limit switch things
//  val = digitalRead(motorLimInputPin);
//
//  //debounce the limit switch first
//  if (val != buttonState && millis() - time > debounce) {
//    if (buttonState == HIGH){
//      presses = presses + 1;
//      Serial.println(presses);
//      time = millis();
//
//      //and have stepper spin in the opposite direction
//
//     }
//   }
//
//   //spin stepper motor
//  digitalWrite(EnablePin,LOW); // Set Enable low
//  digitalWrite(DirPin,HIGH); // Set Dir high
//  Serial.println("Loop 200 steps (1 rev)");
//  for(x = 0; x < 200; x++) // Loop 200 times
//  {
//    digitalWrite(5,HIGH); // Output high
//    delay(10); // Wait
//    digitalWrite(StepPin,LOW); // Output low
//    delay(100); // Wait
//
//    }
//
//  buttonState = val;
}

void stepMotor(bool clockwise, int steps) {
  if (clockwise) {
    digitalWrite(dirPin,HIGH); // Set Dir high
  } else {
    digitalWrite(dirPin,LOW); // Set Dir high
  }
  for (int i=0; i<steps; i++) {
    Serial.println("Step");
    digitalWrite(stepPin,HIGH); // Output high
    delay(1); // Wait
    digitalWrite(stepPin,LOW); // Output low
    delay(1); // Wait
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
