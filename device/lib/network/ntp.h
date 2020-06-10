#ifndef _NTP_H
#define _NTP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "lcd.h"

#define DNS_LOOKUP_INTERVAL 60000  // Every minute = 60 * 1000
#define NTP_PACKET_SIZE 48
#define UPDATE_INTERVAL 3600000  // Every 1 hour = 60 * 60 * 1000
#define FAILED_UDP_TIME 60000    // 1 minute timeout for UDP packet

class Ntp {
  private:
    bool initialized = false;
    WiFiUDP UDP;
    const char* NTPServerName = "asia.pool.ntp.org";
    IPAddress timeServerIP;           // NTP server IP address
    byte NTPBuffer[NTP_PACKET_SIZE];  // buffer to hold incoming and outgoing
                                      // packets

    unsigned long lastDnsLookup = 0;
    unsigned long lastNtpRequest = 0;
    unsigned long lastNtpResponse = millis();
    uint32_t unixTime = 0;

    void sendNTPpacket(IPAddress& address) {
      lastNtpRequest = millis();
      Serial.println("[NTP] Sending request ...");
      memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
      // Initialize values needed to form NTP request
      NTPBuffer[0] = 0b11100011;  // LI, Version, Mode
      // send a packet requesting a timestamp:
      UDP.beginPacket(address, 123);  // NTP requests are to port 123
      UDP.write(NTPBuffer, NTP_PACKET_SIZE);
      UDP.endPacket();
    }

    void startUDP() {
      Serial.println("[NTP] Starting UDP");
      UDP.begin(123);  // Start listening for UDP messages on port 123
      Serial.print("[NTP] UDP Local port: ");
      Serial.println(UDP.localPort());
    }

    int32_t getNtpTime() {
      if (UDP.parsePacket() == 0) {  // If there's no response (yet)
        return 0;
      }
      UDP.read(NTPBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
      // Combine the 4 timestamp bytes into one 32-bit number
      uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) |
                        (NTPBuffer[42] << 8) | NTPBuffer[43];
      // Convert NTP time to a UNIX timestamp:
      // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
      const uint32_t seventyYears = 2208988800UL;
      // IST offset from UTC is 5 hours and 30 minutes = 19800 seconds
      const uint32_t istOffset = 19800;
      // subtract seventy years and add time offset for IST
      uint32_t unixTime = NTPTime - seventyYears + istOffset;
      return unixTime;
    }

    void init(bool forceInit) {
      if (initialized) {
        return;
      }
      unsigned long currentMillis = millis();
      if (!forceInit && currentMillis - lastDnsLookup < DNS_LOOKUP_INTERVAL) {
        return;
      }
      lastDnsLookup = currentMillis;

      Serial.println("[NTP] DNS lookup started");
      if (!WiFi.hostByName(NTPServerName,timeServerIP)) {
        Serial.println("[NTP] DNS lookup failed");
      } else {
        Serial.print("[NTP] Time server IP: ");
        Serial.println(timeServerIP);
        sendNTPpacket(timeServerIP);
        initialized = true;
      }
    }

    void sync() {
      if (!initialized) {
        return;
      }
      unsigned long currentMillis = millis();
      if (currentMillis - lastNtpRequest > UPDATE_INTERVAL) {
        sendNTPpacket(timeServerIP);
      }

      // We sent a UDP request but never got a response, then try again
      if (lastNtpRequest - lastNtpResponse > 0 &&
          lastNtpRequest - lastNtpResponse < UPDATE_INTERVAL &&
          currentMillis - lastNtpRequest > FAILED_UDP_TIME) {
        sendNTPpacket(timeServerIP);
      }

      uint32_t time = getNtpTime();  // Check if an NTP response has arrived
      if (time) {                    // If a new timestamp has been received
        unixTime = time;
        Serial.print("[NTP] Response: ");
        Serial.println(unixTime);
        lastNtpResponse = currentMillis;
      }
    }

  public:
    void setup() {
      startUDP();
      init(true);
    }

    void update() {
      if (!initialized) {
        init(false);
      } else {
        sync();
      }
    }

    uint32_t getTime() {
      return unixTime + (millis() - lastNtpResponse) / 1000;
    }
};

#endif