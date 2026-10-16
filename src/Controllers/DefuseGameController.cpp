#include "DefuseGameController.h"
#include <string.h>

bool DefuseGameController::plant(const char* entered, const char* expected, uint32_t now) {
    if (state_ != State::Planting || !duration_ || !entered[0] || strcmp(entered, expected)) return false;
    state_ = State::Armed;
    start_ = now;
    hold_.reset();
    return true;
}

void DefuseGameController::update(uint32_t now, bool blue, bool red) {
    if (state_ != State::Armed) return;
    // The deadline wins when a hold finishes at or after expiration.
    if (!remaining(now)) { state_ = State::Exploded; hold_.reset(); return; }
    const uint32_t held = hold_.update(blue && red, now);
    progress_ = static_cast<uint8_t>(held >= Config::defuseMs ? 100 : held * 100UL / Config::defuseMs);
    if (hold_.active() && held >= Config::defuseMs) {
        state_ = State::Defused;
        defusedAfter_ = Timing::elapsed(now, start_);
        hold_.reset();
    }
}
