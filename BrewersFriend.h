#ifndef _BREWERS_FRIEND_H
#define _BREWERS_FRIEND_H
#include "Poster.h"

class BrewersFriend : public Poster {
  public:
    BrewersFriend(const String key) : Poster(key) {};
    bool postOneUpdate(float angle, float temperature, long battery) override;
};


#endif
