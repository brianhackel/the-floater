#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H
#include <Wire.h>
#include "Adafruit_MCP9808.h"

class Temperature {
  private:
   Adafruit_MCP9808 tempsensor;

  public:
   Temperature();
   float getTemperatureF();
};
#endif