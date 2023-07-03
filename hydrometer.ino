#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <TickTwo.h>
#include "temperature.h"
#include "Lights.h"
#include "FileSystem.h"
#include "IFTTT.h"
#include "Mpu6050.h"

#define RED_LED 0
#define BLUE_LED 2

AsyncWebServer server(80);
boolean restart = false;
Temperature t;
Lights lights(BLUE_LED, RED_LED);
TickTwo redBlinker([](){lights.toggleRed();}, 250, 0, MILLIS);
TickTwo blueBlinker([](){lights.toggleBlue();}, 250, 0, MILLIS);
Mpu6050 mpu;
bool configMode = false;

void flashError() {
  // blink red for 3 seconds to show failure
  unsigned long tStart = millis();
  blueBlinker.stop();
  redBlinker.start();
  while (millis() - tStart < 3000) {
    redBlinker.update();
  }
  redBlinker.stop();
  lights.turnOffRed();
}

void sleep() {
  int sleepUs = FileSystem::getSleepDurationUs();
  Serial.println("in sleepDuration.txt : " + String(sleepUs));
  Serial.println("going to sleep for " + String(sleepUs / 1000000) + " seconds");
  mpu.sleep();
  delay(500);
  ESP.deepSleep(sleepUs);
  delay(200);
}

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

  if (!FileSystem::init()) {
    Serial.println("File System failed to init");
    flashError();
    ESP.restart();
  }
  String ssid, pass;
  long tStart;

  configMode = FileSystem::isConfigMode();

  if (FileSystem::wifiCredentialsReady(&ssid, &pass)) {
    if (!initWiFi(ssid, pass)) {
      flashError();
      // purging the files to drop down to captive portal mode
      FileSystem::clearAll();
      ESP.restart();
    }
    // at this point, we have successfully connected to WiFi
    blueBlinker.stop();
    lights.turnOffBlue();
    if (!mpu.init()) {
      Serial.println("Could not init accelerometer");
      flashError();
      ESP.restart();
    }
    if (!t.begin()) {
      Serial.println("Could not init temperature sensor");
      flashError();
      ESP.restart();
    }
    if(configMode) {
      setupStateServer();
      blueBlinker.stop();
      // we want the config mode to blink SLOW
      blueBlinker = TickTwo([](){lights.toggleBlue();}, 1000, 0, MILLIS);
      blueBlinker.start();
    } else {
      // do the stuff we need to do to log once
      String key;
      String event;
      if (FileSystem::getIftttDetails(&key, &event)) {
        IFTTT poster(key, event);
        if (!poster.postOneUpdate(mpu.measureAngle(), t.getTemperatureF())) {
          // we failed to post an update
          // blink red for 3 seconds to show failure
          flashError();
        }
      } else if (FileSystem::getBrewersFriendKey(&key)) {
        Serial.println("ERROR: brewers friend not implemented yet!");
        FileSystem::clearBrewersFriend();
      } else {
        FileSystem::setConfigMode(true);
      }
      sleep();
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
    blueBlinker.update();
    redBlinker.update();
    if (configMode) {
      MDNS.update();
    }
  }
}

