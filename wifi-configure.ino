#define WIFI_CONNECT_TIMEOUT_MILLIS 30000

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
String hostname = "hydrometer";

// Initialize WiFi
bool initWiFi(String ssid, String pass) {
  Serial.println(ssid);
  Serial.println(pass);

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
  blueBlinker.stop();
  if (!WiFi.isConnected()) {
    return false;
  }

  Serial.println(WiFi.localIP());

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started. connect at " + hostname + ".local");
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
        // HTTP POST ssid value
        if (p->name() == PARAM_INPUT_1) {
          String ssid = p->value().c_str();
          Serial.print("SSID set to: ");
          Serial.println(ssid);
          // Write file to save value
          FileSystem::writeSsidToFile(ssid.c_str());
        }
        // HTTP POST pass value
        if (p->name() == PARAM_INPUT_2) {
          String pass = p->value().c_str();
          Serial.print("Password set to: ");
          Serial.println(pass);
          // Write file to save value
          FileSystem::writePassToFile(pass.c_str());
        }

        //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    restart = true;
    request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to http://" + hostname + ".local");
  });
  server.begin();
}