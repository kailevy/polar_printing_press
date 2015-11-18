int inputPin = 9;

int buttonState;
int val;
int presses = 0;

long time = 0;
long debounce = 200;

void setup1() {
   pinMode(inputPin, INPUT);
   digitalWrite(inputPin, HIGH);

   Serial.begin(9600);
   buttonState = digitalRead(inputPin); // store initial button state (should be high)
}

void loop1() {
   val = digitalRead(inputPin);

   if (val != buttonState && millis() - time > debounce) {
     if (buttonState == HIGH){
         presses = presses + 1;
         Serial.println(presses);
         time = millis();
     }
   }

   buttonState = val;
}
