const int EnablePin= 6;
const int StepPin = 5;
const int DirPin = 4;

int x; 
#define BAUD (9600) //Define the serial communication


const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;


const int MARKER_PINS[1] = { 0 }; // the pins that the marker solenoids are on



int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1; 

void setup() {

  Serial.begin(BAUD);
  pinMode(EnablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(StepPin, OUTPUT); // Step of stepper motor
  pinMode(DirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(EnablePin,LOW);//set Enable of stepper low
  digitalWrite(DirPin,HIGH);


  moveToBeginning(); // center the pens



  
}

void loop() {
  digitalWrite(6,LOW); // Set Enable low
  Serial.println("Loop 200 steps (1 rev)");
  for(x = 0; x < 200; x++) // Loop 200 times
  {
    digitalWrite(5,HIGH); // Output high
    delay(5); // Wait
    digitalWrite(5,LOW); // Output low
    delay(5); // Wait
  }
  Serial.println("Pause");
  delay(100000); // pause one second
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
