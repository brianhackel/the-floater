#include "temperature.h"

bool Temperature::begin() {
  tempsensor = Adafruit_MCP9808();
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    return false;
  }
  tempsensor.setResolution(1); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
  return true;
}

float Temperature::getTemperatureF() {
  // wake up MCP9808 - power consumption ~200 mikro Ampere
  tempsensor.wake();
  // woke up, ready to read!
  float f = tempsensor.readTempF();
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  return f;
}