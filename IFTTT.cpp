#include "IFTTT.h"

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

