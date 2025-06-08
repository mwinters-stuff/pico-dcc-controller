#include <stdio.h>

#include <chrono>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <Versatile_RotaryEncoder.h>
#include "pot_reader.h"
// #include "debounce_button.h"
#include "debounce_button_pcf8575.h"


#include "defines.h"
#include "keypad_scan.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "sensor/PCF8575.h"

queue_t input_queue;

char keypad_keys[KEYPAD_ROW_NUM][KEYPAD_COLUMN_NUM] = {KEYPAD_KEYS};
uint8_t keypad_pin_rows[KEYPAD_ROW_NUM] =
    KEYPAD_ROW_PINS;  // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
uint8_t keypad_pin_column[KEYPAD_COLUMN_NUM] =
    KEYPAD_COLUMN_PINS;  // GIOP16, GIOP4, GIOP0 connect to the column pins



void core1_entry() {
  int last_speed_value = -1; // Initialize to an invalid value
  Versatile_RotaryEncoder encoder(ROTARY_clk, ROTARY_dt, ROTARY_sw);
  PicoKeypad keypad(MAKE_KEYMAP(keypad_keys), keypad_pin_rows,
                    keypad_pin_column, KEYPAD_ROW_NUM, KEYPAD_COLUMN_NUM);
  PotReader pot_reader(POT_PIN);
  // picodebounce::PicoDebounceButton reverseButton(BUTTON_REVERSE_PIN, 10); // Button with 10ms debounce, active low


  i2c_init(I2C_PORT_1, 400 * 1000);

  gpio_set_function(I2C_SDA_1, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_1, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_1);
  gpio_pull_up(I2C_SCL_1);


  PCF8575 pcf8575(PCF8574_I2C_ADDR);
  if (!pcf8575.begin(I2C_PORT_1)) {
    printf("PCF8575 initialization failed!\n");
    return;
  }

  picodebounce::PCF8575DebounceButton reverseButtonPCF(&pcf8575, PCF8574_PIN_REVERSE, 10); // Button with 10ms debounce, active low

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
    int pot_value = pot_reader.readValue();
    if (pot_value >= 0) {
      // Scale pot_value (0-4095) to speed (0-MAX_LOCO_SPEED)
      int speed = pot_value;
      if(speed == 1) {
        speed = 0; // Treat 1 as stop
      }
      if (last_speed_value + 1 < speed || last_speed_value - 1 > speed ) {
        // Only send input if the speed value has changed significantly
        last_speed_value = speed;

        printf("Potentiometer value: %d, Speed: %d\n", pot_value, speed);
        input_type pot_input = {
          .input_source = INPUT_POT,
          .value = static_cast<uint8_t>(speed)
        };
        queue_add_blocking(&input_queue, &pot_input);
      }
    }

    if(reverseButtonPCF.update()) {
      if (reverseButtonPCF.getPressed()) {
        printf("PCF Button pressed, sending input\n");
        input_type button_input = {
          .input_source = INPUT_BUTTON_REVERSE,
          .value = 1 // Button pressed
        };
        queue_add_blocking(&input_queue, &button_input);
      } else if (reverseButtonPCF.getReleased()) {
        printf("PCF Button released, sending input\n");
        input_type button_input = {
          .input_source = INPUT_BUTTON_REVERSE,
          .value = 0 // Button released
        };
        queue_add_blocking(&input_queue, &button_input);
      }
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
