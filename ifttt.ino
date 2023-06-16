
// FIXME: move the key to the configuration and into LittleFS
#define KEY "cnyJ7UpiB9U1QAAfP7mQo5"        // Webhooks Key
#define EVENT "append_beer"                 // Webhooks Event Name

#include <ESP8266HTTPClient.h>

bool postOneUpdate(float angle, float temperature) {
  WiFiClient client;
  HTTPClient http;
  String url = "http://maker.ifttt.com/trigger/";
  url += EVENT;
  url += "/json/with/key/";
  url += KEY;
  http.begin(client, url);
  
  http.addHeader("Content-Type", "application/json");
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += angle;
  jsonString += "\"}";

  // Send HTTP POST request
  int httpResponseCode = http.POST(jsonString);
  http.end();
  return httpResponseCode == 200;
}
