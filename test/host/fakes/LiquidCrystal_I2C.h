#pragma once
#include "Arduino.h"
constexpr int POSITIVE = 1, NEGATIVE = 0;
class LiquidCrystal_I2C {
public:
    LiquidCrystal_I2C(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t) {}
    void setBacklightPin(uint8_t, int) {}
    void setBacklight(int) {}
    void begin(uint8_t, uint8_t) {
        for (auto& row : Fake::lcd) { memset(row, ' ', 20); row[20] = 0; }
    }
    void createChar(uint8_t, byte*) {}
    void setCursor(uint8_t column, uint8_t row) { column_ = column; row_ = row; }
    void write(uint8_t value) {
        if (column_ < 20 && row_ < 4) Fake::lcd[row_][column_++] = value;
        ++Fake::lcdWrites;
    }
private:
    uint8_t column_ = 0, row_ = 0;
};
