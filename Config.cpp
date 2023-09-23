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
  save();
}

long Config::getSleepDurationUs() {
  return _conf.sleepDurationUs;
}

void Config::setSleepDuration(long durationUs) {
  _conf.sleepDurationUs = durationUs;
  save();
}

void Config::load() {
  File file = LittleFS.open(filename, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<64> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  _conf.configMode = doc["configMode"] | 0;
  _conf.sleepDurationUs = doc["sleepDurationUs"] | DEFAULT_SLEEP_US;

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
  StaticJsonDocument<32> doc;

  // Set the values in the document
  doc["configMode"] = _conf.configMode;
  doc["sleepDurationUs"] = _conf.sleepDurationUs;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}