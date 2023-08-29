#ifndef _BATTERY_H
#define _BATTERY_H
#include <Arduino.h>

#define BATTERY_COUNTS_HIGH 783
#define BATTERY_COUNTS_LOW 580

class Battery {
  public:
   static long getPercentage();
};
#endif