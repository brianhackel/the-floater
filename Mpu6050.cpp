#include "Mpu6050.h"

bool Mpu6050::init() {
  if (!this->_mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return false;
  }

  wake();

  FileSystem::getOffsets(&x_offset, &z_offset);

  this->_mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  this->_mpu.setGyroStandby(true, true, true);
  this->_mpu.setTemperatureStandby(true);
  this->_mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  return true;
}

void Mpu6050::sleepMpu6050(bool shouldSleep) {
   this->_mpu.enableSleep(shouldSleep);
}

void Mpu6050::tare() {
  sensors_event_t a;
  float accelX, accelY, accelZ;
  RunningMedian ex = RunningMedian(N_SAMPLES);
  RunningMedian zee = RunningMedian(N_SAMPLES);
  for (int i = 0; i < N_SAMPLES; i++) {
    this->_mpu.getAccelerometerSensor()->getEvent(&a);
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    ex.add(accelX);
    zee.add(accelZ);
  }
  FileSystem::writeOffsetsToFile(ex.getAverage(N_SAMPLES_TO_AVG), zee.getAverage(N_SAMPLES_TO_AVG));
}

float Mpu6050::measureAngle() {
  sensors_event_t a;
  float accelX, accelY, accelZ;
  this->samples.clear();
  for (int i = 0; i < N_SAMPLES; i++) {
    this->_mpu.getAccelerometerSensor()->getEvent(&a);
    accelX = a.acceleration.x - x_offset;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z - z_offset;
    this->samples.add(acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI);
  }
  return this->samples.getAverage(N_SAMPLES_TO_AVG);
}
