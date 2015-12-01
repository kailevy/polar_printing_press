
String commands[10];
int index = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  readSerialCommand();
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
    if (serialString == "done") {
      for (int i=0; i<10; i++) {
        Serial.println(commands[i]);
      }
    } else {
      commands[index] = serialString;
      index++;
    }
  }
}
