#define WIFI_CONNECT_TIMEOUT_MILLIS 60000

String hostname = "the-floater";

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
  String json = "{";
  json += "\"time\": " + String(FileSystem::getSleepDurationUs() / 60000000ul);
  String key, event;
  if (FileSystem::getIftttDetails(&key, &event)) {
    json += ", \"type\": \"ifttt\"";
    json += ", \"iftttKey\": \"" + key + "\"";
    json += ", \"iftttEvent\": \"" + event + "\"";
  } else if(FileSystem::getBrewersFriendKey(&key)) {
    json += ", \"type\": \"brewersFriend\"";
    json += ", \"brewersFriendKey\": \"" + key + "\"";
  }
  json += "}";
  const String logging_json = json;

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });
  
  server.serveStatic("/", LittleFS, "/");

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"angle\":\"" + String(mpu.measureAngle())
                                            + "\", \"temperature\":\"" + String(t.getTemperatureF())
                                            + "\", \"battery\":\"" + String(Battery::getPercentage())
                                            + "\"}");
  });

  server.on("/logging_info", HTTP_GET, [logging_json](AsyncWebServerRequest *request) {
    request->send(200, "application/json", logging_json);
  });

  server.on("/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
    FileSystem::clearAll();
    restart = true;
    request->send(200, "text/plain", "Done. The-Floater will restart. You will need to connect to The-Floater's WiFi network to reconfigure.");
  });

  server.on("/standby", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Done. The-Floater is going to standby mode. You will need to press the RESET button on the board or toggle power to wake it up.");
    standby = true;
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    int timeMins = 0;
    if (request->hasParam("time", true, false)) {
      AsyncWebParameter* p = request->getParam("time", true, false);
      // convert from minutes to micros
      timeMins = p->value().toInt();
      FileSystem::writeSleepDurationToFile(timeMins * 60000000l);
    }
    if (request->hasParam("failures", true, false)) {
      AsyncWebParameter* p = request->getParam("time", true, false);
      FileSystem::writeAllowedFailures(p->value().toInt());
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
    request->send(200, "text/plain", "Done. The-Floater will restart and begin logging at " + String(timeMins) + " minute intervals.");
  });
  
  server.begin();
}

void setupAccessPoint() {
  Serial.println("Setting AP (Access Point)");
  WiFi.softAP("The-Floater");

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", WiFi.softAPIP());
  WiFi.scanNetworks(false);

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "[";
    int n = WiFi.scanComplete();
    if(n == -2) {
      WiFi.scanNetworks(true);
    } else if(n > 0) {
      for (int i = 0; i < n; ++i){
        if(i > 0) {
          json += ",";
        }
        json += "\"" + WiFi.SSID(i) + "\"";
      }
      WiFi.scanDelete();
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "application/json", json);
  }).setFilter(ON_AP_FILTER);

  server.serveStatic("/", LittleFS, "/");

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/wifimanager.html", "text/html");
  }).setFilter(ON_AP_FILTER);

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(LittleFS, "/wifimanager.html", "text/html");
  });
    
  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    String ssid;
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        if (p->name() == "ssid") {
          ssid = p->value();
          FileSystem::writeSsidToFile(ssid.c_str());
        }
        if (p->name() == "pass") {
          FileSystem::writePassToFile(p->value().c_str());
        }
      }
    }
    FileSystem::setConfigMode(true);
    restart = true;
    String linkStr = "http://" + hostname + ".local";
    request->send(200, "text/html", "Done. The-Floater will restart. Please connect to the \"" + ssid + "\" network and go to <a href='" + linkStr + "'>" + linkStr + "</a> for configuration");
  }).setFilter(ON_AP_FILTER);

  server.begin();
}