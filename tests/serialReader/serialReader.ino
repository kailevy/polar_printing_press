const int NUM_COMMANDS = 80;

int numRotations;
float percentPerRotation;

String commands[NUM_COMMANDS];
unsigned int angles[NUM_COMMANDS];
int markers[NUM_COMMANDS];
int ups[NUM_COMMANDS];
int index = 0;

boolean initialized = 0;

void setup() {
  Serial.begin(9600);

}

void loop() {
  if (initialized) {
    readSerialCommand();
  } else {
    initializeSerialVars();
  }
}

String readSerialString() {
  // we can only get one byte at a time, so we have to fill a string, and then parse it
//  String serialString = "";
//  bool done = false;
//  while (Serial.available() && !done) {
//    delay(2);  // delay is apparently necessary
//    if (Serial.available() > 0) {
//      char c = Serial.read();
//      if (c=='\n') { // Don't overflow serial string
//        done = true;
//        break;
//      }
//      serialString += c;
//    }
//  }
  String serialString = Serial.readStringUntil(';');
  serialString.trim();
  return serialString;
}

void initializeSerialVars() {
  String serialString = readSerialString();
  
  if (serialString.length() > 0) {
    int commaIndex = serialString.indexOf(",");
    String rotationsString = serialString.substring(0, commaIndex);
    String percentString = serialString.substring(commaIndex+1);
    
    numRotations = rotationsString.toInt();
    percentPerRotation = percentString.toFloat();
    initialized = 1;
    Serial.println("a");
  }
}

void readSerialCommand() {
  String serialString = readSerialString();

  if (serialString.length() > 0) {
    if (serialString == "d") {
      for (int i=0; i<NUM_COMMANDS; i++) {
        Serial.println(angles[i]);
//        Serial.println(markers[i]);
//        Serial.println(ups[i]);
      }
      Serial.println("a");
    } else {
      int commaIndex = serialString.indexOf(",");
      String angleString = serialString.substring(0, commaIndex);
      unsigned int angle = angleString.toInt();
      String rest = serialString.substring(commaIndex+1);
      commaIndex = rest.indexOf(",");
      String upString = rest.substring(0,commaIndex);
      int up = upString.toInt();
      String markerString = rest.substring(commaIndex+1);
      int marker = markerString.toInt();

//      if (angle > 10000) {
//        Serial.println("OVER 10000");
//      }
      
      commands[index] = serialString;
      angles[index] = angle;
      markers[index] = marker;
      ups[index] = up;
      index++;
      if (index == NUM_COMMANDS) {
        index = 0;
      }
    }
  }
}
