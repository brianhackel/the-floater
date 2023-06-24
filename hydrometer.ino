#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <TickTwo.h>
#include "temperature.h"
#include "Lights.h"
#include "FileSystem.h"

#define CONFIG_MODE false
#define DEEPSLEEP_DURATION 60e6  // microseconds
#define RED_LED 0
#define BLUE_LED 2

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean restart = false;

Temperature t;
Lights lights(BLUE_LED, RED_LED);
TickTwo redBlinker([](){lights.flashRed(250);}, 250, 0, MILLIS);
TickTwo blueBlinker([](){lights.flashBlue(250);}, 250, 0, MILLIS);
FileSystem fileSys;

void setup() {
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

  // TODO: returns a bool, error checking please
  fileSys.init();
  String ssid, pass;
  long tStart;

  if (fileSys.wifiCredentialsReady(&ssid, &pass)) {
    if (!initWiFi(ssid, pass)) {
      // blink red for 3 seconds to show failure
      blueBlinker.pause();
      redBlinker.start();
      tStart = millis();
      while (millis() - tStart < 3000) {
        redBlinker.update();
      }
      redBlinker.stop();
      // TODO: consider purging the files to drop down to captive portal mode
    }
    initMpu6050(); // TODO: read the bool and respond accordingly
    t.begin(); // TODO: read the bool and respond accordingly
    if(CONFIG_MODE) {
      setupStateServer();
    } else {
      // do the stuff we need to do to log once
      float angle;
      measure(&angle);
      float temperature = t.getTemperatureF();
      if (!postOneUpdate(angle, temperature)) {
        // we failed to post an update
        // blink red for 3 seconds to show failure
        blueBlinker.pause();
        redBlinker.start();
        tStart = millis();
        while (millis() - tStart < 3000) {
          redBlinker.update();
        }
        redBlinker.stop();
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
    blueBlinker.start();
    redBlinker.start();
  }
}

void loop() {
  if (restart){
    delay(5000);
    ESP.restart();
  } else {
    // alternate red and blue
    blueBlinker.update();
    redBlinker.update();
    MDNS.update();
  }
}

