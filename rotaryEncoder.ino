/*
** Sample the encoder at 200Hz using the millis() function
*/

unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 12;  // pin 12
const int pin_B = 11;  // pin 11
int ledPin = 13; //onboard arduino pin

unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;

void setup()  {

  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime; 
  pinMode(ledPin, OUTPUT); // pin 13 output 
} 

void loop()  {
  // get the current elapsed time
  currentTime = millis();
  if(currentTime >= (loopTime + 5)){
    // 5ms since last check of encoder = 200Hz  
    encoder_A = digitalRead(pin_A);    // Read encoder pins
    encoder_B = digitalRead(pin_B);   
    if((!encoder_A) && (encoder_A_prev)){
      // A has gone from high to low 
      if(encoder_B) {
        // B is high so clockwise
        // increase the brightness, dont go over 255
        digitalWrite(ledPin, HIGH); //Led is on 
      }
        
   
      else {
        // B is low so counter-clockwise      
        // decrease the brightness, dont go below 0
        digitalWrite(ledPin, LOW); //led off               
      }   

    }   
    encoder_A_prev = encoder_A;     // Store value of A for next time    
      
   
    loopTime = currentTime;  // Updates loopTime
  }
  // Other processing can be done here
                           
}

