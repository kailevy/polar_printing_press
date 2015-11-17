
const int EnablePin= 6;//Enable value of stepper motor. if it's not low it won't work
const int StepPin = 5; // Step of stepper motor
const int DirPin = 4; //Dir of stepper motor. high is ?clockwise? and low is ?counter?
int x; //variable for stepper driver looping

#define BAUD (9600) //Define the serial communication

const int limInputPin = 9; //input pin for limit switch
int buttonState; //state for limit switch
int val; //val for limit switch
int presses = 0; //times limit switch is pressed
long time = 0; // time for limit switch
long debounce = 200; //debounce for limit switch

const int STEPS_PER_ROTATION = 200;
const unsigned long ROTATIONS_PER_RADIUS = 100;


const int MARKER_PINS[1] = { 0 }; // the pins that the marker solenoids are on




int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1; 

void setup() {

  Serial.begin(BAUD);
  pinMode(EnablePin, OUTPUT); //Enable
  pinMode(StepPin, OUTPUT); //Step
  pinMode(DirPin,OUTPUT); //Dir
  digitalWrite(EnablePin,LOW);//set Enable of stepper low

  pinMode(limInputPin, INPUT);
  digitalWrite(limInputPin, HIGH);
  buttonState = digitalRead(limInputPin); // store initial button state (should be high)
  
  moveToBeginning(); // center the pens 
}

void loop() 
{
  val = digitalRead(limInputPin);

  //debounce the limit switch first 
  if (val != buttonState && millis() - time > debounce) {
    if (buttonState == HIGH){
      presses = presses + 1;
      Serial.println(presses);
      time = millis();
     }
   }
   
   //spin stepper motor
  digitalWrite(6,LOW); // Set Enable low
  digitalWrite(4,HIGH); // Set Dir high
  Serial.println("Loop 200 steps (1 rev)");
  for(x = 0; x < 200; x++) // Loop 200 times
  {
    digitalWrite(5,HIGH); // Output high
    delay(10); // Wait
    digitalWrite(5,LOW); // Output low
    delay(100); // Wait
  }
  Serial.println("Pause");
  delay(1000); // pause one second

  buttonState = val;
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
