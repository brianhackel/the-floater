#include "AdafruitIO_WiFi.h"
#define IO_USERNAME  "reddogbrewing"
#define IO_KEY       "aio_gJDo38qF8q2wqc8VcWaS4UVGHoej"
AdafruitIO_WiFi *io;

bool postOneUpdate(String ssid, String pass, float angle, float temperature) {
  connectAIO(ssid, pass);
  feedWrite("tilt", angle);
  feedWrite("temperature", temperature);
  return true;
}


void feedWrite(String name, float value){
   // set up feeds
  AdafruitIO_Feed *feed = io->feed(name.c_str());
  Serial.println("sending value " + String(value) + " to feed " + name);
  // send data to feed
  feed->save(value);
  // write data to AIO
  io->run();
}

void connectAIO(String ssid, String pass) {
  io = new AdafruitIO_WiFi(IO_USERNAME, IO_KEY, ssid.c_str(), pass.c_str());
  Serial.println("Connecting to Adafruit IO...");
  io->connect();

  // wait for a connection
  while (io->status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io->statusText());
}