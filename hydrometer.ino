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
const int ledPin = 2;

void setup() {
  pinMode(ledPin, OUTPUT);
/*  int reason = ESP.getResetInfoPtr()->reason;

  switch (reason) {
      case REASON_DEFAULT_RST:
          // Normal Power up
          break;
      case REASON_WDT_RST:
          break;
      case REASON_EXCEPTION_RST:
          break;
      case REASON_SOFT_WDT_RST:
          break;
      case REASON_SOFT_RESTART:
          break;
      case REASON_DEEP_SLEEP_AWAKE:
          break;
      case REASON_EXT_SYS_RST:
          break;

      default:
          break;
      }
      REASON_DEFAULT_RST = 0, // normal startup by power on 
      REASON_WDT_RST = 1, // hardware watch dog reset 
      REASON_EXCEPTION_RST = 2, // exception reset, GPIO status won’t change 
      REASON_SOFT_WDT_RST   = 3, // software watch dog reset, GPIO status won’t change 
      REASON_SOFT_RESTART = 4, // software restart ,system_restart , GPIO status won’t change 
      REASON_DEEP_SLEEP_AWAKE = 5, // wake up from deep-sleep 
      REASON_EXT_SYS_RST      = 6 // external system reset 
      */
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
    // TODO: we want to flash the pin ONLY if we're not connected to WiFi
//    digitalWrite(ledPin, LOW);
//    delay(250);
//    digitalWrite(ledPin, HIGH);
//    delay(250);
    /*
    float angle = startingAngle - getDisplacementAngle();
    Serial.print("Displacement Angle: ");
    Serial.print(angle);
    Serial.println(" degrees");

    delay(2000);*/
  }
}

