#ifndef _POSTER_H
#define _POSTER_H
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "FileSystem.h"

class Poster {
  protected:
    const String _url;
    const String _name;

  public:
    Poster(const String& url, const String& name) : _url(url), _name(name) {};
    bool postOneUpdate(float angle, float temperature, long batteryPercentage);
    virtual String getJson(float angle, float temperature, long batteryPercentage) const = 0;
};

#endif
