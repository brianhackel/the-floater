#ifndef _POSTER_H
#define _POSTER_H
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "FileSystem.h"

class Poster {
  protected:
    const String _key;

  public:
    Poster(const String key) : _key(key) {};
    virtual bool postOneUpdate(float angle, float temperature, long battery) = 0;
};

#endif
