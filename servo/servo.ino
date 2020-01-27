#include "Servo.h"
Servo sd1;
void setup() {
  // put your setup code here, to run once:
  sd1.attach(D1);
}

void loop() {
  // put your main code here, to run repeatedly:
  sd1.write(180);
  delay(1000);
  sd1.write(-180);
  delay(1000);
  //delay(2000);
  //sd1.write(10);
  //delay(2000);
}
