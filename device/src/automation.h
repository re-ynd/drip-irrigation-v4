#ifndef _AUTOMATION_H
#define _AUTOMATION_H

#include <EEPROM.h>

#include "lcd.h"
#include "linkedlist.h"
#include "ntp.h"
#include "sprinkler.h"
#include "utils.h"
#include "websocket.h"
#include "schedule.h"
#include "webserver.h"

#define APP_VERSION 77

#define UPDATE_INTERVAL 1000  // every second
#define EEPROM_APP_VERSION_LOCATION 00
#define EEPROM_SCHEDULE_COUNT_LOCATION 10
#define EEPROM_SCHEDULE_START_LOCATION 11
#define EEPROM_SCHEDULE_SPACE 10

class Automation : SprinklerListener, WebSocketListener {
  private:
    Ntp ntp;
    WebSocket webSocket;
    Sprinkler sprinkler;
    WebServer webServer;

    unsigned int lastUpdateMillis;
    Lcd* lcd;
    String ipAddress;
    MyLinkedList<Schedule*> schedules;
    Schedule* nextSchedule;

    void onClientConnected(uint32_t num) {
      webSocket.send(num, "ip_address:" + ipAddress);
      webSocket.send(num, sprinkler.getState());
      sendScheduleInfoToClients();
      updateLcdAndClients(ntp.getTime());
    }

    void sendScheduleInfoToClients() {
      String scheduleData = "schedule";
      for (int i = 0; i < schedules.size(); i++) {
        scheduleData = scheduleData + ":" + schedules.get(i)->getState();
      }
      webSocket.sendAll(scheduleData);
    }

    void onMessageFromClient(String msg) {
      MyLinkedList<String> tokens;
      Utils::split(&tokens, msg, ":");
      bool handled = false;
      if (tokens.size() > 0) {
        String command = tokens.get(0);
        tokens.remove(0);
        if (command == "sprinkler") {
          sprinkler.handleCommand(&tokens);
          handled = true;
        } else if (command == "schedule") {
          saveSchedule(&tokens);
        }
      }
      if (!handled) {
        Serial.println("[Automation] Unknown command: " + msg);
      }
    }

    void loadSchedule() {
      for (int i = 0; i < schedules.size(); i++) {
        delete schedules.get(i);
      }
      schedules.clear();

      int appVersion = EEPROM.read(EEPROM_APP_VERSION_LOCATION);
      if (appVersion == APP_VERSION) {
        int numSchedules = EEPROM.read(EEPROM_SCHEDULE_COUNT_LOCATION);
        for (int i = 0; i < numSchedules; i++) {
          schedules.add(new Schedule(EEPROM_SCHEDULE_START_LOCATION +
                                     i * EEPROM_SCHEDULE_SPACE));
        }
      } else {
        EEPROM.write(EEPROM_APP_VERSION_LOCATION, APP_VERSION);
        EEPROM.write(EEPROM_SCHEDULE_COUNT_LOCATION, 0);
        EEPROM.commit();
      }
    }

    void saveSchedule(MyLinkedList<String>* args) {
      for (int i = 0; i < args->size(); i++) {
        Schedule::save(args->get(i), EEPROM_SCHEDULE_START_LOCATION + i * EEPROM_SCHEDULE_SPACE);
      }
      EEPROM.write(EEPROM_SCHEDULE_COUNT_LOCATION, args->size());
      EEPROM.commit();
      loadSchedule();
      sendScheduleInfoToClients();
    }

    void onSprinklerStateChanged(bool on) {
      webSocket.sendAll(sprinkler.getState());
    }

    void updateLcdAndClients(int timeInSeconds) {
      lastUpdateMillis = millis();

      String ram = String(ESP.getFreeHeap() / 1024);
      String time = Utils::formatTimeFull(timeInSeconds);
      String line1 = time + " " + ram + "KB " + webSocket.numClients() + "  ";
      lcd->showLine1(line1.c_str());
      if (sprinkler.isOn()) {
        String line2;
        line2 = "SPRNKLR ON " + Utils::formatDuration(sprinkler.getTimeLeft());
        lcd->showLine2(line2.c_str());
      } else {
        String line2 = "OFF  ";
        if (nextSchedule != nullptr) {
          line2 = line2 + Utils::padZero(nextSchedule->hh) + ":" + Utils::padZero(nextSchedule->mm);
          line2 = line2 + " " + Utils::formatDuration(nextSchedule->duration * 60 * 1000);
        } else {
          line2 = "                ";
        }
        lcd->showLine2(line2.c_str());
      }

      if (sprinkler.isOn()) {
        webSocket.sendAll(sprinkler.getState());
      }
      webSocket.sendAll("system:" + time + ":" + ram);
    }

    void checkSchedule(uint32_t time) {
      nextSchedule = schedules.size() == 0 ? nullptr : schedules.get(0);
      int nextScheduleDiff = 100000;
      for (int i = 0; i < schedules.size(); i++) {
        Schedule* schedule = schedules.get(i);
        uint32_t scheduleTime = schedule->hh * 3600 + schedule->mm * 60;
        int diff = scheduleTime - time % 86400; // Find seconds since last midnight
        // Turn on sprinkler if the time is within 2 seconds
        if (diff >= 0 && diff < 2) {
          sprinkler.turnOn(schedule->duration * 60);
        } else if (diff > 10 && diff < nextScheduleDiff) {
          nextSchedule = schedule;
          nextScheduleDiff = diff;
        }
      }
    }

  public:
    Automation() : webSocket(WEB_SOCKET_PORT), sprinkler(SPRINKLER_PIN), 
        webServer(WEB_SERVER_PORT) {}

    bool setup(Lcd* lcd, String ipAddress) {
      EEPROM.begin(512);
      loadSchedule();

      this->lcd = lcd;
      this->ipAddress = ipAddress;

      lcd->showMessage("Setting up NTP", "");
      ntp.setup();
      sprinkler.setup(this);
      lcd->showMessage("Setting up", "web socket");
      webSocket.setup(this);
      lcd->showMessage("Setting up", "SPIFFS");
      Serial.println("[Automation] Setting up SPIFFS");
      if (!SPIFFS.begin()) {
        Serial.println("[Automation] Failed to setup SPIFFS");
        return false;
      }
      lcd->showMessage("Setting up", "web server");
      webServer.setup();

      return true;
    }

    void update() {
      webSocket.handle();
      ntp.update();

      if (millis() - lastUpdateMillis > UPDATE_INTERVAL) {
        uint32_t time = ntp.getTime();
        checkSchedule(time);
        sprinkler.update();
        updateLcdAndClients(time);
      }
    }
};

#endif