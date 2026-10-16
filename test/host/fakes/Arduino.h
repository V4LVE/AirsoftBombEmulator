#pragma once
#include <stdint.h>
#include <string.h>
#include <vector>

using byte = uint8_t;
constexpr int LOW = 0, HIGH = 1, INPUT_PULLUP = 2, OUTPUT = 3;
class __FlashStringHelper;
#define F(value) reinterpret_cast<const __FlashStringHelper*>(value)
inline char pgm_read_byte(const char* source) { return *source; }
namespace Fake {
struct Sound { uint32_t at; unsigned frequency; unsigned duration; };
inline uint32_t now = 0;
inline bool pins[32] = {};
inline char key = 0;
inline std::vector<Sound> sounds;
inline unsigned frequency = 0;
inline char lcd[4][21] = {};
inline unsigned lcdWrites = 0;
inline unsigned ledShows = 0;
}
inline uint32_t millis() { return Fake::now; }
inline void pinMode(uint8_t, int) {}
inline int digitalRead(uint8_t pin) { return Fake::pins[pin] ? LOW : HIGH; }
inline void tone(uint8_t, unsigned frequency, unsigned duration = 0) {
    Fake::frequency = frequency;
    Fake::sounds.push_back({Fake::now, frequency, duration});
}
inline void noTone(uint8_t) { Fake::frequency = 0; }
