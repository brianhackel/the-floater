#include "Poster.h"

bool IFTTT::postOneUpdate(float angle, float temperature, long battery) {
  WiFiClient client;
  HTTPClient http;
  String url = "http://maker.ifttt.com/trigger/";
  url += _event;
  url += "/json/with/key/";
  url += _key;
  http.begin(client, url);
  
  http.addHeader("Content-Type", "application/json");
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += angle;
  jsonString += "\",\"battery\":\"";
  jsonString += battery;
  jsonString += "\"}";

  // Send HTTP POST request
  Serial.println("posting to IFTTT: " + jsonString);
  int httpResponseCode = http.POST(jsonString);
  http.end();
  return httpResponseCode == 200;
}

bool BrewersFriend::postOneUpdate(float angle, float temperature, long battery) {
  // FIXME: remove duplicate code by moving up to the base class Poster
  WiFiClient client;
  HTTPClient http;
  String url = "https://log.brewersfriend.com/stream/";
  url += _key;
  http.begin(client, url);
  
  http.addHeader("Content-Type", "application/json");
  String jsonString = "{\"name:\" \"The-Floater\",";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"temp_unit\":\"F\",";
  jsonString += "gravity\":\"";
  // FIXME: have to convert the angle to the Gravity reading here
  jsonString += angle;
  jsonString += "gravity_unit\":\"P\"";
  jsonString += "\",\"battery\":\"";
  // FIXME: have to convert the battery from a percentage here to a "voltage reading" scale factor still TBD from BrewersFriend
  jsonString += battery;
  jsonString += "\"}";

  // Send HTTP POST request
  Serial.println("posting to BrewersFriend: " + jsonString);
  int httpResponseCode = http.POST(jsonString);
  http.end();
  return httpResponseCode == 200;
}
