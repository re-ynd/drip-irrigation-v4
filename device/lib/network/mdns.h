#ifndef _MDNS_H
#define _MDNS_H

#include <ESP8266mDNS.h>

class Mdns {
  public:
    bool setup(const char* hostname) {
      // Set up mDNS responder:
      // - first argument is the domain name, in this example
      //   the fully-qualified domain name is "esp8266.local"
      // - second argument is the IP address to advertise
      //   we send our IP address on the WiFi network
      if (!MDNS.begin(hostname)) {
        Serial.println("[mDNS] Error setting up MDNS responder!");
        return false;
      }
      Serial.print("[mDNS] Responder started with hostname: ");
      Serial.println(hostname);
      return true;
    }

    void update() {
      MDNS.update();
    }
};

#endif