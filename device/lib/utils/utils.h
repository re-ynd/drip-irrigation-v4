#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include <Arduino.h>
#include "linkedlist.h"

class Utils {
  public:
    static void split(MyLinkedList<String>* list, String text, String delimiter, int num = 0) {
      int index = -1;
      int oldIndex = 0;
      do {
        index = text.indexOf(delimiter, index + 1);
        String token = index < 0 ? text.substring(oldIndex) : text.substring(oldIndex, index);
        list->add(token);
        oldIndex = index + 1;
      } while (index >= 0);
    }

    static String formatDuration(unsigned long duration) {
      unsigned int seconds = duration / 1000;
      unsigned int minutes = seconds / 60;
      seconds %= 60;
      String mm = (minutes < 10 ? "0" : "") + String(minutes);
      String ss = (seconds < 10 ? "0" : "") + String(seconds);
      return mm + ":" + ss;
    }

    static String formatTimeFull(uint32_t time) {
      unsigned int seconds = time % 60;
      unsigned int minutes = time / 60 % 60;
      unsigned int hours = time / 3600 % 24;
      String hh = padZero(hours);
      String mm = padZero(minutes);
      String ss = padZero(seconds);
      return hh + ":" + mm + ":" + ss;
    }

    static String padZero(uint32 val) {
      return (val < 10 ? "0" : "") + String(val);
    }
};

#endif