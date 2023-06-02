#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

Adafruit_MPU6050 mpu;

float startingAngle;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  sensors_event_t startingOrientation, startG, startTemp;
  mpu.getEvent(&startingOrientation, &startG, &startTemp);  // Store the starting orientation

  // TODO: need a function for getting the readings through averaging

  startingAngle = getDisplacementAngle(startingOrientation.acceleration.x, startingOrientation.acceleration.y, startingOrientation.acceleration.z);
  Serial.print("starting Angle: ");
  Serial.print(startingAngle);
  Serial.println(" degrees");

  delay(5000);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.println(a.acceleration.z);

  float angle = startingAngle - getDisplacementAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z);
  Serial.print("Displacement Angle: ");
  Serial.print(angle);
  Serial.println(" degrees");

  delay(2000);
}

float getDisplacementAngle(float accelX, float accelY, float accelZ) {
  // float angleX = atan2(accelY, sqrt(accelX * accelX + accelZ * accelZ));
  // float angleY = atan2(-accelX, sqrt(accelY * accelY + accelZ * accelZ));

  // float angle = sqrt(angleX * angleX + angleY * angleY);
  // return angle * (180.0 / M_PI);

  return acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI;
}
