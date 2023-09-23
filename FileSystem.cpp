#include "FileSystem.h"

const char* FileSystem::iftttKeyPath = "/iftttKey.txt";
const char* FileSystem::iftttEventPath = "/iftttEvent.txt";
const char* FileSystem::brewersFriendKeyPath = "/brewersFriendKey.txt";
const char* FileSystem::logTypePath = "/logType.txt";
const char* FileSystem::consecutiveFailuresPath = "/nFailures.txt";
const char* FileSystem::allowedFailuresPath = "/allowedFailures.txt";
const char* FileSystem::coefficientsPath = "/coefficients.txt";
const char* FileSystem::offsetsPath = "/offsets.txt";

#define DEFAULT_ALLOWED_FAILURES 5

bool FileSystem::init() {
  return LittleFS.begin();
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

bool FileSystem::getCoeffs(float *c3, float *c2, float *c1, float *c0) {
  String _line = readFile(coefficientsPath);
  *c3 = 0.0;
  *c2 = 0.0;
  *c1 = 0.0;
  *c0 = 0.0;
  if (_line == "") {
    return false;
  } else {
    float coeffs[4];
    char *p, str[128];
    strncpy (str, _line.c_str(), 128);
    p = strtok(str, ",");
    int count = 0;
    while (p != NULL) {
      coeffs[count] = atof(p);
      count++;
      p = strtok(NULL, ",");
    }
    if (count >= 4) {
      *c3 = coeffs[0];
      *c2 = coeffs[1];
      *c1 = coeffs[2];
      *c0 = coeffs[3];
      return true;
    } else {
      return false;
    }
  }
}

void FileSystem::getOffsets(float *ex, float *zee) {
  String _line = readFile(offsetsPath);
  *ex = 0.0;
  *zee = 0.0;
  if (_line == "") {
    return;
  } else {
    float offsets[2];
    char *p, str[128];
    strncpy (str, _line.c_str(), 128);
    p = strtok(str, ",");
    int count = 0;
    while (p != NULL) {
      offsets[count] = atof(p);
      count++;
      p = strtok(NULL, ",");
    }
    if (count >= 2) {
      *ex = offsets[0];
      *zee = offsets[1];
    }
  }
}

void FileSystem::writeCoeffsToFile(const float c3, const float c2, const float c1, const float c0) {
  String line = "";
  line = line + c3 + "," + c2 + "," + c1 + "," + c0;
  writeFile(coefficientsPath, line.c_str());
}

void FileSystem::writeOffsetsToFile(const float ex, const float zee) {
  String line = "";
  line = line + ex + "," + zee;
  writeFile(offsetsPath, line.c_str());
}

unsigned int FileSystem::getAllowedFailures() {
  String _failures = readFile(allowedFailuresPath);
  return _failures.isEmpty() ? DEFAULT_ALLOWED_FAILURES : strtoul(_failures.c_str(), NULL, 10);
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
    Serial.println(String(path) + " - failed to open file for reading");
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
    Serial.println(String(path) + " - failed to open file for writing");
    return;
  }
  if(!file.print(message)){
    Serial.println(String(path) + " - frite failed");
  }
  file.close();
}
