#include <math.h>
#include <RunningMedian.h>

Adafruit_MPU6050 mpu;
float startingAngle;
#define N_SAMPLES 49
#define N_SAMPLES_TO_AVG 29
RunningMedian samples = RunningMedian(N_SAMPLES);

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

// Replaces placeholder with current displacement from starting
String processor(const String& var) {
  if(var == "STATE") {
    float angle = startingAngle - getDisplacementAngle();
    return String(angle);
  }
  return String();
}

void setupLedServer() {  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });
  
  server.serveStatic("/", LittleFS, "/");
  
  // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });
  server.begin();
}

float getDisplacementAngle() {
  sensors_event_t a, g, temp;
  float accelX, accelY, accelZ;
  int i;
  samples.clear();
  for (i = 0; i < N_SAMPLES; i++) {
    mpu.getEvent(&a, &g, &temp);
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    samples.add(acos(abs(accelY) / (sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ))) * 180.0 / M_PI);
  }
  return samples.getAverage(N_SAMPLES_TO_AVG);
  
}