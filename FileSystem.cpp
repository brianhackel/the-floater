#include "FileSystem.h"

const char* FileSystem::ssidPath = "/ssid.txt";
const char* FileSystem::passPath = "/pass.txt";
const char* FileSystem::configModePath = "/configMode.txt";


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

bool FileSystem::isConfigMode() {
  String _configMode = readFile(configModePath);
  return _configMode == "1";
}

// Read File from LittleFS
String FileSystem::readFile(const char * path){
  Serial.printf("Reading file: %s\r\n", path);

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
void FileSystem::writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = LittleFS.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}
