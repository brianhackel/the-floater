#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
  private:
    static const char* coefficientsPath;
    static String readFile(const char *path);
    static void writeFile(const char *path, const char *message);

  public:
    FileSystem() {};
    static bool init();
    static bool getCoeffs(float *c3, float *c2, float *c1, float *c0);
    static void writeCoeffsToFile(const float c3, const float c2, const float c1, const float c0);

};
#endif