#include <Arduino.h>

#include "constants.h"
#include "led.h"
#include "mdns.h"
#include "ota.h"
#include "websocket.h"
#include "wifi.h"
#include "websocket.h"
#include "sprinkler.h"
#include "automation.h"
#include "lcd.h"
#include "ntp.h"

Led led(LED_PIN);
Lcd lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

Ota ota(&led, &lcd);
Wifi wifi;
Mdns mdns;
Automation automation;
bool setupFailed = false;

void setup(void) {
  Serial.begin(115200);
  Serial.println("");
  led.setup();
  led.turnOn();
  lcd.setup();

  lcd.showMessage("Setting up OTA", OTA_HOSTNAME);
  ota.setup(OTA_HOSTNAME, OTA_PASSWORD);
  lcd.showMessage("Connecting to", WIFI_SSID);
  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
  lcd.showMessage("Setting mDNS", HOSTNAME);
  setupFailed = !mdns.setup(HOSTNAME);
  if (!setupFailed) {
    setupFailed = !automation.setup(&lcd, wifi.getIp());
  }

  if (setupFailed) {
    led.blink(300, 300);
  } else {
    led.blink(10, 1000);
    lcd.showMessage("Ready", "");
  }
}

void loop(void) {
  ota.handle();
  led.update();
  if (!setupFailed && !ota.isBusy()) {
    mdns.update();
    automation.update();
  }
}
