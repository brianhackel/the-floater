Adafruit_MPU6050 mpu;
float startingAngle;

// TODO: consider having this return a bool for success/failure
void initMpu6050() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  
  startingAngle = getDisplacementAngle();
  Serial.print("starting Angle: ");
  Serial.print(startingAngle);
  Serial.println(" degrees");
}

float getDisplacementAngle() {
  sensors_event_t a, g, temp;
  // TODO: need a function for getting the readings through averaging
  mpu.getEvent(&a, &g, &temp);
  float accelX = a.acceleration.x;
  float accelY = a.acceleration.y;
  float accelZ = a.acceleration.z;
  return acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI;
}