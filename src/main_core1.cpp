#include <stdio.h>

#include <chrono>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <Versatile_RotaryEncoder.h>

#include "defines.h"
#include "keypad_scan.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"

queue_t input_queue;

char keypad_keys[KEYPAD_ROW_NUM][KEYPAD_COLUMN_NUM] = {KEYPAD_KEYS};
uint8_t keypad_pin_rows[KEYPAD_ROW_NUM] =
    KEYPAD_ROW_PINS;  // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
uint8_t keypad_pin_column[KEYPAD_COLUMN_NUM] =
    KEYPAD_COLUMN_PINS;  // GIOP16, GIOP4, GIOP0 connect to the column pins



void core1_entry() {
  Versatile_RotaryEncoder encoder(ROTARY_clk, ROTARY_dt, ROTARY_sw);
  PicoKeypad keypad(MAKE_KEYMAP(keypad_keys), keypad_pin_rows,
                    keypad_pin_column, KEYPAD_ROW_NUM, KEYPAD_COLUMN_NUM);
  
  encoder.setHandleRotate([](int direction) {
    if (direction == Versatile_RotaryEncoder::left) {
      input_type rotary_input = {
        .input_source = INPUT_ROTARY,
        .value = ROTARY_LEFT
      };
      queue_add_blocking(&input_queue, &rotary_input);
    } else if (direction == Versatile_RotaryEncoder::right) {
      input_type rotary_input = {
        .input_source = INPUT_ROTARY,
        .value = ROTARY_RIGHT
      };
      queue_add_blocking(&input_queue, &rotary_input);
    }
  });
  encoder.setHandlePressRelease([]() {
    input_type rotary_input = {
      .input_source = INPUT_ROTARY,
      .value = ROTARY_PRESS
    };
    queue_add_blocking(&input_queue, &rotary_input);
});
  encoder.setHandleDoublePress([]() {
    input_type rotary_input = {
      .input_source = INPUT_ROTARY,
      .value = ROTARY_DOUBLEPRESS
    };
    queue_add_blocking(&input_queue, &rotary_input);

  });
  encoder.setHandleLongPressRelease([]() {
    input_type rotary_input = {
      .input_source = INPUT_ROTARY,
      .value = ROTARY_LONGPRESS
    };
    queue_add_blocking(&input_queue, &rotary_input);

  });

  while (true) {
    encoder.ReadEncoder();
    char key = keypad.getKey();
    if (key != 0) {
      input_type keypad_input = {
        .input_source = INPUT_KEYPAD,
        .value = key
      };
      queue_add_blocking(&input_queue, &keypad_input);
    }
    sleep_ms(1);
  }
}

bool getFromQueue(input_type &input){
  return queue_try_remove(&input_queue, &input);
}

void launchCore1(){
  queue_init(&input_queue, sizeof(input_type), 2);
  multicore_launch_core1(core1_entry);
}
