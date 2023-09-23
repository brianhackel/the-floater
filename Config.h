#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define DEFAULT_SLEEP_US 30 * 60000000

class Config {
  private:
    struct Conf {
      int configMode;
      long sleepDurationUs;
    };

    static const char* filename;
    Conf _conf;
    void load();
    void save();

  public:
    Config();
    bool isConfigMode();
    void setConfigMode(bool configMode);
    long getSleepDurationUs();
    void setSleepDuration(long durationUs);


};
#endif