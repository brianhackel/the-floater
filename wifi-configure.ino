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

  if (configuration.isConfigMode()) {
    Serial.println(WiFi.localIP());
    if (!MDNS.begin(hostname)) {
      Serial.println("Error setting up MDNS responder!");
      return false;
    }
    Serial.println("mDNS responder started. connect at " + hostname + ".local");
    }
  return true;
}

String processor( const String& var ){
  if( var == "version" ){
      return VERSION;
  }
  return "";
}

void setupStateServer() {
  String json = "{";
  json += "\"time\": " + String(configuration.getSleepDurationUs() / 60000000ul);
  String key, event;
  if (configuration.getIftttDetails(&key, &event)) {
    json += ", \"type\": \"ifttt\"";
    json += ", \"iftttKey\": \"" + key + "\"";
    json += ", \"iftttEvent\": \"" + event + "\"";
  } else if(configuration.getBrewersFriendKey(&key)) {
    json += ", \"type\": \"brewersFriend\"";
    json += ", \"brewersFriendKey\": \"" + key + "\"";
    float c2, c1, c0;
    configuration.getCoeffs(&c2, &c1, &c0);
    json += ", \"bfSquaredCoeff\": " + String(c2,12);
    json += ", \"bfFirstDegreeCoeff\": " + String(c1,12);
    json += ", \"bfZeroDegreeCoeff\": " + String(c0,12);
  }
  json += "}";
  const String logging_json = json;

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html", false, processor);
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
    LittleFS.remove(Config::filename);
    restart = true;
    request->send(200, "text/plain", "Done. The-Floater will restart. You will need to connect to The-Floater's WiFi network to reconfigure.");
  });

  server.on("/standby", HTTP_POST, [](AsyncWebServerRequest *request) {
    configuration.clearLoggingConfigs();
    configuration.setConfigMode(false);
    request->send(200, "text/plain", "Done. The-Floater is going to standby mode. To wake: connect to power; press RESET button; wait for blue light; press RESET button again.");
    standby = true;
  });

  server.on("/tare", HTTP_POST, [](AsyncWebServerRequest *request) {
    float offsetX, offsetZ;
    mpu.tare(&offsetX, &offsetZ);
    configuration.setOffsets(offsetX, offsetZ);
    request->send(200);
    restart = true;
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    int timeMins = 0;
    if (request->hasParam("time", true, false)) {
      AsyncWebParameter* p = request->getParam("time", true, false);
      // convert from minutes to micros
      timeMins = p->value().toInt();
      configuration.setSleepDuration(timeMins * 60000000l);
    }
    if (request->hasParam("logType", true, false)) {
      configuration.clearLoggingConfigs();
      String type = request->getParam("logType", true, false)->value();
      if (type.equalsIgnoreCase("ifttt")) {
        configuration.setLogType(LogType::IFTTT);
        configuration.setIftttDetails(request->getParam("iftttKey", true, false)->value().c_str(),
                                      request->getParam("iftttEvent", true, false)->value().c_str());
      } else if (type.equalsIgnoreCase("brewersFriend")) {
        configuration.setLogType(LogType::BrewersFriend);
        configuration.setBrewersFriendDetails(request->getParam("brewersFriendKey", true, false)->value().c_str());
        float c2 = 0, c1 = 0, c0 = 0;
        if (request->hasParam("bfSquaredCoeff", true, false))
          c2 = request->getParam("bfSquaredCoeff", true, false)->value().toFloat();
        if (request->hasParam("bfFirstDegreeCoeff", true, false))
          c1 = request->getParam("bfFirstDegreeCoeff", true, false)->value().toFloat();
        if (request->hasParam("bfZeroDegreeCoeff", true, false))
          c0 = request->getParam("bfZeroDegreeCoeff", true, false)->value().toFloat();
        configuration.setCoeffs(c2, c1, c0);
      }
    }
//    for(int i=0;i<params;i++){
//      AsyncWebParameter* p = request->getParam(i);
//      Serial.println(p->name() + ": " + p->value() + "  post:  " + p->isPost() + "   file: " + p->isFile());
//    }
    configuration.setConfigMode(false);
    request->send(200, "text/plain", "Done. The-Floater will restart and begin logging at " + String(timeMins) + " minute intervals.");
    restart = true;
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
    String ssid = "", pass = "";
    if (request->hasParam("ssid", true, false)) {
      ssid = request->getParam("ssid", true, false)->value();
    }
    if (request->hasParam("pass", true, false)) {
      pass = request->getParam("pass", true, false)->value();
    }
    configuration.setWifiCredentials(ssid, pass);
    configuration.setConfigMode(true);
    restart = true;
    String linkStr = "http://" + hostname + ".local";
    request->send(200, "text/html", "Done. The-Floater will restart. Please connect to the \"" + ssid + "\" network and go to <a href='" + linkStr + "'>" + linkStr + "</a> for configuration");
  }).setFilter(ON_AP_FILTER);

  server.begin();
}