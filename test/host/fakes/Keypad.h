#pragma once
#include "Arduino.h"
#define makeKeymap(value) reinterpret_cast<char*>(value)
class Keypad {
public:
    Keypad(char*, byte*, byte*, byte, byte) {}
    void setDebounceTime(uint16_t) {}
    char getKey() { const char key = Fake::key; Fake::key = 0; return key; }
};
