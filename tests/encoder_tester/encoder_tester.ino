
const unsigned long ROTATIONS_PER_RADIUS = 100;
const int ROTARY_ENCODER_READ_DELAY = 0;

int rotationSteps = 0;
unsigned long currentTime;
unsigned long loopTime;
unsigned char encoderA;
unsigned char encoderB;
unsigned char encoderAprev=0;
const int rotaryEncoderA = 12;  // pin 12
const int rotaryEncoderB = 11;  // pin 11

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(rotaryEncoderA, INPUT);
  pinMode(rotaryEncoderB, INPUT);
  currentTime = millis();
  loopTime = currentTime;
  
}

void loop() {
  Serial.println(rotationSteps);
  readEncoder();
}  
void readEncoder() {
  // get the current elapsed time
  currentTime = millis();
  if(currentTime >= (loopTime + ROTARY_ENCODER_READ_DELAY)){ // some delay
    // 5ms since last check of encoder = 200Hz
    encoderA = digitalRead(rotaryEncoderA);    // Read encoder pins
    encoderB = digitalRead(rotaryEncoderB);
    if((!encoderA) && (encoderAprev)){
      Serial.println("a");
      // A has gone from high to low
      if(encoderB) {
        Serial.println("b");
        // B is high so clockwise
        rotationSteps--;
      } else {
        Serial.println("c");
        // B is low so counter-clockwise
        rotationSteps++;
      }
    }
    encoderAprev = encoderA;     // Store value of A for next time
    loopTime = currentTime;  // Updates loopTime
  }
}
