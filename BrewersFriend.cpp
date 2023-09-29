#include "BrewersFriend.h"

const String BrewersFriend::BASE_URL = "https://log.brewersfriend.com/stream/";
const String BrewersFriend::NAME = "BrewersFriend";

String BrewersFriend::getJson(float angle, float temperature, long battery) const {
  String jsonString = "{\"name:\" \"The-Floater\",";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"temp_unit\":\"F\",";
  jsonString += "gravity\":\"";
  // convert the angle to the Gravity reading here
  float gravity = coefficients[0] * pow(angle, 2) + coefficients[1] * angle + coefficients[2];
  jsonString += gravity;
  jsonString += "gravity_unit\":\"P\"";
  jsonString += "\",\"angle\":\"";
  jsonString += angle;
  jsonString += "\",\"RSSI\":\"";
  jsonString += WiFi.RSSI();
  jsonString += "\",\"battery\":\"";
  // FIXME: have to convert the battery from a percentage here to a "voltage reading" scale factor still TBD from BrewersFriend
  jsonString += battery;
  jsonString += "\"}";
  return jsonString;
}
