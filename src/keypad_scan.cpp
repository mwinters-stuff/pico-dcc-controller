#include "keypad_scan.h"

#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <pico/stdlib.h>

PicoKeypad::PicoKeypad(char *keymap, const uint8_t *rowPins, const uint8_t *columnPins,
                       uint8_t numRows, uint8_t numCols) {
  _keymap = keymap;
  _row_pins = rowPins;
  _column_pins = columnPins;
  _num_rows = numRows;
  _num_cols = numCols;
  _prev_key = 0;
  _debounce_time = 300;  // default value is 300ms
  _prev_time = 0;

  for (int col = 0; col < _num_cols; col++) {
    gpio_init(_column_pins[col]);
    gpio_set_dir(_column_pins[col], GPIO_OUT);
    gpio_put(_column_pins[col], 1);
  }
  for (int row = 0; row < _num_rows; row++) {
    gpio_init(_row_pins[row]);
    gpio_set_dir(_row_pins[row], GPIO_IN);
    gpio_pull_up(_row_pins[row]);
  }
}

char PicoKeypad::getKey() {
  for (int col = 0; col < _num_cols; col++) {
    gpio_put(_column_pins[col], 0);
    for (int row = 0; row < _num_rows; row++) {
      if (gpio_get(_row_pins[row]) == 0) {
        char key = _keymap[row * _num_cols + col];
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if ((_prev_key != key) ||
            (_prev_key == key && (now - _prev_time) > _debounce_time)) {
          _prev_key = key;
          _prev_time = now;
          gpio_put(_column_pins[col], 1);
          return key;
        }
      }
    }
    gpio_put(_column_pins[col], 1);
  }
  return 0;
}

void PicoKeypad::setDebounceTime(uint32_t time) { _debounce_time = time; }
