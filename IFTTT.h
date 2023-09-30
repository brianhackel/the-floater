#ifndef _IFTTT_H
#define _IFTTT_H
#include "Poster.h"

class IFTTT : public Poster {
  private:
    static const String BASE_URL;
    static const String NAME;

  public:
    IFTTT(const String& key, const String& eventName);
    String getJson(float angle, float temperature, long batteryPercentage) const override;
};

#endif
