#include "BrewersFriend.h"

String BrewersFriend::getJson(float angle, float temperature, long battery) {
  float c3, c2, c1, c0;
  if (!FileSystem::getCoeffs(&c3, &c2, &c1, &c0))
    return "";

  String jsonString = "{\"name:\" \"The-Floater\",";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"temp_unit\":\"F\",";
  jsonString += "gravity\":\"";
  // convert the angle to the Gravity reading here
  float gravity = c3 * pow(angle, 3) + c2 * pow(angle, 2) + c1 * angle + c0;
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
