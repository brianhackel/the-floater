#include "FileSystem.h"

const char* FileSystem::ssidPath = "/ssid.txt";
const char* FileSystem::passPath = "/pass.txt";
const char* FileSystem::configModePath = "/configMode.txt";
const char* FileSystem::sleepDurationPath = "/sleepDuration.txt";
const char* FileSystem::iftttKeyPath = "/iftttKey.txt";
const char* FileSystem::iftttEventPath = "/iftttEvent.txt";
const char* FileSystem::brewersFriendKeyPath = "/brewersFriendKey.txt";
const char* FileSystem::logTypePath = "/logType.txt";
const char* FileSystem::consecutiveFailuresPath = "/nFailures.txt";

#define DEFAULT_SLEEP_US 30 * 60000000

bool FileSystem::init() {
  return LittleFS.begin();
}

bool FileSystem::wifiCredentialsReady(String *ssid, String *pass) {
  // Load values saved in LittleFS
  String _ssid = readFile(ssidPath);
  String _pass = readFile(passPath);

  Serial.println(_ssid);
  Serial.println(_pass);

  if(_ssid==""){
    Serial.println("Undefined SSID.");
    *ssid = "";
    *pass = "";
    return false;
  } else {
    *ssid = _ssid;
    *pass = _pass;
    return true;
  }
}

bool FileSystem::getIftttDetails(String *key, String *event) {
  // Load values saved in LittleFS
  String _key = readFile(iftttKeyPath);
  String _event = readFile(iftttEventPath);

  if(_key == ""){
    *key = "";
    *event = "";
    return false;
  } else {
    *key = _key;
    *event = _event;
    return true;
  }
}

bool FileSystem::getBrewersFriendKey(String *key) {
  String _key = readFile(brewersFriendKeyPath);
  if (_key == "") {
    *key = "";
    return false;
  } else {
    *key = _key;
    return true;
  }
};

bool FileSystem::isConfigMode() {
  String _configMode = readFile(configModePath);
  return _configMode == "1";
}

unsigned long FileSystem::getSleepDurationUs() {
  String _sleepUs = readFile(sleepDurationPath);
  return _sleepUs.isEmpty() ? DEFAULT_SLEEP_US : strtoul(_sleepUs.c_str(), NULL, 10);
}

void FileSystem::incrementConsecutiveFailures() {
  writeFile(consecutiveFailuresPath, String(getConsecutiveFailures() + 1).c_str());
}

unsigned int FileSystem::getConsecutiveFailures() {
  String n = readFile(consecutiveFailuresPath);
  return n.isEmpty() ? 0 : strtoul(n.c_str(), NULL, 10);
}

// Read File from LittleFS
String FileSystem::readFile(const char* path){
  File file = LittleFS.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

// Write file to LittleFS
void FileSystem::writeFile(const char* path, const char* message) {
  File file = LittleFS.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(!file.print(message)){
    Serial.println("- frite failed");
  }
  file.close();
}
