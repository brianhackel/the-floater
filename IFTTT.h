#ifndef _IFTTT_H
#define _IFTTT_H
#include "Poster.h"

class IFTTT : public Poster {
  private:
    const String _event;

  public:
    IFTTT(const String key, const String eventName) : Poster(key), _event(eventName) {};
    bool postOneUpdate(float angle, float temperature, long battery) override;
};

#endif
