#ifndef _SPRINKLER_H
#define _SPRINKLER_H

#include <Arduino.h>

#include "linkedlist.h"

#define SPRINKLER_ON_TIME 1800000  // 30 minutes in millis
#define NOTIFY_CLIENT_INTERVAL 1000  // every second

// Abstract class
class SprinklerListener {
  public:
    virtual void onSprinklerStateChanged(bool on) = 0;
};

class Sprinkler {
  private:
    SprinklerListener* stateChangedListener;
    unsigned int pin;
    bool on = false;
    unsigned long duration;
    unsigned long onTime;

    bool changeState(bool on) {
      if (this-> on == on) {
        return false;
      }
      if (on) {
        unsigned long currentMillis = millis();
        onTime = currentMillis;
      }
      digitalWrite(this->pin, on ? HIGH : LOW);
      this->on = on;
      stateChangedListener->onSprinklerStateChanged(on);
      return true;
    }

  public:
    Sprinkler(unsigned int pin) {
      this->pin = pin;
    }

    void setup(SprinklerListener* callback) {
      this->stateChangedListener = callback;
      pinMode(this->pin, OUTPUT);
    }

    void turnOn(int durationInSeconds) {
      if (changeState(true)) {
        duration = durationInSeconds > 0 ? durationInSeconds * 1000 : SPRINKLER_ON_TIME;
      }
    }

    void turnOff() {
      changeState(false);
    }

    void handleCommand(MyLinkedList<String>* args) {
      String command = args->size() > 0 ? args->get(0) : "";
      int duration = args->size() > 1 ? args->get(1).toInt() : 0;
      if (command == "on") {
        turnOn(duration);
      } else {
        turnOff();
      }
    }

    void update() {
      if (on) {
        unsigned long currentMillis = millis();
        if (currentMillis - onTime > duration) {
          turnOff();
        }
      }
    }

    String getState() {
      String state = on ? "true" : "false";
      unsigned long sprinklerTimeLeft = getTimeLeft();
      return "sprinkler:" + state + ":" + String(sprinklerTimeLeft) + ":" + String(duration);
    }

    bool isOn() {
      return on;
    }

    unsigned long getTimeLeft() {
      return on ? duration - millis() + onTime : 0;
    }
};

#endif