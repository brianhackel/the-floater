#include <Wire.h>
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
#include "IFTTT.h"
#include "Mpu6050.h"

#define CONFIG_MODE false
#define DEEPSLEEP_DURATION 60e6  // microseconds
#define RED_LED 0
#define BLUE_LED 2

// FIXME: move the key to the configuration and into LittleFS
#define IFTTT_KEY "cnyJ7UpiB9U1QAAfP7mQo5"        // Webhooks Key
#define IFTTT_EVENT "append_beer"                 // Webhooks Event Name

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean restart = false;

Temperature t;
Lights lights(BLUE_LED, RED_LED);
TickTwo redBlinker([](){lights.toggleRed();}, 250, 0, MILLIS);
TickTwo blueBlinker([](){lights.toggleBlue();}, 250, 0, MILLIS);
FileSystem fileSys;
IFTTT poster(IFTTT_KEY, IFTTT_EVENT);
Mpu6050 mpu;

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
      tStart = millis();
      redBlinker.start();
      while (millis() - tStart < 3000) {
        redBlinker.update();
      }
      redBlinker.stop();
      // TODO: purging the files to drop down to captive portal mode
      fileSys.writeSsidToFile("");
      fileSys.writePassToFile("");
      ESP.restart();
    }
    // at this point, we have successfully connected to WiFi
    blueBlinker.stop();
    lights.turnOffBlue();
    mpu.init(); // TODO: read the bool and respond accordingly
    t.begin(); // TODO: read the bool and respond accordingly
    if(CONFIG_MODE) {
      // Route for root / web page
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false);
      });
      
      server.serveStatic("/", LittleFS, "/");

      server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
        String msg = "{\"angle\":\"" + String(mpu.measureAngle()) + "\", \"temperature\":\"" + String(t.getTemperatureF()) + "\"}";
        request->send(200, "application/json", msg);
      });
      
      server.begin();
      blueBlinker.stop();
      // we want the config mode to blink SLOW
      blueBlinker = TickTwo([](){lights.toggleBlue();}, 1000, 0, MILLIS);
      blueBlinker.start();
    } else {
      // do the stuff we need to do to log once
      float angle = mpu.measureAngle();
      float temperature = t.getTemperatureF();
      if (!poster.postOneUpdate(angle, temperature)) {
        // we failed to post an update
        // blink red for 3 seconds to show failure
        tStart = millis();
        redBlinker.start();
        while (millis() - tStart < 3000) {
          redBlinker.update();
        }
        redBlinker.stop();
        lights.turnOffRed();
      }
      // then go to sleep, to wake in some amount of time
      Serial.println("going to sleep: " + String(DEEPSLEEP_DURATION / 1000000));
      mpu.sleep();
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
    blueBlinker.update();
    redBlinker.update();
    MDNS.update();
  }
}

