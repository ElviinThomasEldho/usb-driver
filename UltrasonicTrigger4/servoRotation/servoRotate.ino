#include <Servo.h>

Servo myservo;  // create Servo object to control a servo

int pos = 90;    // variable to store the servo position, start at the middle position
int inc = 5;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  Serial.begin(9600); // initialize serial communication at 9600 bits per second
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read(); // read the incoming byte

    if (pos >= 180 || pos <= 0) {
      inc = -inc;
    }
    pos += inc;

    myservo.write(pos); // move the servo to the new position
  }
}
