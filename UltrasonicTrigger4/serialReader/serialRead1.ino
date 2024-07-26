#include <Servo.h>

char receivedChar;
boolean newData = false;
Servo myservo;

int pos = 0;
int dir = 0;
int inc = 5;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
    pinMode(LED_BUILTIN, OUTPUT);
    myservo.attach(9);  
}

void loop() {
  recvOneChar();

  if (newData == true) {
      Serial.print("This just in ... ");
      Serial.println(receivedChar);
      newData = false;

      if(dir == 0 && pos < 180) {
        pos = pos + inc;
        myservo.write(pos); 
      } else if (dir == 1 && pos > 0) {
        pos = pos - inc;
        myservo.write(pos); 
      } else if (dir == 0 && pos >= 180) {
        dir = 1;
        pos = pos - inc;
      } else if(dir == 1 && pos <= 0) {
        dir = 0;
        pos = pos + inc;
      }

      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(100);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(100);  S
  }
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}
S