#ifndef _BATTERY_H
#define _BATTERY_H
#include <Arduino.h>

class Battery {
  public:
   static long getPercentage();
};
#endif