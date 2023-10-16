#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define DEFAULT_SLEEP_US 30 * 60000000
#define WIFI_CRED_MAX_LEN 64
#define LOG_KEY_STR_LEN 128
#define N_COEFFICIENTS 3 // squared, first-degree, constant

enum class LogType {None, IFTTT, BrewersFriend, GoogleSheets};
enum class Mode {Config, Operate, Standby};

class Config {
  private:
    struct Conf {
      Mode mode;
      long sleepDurationUs;
      char ssid[WIFI_CRED_MAX_LEN];
      char pass[WIFI_CRED_MAX_LEN];
      float offset_x;
      float offset_z;
      LogType logType;
      char iftttKey[LOG_KEY_STR_LEN];
      char iftttEventName[LOG_KEY_STR_LEN];
      char brewersFriendKey[LOG_KEY_STR_LEN];
      float coefficients[N_COEFFICIENTS]; // 0-->squared, 1-->first-degree, 2-->constant
      char googleSheetsDeploymentId[LOG_KEY_STR_LEN];
      char googleSheetsSpreadsheetId[LOG_KEY_STR_LEN];
      char googleSheetsSheetName[LOG_KEY_STR_LEN];
    };

    Conf _conf;
    void load();

  public:
    static const char* filename;
    Config();
    void save();
    void print();
    Mode getMode();
    void setMode(Mode mode);
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
    void getCoeffs(float *c2, float *c1, float *c0);
    void setCoeffs(const float c2, const float c1, const float c0);
    bool getGoogleSheetsDetails(String *deploymentId, String *spreadsheetId, String *sheetName);
    void setGoogleSheetsDetails(const String& deploymentId, const String& spreadsheetId, const String& sheetName);
};
#endif