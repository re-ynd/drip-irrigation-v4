#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led {
  private:
    unsigned int pin;
    unsigned int onMillis;
    unsigned int offMillis;
    unsigned long lastUpdateTimeInMillis;
    bool on;

    void changeState(bool on) {
      digitalWrite(pin, on ? LOW : HIGH);
      this->on = on;
      this->lastUpdateTimeInMillis = millis();
    }

  public:
    Led(unsigned int pin) {
      this->pin = pin;
    }

    void turnOn() {
      this->changeState(true);
    }

    void turnOff() { 
      this->changeState(false); 
    }

    void setup() { 
      pinMode(pin, OUTPUT); 
    }

    void blink(unsigned int onMillis, unsigned int offMillis) {
      Serial.print("[LED] Blinking time updated. On: ");
      Serial.print(onMillis);
      Serial.print(" off: ");
      Serial.println(offMillis);
      this->onMillis = onMillis;
      this->offMillis = offMillis;
    }

    void update() {
      unsigned long currentMillis = millis();
      if (on && currentMillis - lastUpdateTimeInMillis > onMillis) {
        turnOff();
      }
      if (!on && currentMillis - lastUpdateTimeInMillis > offMillis) {
        turnOn();
      }
    }
};

#endif