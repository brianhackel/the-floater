#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Arduino.h>
#include <LittleFS.h>

class FileSystem {
  private:
    // File paths to save input values permanently
    static const char* ssidPath;
    static const char* passPath;

  public:
   FileSystem() {};
   bool init();
   bool wifiCredentialsReady(String *ssid, String *pass);
   static String readFile(const char *path);
   static void writeFile(const char *path, const char *message);
   static void writeSsidToFile(const char *message) {
     writeFile(ssidPath, message);
   };
   static void writePassToFile(const char *message) {
     writeFile(passPath, message);
   };
};
#endif