#ifndef _BREWERS_FRIEND_H
#define _BREWERS_FRIEND_H
#include "Poster.h"

class BrewersFriend : public Poster {
  private:
    const String BASE_URL = "https://log.brewersfriend.com/stream/";
    const String NAME = "BrewersFriend";
    
  public:
    BrewersFriend(const String key) : Poster(BASE_URL + key, NAME) {};
    String getJson(float angle, float temperature, long battery) override;
};


#endif
