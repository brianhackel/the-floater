#ifndef _IFTTT_H
#define _IFTTT_H
#include "Poster.h"

class IFTTT : public Poster {
  private:
    const String BASE_URL = "http://maker.ifttt.com/trigger/";
    const String NAME = "IFTTT";

  public:
    IFTTT(const String key, const String eventName) : Poster(BASE_URL + eventName + "/json/with/key/" + key, NAME) {};
    String getJson(float angle, float temperature, long batteryPercentage) override;
};

#endif
