
// FIXME: move the key to the configuration and into LittleFS
#define KEY "cnyJ7UpiB9U1QAAfP7mQo5"        // Webhooks Key
#define EVENT "append_beer"                 // Webhooks Event Name

bool postOneUpdate(float angle, float temperature) {
  feedWrite(temperature, angle);
  return true;
}

void feedWrite(float temperature, float tilt){
  // construct the JSON payload
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += tilt;
  jsonString += "\"}";
  int jsonLength = jsonString.length();  
  String lenString = String(jsonLength);
  // connect to the Maker event server
  WiFiClient client;
  client.connect("maker.ifttt.com", 80);
  // construct the POST request
  String postString = "";
  postString += "POST /trigger/";
  postString += EVENT;
  postString += "/with/key/";
  postString += KEY;
  postString += " HTTP/1.1\r\n";
  postString += "Host: maker.ifttt.com\r\n";
  postString += "Content-Type: application/json\r\n";
  postString += "Content-Length: ";
  postString += lenString + "\r\n";
  postString += "\r\n";
  postString += jsonString; // combine post request and JSON
  
  client.print(postString);
  delay(500);
  client.stop();
}
