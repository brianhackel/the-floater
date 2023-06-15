#include <math.h>
#include <RunningMedian.h>

Adafruit_MPU6050 mpu;
float startingAngle;
// TODO: this should be moved to a calibration field
#define TEMPERATURE_CALIBRATION_OFFSET_DEGREES_F 10
#define N_SAMPLES 49
#define N_SAMPLES_TO_AVG 29
RunningMedian angleSamples = RunningMedian(N_SAMPLES);
RunningMedian tempSamples = RunningMedian(N_SAMPLES);

// TODO: consider having this return a bool for success/failure
void initMpu6050(bool config) {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.enableSleep(false);

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  
  if (config) {
    float temperature;
    measure(&startingAngle, &temperature);
    Serial.print("starting Angle: ");
    Serial.print(startingAngle);
    Serial.println(" degrees");
  }
}

void setupStateServer() {  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });
  
  server.serveStatic("/", LittleFS, "/");

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    float angle, temperature;
    measure(&angle, &temperature);
    request->send(200, "application/json", "{\"angle\":\"" + String(startingAngle - angle) + "\", \"temperature\":\"" + String(temperature) + "\"}");
  });
  
  server.begin();
}

void measure(float *angle, float *temperature) {
  sensors_event_t a, g, temp;
  float accelX, accelY, accelZ;
  int i;
  angleSamples.clear();
  tempSamples.clear();
  for (i = 0; i < N_SAMPLES; i++) {
    mpu.getEvent(&a, &g, &temp);
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    angleSamples.add(acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI);
    tempSamples.add(temp.temperature);
  }
  *angle = angleSamples.getAverage(N_SAMPLES_TO_AVG);
  *temperature = (tempSamples.getAverage(N_SAMPLES_TO_AVG) * (9.0/5.0)) + 32.0;
  *temperature = *temperature - TEMPERATURE_CALIBRATION_OFFSET_DEGREES_F;
}