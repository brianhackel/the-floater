#include "Battery.h"

long Battery::getPercentage() {
  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 1M & 220K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  // BMH: empirically, we will use different values (see header for definitions)
  int level = analogRead(A0);

  // convert battery level to percent
  level = map(level, BATTERY_COUNTS_LOW, BATTERY_COUNTS_HIGH, 0, 100);
  return level;
}
