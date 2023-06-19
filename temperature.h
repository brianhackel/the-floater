#ifndef _TEMPERATURE_H
#define _TEMPERATURE_H
#include "Adafruit_MCP9808.h"

class Temperature {
  private:
   Adafruit_MCP9808 tempsensor;

  public:
   Temperature() {};
   bool begin();
   float getTemperatureF();
};
#endif