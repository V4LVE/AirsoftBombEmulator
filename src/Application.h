#pragma once
#include <Keypad.h>
#include "Config.h"
#include "Core/DigitInput.h"
#include "Controllers/DisplayController.h"
#include "Controllers/DominationGameController.h"
#include "Controllers/DefuseGameController.h"
#include "Controllers/EffectsController.h"
#include "Drivers/Button.h"

class Application {
public:
    Application();
    void begin();
    void update();
private:
    enum class Screen : uint8_t { Main, Games, DominationSetup, DefuseSetup,
        DominationTime, DefuseTime, Code, Domination, Defuse };
    void handleKey(char key, uint32_t now);
    void edit(char key);
    void show(Screen screen);
    void render(uint32_t now);
    bool isEditing() const;
    GameSettings settings_;
    DigitInput input_;
    DisplayController display_;
    LEDDriver leds_;
    EffectsController effects_;
    Button blue_, red_;
    Keypad keypad_;
    DominationGameController domination_;
    DefuseGameController defuse_;
    Screen screen_ = Screen::Main;
    bool dirty_ = true, invalid_ = false, wrongCode_ = false, aborted_ = false;
    uint32_t lastRender_ = 0;
};
