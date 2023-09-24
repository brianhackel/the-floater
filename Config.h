#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define DEFAULT_SLEEP_US 30 * 60000000
#define WIFI_CRED_MAX_LEN 64

class Config {
  private:
    struct Conf {
      int configMode;
      long sleepDurationUs;
      char ssid[WIFI_CRED_MAX_LEN];
      char pass[WIFI_CRED_MAX_LEN];
    };

    static const char* filename;
    Conf _conf;
    void load();

  public:
    Config();
    void save();
    bool isConfigMode();
    void setConfigMode(bool configMode);
    long getSleepDurationUs();
    void setSleepDuration(long durationUs);
    bool areWifiCredentialsReady(String *ssid, String *pass);
    void setWifiCredentials(const String& ssid, const String& pass);
};
#endif