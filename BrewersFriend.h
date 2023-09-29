#ifndef _BREWERS_FRIEND_H
#define _BREWERS_FRIEND_H
#include "Poster.h"
#include "Config.h"

class BrewersFriend : public Poster {
  private:
    static const String BASE_URL;
    static const String NAME;
    float coefficients[N_COEFFICIENTS];
    
  public:
    BrewersFriend(const String& key, float c2, float c1, float c0) : Poster(BASE_URL + key, NAME) {
      coefficients[0] = c2;
      coefficients[1] = c1;
      coefficients[2] = c0;
    };
    String getJson(float angle, float temperature, long battery) const override;
};


#endif
