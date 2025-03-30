// pico_keypad.h
#ifndef _PICO_KEYPAD_H
#define _PICO_KEYPAD_H

#include <stdint.h>
#include <hardware/pio.h>

#define MAKE_KEYMAP(x) ((char*)x)

class PicoKeypad {
private:
    uint8_t _num_rows;
    uint8_t _num_cols;
    char *_keymap;
    uint8_t *_row_pins;
    uint8_t *_column_pins;
    char _prev_key;
    uint32_t _debounce_time;
    uint32_t _prev_time;

public:
    PicoKeypad(char *keymap, uint8_t *rowPins, uint8_t *columnPins, uint8_t numRows, uint8_t numCols);

    char getKey();
    void setDebounceTime(uint32_t time);
};

#endif // PICO_KEYPAD_H
