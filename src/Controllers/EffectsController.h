#pragma once
#include <Arduino.h>
#include "Drivers/LEDDriver.h"

// All sounds and light pulses advance from the main loop.
class EffectsController {
public:
    explicit EffectsController(LEDDriver& leds) : leds_(leds) {}
    void begin() { pinMode(Config::buzzerPin, OUTPUT); stop(); }
    void stop() {
        mode_ = Mode::Silent;
        noTone(Config::buzzerPin);
        leds_.off();
    }
    void countdown(uint32_t now) { stop(); mode_ = Mode::Countdown; last_ = now; }
    void alarm(uint32_t now) {
        stop(); mode_ = Mode::Alarm; last_ = now;
        tone(Config::buzzerPin, Config::alarmFrequency);
        leds_.pulse(Light::Red, now);
    }
    void victory(uint32_t now) {
        stop(); mode_ = Mode::Victory; note_ = 0; last_ = now;
        playNote();
    }
    void update(uint32_t now, uint32_t left, uint32_t duration, Light color) {
        leds_.update(now);
        if (mode_ == Mode::Countdown && left) {
            const uint32_t interval = Timing::beepInterval(left, duration, Config::fastestBeepMs, Config::slowestBeepMs);
            if (Timing::elapsed(now, last_) >= interval) {
                last_ = now;
                tone(Config::buzzerPin, Config::beepFrequency, Config::beepDurationMs);
                leds_.pulse(color, now);
            }
        } else if (mode_ == Mode::Alarm) {
            if (Timing::elapsed(now, last_) >= Config::alarmDurationMs) stop();
        } else if (mode_ == Mode::Victory) {
            if (Timing::elapsed(now, last_) >= Config::victoryStepsMs[note_]) {
                last_ = now;
                if (++note_ >= Config::victoryNoteCount) stop();
                else playNote();
            }
        }
    }
private:
    enum class Mode : uint8_t { Silent, Countdown, Alarm, Victory };
    void playNote() {
        tone(Config::buzzerPin, Config::victoryNotes[note_], Config::victoryDurationsMs[note_]);
    }
    LEDDriver& leds_;
    Mode mode_ = Mode::Silent;
    uint32_t last_ = 0;
    uint8_t note_ = 0;
};
