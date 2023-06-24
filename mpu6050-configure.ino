#include <math.h>
#include <RunningMedian.h>

Adafruit_MPU6050 mpu;
#define N_SAMPLES 49
#define N_SAMPLES_TO_AVG 29
RunningMedian angleSamples = RunningMedian(N_SAMPLES);

bool initMpu6050() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    return false;
  }

  sleepMpu6050(false);

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroStandby(true, true, true);
  mpu.setTemperatureStandby(true);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  return true;
}

void sleepMpu6050(bool shouldSleep) {
  mpu.enableSleep(shouldSleep);
}

void setupStateServer() {  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });
  
  server.serveStatic("/", LittleFS, "/");

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    float angle;
    float temperature = 0.0;
    measure(&angle);
    // TODO: this needs to move out so we can incorporate temperature from the MCP9808
    //request->send(200, "application/json", "{\"angle\":\"" + String(startingAngle - angle) + "\", \"temperature\":\"" + String(temperature) + "\"}");
    request->send(200, "application/json", "{\"angle\":\"" + String(angle) + "\", \"temperature\":\"" + String(temperature) + "\"}");
  });
  
  server.begin();
}

void measure(float *angle) {
  sensors_event_t a;
  float accelX, accelY, accelZ;
  int i;
  angleSamples.clear();
  for (i = 0; i < N_SAMPLES; i++) {
    mpu.getAccelerometerSensor()->getEvent(&a);
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    angleSamples.add(acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI);
  }
  *angle = angleSamples.getAverage(N_SAMPLES_TO_AVG);
}