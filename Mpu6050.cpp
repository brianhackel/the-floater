#include "Mpu6050.h"


bool Mpu6050::init() {
  if (!this->_mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return false;
  }

  wake();

  this->_mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  this->_mpu.setGyroStandby(true, true, true);
  this->_mpu.setTemperatureStandby(true);
  this->_mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  return true;
}

void Mpu6050::sleepMpu6050(bool shouldSleep) {
   this->_mpu.enableSleep(shouldSleep);
}

float Mpu6050::measureAngle() {
  sensors_event_t a;
  float accelX, accelY, accelZ;
  this->samples.clear();
  for (int i = 0; i < N_SAMPLES; i++) {
    this->_mpu.getAccelerometerSensor()->getEvent(&a);
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    this->samples.add(acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI);
  }
  return this->samples.getAverage(N_SAMPLES_TO_AVG);
}
