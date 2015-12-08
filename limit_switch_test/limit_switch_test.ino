int inputPin = 9;

int limitSwitchState;
int limitSwitchVal;
int limitSwitchPresses = 0;
long limitSwitchLastToggleTime = 0;
long limitSwitchDebounceTime = 50;

void setup() {
   pinMode(inputPin, INPUT);
   digitalWrite(inputPin, HIGH);

   Serial.begin(9600);
   limitSwitchState = digitalRead(inputPin); // store initial button state (should be high)
}

void loop() {
  limitSwitchVal = digitalRead(inputPin);
   
  if (limitSwitchVal != limitSwitchState && millis() - limitSwitchLastToggleTime > limitSwitchDebounceTime) {
    limitSwitchPresses = limitSwitchPresses + 1;
    Serial.println(limitSwitchPresses);
    limitSwitchState = limitSwitchVal;
    limitSwitchLastToggleTime = millis();
  }

}
