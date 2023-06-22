#include "Lights.h"

Lights::Lights(int bluePin, int redPin) {
  _bluePin = bluePin;
  _redPin = redPin;
  pinMode(_redPin, OUTPUT);
  pinMode(_bluePin, OUTPUT);
  digitalWrite(bluePin, HIGH);
  digitalWrite(redPin, HIGH);
}

void Lights::flashRed(int millis) {
  flash(_redPin, millis);
}

void Lights::flashBlue(int millis){
  flash(_bluePin, millis);
}

void Lights::flash(int whichPin, int millis) {
  digitalWrite(whichPin, LOW);
  delay(millis);
  digitalWrite(whichPin, HIGH);
}