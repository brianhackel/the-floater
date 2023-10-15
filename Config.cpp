#include "Config.h"

const char* Config::filename = "/config.json";

Config::Config() {
  LittleFS.begin();
  load();
}

LogType operator|(LogType lhs, LogType rhs) {
    return static_cast<LogType>(static_cast<int>(lhs) | static_cast<int>(rhs));
};

bool convertToJson(const LogType& src, JsonVariant dst) {
  return dst.set((int)src);
};

void convertFromJson(JsonVariantConst src, LogType& dst) {
  int t = src.as<const int>();
  dst = (LogType)t;
};

Mode operator|(Mode lhs, Mode rhs) {
    return static_cast<Mode>(static_cast<int>(lhs) | static_cast<int>(rhs));
};

bool convertToJson(const Mode& src, JsonVariant dst) {
  return dst.set((int)src);
};

void convertFromJson(JsonVariantConst src, Mode& dst) {
  int t = src.as<const int>();
  dst = (Mode)t;
};

Mode Config::getMode() {
  return _conf.mode;
}

void Config::setMode(Mode mode) {
  _conf.mode = mode;
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

bool Config::getIftttDetails(String *key, String *event) {
  if(_conf.iftttKey[0] == 0) {
    *key = "";
    *event = "";
    return false;
  } else {
    *key = _conf.iftttKey;
    *event = _conf.iftttEventName;
    return true;
  }
}

bool Config::getBrewersFriendKey(String *key) {
  if (_conf.brewersFriendKey[0] == 0) {
    *key = "";
    return false;
  } else {
    *key = _conf.brewersFriendKey;
    return true;
  }
}

LogType Config::getLogType() {
  return _conf.logType;
}

void Config::setIftttDetails(const String& key, const String& event) {
  snprintf(_conf.iftttKey, LOG_KEY_STR_LEN, key.c_str());
  snprintf(_conf.iftttEventName, LOG_KEY_STR_LEN, event.c_str());
}

void Config::setBrewersFriendDetails(const String& key) {
  snprintf(_conf.brewersFriendKey, LOG_KEY_STR_LEN, key.c_str());
}

void Config::setLogType(LogType type) {
  _conf.logType = type;
}

void Config::getCoeffs(float *c2, float *c1, float *c0) {
  *c2 = _conf.coefficients[0];
  *c1 = _conf.coefficients[1];
  *c0 = _conf.coefficients[2];
}

void Config::setCoeffs(const float c2, const float c1, const float c0) {
  _conf.coefficients[0] = c2;
  _conf.coefficients[1] = c1;
  _conf.coefficients[2] = c0;
}

bool Config::getGoogleSheetsDetails(String *deploymentId, String *spreadsheetId, String *sheetName) {
  if(_conf.googleSheetsDeploymentId[0] == 0) {
    *deploymentId = "";
    *spreadsheetId = "";
    *sheetName = "";
    return false;
  } else {
    *deploymentId = _conf.googleSheetsDeploymentId;
    *spreadsheetId = _conf.googleSheetsSpreadsheetId;
    *sheetName = _conf.googleSheetsSheetName;
    return true;
  }
}

void Config::setGoogleSheetsDetails(const String& deploymentId, const String& spreadsheetId, const String& sheetName) {
  snprintf(_conf.googleSheetsDeploymentId, LOG_KEY_STR_LEN, deploymentId.c_str());
  snprintf(_conf.googleSheetsSpreadsheetId, LOG_KEY_STR_LEN, spreadsheetId.c_str());
  snprintf(_conf.googleSheetsSheetName, LOG_KEY_STR_LEN, sheetName.c_str());
}


void Config::print() {
  Serial.println("mode: " + String((int)_conf.mode));
  Serial.println("sleepDurationUs: " + String(_conf.sleepDurationUs));
  Serial.println("ssid: " + String(_conf.ssid));
  Serial.println("pass: " + String(_conf.pass));
  Serial.println("offset_x: " + String(_conf.offset_x));
  Serial.println("offset_z: " + String(_conf.offset_z));
  Serial.println("logType: " + String((int)_conf.logType));
  Serial.println("iftttKey: " + String(_conf.iftttKey));
  Serial.println("iftttEvent: " + String(_conf.iftttEventName));
  Serial.println("brewersFriendKey: " + String(_conf.brewersFriendKey));
  Serial.println("coefficients: " + String(_conf.coefficients[0], 12) + "x^2 + "
                                  + String(_conf.coefficients[1], 12) + "x + "
                                  + String(_conf.coefficients[2], 12));
}

void Config::load() {
  File file = LittleFS.open(filename, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<384> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  _conf.mode = (Mode)doc["mode"] | Mode::Config;
  _conf.sleepDurationUs = doc["sleepDurationUs"] | DEFAULT_SLEEP_US;
  strlcpy(_conf.ssid,
          doc["ssid"] | "",
          sizeof(_conf.ssid));
  strlcpy(_conf.pass,
          doc["pass"] | "",
          sizeof(_conf.pass));
  _conf.offset_x = doc["offset_x"] | 0.0;
  _conf.offset_z = doc["offset_z"] | 0.0;
  _conf.logType = (LogType)doc["logType"] | LogType::None;
  strlcpy(_conf.iftttKey,
          doc["iftttKey"] | "",
          sizeof(_conf.iftttKey));
  strlcpy(_conf.iftttEventName,
          doc["iftttEvent"] | "",
          sizeof(_conf.iftttEventName));
  strlcpy(_conf.brewersFriendKey,
          doc["brewersFriendKey"] | "",
          sizeof(_conf.brewersFriendKey));
  for (int i = 0; i < N_COEFFICIENTS; i++) {
    _conf.coefficients[i] = doc["coefficients"][i] | 0.0;
  }

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
  StaticJsonDocument<384> doc;

  // Set the values in the document
  doc["mode"] = (int)_conf.mode;
  doc["sleepDurationUs"] = _conf.sleepDurationUs;
  doc["ssid"] = _conf.ssid;
  doc["pass"] = _conf.pass;
  doc["offset_x"] = _conf.offset_x;
  doc["offset_z"] = _conf.offset_z;
  doc["logType"] = (int)_conf.logType;
  doc["iftttKey"] = _conf.iftttKey;
  doc["iftttEvent"] = _conf.iftttEventName;
  doc["brewersFriendKey"] = _conf.brewersFriendKey;
  for (int i = 0; i < N_COEFFICIENTS; i++) {
    doc["coefficients"][i] = _conf.coefficients[i];
  }

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}
