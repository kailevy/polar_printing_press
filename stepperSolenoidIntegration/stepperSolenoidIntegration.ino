const int EnablePin= 6;
const int StepPin = 5;
const int DirPin = 4;

const int solenoidPin = 9;

const int delayTime = 5;
const int rotationsPerMarker = 1;

int x; 
#define BAUD (9600) //Define the serial communication


const int STEPS_PER_ROTATION = 50;
const unsigned long ROTATIONS_PER_RADIUS = 100;




const int MARKER_PINS[1] = { 0 }; // the pins that the marker solenoids are on

int numsteps = 0;
byte readyForCommand = 1;
unsigned long currentAngle = 0;
bool forwardMotor= 1; 

int solenoidState = LOW;

void setup() {
  Serial.begin(BAUD);
  pinMode(EnablePin, OUTPUT); //Enable value of stepper motor. if it's not low it won't work
  pinMode(StepPin, OUTPUT); // Step of stepper motor
  pinMode(DirPin,OUTPUT); //Dir of stepper motor. high is ?clockwise? and low is ?counter?
  digitalWrite(EnablePin,LOW);//set Enable of stepper low

  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, solenoidState);
}

void loop() {
  digitalWrite(EnablePin,LOW); // Set Enable low
  digitalWrite(DirPin,HIGH); // Set Dir high
  Serial.println("Loop 200 steps (1 rev)");
  for(x = 0; x < STEPS_PER_ROTATION * rotationsPerMarker; x++) // Loop 200 times
  {
    digitalWrite(StepPin,HIGH); // Output high
    delay(delayTime); // Wait
    digitalWrite(StepPin,LOW); // Output low
    delay(delayTime); // Wait
  }

  solenoidState = !solenoidState;
  digitalWrite(solenoidPin, solenoidState);
}
