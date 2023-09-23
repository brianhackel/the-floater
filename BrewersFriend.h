#ifndef _BREWERS_FRIEND_H
#define _BREWERS_FRIEND_H
#include "Poster.h"

class BrewersFriend : public Poster {
  private:
    static const String BASE_URL;
    static const String NAME;
    
  public:
    BrewersFriend(const String& key) : Poster(BASE_URL + key, NAME) {};
    String getJson(float angle, float temperature, long battery) const override;
};


#endif
