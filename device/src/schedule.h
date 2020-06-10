#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include <Arduino.h>
#include <EEPROM.h>

#include "linkedlist.h"
#include "utils.h"

class Schedule {
  public:
    const int hh;
    const int mm;
    const int duration;

  public:
    Schedule(int memoryLocation)
        : hh(EEPROM.read(memoryLocation)),
          mm(EEPROM.read(memoryLocation + 1)),
          duration(EEPROM.read(memoryLocation + 2)) {
    }

    String getState() {
      return String(hh) + "|" + String(mm) + "|" + String(duration);
    }

    static void save(String text, int memoryLocation) {
      MyLinkedList<String> data;
      Utils::split(&data, text, "|");
      for (int j = 0; j < data.size(); j++) {
        EEPROM.write(memoryLocation + j, data.get(j).toInt());
      }
    }
};

#endif