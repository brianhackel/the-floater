#define WIFI_CONNECT_TIMEOUT_MILLIS 30000

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
String hostname = "hydrometer";

// Initialize WiFi
bool initWiFi(String ssid, String pass) {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostname);
  WiFi.begin(ssid.c_str(), pass.c_str());

  uint32_t tStart = millis();
  Serial.println("Connecting to WiFi...");
  blueBlinker.start();
  while (WiFi.status() != WL_CONNECTED && millis() - tStart < WIFI_CONNECT_TIMEOUT_MILLIS) {
    blueBlinker.update();
    Serial.print(".");
    delay(250);
  }
  Serial.println();
  blueBlinker.stop();
  if (!WiFi.isConnected()) {
    return false;
  }

  if (configMode) {
    Serial.println(WiFi.localIP());
    if (!MDNS.begin(hostname)) {
      Serial.println("Error setting up MDNS responder!");
      return false;
    }
    Serial.println("mDNS responder started. connect at " + hostname + ".local");
    }
  return true;
}

void setupStateServer() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });
  
  server.serveStatic("/", LittleFS, "/");

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"angle\":\"" + String(mpu.measureAngle()) + "\", \"temperature\":\"" + String(t.getTemperatureF()) + "\"}");
  });

  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
    FileSystem::clearAll();
    restart = true;
    request->send(200, "text/plain", "Done. HYDROMETER will restart. You will need to connect to the Hydrometer's WiFi network to reconfigure.");
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    int timeMins = 0;
    if (request->hasParam("time", true, false)) {
      AsyncWebParameter* p = request->getParam("time", true, false);
      // convert from minutes to millis
      timeMins = p->value().toInt();
      FileSystem::writeSleepDurationToFile(timeMins * 60000000);
    }
    if (request->hasParam("logType", true, false)) {
      FileSystem::clearLoggingConfigs();
      String type = request->getParam("logType", true, false)->value();
      if (type.equalsIgnoreCase("ifttt")) {
        FileSystem::writeIftttKeyToFile(request->getParam("iftttKey", true, false)->value().c_str());
        FileSystem::writeIftttEventToFile(request->getParam("iftttEvent", true, false)->value().c_str());
      } else if (type.equalsIgnoreCase("brewersFriend")) {
        Serial.println("found brewers friend configuration");
        FileSystem::writeBrewersFriendKeyToFile(request->getParam("brewersFriendKey", true, false)->value().c_str());
        // TODO: need to grab the coefficients and save them to a log file in a way we can easily retrieve and calculate gravity
      }
    }
//    for(int i=0;i<params;i++){
//      AsyncWebParameter* p = request->getParam(i);
//      Serial.println(p->name() + ": " + p->value() + "  post:  " + p->isPost() + "   file: " + p->isFile());
//    }
    FileSystem::setConfigMode(false);
    restart = true;
    request->send(200, "text/plain", "Done. HYDROMETER will restart and begin logging at " + String(timeMins) + " minute intervals.");
  });
  
  server.begin();
}

void setupAccessPoint() {
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Setting AP (Access Point)");
  // NULL sets an open Access Point
  WiFi.softAP("HYDROMETER", NULL);

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_ip, gateway, subnet) ? "Ready" : "Failed!");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP); 

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/wifimanager.html", "text/html");
  });
  
  server.serveStatic("/", LittleFS, "/");
  
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if (p->name() == "ssid") {
          FileSystem::writeSsidToFile(p->value().c_str());
        }
        if (p->name() == "pass") {
          FileSystem::writePassToFile(p->value().c_str());
        }
      }
    }
    FileSystem::setConfigMode(true);
    restart = true;
    request->send(200, "text/plain", "Done. HYDROMETER will restart. Please connect to the SSID network and go to http://" + hostname + ".local for configuration");
  });
  server.begin();
}