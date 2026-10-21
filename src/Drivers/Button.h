#pragma once
#include <Arduino.h>
#include "Config.h"
#include "Core/Timing.h"

class Button {
public:
    explicit Button(uint8_t pin) : pin_(pin) {}
    void begin() { pinMode(pin_, INPUT_PULLUP); }
    void update(uint32_t now) {
        const bool sample = digitalRead(pin_) == LOW;
        if (sample != raw_) { raw_ = sample; changed_ = now; }
        if (Timing::elapsed(now, changed_) >= Config::buttonDebounceMs) pressed_ = raw_;
    }
    bool pressed() const { return pressed_; }
private:
    uint8_t pin_;
    bool raw_ = false, pressed_ = false;
    uint32_t changed_ = 0;
};
