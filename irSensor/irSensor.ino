const int SENSOR_PIN = A5;
const long DEBOUNCE_TIME = 10;
const int SENSOR_THRESHOLD = 100;
const int NUM_PER_ROTATION = 220;

int numSteps = 0;
int sensorValue = 0;
int sensorState = 0; // 0 for black, 1 for white
int prevSensorState = 0;
long sensorLastDebounceTime = 0;

void setup() {
  Serial.begin(9600);

  sensorLastDebounceTime = millis();
}

void loop() {
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
    if (sensorState == 0) {
      numSteps++;
      //Serial.println(numSteps);
      if (numSteps % NUM_PER_ROTATION == 0) {
        Serial.println("ONE FULL ROTATION");
      }
    }
  }
}
