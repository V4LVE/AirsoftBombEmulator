#include "Application.h"
#include <string.h>

namespace {
char keys[Config::keypadRows][Config::keypadColumns];
byte rowPins[Config::keypadRows];
byte columnPins[Config::keypadColumns];
}

Application::Application()
    : effects_(leds_), blue_(Config::blueButtonPin), red_(Config::redButtonPin),
      keypad_(makeKeymap(keys), rowPins, columnPins, Config::keypadRows, Config::keypadColumns) {}

void Application::begin() {
    memcpy(keys, Config::keypadKeys, sizeof(keys));
    memcpy(rowPins, Config::keypadRowPins, sizeof(rowPins));
    memcpy(columnPins, Config::keypadColumnPins, sizeof(columnPins));
    strcpy(settings_.plantCode, Config::defaultPlantCode);
    blue_.begin();
    red_.begin();
    keypad_.setDebounceTime(Config::keypadDebounceMs);
    leds_.begin();
    effects_.begin();
    display_.begin();
    render(millis());
}

void Application::show(Screen screen) {
    screen_ = screen;
    input_.clear();
    invalid_ = wrongCode_ = false;
    dirty_ = true;
}

bool Application::isEditing() const {
    return screen_ == Screen::DominationTime || screen_ == Screen::DefuseTime || screen_ == Screen::Code;
}

void Application::edit(char key) {
    const Screen parent = screen_ == Screen::DominationTime ? Screen::DominationSetup : Screen::DefuseSetup;
    if (key == 'B') { show(parent); return; }
    if (key == '*') input_.erase();
    else if (key == 'A') {
        if (input_.empty()) { invalid_ = true; return; }
        if (screen_ == Screen::Code) strcpy(settings_.plantCode, input_.text());
        else {
            const uint16_t minutes = input_.number();
            if (minutes < Config::minGameMinutes || minutes > Config::maxGameMinutes) {
                invalid_ = true;
                return;
            }
            if (screen_ == Screen::DominationTime) settings_.dominationMinutes = minutes;
            else settings_.defuseMinutes = minutes;
        }
        show(parent);
        return;
    } else input_.append(key, screen_ == Screen::Code ? Config::maxCodeDigits : Config::timeInputDigits);
    invalid_ = false;
}

void Application::handleKey(char key, uint32_t now) {
    if (!key) return;
    dirty_ = true;
    if (isEditing()) { edit(key); return; }
    switch (screen_) {
    case Screen::Main:
        if (key == '1') show(Screen::Games);
        break;
    case Screen::Games:
        if (key == '1') show(Screen::DominationSetup);
        else if (key == '2') show(Screen::DefuseSetup);
        else if (key == 'B') show(Screen::Main);
        break;
    case Screen::DominationSetup:
    case Screen::DefuseSetup: {
        const bool dom = screen_ == Screen::DominationSetup;
        if (key == 'B') show(Screen::Games);
        else if (key == '2') show(dom ? Screen::DominationTime : Screen::DefuseTime);
        else if (key == '3' && !dom) show(Screen::Code);
        else if (key == '1') {
            const uint16_t minutes = dom ? settings_.dominationMinutes : settings_.defuseMinutes;
            if (minutes < Config::minGameMinutes || minutes > Config::maxGameMinutes) {
                invalid_ = true;
                break;
            }
            aborted_ = false;
            effects_.stop();
            if (dom) {
                domination_.start(now, static_cast<uint32_t>(minutes) * 60000UL);
                effects_.countdown(now);
            } else defuse_.start(static_cast<uint32_t>(minutes) * 60000UL);
            show(dom ? Screen::Domination : Screen::Defuse);
        }
        break;
    }
    case Screen::Domination:
        if (key == 'B' && (!domination_.running() || red_.pressed())) {
            if (domination_.running()) {
                domination_.stop();
                aborted_ = true;
                effects_.stop();
            } else { effects_.stop(); show(Screen::DominationSetup); }
        }
        break;
    case Screen::Defuse:
        if (defuse_.state() == DefuseGameController::State::Planting) {
            if (key == 'B') { effects_.stop(); show(Screen::DefuseSetup); }
            else if (key == '*') { input_.erase(); wrongCode_ = false; }
            else if (key == '#') {
                if (defuse_.plant(input_.text(), settings_.plantCode, now)) effects_.countdown(now);
                else wrongCode_ = true;
                input_.clear();
            } else if (input_.append(key)) wrongCode_ = false;
        } else if (key == 'B') {
            if (defuse_.state() != DefuseGameController::State::Armed) {
                effects_.stop();
                show(Screen::DefuseSetup);
            } else if (red_.pressed()) { defuse_.stop(); effects_.stop(); }
        }
        break;
    default: break;
    }
}

void Application::update() {
    uint32_t now = millis();
    blue_.update(now);
    red_.update(now);
    // One keypad poll per iteration. No event listener re-entry or nested loops.
    const char key = keypad_.getKey();
    now = millis();
    uint32_t left = 0, duration = 0;
    Light color = Light::White;
    if (screen_ == Screen::Domination) {
        const bool wasRunning = domination_.running();
        domination_.update(now, blue_.pressed(), red_.pressed());
        if (wasRunning && !domination_.running()) { effects_.alarm(now); dirty_ = true; }
    } else if (screen_ == Screen::Defuse) {
        const auto previous = defuse_.state();
        defuse_.update(now, blue_.pressed(), red_.pressed());
        if (previous != defuse_.state()) {
            dirty_ = true;
            if (defuse_.state() == DefuseGameController::State::Defused) effects_.victory(now);
            else if (defuse_.state() == DefuseGameController::State::Exploded) effects_.alarm(now);
        }
    }
    handleKey(key, now);
    if (screen_ == Screen::Domination && domination_.running()) {
        left = domination_.remaining(now);
        duration = domination_.duration();
        color = domination_.owner() == Team::Blue ? Light::Blue :
                domination_.owner() == Team::Red ? Light::Red : Light::White;
    } else if (screen_ == Screen::Defuse && defuse_.state() == DefuseGameController::State::Armed) {
        left = defuse_.remaining(now);
        duration = defuse_.duration();
        color = Light::Red;
    }
    effects_.update(now, left, duration, color);
    if (dirty_ || Timing::elapsed(now, lastRender_) >= Config::displayRefreshMs) render(now);
}

