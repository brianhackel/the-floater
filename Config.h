#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define DEFAULT_SLEEP_US 30 * 60000000
#define WIFI_CRED_MAX_LEN 64
#define LOG_KEY_STR_LEN 128

enum class LogType {None, IFTTT, BrewersFriend};

class Config {
  private:
    struct Conf {
      int configMode;
      long sleepDurationUs;
      char ssid[WIFI_CRED_MAX_LEN];
      char pass[WIFI_CRED_MAX_LEN];
      float offset_x;
      float offset_z;
      LogType logType;
      char iftttKey[LOG_KEY_STR_LEN];
      char iftttEventName[LOG_KEY_STR_LEN];
      char brewersFriendKey[LOG_KEY_STR_LEN];
    };

    static const char* filename;
    Conf _conf;
    void load();

  public:
    Config();
    void save();
    void print();
    bool isConfigMode();
    void setConfigMode(bool configMode);
    long getSleepDurationUs();
    void setSleepDuration(long durationUs);
    bool areWifiCredentialsReady(String *ssid, String *pass);
    void setWifiCredentials(const String& ssid, const String& pass);
    void clearWifiCredentials() {setWifiCredentials("", "");};
    void getOffsets(float *x, float *z);
    void setOffsets(float x, float z);
    bool getIftttDetails(String *key, String *event);
    bool getBrewersFriendKey(String *key);
    LogType getLogType();
    void setIftttDetails(const String& key, const String& event);
    void setBrewersFriendDetails(const String& key);
    void setLogType(LogType type);
    void clearLoggingConfigs() { setLogType(LogType::None); };
};
#endif