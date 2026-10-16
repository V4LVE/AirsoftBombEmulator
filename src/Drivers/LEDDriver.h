#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Config.h"
#include "Core/Timing.h"

enum class Light : uint8_t { Off, White, Blue, Red };

namespace LEDDetail {
// Keep FastLED types out of the hardware-independent configuration and game rules.
constexpr EOrder colorOrder(Config::LedColorOrder order) {
    return order == Config::LedColorOrder::RGB ? RGB :
           order == Config::LedColorOrder::RBG ? RBG :
           order == Config::LedColorOrder::GRB ? GRB :
           order == Config::LedColorOrder::GBR ? GBR :
           order == Config::LedColorOrder::BRG ? BRG : BGR;
}
}

class LEDDriver {
public:
    void begin() {
        FastLED.addLeds<WS2812, Config::ledPin, LEDDetail::colorOrder(Config::ledColorOrder)>(leds_, Config::ledCount);
        FastLED.setBrightness(Config::brightness);
        fill_solid(leds_, Config::ledCount, CRGB::Black);
        FastLED.show();
    }
    void pulse(Light color, uint32_t now) {
        set(color);
        started_ = now;
        active_ = true;
    }
    void update(uint32_t now) {
        if (active_ && Timing::elapsed(now, started_) >= Config::ledPulseMs) off();
    }
    void off() { active_ = false; set(Light::Off); }
private:
    void set(Light color) {
        if (color == color_) return;
        color_ = color;
        CRGB value = CRGB::Black;
        if (color == Light::Red) value = CRGB::Red;
        if (color == Light::Blue) value = CRGB::Blue;
        if (color == Light::White) value = CRGB::White;
        fill_solid(leds_, Config::ledCount, value);
        FastLED.show();
    }
    CRGB leds_[Config::ledCount];
    Light color_ = Light::Off;
    bool active_ = false;
    uint32_t started_ = 0;
};
