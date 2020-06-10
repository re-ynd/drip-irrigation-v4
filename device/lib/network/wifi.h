#ifndef _WIFI_H
#define _WIFI_H

#include <ESP8266WiFi.h>

class Wifi {
  public:
    void connect(String ssid, String password) {
      // Connect to WiFi network
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

      // Wait for connection
      Serial.print("[WiFi] Connecting to ");
      Serial.print(ssid);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
      }
      Serial.println("");
      Serial.print("[WiFi] Connected to ");
      Serial.println(ssid);
      Serial.print("[WiFi] IP address: ");
      Serial.println(WiFi.localIP());
    }

    String getIp() {
      return WiFi.localIP().toString();
    }
};

#endif