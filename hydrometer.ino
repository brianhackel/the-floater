#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <TickTwo.h>
#include "temperature.h"
#include "Lights.h"
#include "FileSystem.h"
#include "Mpu6050.h"
#include <DNSServer.h>
#include "Battery.h"
#include "BrewersFriend.h"
#include "IFTTT.h"

#define VERSION "1.0.0"

#define RED_LED 0
#define BLUE_LED 2
#define CONFIG_MODE_TIMEOUT_MILLIS 300000  // 5 minutes

AsyncWebServer server(80);
DNSServer dnsServer;
boolean restart = false;
boolean standby = false;
Temperature t;
Lights lights(BLUE_LED, RED_LED);
TickTwo redBlinker([](){lights.toggleRed();}, 250, 0, MILLIS);
TickTwo blueBlinker([](){lights.toggleBlue();}, 250, 0, MILLIS);
Mpu6050 mpu;
bool configMode = false;
long configStartMs;
long activePortalStartMs;

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
  unsigned long sleepUs = FileSystem::getSleepDurationUs();
  Serial.println("going to sleep for " + String(sleepUs / 1000000) + " seconds");
  mpu.sleep();
  delay(500);
  ESP.deepSleep(sleepUs);
  delay(200);
}

void setup() {
  Serial.begin(115200);

  if (!FileSystem::init()) {
    Serial.println("File System failed to init");
    flashError();
    ESP.restart();
  }
  String ssid, pass;
  long tStart;

  configMode = FileSystem::isConfigMode();

  rst_info *rinfo;
  rinfo = ESP.getResetInfoPtr();

  if (rinfo->reason == REASON_EXT_SYS_RST) {
    if (configMode) {
      FileSystem::clearAll();
    } else {
      FileSystem::setConfigMode(true);
    }
    blueBlinker.stop();
    redBlinker.stop();
    lights.turnOffRed();
    lights.toggleRed();
    delay(3);
    lights.turnOffRed();
    restart = true;
    return;
  }

  if (FileSystem::wifiCredentialsReady(&ssid, &pass)) {
    if (Battery::getPercentage() < 20) {
      lights.turnOffRed();
      lights.toggleRed();
    }
    if (!initWiFi(ssid, pass)) {
      if (configMode || (FileSystem::getConsecutiveFailures() > FileSystem::getAllowedFailures())) {
        // purging the files to drop down to captive portal mode
        FileSystem::clearAll();
        ESP.restart();
      } else {
        FileSystem::incrementConsecutiveFailures();
        sleep();
      }
    }
    // at this point, we have successfully connected to WiFi
    FileSystem::resetConsecutiveFailures();
    blueBlinker.stop();
    lights.turnOffBlue();
    if (!mpu.init()) {
      Serial.println("Could not init accelerometer");
      flashError();
      sleep();
    }
    if (!t.begin()) {
      Serial.println("Could not init temperature sensor");
      flashError();
      sleep();
    }
    if(configMode) {
      setupStateServer();
      blueBlinker.stop();
      // we want the config mode to blink SLOW
      blueBlinker = TickTwo([](){lights.toggleBlue();}, 1000, 0, MILLIS);
      blueBlinker.start();
      configStartMs = millis();
    } else {
      // do the stuff we need to do to log once
      String key;
      String event;
      Poster *poster;
      if (FileSystem::getIftttDetails(&key, &event)) {
        poster = new IFTTT(key, event);
      } else if (FileSystem::getBrewersFriendKey(&key)) {
        poster = new BrewersFriend(key);
      } else {
        // nothing configured for logging, that's fine. we just won't log until we're configured properly
      }
      if (poster != NULL && !poster->postOneUpdate(mpu.measureAngle(), t.getTemperatureF(), Battery::getPercentage())) {
        // we failed to post an update
        // blink red for 3 seconds to show failure
        flashError();
      }
      sleep();
    }
  } else {
    // the submit POST will set the reset flag to true to signal the loop
    // i think we should turn on a LONG flasher and have the post turn it off
    FileSystem::setConfigMode(false);
    setupAccessPoint();
    activePortalStartMs = millis();
    blueBlinker.start();
    redBlinker.start();
  }
}

void loop() {
  if (standby) {
    delay(1000);
    ESP.deepSleep(0);
  }
  if (restart) {
    delay(1000);
    ESP.restart();
  } else {
    blueBlinker.update();
    redBlinker.update();
    if (FileSystem::isConfigMode()) {
      if (millis() - configStartMs > CONFIG_MODE_TIMEOUT_MILLIS) {
        FileSystem::setConfigMode(false);
        restart = true;
        return;
      }
      MDNS.update();
    } else {
      if (millis() - activePortalStartMs > CONFIG_MODE_TIMEOUT_MILLIS) {
        standby = true;
        return;
      }
      dnsServer.processNextRequest();
    }
  }
}

