#include "IFTTT.h"

String IFTTT::getJson(float angle, float temperature, long battery) {
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

