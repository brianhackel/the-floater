#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
  private:
    // File paths to save input values permanently
    static const char* iftttKeyPath;
    static const char* iftttEventPath;
    static const char* brewersFriendKeyPath;
    static const char* logTypePath;
    static const char* consecutiveFailuresPath;
    static const char* allowedFailuresPath;
    static const char* coefficientsPath;
    static const char* offsetsPath;
    static String readFile(const char *path);
    static void writeFile(const char *path, const char *message);

  public:
    FileSystem() {};
    static bool init();
    static bool getIftttDetails(String *key, String *event);
    static bool getBrewersFriendKey(String *key);
    static bool getCoeffs(float *c3, float *c2, float *c1, float *c0);
    static unsigned int getAllowedFailures();
    static unsigned int getConsecutiveFailures();
    static void incrementConsecutiveFailures();

    static void writeBrewersFriendKeyToFile(const char* key) { writeFile(brewersFriendKeyPath, key); }
    static void writeLogTypeToFile(const char* type) { writeFile(logTypePath, type); }
    static void writeIftttKeyToFile(const char* key) { writeFile(iftttKeyPath, key); }
    static void writeIftttEventToFile(const char* event) { writeFile(iftttEventPath, event); }
    static void writeAllowedFailures(const unsigned int n) { writeFile(allowedFailuresPath, String(n).c_str()); }
    static void resetConsecutiveFailures() { writeFile(consecutiveFailuresPath, "0"); }
    static void writeCoeffsToFile(const float c3, const float c2, const float c1, const float c0);

    static void clearIfttt() {
      LittleFS.remove(iftttKeyPath);
      LittleFS.remove(iftttEventPath);
    }

    static void clearBrewersFriend() { LittleFS.remove(brewersFriendKeyPath); }

    static void clearLoggingConfigs() {
      clearBrewersFriend();
      clearIfttt();
    }

    static void clearAll() {
      resetConsecutiveFailures();
      clearLoggingConfigs();
    }
};
#endif