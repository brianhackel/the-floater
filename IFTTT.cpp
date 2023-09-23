#include "IFTTT.h"

const String IFTTT::NAME = "IFTTT";
const String IFTTT::BASE_URL = "http://maker.ifttt.com/trigger/";

IFTTT::IFTTT(const String& key, const String& eventName) : Poster(BASE_URL + eventName + "/json/with/key/" + key, NAME) {};

String IFTTT::getJson(float angle, float temperature, long battery) const {
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += angle;
  jsonString += "\",\"battery\":\"";
  jsonString += battery;
  jsonString += "\"}";
  return jsonString;
}

