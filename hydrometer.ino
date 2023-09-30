#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <TickTwo.h>
#include "temperature.h"
#include "Lights.h"
#include "Mpu6050.h"
#include <DNSServer.h>
#include "Battery.h"
#include "BrewersFriend.h"
#include "IFTTT.h"
#include "Config.h"

#define VERSION "1.0.0"

#define RED_LED 0
#define BLUE_LED 2
#define CONFIG_MODE_TIMEOUT_MILLIS 300000  // 5 minutes
#define STANDBY_SLEEP_TIME_US 60 * 60 * 1000000 // 1 hour
#define DEFAULT_ALLOWED_FAILURES 5

AsyncWebServer server(80);
DNSServer dnsServer;
boolean restart = false;
boolean standby = false;
Temperature t;
Lights lights(BLUE_LED, RED_LED);
TickTwo redBlinker([](){lights.toggleRed();}, 250, 0, MILLIS);
TickTwo blueBlinker([](){lights.toggleBlue();}, 250, 0, MILLIS);
Mpu6050 mpu;
Config configuration;
long configStartMs;
long activePortalStartMs;
const String consecutiveFailuresPath = "./consecutiveFailures.txt";

void flashError() {
  server.end();
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
  unsigned long sleepUs = configuration.getSleepDurationUs();
  configuration.save();
  Serial.println("going to sleep for " + String(sleepUs / 1000000) + " seconds");
  mpu.sleep();
  delay(500);
  ESP.deepSleep(sleepUs);
  delay(200);
}

int getConsecutiveFailures() {
  File file = LittleFS.open(consecutiveFailuresPath, "r");
  if(!file || file.isDirectory()){
    Serial.println(String(consecutiveFailuresPath) + " - failed to open file for reading");
    return 0;
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent.isEmpty() ? 0 : strtoul(fileContent.c_str(), NULL, 10);
}

void setConsecutiveFailures(int n) {
  File file = LittleFS.open(consecutiveFailuresPath, "w");
  if(!file){
    Serial.println(String(consecutiveFailuresPath) + " - failed to open file for writing");
    return;
  }
  if(!file.println(String(n))){
    Serial.println(String(consecutiveFailuresPath) + " - frite failed");
  }
  file.close();
}

void incrementConsecutiveFailures() {
  setConsecutiveFailures(getConsecutiveFailures() + 1);
}

void doRestart() {
  server.end();
  delay(1000);
  configuration.save();
  ESP.restart();
}

void setup() {
  Serial.begin(115200);

  configuration.print();

  String ssid, pass;
  long tStart;

  rst_info *rinfo;
  rinfo = ESP.getResetInfoPtr();
  if (rinfo->reason == REASON_EXT_SYS_RST) {
    if (configuration.isConfigMode()) {
      LittleFS.remove(Config::filename);
      configuration.clearWifiCredentials();
    } else {
      configuration.setConfigMode(true);
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

  if (configuration.areWifiCredentialsReady(&ssid, &pass)) {
    if (Battery::getPercentage() < 20) {
      lights.turnOffRed();
      lights.toggleRed();
    }
    if (!initWiFi(ssid, pass)) {
      if (configuration.isConfigMode() || (getConsecutiveFailures() > DEFAULT_ALLOWED_FAILURES)) {
        // purging the files to drop down to captive portal mode
        LittleFS.remove(Config::filename);
        doRestart();
      } else {
        incrementConsecutiveFailures();
        sleep();
      }
    }
    // at this point, we have successfully connected to WiFi
    setConsecutiveFailures(0);
    blueBlinker.stop();
    lights.turnOffBlue();
    float offsetX, offsetZ;
    configuration.getOffsets(&offsetX, &offsetZ);
    if (!mpu.init(offsetX, offsetZ)) {
      Serial.println("Could not init accelerometer");
      flashError();
      sleep();
    }
    if (!t.begin()) {
      Serial.println("Could not init temperature sensor");
      flashError();
      sleep();
    }
    if(configuration.isConfigMode()) {
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
      Poster *poster = NULL;
      switch (configuration.getLogType()) {
        case LogType::IFTTT:
          if (configuration.getIftttDetails(&key, &event))
            poster = new IFTTT(key, event);
          break;
        case LogType::BrewersFriend:
          if (configuration.getBrewersFriendKey(&key)) {
            float c2, c1, c0;
            configuration.getCoeffs(&c2, &c1, &c0);
            poster = new BrewersFriend(key, c2, c1, c0);
          }
          break;
        default:
          // nothing configured for logging, that's fine. we just won't log until we're configured properly
          break;
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
    configuration.setConfigMode(false);
    setupAccessPoint();
    activePortalStartMs = millis();
    blueBlinker.start();
    redBlinker.start();
  }
}

void loop() {
  if (standby) {
    server.end();
    configuration.clearLoggingConfigs();
    configuration.setConfigMode(false);
    configuration.setSleepDuration(STANDBY_SLEEP_TIME_US);
    configuration.save();
    sleep();
  }
  if (restart) {
    doRestart();
  } else {
    blueBlinker.update();
    redBlinker.update();
    if (configuration.isConfigMode()) {
      if (millis() - configStartMs > CONFIG_MODE_TIMEOUT_MILLIS) {
        configuration.setConfigMode(false);
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

