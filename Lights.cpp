#include "Lights.h"

Lights::Lights(int bluePin, int redPin) {
  _bluePin = bluePin;
  _redPin = redPin;
  pinMode(_redPin, OUTPUT);
  pinMode(_bluePin, OUTPUT);
  digitalWrite(bluePin, HIGH);
  digitalWrite(redPin, HIGH);
}

void Lights::toggleRed() {
  _redState = !_redState;
  digitalWrite(_redPin, _redState);
}

void Lights::toggleBlue() {
  _blueState = !_blueState;
  digitalWrite(_bluePin, _blueState);
}

void Lights::turnOffRed() {
    digitalWrite(_redPin, false);
    _redState = false;
}

void Lights::turnOffBlue() {
    digitalWrite(_bluePin, false);
    _blueState = false;
}
