#ifndef _LIGHTS_H
#define _LIGHTS_H
#include <Arduino.h>

class Lights {
  private:
   int _bluePin;
   int _redPin;
   bool _redState = false;
   bool _blueState = false;

  public:
   Lights(int bluePin, int redPin);
   void toggleRed();
   void toggleBlue();
   void turnOffRed();
   void turnOffBlue();
};
#endif