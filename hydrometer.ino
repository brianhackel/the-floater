#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

boolean restart = false;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  initFS();

  if(initWiFi()) {
    initMpu6050();
    setupLedServer();
  } else {
    setupAccessPoint();
  }
}

void loop() {
  if (restart){
    delay(5000);
    ESP.restart();
  } else {
    MDNS.update();
    /*
    float angle = startingAngle - getDisplacementAngle();
    Serial.print("Displacement Angle: ");
    Serial.print(angle);
    Serial.println(" degrees");

    delay(2000);*/
  }
}

