#include <Arduino.h>

#define PUF_MAX_EVENTS 10
#include "rhio-puf.h"

Puf puf;

pufcb event1(int param) {
  Serial.println(F("EV1 - Change led status"));
  digitalWrite(9, !digitalRead(9));
  puf.emit(0x02, digitalRead(9));
  return 0;
}

pufcb event2(int ledStatus) {
  Serial.print(F("EV2 - LED status:"));
  Serial.println(ledStatus);
  return 0;
}

void setup() {
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("SETUP"));
  puf.on(0x01, event1);
  puf.on(0x02, event2);
}

void loop() {
  puf.emit(0x01, 0);
  delay(1000);
}
