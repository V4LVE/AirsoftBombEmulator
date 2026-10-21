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
enum EOrder { RGB = 0012, RBG = 0021, GRB = 0102, GBR = 0120, BRG = 0201, BGR = 0210 };
namespace Fake {
inline CRGB stripColor;
inline EOrder stripOrder = GRB;
inline uint8_t wireBytes[3] = {};
}
inline void fill_solid(CRGB* data, unsigned count, CRGB color) {
    Fake::stripColor = color;
    for (unsigned i = 0; i < count; ++i) data[i] = color;
}
constexpr int WS2812 = 0;
struct FakeFastLED {
    template<int, uint8_t, EOrder order> void addLeds(CRGB*, unsigned) { Fake::stripOrder = order; }
    void setBrightness(uint8_t) {}
    void show() {
        const uint8_t channels[] = {Fake::stripColor.r, Fake::stripColor.g, Fake::stripColor.b};
        Fake::wireBytes[0] = channels[(Fake::stripOrder >> 6) & 3];
        Fake::wireBytes[1] = channels[(Fake::stripOrder >> 3) & 3];
        Fake::wireBytes[2] = channels[Fake::stripOrder & 3];
        ++Fake::ledShows;
    }
};
inline FakeFastLED FastLED;
