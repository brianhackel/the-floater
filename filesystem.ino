// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

bool wifiCredentialsReady(String *ssid, String *pass) {
  // Load values saved in LittleFS
  String _ssid = readFile(LittleFS, ssidPath);
  String _pass = readFile(LittleFS, passPath);

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

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
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
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
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