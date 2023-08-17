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
#include <DoubleResetDetector.h>
#include <DNSServer.h>

#define RED_LED 0
#define BLUE_LED 2

#define N_MAX_CONSECUTIVE_FAILURES 5

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

AsyncWebServer server(80);
DNSServer dnsServer;
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
  unsigned long sleepUs = FileSystem::getSleepDurationUs();
  Serial.println("going to sleep for " + String(sleepUs / 1000000) + " seconds");
  mpu.sleep();
  drd.stop();
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

  if (drd.detectDoubleReset()) {
    drd.stop();
    FileSystem::setConfigMode(true);
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
    if (!initWiFi(ssid, pass)) {
      if (FileSystem::getConsecutiveFailures() > N_MAX_CONSECUTIVE_FAILURES) {
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
    FileSystem::setConfigMode(false);
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
    if (FileSystem::isConfigMode()) {
      MDNS.update();
    } else {
      dnsServer.processNextRequest();
    }
  }
  // checking for double reset press
  drd.loop();
}

