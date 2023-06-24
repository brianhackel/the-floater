#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

class FileSystem {
  private:
    // File paths to save input values permanently
    const char* ssidPath = "/ssid.txt";
    const char* passPath = "/pass.txt";

  public:
   FileSystem() {};
};
#endif