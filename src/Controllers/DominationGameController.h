#pragma once
#include "Config.h"
#include "Core/Timing.h"

enum class Team : uint8_t { Neutral, Blue, Red };

// Pure game rules; hardware and presentation are handled by Application.
class DominationGameController {
public:
    void start(uint32_t now, uint32_t duration);
    void update(uint32_t now, bool blue, bool red);
    void stop() { running_ = false; hold_.reset(); }
    bool running() const { return running_; }
    Team owner() const { return owner_; }
    uint32_t remaining(uint32_t now) const { return Timing::remaining(now, start_, duration_); }
    uint32_t duration() const { return duration_; }
    uint32_t blueTime() const { return blueTime_; }
    uint32_t redTime() const { return redTime_; }
    uint8_t progress() const { return progress_; }
private:
    void creditUntil(uint32_t elapsed);
    bool running_ = false;
    Team owner_ = Team::Neutral, contender_ = Team::Neutral;
    Timing::Hold hold_;
    uint32_t start_ = 0, duration_ = 0, lastElapsed_ = 0;
    uint32_t blueTime_ = 0, redTime_ = 0;
    uint8_t progress_ = 0;
};
