#pragma once
#include "debounce_button_base.h"

namespace picodebounce {

class PicoDebounceButton : public DebounceButtonBase {
public:
    explicit PicoDebounceButton(uint8_t pin,
                                uint16_t interval = 10,
                                bool state = PRESSED,
                                bool invert = false)
        : DebounceButtonBase(interval, state, invert), mPin(pin)
    {
        gpio_init(mPin);
        gpio_set_dir(mPin, GPIO_IN);
        gpio_pull_up(mPin);
    }

    bool readPin() const override {
        return gpio_get(mPin);
    }

private:
    const uint8_t mPin;
};

} // namespace picodebounce