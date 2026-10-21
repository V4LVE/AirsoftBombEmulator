#pragma once
#include <stdint.h>
#include <string.h>
#include "Config.h"

class DigitInput {
public:
    void clear() { size_ = 0; value_[0] = '\0'; }
    bool append(char key, uint8_t limit = Config::maxCodeDigits) {
        if (key < '0' || key > '9' || size_ >= limit || size_ >= Config::maxCodeDigits) return false;
        value_[size_++] = key;
        value_[size_] = '\0';
        return true;
    }
    void erase() { if (size_) value_[--size_] = '\0'; }
    bool empty() const { return size_ == 0; }
    const char* text() const { return value_; }
    uint16_t number() const {
        uint16_t result = 0;
        // Used only for three-digit game durations.
        for (uint8_t i = 0; i < size_ && i < Config::timeInputDigits; ++i) result = result * 10 + value_[i] - '0';
        return result;
    }
private:
    char value_[Config::maxCodeDigits + 1] = {};
    uint8_t size_ = 0;
};
