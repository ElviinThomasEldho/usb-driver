const int trigPin = 9; 
const int echoPin = 10;


const float distanceTreshold = 10;


float duration, distance; 


void setup() { 
 pinMode(trigPin, OUTPUT); 
 pinMode(echoPin, INPUT); 
 Serial.begin(9600); 
} 


void loop() { 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 

 duration = pulseIn(echoPin, HIGH);
 distance = (duration*.0343)/2; 


//  Serial.print("Distance: "); 
//  Serial.println(distance); 


 if (distance < distanceTreshold) {
   Serial.println("Object Detected"); 
 } else {
   Serial.println("Object Not Detected"); 
 }


 delay(500); 
} 
