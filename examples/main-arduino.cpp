#include <Arduino.h>

#include "rhio-state-machine.h"

StateMachine states(2);

unsigned long time = millis();

void ledOn() {
  if (millis() - time >= 1000) {
    time = millis();
    states.set(0x02);
    digitalWrite(9, HIGH);
  }
};

void ledOff() {
  if (millis() - time >= 1000) {
    time = millis();
    digitalWrite(9, LOW);
    states.set(0x01);
  }
};

void setup() {
  pinMode(9, OUTPUT);
  states.add(0x01, ledOn);
  states.add(0x02, ledOff);
  states.set(0x01);
}

void loop() { states.run(); }
