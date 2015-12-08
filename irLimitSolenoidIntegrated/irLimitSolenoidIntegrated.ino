const int SENSOR_PIN = A5;
const long DEBOUNCE_TIME = 10;
const int SENSOR_THRESHOLD = 100;
const int NUM_PER_ROTATION = 360;

int numSteps = 0;
int sensorValue = 0;
int sensorState = 0; // 0 for black, 1 for white
int prevSensorState = 0;
long sensorLastDebounceTime = 0;

int inputPin = 9;

int limitSwitchState;
int limitSwitchVal;
int limitSwitchPresses = 0;
long limitSwitchLastToggleTime = 0;
long limitSwitchDebounceTime = 50;

int num1 = 10;
int solenoidState = LOW;

void setup() {
  Serial.begin(9600);

  sensorLastDebounceTime = millis();
  
  pinMode(num1, OUTPUT);
  digitalWrite(num1, solenoidState);
  
  pinMode(inputPin, INPUT);
  digitalWrite(inputPin, HIGH);
  
  limitSwitchState = digitalRead(inputPin); // store initial button state (should be high)
}

void loop() {
  limitSwitchVal = digitalRead(inputPin);
   
  if (limitSwitchVal != limitSwitchState && millis() - limitSwitchLastToggleTime > limitSwitchDebounceTime) {
    limitSwitchPresses = limitSwitchPresses + 1;
    limitSwitchState = limitSwitchVal;
    limitSwitchLastToggleTime = millis();
    if (limitSwitchState == HIGH) {
      Serial.println(numSteps % 360);
      numSteps = 0;
    }
  }
  
  sensorValue = analogRead(SENSOR_PIN);

  //Serial.println(sensorValue);

  int currentState;
  if (sensorValue >= SENSOR_THRESHOLD) {
    currentState = 1;
  } else {
    currentState = 0;
  }

  if (currentState != sensorState) {
    sensorState = currentState;
    numSteps++;
    //Serial.println(numSteps);
    if (numSteps % 90 == 0) {
      solenoidState = !solenoidState;
      digitalWrite(num1, solenoidState);
      //Serial.println("ONE FULL ROTATION");
    }
  }
}