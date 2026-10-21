#pragma once
#include "Config.h"
#include "Core/Timing.h"

class DefuseGameController {
public:
    enum class State : uint8_t { Planting, Armed, Defused, Exploded, Aborted };
    void start(uint32_t duration) { *this = DefuseGameController(); duration_ = duration; }
    bool plant(const char* entered, const char* expected, uint32_t now);
    void update(uint32_t now, bool blue, bool red);
    void stop() { state_ = State::Aborted; hold_.reset(); }
    State state() const { return state_; }
    uint32_t remaining(uint32_t now) const { return Timing::remaining(now, start_, duration_); }
    uint32_t duration() const { return duration_; }
    uint32_t defusedAfter() const { return defusedAfter_; }
    uint8_t progress() const { return progress_; }
private:
    State state_ = State::Planting;
    Timing::Hold hold_;
    uint32_t start_ = 0, duration_ = 0, defusedAfter_ = 0;
    uint8_t progress_ = 0;
};
