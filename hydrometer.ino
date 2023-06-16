#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <TickTwo.h>

#define CONFIG_MODE false
#define DEEPSLEEP_DURATION 60e6  // microseconds
#define RED_LED 0
#define BLUE_LED 2

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean restart = false;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

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
    initWiFi(ssid, pass); // FIXME: this now returns a bool; we should stop and respond if it's false
    if(CONFIG_MODE) {
      initMpu6050(true);
      setupStateServer();
    } else {
      // do the stuff we need to do to log once
      initMpu6050(false);
      float angle, temperature;
      measure(&angle, &temperature);
      if (!postOneUpdate(angle, temperature)) {
        // TODO: if we get here, it means we CAN'T connect to the wifi, then we have to drop down into configuration mode (probably by deleting the files)
      }
      // then go to sleep, to wake in some amount of time
      Serial.println("going to sleep: " + String(DEEPSLEEP_DURATION / 1000000));
      sleepMpu6050(true);
      delay(500);
      ESP.deepSleep(DEEPSLEEP_DURATION);
      delay(200);
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

