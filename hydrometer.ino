#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

#define CONFIG_MODE false

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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
  String ssid, pass;

  if (wifiCredentialsReady(&ssid, &pass)) {
    if(CONFIG_MODE) {
      initWiFi(ssid, pass);
      initMpu6050(true);
      setupStateServer();
    } else {
      // do the stuff we need to do to log once
      initMpu6050(false);
      float angle, temperature;
      measure(&angle, &temperature);
      if (!postOneUpdate(ssid, pass, angle, temperature)) {
        // TODO: if we get here, it means we CAN'T connect to the wifi, then we have to drop down into configuration mode (probably by deleting the files
      }
      // then go to sleep, to wake in some amount of time
    }
  } else {
    // the submit POST will set the reset flag to true to signal the loop
    // i think we should turn on a LONG flasher and have the post turn it off
    setupAccessPoint();
  }
}

void loop() {
  if (restart){
    delay(5000);
    ESP.restart();
  } else {
    MDNS.update();
  }
}

