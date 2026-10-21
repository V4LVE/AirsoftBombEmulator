#include "DominationGameController.h"

void DominationGameController::start(uint32_t now, uint32_t duration) {
    *this = DominationGameController();
    start_ = now;
    duration_ = duration;
    running_ = duration > 0;
}

void DominationGameController::creditUntil(uint32_t elapsed) {
    if (elapsed <= lastElapsed_) return;
    const uint32_t delta = elapsed - lastElapsed_;
    if (owner_ == Team::Blue) blueTime_ += delta;
    if (owner_ == Team::Red) redTime_ += delta;
    lastElapsed_ = elapsed;
}

void DominationGameController::update(uint32_t now, bool blue, bool red) {
    if (!running_) return;
    const uint32_t used = duration_ - remaining(now);

    const Team requested = blue == red ? Team::Neutral : (blue ? Team::Blue : Team::Red);
    if (requested != contender_) hold_.reset();
    contender_ = requested;
    if (requested == Team::Neutral || requested == owner_) {
        hold_.reset();
        progress_ = 0;
    } else {
        const uint32_t held = hold_.update(true, now);
        const uint32_t holdStarted = Timing::elapsed(now, start_) - held;
        progress_ = static_cast<uint8_t>(held >= Config::captureMs ? 100 : held * 100UL / Config::captureMs);
        // Split scoring at actual hold thresholds, even after a late loop iteration.
        const uint32_t neutralAt = holdStarted + Config::neutralizeMs;
        const uint32_t captureAt = holdStarted + Config::captureMs;
        if (held >= Config::neutralizeMs && neutralAt <= used) {
            creditUntil(neutralAt);
            owner_ = Team::Neutral;
        }
        if (held >= Config::captureMs && captureAt <= used) {
            creditUntil(captureAt);
            owner_ = requested;
            hold_.reset();
        }
    }
    creditUntil(used);
    if (used == duration_) stop();
}
