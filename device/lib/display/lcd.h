#ifndef _LCD_H
#define _LCD_H

#include <LiquidCrystal.h>

class Lcd : public LiquidCrystal {
  public:
    Lcd(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
        : LiquidCrystal(rs, en, d4, d5, d6, d7) {}

    void setup() {
      begin(16, 2);
    }

    void showMessage(const char* line1, const char* line2) {
      clear();
      showLine1(line1);
      showLine2(line2);
    }

    void showLine1(const char* text) {
      setCursor(0, 0);
      print(text);
    }

    void showLine2(const char* text) {
      setCursor(0, 1);
      print(text);
    }
};

#endif