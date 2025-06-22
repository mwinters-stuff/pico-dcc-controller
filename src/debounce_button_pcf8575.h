
#pragma once
#include "debounce_button_base.h"
#include "sensor/PCF8575.h"

namespace picodebounce {

class PCF8575DebounceButton : public DebounceButtonBase {
 public:
  explicit PCF8575DebounceButton(PCF8575 *pcf8575, uint8_t pin,
                                 uint16_t interval = 10, bool state = PRESSED,
                                 bool invert = false)
      : DebounceButtonBase(interval, state, invert),
        mPCF8575(pcf8575),
        mPin(pin) {}

  bool readPin() const override { return mPCF8575->read(mPin); }

 private:
  PCF8575 *mPCF8575;
  const uint8_t mPin;
};

}  // namespace picodebounce
