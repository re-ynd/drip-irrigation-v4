#ifndef _WEB_SERVER_H
#define _WEB_SERVER_H

#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"

class WebServer {
  private:
    AsyncWebServer server;
  
  public:
    WebServer(int port) : server(port) {}

    void setup() {
      Serial.println("[WebServer]: Setting up server");
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
      });
      server.serveStatic("/", SPIFFS, "/");
      server.begin();
      Serial.println("[WebServer]: Server started");
    }
};

#endif