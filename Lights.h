#ifndef _LIGHTS_H
#define _LIGHTS_H
#include <Arduino.h>

class Lights {
  private:
   int _bluePin;
   int _redPin;
   void flash(int whichPin, int millis);

  public:
   Lights(int bluePin, int redPin);
   void flashRed(int millis);
   void flashBlue(int millis);
};
#endif