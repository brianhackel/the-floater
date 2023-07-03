#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
  private:
    // File paths to save input values permanently
    static const char* ssidPath;
    static const char* passPath;
    static const char* configModePath;
    static const char* sleepDurationPath;
    static const char* iftttKeyPath;
    static const char* iftttEventPath;
    static const char* brewersFriendKeyPath;
    static const char* logTypePath;
    static String readFile(const char *path);
    static void writeFile(const char *path, const char *message);

  public:
    FileSystem() {};
    static bool init();
    static bool wifiCredentialsReady(String *ssid, String *pass);
    static void setConfigMode(bool configMode) {
      writeFile(configModePath, configMode ? "1" : "0");
    };
    static bool getIftttDetails(String *key, String *event);
    static bool getBrewersFriendKey(String *key);
    static bool isConfigMode();
    static int getSleepDurationUs();
    static void writeBrewersFriendKeyToFile(const char* key) {
      writeFile(brewersFriendKeyPath, key);
    };
    static void writeLogTypeToFile(const char* type) {
      writeFile(logTypePath, type);
    };
    static void writeIftttKeyToFile(const char* key) {
      writeFile(iftttKeyPath, key);
    };
    static void writeIftttEventToFile(const char* event) {
      writeFile(iftttEventPath, event);
    };
    static void writeSleepDurationToFile(const int durationUs) {
      writeFile(sleepDurationPath, String(durationUs).c_str());
    };
    static void writeSsidToFile(const char* message) {
      writeFile(ssidPath, message);
    };
    static void writePassToFile(const char* message) {
      writeFile(passPath, message);
    };
    static void clearIfttt() {
      LittleFS.remove(iftttKeyPath);
      LittleFS.remove(iftttEventPath);
    };
    static void clearBrewersFriend() {
      LittleFS.remove(brewersFriendKeyPath);
    }
    static void clearWifi() {
      LittleFS.remove(ssidPath);
      LittleFS.remove(passPath);
    };
    static void clearLoggingConfigs() {
      clearBrewersFriend();
      clearIfttt();
    };
    static void clearAll() {
      clearWifi();
      setConfigMode(true);
      clearBrewersFriend();
      clearIfttt();
    };
};
#endif