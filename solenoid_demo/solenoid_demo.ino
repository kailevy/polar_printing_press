int num1 = 13;
int num2 = 12;
int num3 = 11;

void setup() {
  // initialize the digital pins as an output.
  pinMode(num1, OUTPUT);
  pinMode(num2, OUTPUT);
  pinMode(num3, OUTPUT);
}

void loop() {
  digitalWrite(num1, HIGH);
  digitalWrite(num3, LOW);
  delay(100);
  digitalWrite(num2, HIGH);
  digitalWrite(num1, LOW);
  delay(100);
  digitalWrite(num3, HIGH);
  digitalWrite(num2, LOW);
  delay(100);
}