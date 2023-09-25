#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
  private:
    // File paths to save input values permanently
    static const char* consecutiveFailuresPath;
    static const char* allowedFailuresPath;
    static const char* coefficientsPath;
    static String readFile(const char *path);
    static void writeFile(const char *path, const char *message);

  public:
    FileSystem() {};
    static bool init();
    static bool getCoeffs(float *c3, float *c2, float *c1, float *c0);
    static unsigned int getAllowedFailures();
    static unsigned int getConsecutiveFailures();
    static void incrementConsecutiveFailures();

    static void writeAllowedFailures(const unsigned int n) { writeFile(allowedFailuresPath, String(n).c_str()); }
    static void resetConsecutiveFailures() { writeFile(consecutiveFailuresPath, "0"); }
    static void writeCoeffsToFile(const float c3, const float c2, const float c1, const float c0);

    static void clearAll() {
      resetConsecutiveFailures();
    }
};
#endif