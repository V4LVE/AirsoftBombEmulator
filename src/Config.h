#pragma once
#include <stdint.h>

// Hardware and behavior defaults. Runtime menu settings live in GameSettings.
namespace Config {
enum class LedColorOrder : uint8_t { RGB, RBG, GRB, GBR, BRG, BGR };
constexpr uint8_t buzzerPin = 10, blueButtonPin = 11, redButtonPin = 12;
constexpr uint8_t ledPin = 13, ledCount = 60, brightness = 255;
// BRG swaps the green/blue channels relative to NEOPIXEL's fixed GRB order.
constexpr LedColorOrder ledColorOrder = LedColorOrder::BRG;
constexpr uint8_t lcdAddress = 0x27, lcdColumns = 20, lcdRows = 4;
constexpr uint8_t keypadRows = 4, keypadColumns = 4;
constexpr uint8_t keypadRowPins[keypadRows] = {9, 8, 7, 6};
constexpr uint8_t keypadColumnPins[keypadColumns] = {5, 4, 3, 2};
constexpr char keypadKeys[keypadRows][keypadColumns] = {
    {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}
};
constexpr uint8_t lcdEnable = 2, lcdReadWrite = 1, lcdRegisterSelect = 0;
constexpr uint8_t lcdData4 = 4, lcdData5 = 5, lcdData6 = 6, lcdData7 = 7;
constexpr uint8_t lcdBacklightPin = 3;
constexpr bool lcdBacklightPositive = true;
constexpr uint16_t keypadDebounceMs = 10, buttonDebounceMs = 25;
constexpr uint32_t displayRefreshMs = 100;
constexpr uint32_t neutralizeMs = 4000, captureMs = 8000, defuseMs = 8000;
constexpr uint32_t fastestBeepMs = 30, slowestBeepMs = 5000;
constexpr uint16_t beepDurationMs = 12, ledPulseMs = 12;
constexpr uint16_t beepFrequency = 2000, alarmFrequency = 2500;
constexpr uint32_t alarmDurationMs = 3000;
constexpr uint8_t maxCodeDigits = 6;
constexpr uint8_t timeInputDigits = 3;
constexpr uint16_t minGameMinutes = 1, maxGameMinutes = 999;
constexpr uint16_t defaultGameMinutes = 0; // Require a duration before starting.
constexpr char defaultPlantCode[] = "1234";
constexpr uint16_t victoryNotes[] = {523, 659, 784, 1047};
constexpr uint16_t victoryDurationsMs[] = {150, 150, 200, 300};
constexpr uint16_t victoryStepsMs[] = {180, 180, 250, 350};
constexpr uint8_t victoryNoteCount = 4;
static_assert(neutralizeMs < captureMs, "Neutralization must precede capture");
static_assert(fastestBeepMs > beepDurationMs && fastestBeepMs > ledPulseMs, "Pulses need an off period");
static_assert(slowestBeepMs >= fastestBeepMs, "Invalid beep range");
static_assert(captureMs && defuseMs, "Hold durations must be positive");
static_assert(sizeof(defaultPlantCode) <= maxCodeDigits + 1, "Default code exceeds input capacity");
static_assert(sizeof(defaultPlantCode) > 1, "Default code must not be empty");
static_assert(timeInputDigits == 3 && maxGameMinutes <= 999 && minGameMinutes > 0, "Time editor supports 1-999 minutes");
static_assert(maxCodeDigits >= timeInputDigits && maxCodeDigits <= 6, "Input and code layouts support up to 6 digits");
static_assert(lcdColumns == 20 && lcdRows == 4, "Screen layouts require a 20x4 LCD");
}

struct GameSettings {
    uint16_t dominationMinutes = Config::defaultGameMinutes;
    uint16_t defuseMinutes = Config::defaultGameMinutes;
    char plantCode[Config::maxCodeDigits + 1] = {};
};
