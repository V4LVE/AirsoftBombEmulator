#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <stdio.h>
#include "Config.h"

// Compose frames in RAM and send only changed spans. No runtime LCD clears.
class DisplayController {
public:
    DisplayController() : lcd_(Config::lcdAddress, Config::lcdEnable, Config::lcdReadWrite,
        Config::lcdRegisterSelect, Config::lcdData4, Config::lcdData5, Config::lcdData6, Config::lcdData7) {}
    void begin() {
        lcd_.setBacklightPin(Config::lcdBacklightPin, Config::lcdBacklightPositive ? POSITIVE : NEGATIVE);
        lcd_.begin(Config::lcdColumns, Config::lcdRows);
        lcd_.setBacklight(HIGH);
        for (uint8_t n = 0; n < 5; ++n) {
            byte glyph[8];
            memset(glyph, (0x1f << (4 - n)) & 0x1f, sizeof(glyph));
            lcd_.createChar(n, glyph);
        }
        memset(shown_, 0xff, sizeof(shown_));
        clearFrame();
    }
    void clearFrame() { memset(frame_, ' ', sizeof(frame_)); }
    void text(uint8_t row, uint8_t column, const char* value) {
        while (row < Config::lcdRows && column < Config::lcdColumns && *value)
            frame_[row][column++] = *value++;
    }
    void text(uint8_t row, uint8_t column, const __FlashStringHelper* value) {
        const char* source = reinterpret_cast<const char*>(value);
        while (row < Config::lcdRows && column < Config::lcdColumns) {
            const char c = pgm_read_byte(source++);
            if (!c) break;
            frame_[row][column++] = c;
        }
    }
    void number(uint8_t row, uint8_t column, uint16_t value) {
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%u", value);
        text(row, column, buffer);
    }
    void time(uint8_t row, uint8_t column, uint32_t seconds) {
        char buffer[12];
        snprintf(buffer, sizeof(buffer), "%02lu:%02lu",
                 static_cast<unsigned long>(seconds / 60), static_cast<unsigned long>(seconds % 60));
        text(row, column, buffer);
    }
    void progress(uint8_t row, uint8_t percent) {
        const uint8_t filled = percent > 100 ? 100 : percent;
        for (uint8_t cell = 0; cell < Config::lcdColumns; ++cell) {
            const int segments = filled * Config::lcdColumns * 5 / 100 - cell * 5;
            frame_[row][cell] = static_cast<char>(segments >= 5 ? 4 : (segments > 0 ? segments - 1 : ' '));
        }
    }
    void flush() {
        for (uint8_t row = 0; row < Config::lcdRows; ++row) {
            uint8_t col = 0;
            while (col < Config::lcdColumns) {
                if (frame_[row][col] == shown_[row][col]) { ++col; continue; }
                lcd_.setCursor(col, row);
                do {
                    lcd_.write(static_cast<uint8_t>(frame_[row][col]));
                    shown_[row][col] = frame_[row][col];
                    ++col;
                } while (col < Config::lcdColumns && frame_[row][col] != shown_[row][col]);
            }
        }
    }
private:
    LiquidCrystal_I2C lcd_;
    char frame_[Config::lcdRows][Config::lcdColumns];
    char shown_[Config::lcdRows][Config::lcdColumns];
};
