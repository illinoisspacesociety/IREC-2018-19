#include <Servo.h>

Servo bVServo;
const int bVOpen = 150;
const int bVClose = 0;
const int bVPin = 2;
const int delayToFlow = 5000;
const int openTime = 1000;
int timeCurr;

void setup() {
  // put your setup code here, to run once:
  bVServo.attach(bVPin);
  timeCurr = millis();
}

void loop() {
  while((millis() - timeCurr) < delayToFlow) {
    bVServo.write(bVClose);
  }
  while((millis()-timeCurr) > delayToFlow && (millis()-timeCurr) < delayToFlow+openTime) {
    bVServo.write(bVOpen);
  }
  while((millis()-timeCurr) > delayToFlow+openTime) {
    bVServo.write(bVClose);
  }
}
