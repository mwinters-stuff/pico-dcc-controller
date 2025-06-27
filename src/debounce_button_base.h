#pragma once
#include <cstdint>
#include "pico/stdlib.h"

namespace picodebounce {

class DebounceButtonBase {
public:
    static const bool PRESSED = false;
    static const bool RELEASED = true;

    // Add a default long press duration (ms)
    static constexpr uint32_t DEFAULT_LONG_PRESS_DURATION = 1000;

    explicit DebounceButtonBase(uint16_t interval = 10, bool state = PRESSED, bool invert = false, uint32_t longPressDuration = DEFAULT_LONG_PRESS_DURATION)
        : mInterval(interval), mState(state), mInvert(invert), mLongPressDuration(longPressDuration) {}

    // To be implemented by derived class
    virtual bool readPin() const = 0;

    // Call in loop; returns true if state changed
    bool update() {
        bool currentState = mInvert ? !readPin() : readPin();
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (currentState != mLastState) {
            mLastStateTime = now;
            mLongPressFired = false; // Reset long press flag on state change
        }
        if ((now - mLastStateTime) > mInterval) {
            if (currentState != mState) {
                mState = currentState;
                mLastState = currentState;
                return true;
            }
        }
        mLastState = currentState;
        return false;
    }

    bool getState() const { return mState; }
    bool getPressed() const { return mState == PRESSED; }
    bool getReleased() const { return mState == RELEASED; }
    uint32_t getDuration() const {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        return now - mLastStateTime;
    }
    uint16_t interval() const { return mInterval; }
    void interval(uint16_t interval) { mInterval = interval; }
    bool invert() const { return mInvert; }
    void invert(bool invert) { mInvert = invert; }

    // Long press support
    void longPressDuration(uint32_t duration) { mLongPressDuration = duration; }
    uint32_t longPressDuration() const { return mLongPressDuration; }

    // Returns true if button is pressed and held longer than longPressDuration (fires once per press)
    bool isLongPressed() {
        if (getPressed() && !mLongPressFired) {
            if (getDuration() >= mLongPressDuration) {
                mLongPressFired = true;
                return true;
            }
        }
        return false;
    }

protected:
    uint16_t mInterval {};
    bool mState;
    bool mInvert;
    bool mLastState {};
    uint32_t mLastStateTime {};
    uint32_t mLongPressDuration {DEFAULT_LONG_PRESS_DURATION};
    bool mLongPressFired {false};
};

} // namespace picodebounce