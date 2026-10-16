#pragma once
#include <stdint.h>

namespace Timing {
// Unsigned subtraction keeps intervals correct across millis() rollover.
inline uint32_t elapsed(uint32_t now, uint32_t since) { return now - since; }
inline uint32_t remaining(uint32_t now, uint32_t start, uint32_t duration) {
    const uint32_t used = elapsed(now, start);
    return used >= duration ? 0 : duration - used;
}
inline uint32_t seconds(uint32_t milliseconds) { return (milliseconds + 999) / 1000; }
inline uint32_t beepInterval(uint32_t left, uint32_t duration, uint32_t fastest, uint32_t slowest) {
    if (!duration) return fastest;
    if (left > duration) left = duration;
    return fastest + static_cast<uint32_t>(static_cast<uint64_t>(left) * (slowest - fastest) / duration);
}

class Hold {
public:
    void reset() { active_ = false; }
    uint32_t update(bool pressed, uint32_t now) {
        if (!pressed) { reset(); return 0; }
        if (!active_) { active_ = true; start_ = now; }
        return elapsed(now, start_);
    }
    bool active() const { return active_; }
private:
    bool active_ = false;
    uint32_t start_ = 0;
};
}
