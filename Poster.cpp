#include "WiFiClientSecureBearSSL.h"
#include "Poster.h"

bool Poster::postOneUpdate(float angle, float temperature, long batteryPercentage) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, _url);
  
  http.addHeader("Content-Type", "application/json");
  String jsonString = getJson(angle, temperature, batteryPercentage);

  if (jsonString.isEmpty())
    return false;

  // Send HTTP POST request
  Serial.println("posting to " + _name + ": " + jsonString);
  int httpResponseCode = http.POST(jsonString);
  Serial.println(httpResponseCode);
  Serial.println(http.errorToString(httpResponseCode));
  http.end();
  return httpResponseCode == 200 || httpResponseCode == 302;
}

