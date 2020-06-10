#ifndef _OTA_H
#define _OTA_H

#include <ArduinoOTA.h>
#include <FS.h>

#include "lcd.h"
#include "led.h"

class Ota {
  private:
    Led* led;
    Lcd* lcd;
    bool busy;

  public:
    Ota(Led* led, Lcd* lcd) {
      this->led = led;
      this->lcd = lcd;
    }

    void setup(const char* hostname, const char* password) {
      ArduinoOTA.setHostname(hostname);
      ArduinoOTA.setPassword(password);

      ArduinoOTA.onStart([this]() {
        led->blink(300, 300);
        Serial.println("[OTA] Started");
        lcd->showMessage("OTA Started", "");
        SPIFFS.end();
        busy = true;
      });
      ArduinoOTA.onEnd([this]() { 
        Serial.println("\n[OTA] Ended");
        SPIFFS.begin();
        busy = false;
      });
      ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        led->update();
        unsigned int percent = (progress / (total / 100));
         Serial.printf("[OTA] Progress: %u%%\r", percent);
        String line1 = "OTA Progress ";
        line1 += percent;
        line1 += "%%";
        char line2[17];
        int i = 0;
        for (i = 0; i < percent * 16 / 100; i++) {
          line2[i] = '#';
        }
        line2[i] = '\0';
        lcd->showMessage(line1.c_str(), line2);
      });
      ArduinoOTA.onError([this](ota_error_t error) {
        led->blink(100, 100);
        Serial.printf("[OTA] Error[%u]: ", error);
        const char* errorMessage = "Unknown";
        switch (error) {
          case OTA_AUTH_ERROR:
            errorMessage = "Auth Failed";
            break;
          case OTA_BEGIN_ERROR:
            errorMessage = "Begin Failed";
            break;
          case OTA_CONNECT_ERROR:
            errorMessage = "Connect Failed";
            break;
          case OTA_RECEIVE_ERROR:
            errorMessage = "Receive Failed";
            break;
          case OTA_END_ERROR:
            errorMessage = "End Failed";
            break;
        }
        lcd->showMessage("OTA Error " + error, errorMessage);
        Serial.println(errorMessage);
        busy = false;
      });
      ArduinoOTA.begin();
      Serial.println("[OTA] Service ready");
    }

    void handle() {
      ArduinoOTA.handle();
    }

    bool isBusy() {
      return busy;
    }
};

#endif