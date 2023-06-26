#ifndef _IFTTT_H
#define _IFTTT_H
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

class IFTTT {
  private:
   const String _key;
   const String _event;

  public:
   IFTTT(const String key, const String event) : _key(key), _event(event) {
   };
   bool postOneUpdate(float angle, float temperature);
};
#endif