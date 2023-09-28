#include "FileSystem.h"

const char* FileSystem::coefficientsPath = "/coefficients.txt";

bool FileSystem::init() {
  return LittleFS.begin();
}

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

void FileSystem::writeCoeffsToFile(const float c3, const float c2, const float c1, const float c0) {
  String line = "";
  line = line + c3 + "," + c2 + "," + c1 + "," + c0;
  writeFile(coefficientsPath, line.c_str());
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
