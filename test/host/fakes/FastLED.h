#pragma once
#include "Arduino.h"
struct CRGB {
    uint8_t r = 0, g = 0, b = 0;
    static const CRGB Black, Red, Blue, White;
};
inline const CRGB CRGB::Black = {0, 0, 0};
inline const CRGB CRGB::Red = {255, 0, 0};
inline const CRGB CRGB::Blue = {0, 0, 255};
inline const CRGB CRGB::White = {255, 255, 255};
namespace Fake { inline CRGB stripColor; }
inline void fill_solid(CRGB* data, unsigned count, CRGB color) {
    Fake::stripColor = color;
    for (unsigned i = 0; i < count; ++i) data[i] = color;
}
constexpr int NEOPIXEL = 0;
struct FakeFastLED {
    template<int, uint8_t> void addLeds(CRGB*, unsigned) {}
    void setBrightness(uint8_t) {}
    void show() { ++Fake::ledShows; }
};
inline FakeFastLED FastLED;
