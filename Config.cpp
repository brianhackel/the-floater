#include "Config.h"

const char* Config::filename = "/config.json";

Config::Config() {
  LittleFS.begin();
  load();
}

bool Config::isConfigMode() {
  return _conf.configMode == 1;
}

void Config::setConfigMode(bool configMode) {
  _conf.configMode = configMode ? 1 : 0;
}

long Config::getSleepDurationUs() {
  return _conf.sleepDurationUs;
}

void Config::setSleepDuration(long durationUs) {
  _conf.sleepDurationUs = durationUs;
}

bool Config::areWifiCredentialsReady(String *ssid, String *pass) {
  if(_conf.ssid[0] == 0){
    Serial.println("Undefined SSID.");
    *ssid = "";
    *pass = "";
    return false;
  } else {
    *ssid = String(_conf.ssid);
    *pass = String(_conf.pass);
    return true;
  }
}

void Config::setWifiCredentials(const String& ssid, const String& pass) {
  snprintf(_conf.ssid, WIFI_CRED_MAX_LEN, ssid.c_str());
  snprintf(_conf.pass, WIFI_CRED_MAX_LEN, pass.c_str());
}

void Config::getOffsets(float *x, float *z) {
  *x = _conf.offset_x;
  *z = _conf.offset_z;
}

void Config::setOffsets(float x, float z) {
  _conf.offset_x = x;
  _conf.offset_z = z;
}

void Config::print() {
  Serial.println("configMode: " + String(_conf.configMode));
  Serial.println("sleepDurationUs: " + String(_conf.sleepDurationUs));
  Serial.println("ssid: " + String(_conf.ssid));
  Serial.println("pass: " + String(_conf.pass));
  Serial.println("offset_x: " + String(_conf.offset_x));
  Serial.println("offset_z: " + String(_conf.offset_z));
}

void Config::load() {
  File file = LittleFS.open(filename, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<192> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  _conf.configMode = doc["configMode"] | 0;
  _conf.sleepDurationUs = doc["sleepDurationUs"] | DEFAULT_SLEEP_US;
  strlcpy(_conf.ssid,
          doc["ssid"] | "",
          sizeof(_conf.ssid));
  strlcpy(_conf.pass,
          doc["pass"] | "",
          sizeof(_conf.pass));
  _conf.offset_x = doc["offset_x"] | 0.0;
  _conf.offset_z = doc["offset_z"] | 0.0;

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

void Config::save() {
  LittleFS.remove(filename);

  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<192> doc;

  // Set the values in the document
  doc["configMode"] = _conf.configMode;
  doc["sleepDurationUs"] = _conf.sleepDurationUs;
  doc["ssid"] = _conf.ssid;
  doc["pass"] = _conf.pass;
  doc["offset_x"] = _conf.offset_x;
  doc["offset_z"] = _conf.offset_z;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}
