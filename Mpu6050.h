#ifndef _MPU6050_H
#define _MPU5060_H
#include <Arduino.h>
#include <math.h>
#include <Adafruit_MPU6050.h>
#include <RunningMedian.h>
#include "FileSystem.h"

#define N_SAMPLES 49
#define N_SAMPLES_TO_AVG 29

class Mpu6050 {
  private:
   Adafruit_MPU6050 _mpu;
   RunningMedian samples = RunningMedian(N_SAMPLES);
   void sleepMpu6050(bool shouldSleep);
   float x_offset = 0;
   float z_offset = 0;

  public:
   Mpu6050(){};
   bool init();
   void sleep() {sleepMpu6050(true);};
   void wake() {sleepMpu6050(false);};
   float measureAngle();
   void tare();
};
#endif