#ifndef _POSTER_H
#define _POSTER_H
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

class Poster {
  protected:
    const String _key;

  public:
    Poster(const String key) : _key(key) {};
    virtual bool postOneUpdate(float angle, float temperature, long battery) = 0;
};

class IFTTT : public Poster {
  private:
    const String _event;

  public:
    IFTTT(const String key, const String eventName) : Poster(key), _event(eventName) {};
    bool postOneUpdate(float angle, float temperature, long battery);
};

class BrewersFriend : public Poster {
  public:
    BrewersFriend(const String key) : Poster(key) {};
    bool postOneUpdate(float angle, float temperature, long battery);
};

#endif
