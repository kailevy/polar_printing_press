int num1 = 9;
//int num2 = 12;
//int num3 = 11;

void setup() {
  // initialize the digital pins as an output.
  pinMode(num1, OUTPUT);
  //pinMode(num2, OUTPUT);
  //pinMode(num3, OUTPUT);
}

void loop() {
  digitalWrite(num1, HIGH);
  delay(75);
  digitalWrite(num1, LOW);
  delay(75);
}
