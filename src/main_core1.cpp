#include <stdio.h>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include <Versatile_RotaryEncoder.h>
#include "pot_reader.h"
#include "debounce_button_pcf8575.h"
#include "debounce_button.h"

#include "defines.h"
#include "keypad_scan.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "sensor/PCF8575.h"

#include "main_core1.h"

queue_t input_queue;
queue_t led_queue;
queue_t tcp_fail_queue;

const char keypad_keys[KEYPAD_ROW_NUM][KEYPAD_COLUMN_NUM] = {KEYPAD_KEYS};
const uint8_t keypad_pin_rows[KEYPAD_ROW_NUM] = KEYPAD_ROW_PINS;  // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
const uint8_t keypad_pin_column[KEYPAD_COLUMN_NUM] = KEYPAD_COLUMN_PINS;  // GIOP16, GIOP4, GIOP0 connect to the column pins

struct buttons{
  uint8_t input_source;
  std::shared_ptr<picodebounce::DebounceButtonBase> button;
};

const uint8_t led_pins[] = {
    PCF8575_PIN_LOCO_SHOW_SPEED_1,
    PCF8575_PIN_LOCO_SHOW_SPEED_2,
    PCF8575_PIN_LOCO_SHOW_SPEED_3,
    PCF8575_PIN_LOCO_SHOW_SPEED_4,
    PCF8575_PIN_LOCO_SHOW_SPEED_5,
    PCF8575_PIN_LOCO_SHOW_SPEED_6,
    PCF8575_PIN_LOCO_SHOW_SPEED_7,
    PCF8575_PIN_LOCO_SHOW_SPEED_8,
    PCF8575_PIN_LOCO_LED_1,
    PCF8575_PIN_LOCO_LED_2,
    PCF8575_PIN_LOCO_LED_3,
    PCF8575_PIN_LOCO_LED_4
};

const std::vector<std::string> inputButtonNames(std::begin(INPUT_BUTTONS_NAME), std::end(INPUT_BUTTONS_NAME));

void turn_off_loco_leds(){
  queue_led_command(PCF8575_PIN_LOCO_LED_1, false);
  queue_led_command(PCF8575_PIN_LOCO_LED_2, false);
  queue_led_command(PCF8575_PIN_LOCO_LED_3, false);
  queue_led_command(PCF8575_PIN_LOCO_LED_4, false);
}

uint8_t get_loco_led_from_input(uint8_t input_source) {
    switch (input_source) {
        case 0: return PCF8575_PIN_LOCO_LED_1;
        case 1: return PCF8575_PIN_LOCO_LED_2;
        case 2: return PCF8575_PIN_LOCO_LED_3;
        case 3: return PCF8575_PIN_LOCO_LED_4;
        default: return 0xFF; // Invalid LED pin
    }
}

void process_led_queue(PCF8575& pcf8575) {
    uint8_t led_cmd;
    while (queue_try_remove(&led_queue, &led_cmd)) {
        uint8_t pin = led_cmd & 0x0F;      // lower 4 bits: pin 0-15
        bool on = (led_cmd & 0x80) != 0;   // high bit: 1=on, 0=off

        if (pin < 16) {
            pcf8575.write(pin, on ? 0 : 1); // LOW to turn on, HIGH to turn off (typical for PCF8575)
            printf("LED %d turned %s\n", pin, on ? "ON" : "OFF");
        }
    }
}

void queue_led_command(uint8_t pin, bool on) {
    if (pin > 15) return; // Only valid for pins 0-15
    uint8_t cmd = (on ? 0x80 : 0x00) | (pin & 0x0F);
    queue_add_blocking(&led_queue, &cmd);
}


void test_loco_leds(PCF8575& pcf8575) {

    // Turn all LEDs off before starting
    for (uint8_t i = 0; i < sizeof(led_pins); ++i) {
        pcf8575.write(led_pins[i], 1); // HIGH = off for PCF8575
    }
    sleep_ms(500);

    // Step through each LED, turning it on for 1 second
    for (uint8_t i = 0; i < sizeof(led_pins); ++i) {
        pcf8575.write(led_pins[i], 0); // LOW = on
    }
    sleep_ms(1000);
    for (uint8_t i = 0; i < sizeof(led_pins); ++i) {
        pcf8575.write(led_pins[i], 1); // HIGH = off
    }
}

void core1_entry() {
  int last_speed_value = -1; // Initialize to an invalid value
  Versatile_RotaryEncoder encoder(ROTARY_clk, ROTARY_dt, ROTARY_sw);
  PicoKeypad keypad(MAKE_KEYMAP(keypad_keys), keypad_pin_rows,
                    keypad_pin_column, KEYPAD_ROW_NUM, KEYPAD_COLUMN_NUM);
  PotReader pot_reader(POT_PIN);

  i2c_init(I2C_PORT_1, 400 * 1000);

  gpio_set_function(I2C_SDA_1, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_1, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_1);
  gpio_pull_up(I2C_SCL_1);


  PCF8575 pcf8575(PCF8575_I2C_ADDR);
  if (!pcf8575.begin(I2C_PORT_1)) {
    printf("PCF8575 initialization failed!\n");
    return;
  }

  printf("PCF8575 initialized successfully!\n");

  test_loco_leds(pcf8575);

  auto buttonVector = std::vector<buttons>{
    {
      .input_source = INPUT_BUTTON_LOCO_1,
      .button = std::make_shared<picodebounce::PCF8575DebounceButton>(&pcf8575, PCF8575_PIN_LOCO_1, DEBOUNCE_TIME_MS)
    },
    {
      .input_source = INPUT_BUTTON_LOCO_2,
      .button = std::make_shared<picodebounce::PCF8575DebounceButton>(&pcf8575, PCF8575_PIN_LOCO_2, DEBOUNCE_TIME_MS)
    },
    {
      .input_source = INPUT_BUTTON_LOCO_3,
      .button = std::make_shared<picodebounce::PCF8575DebounceButton>(&pcf8575, PCF8575_PIN_LOCO_3, DEBOUNCE_TIME_MS)
    },
    {
      .input_source = INPUT_BUTTON_LOCO_4,
      .button = std::make_shared<picodebounce::PCF8575DebounceButton>(&pcf8575, PCF8575_PIN_LOCO_4, DEBOUNCE_TIME_MS)
    },
    {
      .input_source = INPUT_BUTTON_REVERSE,
      .button = std::make_shared<picodebounce::PicoDebounceButton>(PIN_REVERSE, DEBOUNCE_TIME_MS)
    },
    {
      .input_source = INPUT_BUTTON_STOP_ALL,
      .button = std::make_shared<picodebounce::PicoDebounceButton>(PIN_STOP_ALL, DEBOUNCE_TIME_MS)
    }
  };

  encoder.setHandleRotate([](int direction) {
    if (direction == Versatile_RotaryEncoder::left) {
      input_type rotary_input = {
        .input_source = INPUT_ROTARY,
        .value = ROTARY_RIGHT
      };
      queue_add_blocking(&input_queue, &rotary_input);
    } else if (direction == Versatile_RotaryEncoder::right) {
      input_type rotary_input = {
        .input_source = INPUT_ROTARY,
        .value = ROTARY_LEFT
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

  uint32_t last_pot_check = 0; // Add this before the while loop

  while (true) {
    process_led_queue(pcf8575);
    encoder.ReadEncoder();

    char key = keypad.getKey();
    if (key != 0) {
      input_type keypad_input = {
        .input_source = INPUT_KEYPAD,
        .value = key
      };
      queue_add_blocking(&input_queue, &keypad_input);
    }

    // Only run potentiometer section every 100ms
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_pot_check >= 100) {
      last_pot_check = now;

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
    }

    for (auto &button : buttonVector) {
      if (button.button->update()) {
        if (button.button->getPressed()) {
          printf("Button %d %s pressed, sending input\n", static_cast<int>(button.input_source), inputButtonNames[static_cast<int>(button.input_source)].c_str());
          input_type button_input = {
            .input_source = button.input_source,
            .value = BUTTON_PRESSED // Button pressed
          };
          queue_add_blocking(&input_queue, &button_input);
        } else if (button.button->getReleased()) {
          printf("Button %d %s released, sending input\n", static_cast<int>(button.input_source), inputButtonNames[static_cast<int>(button.input_source)].c_str());
          input_type button_input = {
            .input_source = button.input_source,
            .value = BUTTON_RELEASED // Button released
          };
          queue_add_blocking(&input_queue, &button_input);
        } else if(button.button->isLongPressed()) {
          printf("Button %d %s long pressed, sending input\n", static_cast<int>(button.input_source), inputButtonNames[static_cast<int>(button.input_source)].c_str());
          input_type button_input = {
            .input_source = button.input_source,
            .value = BUTTON_LONG_PRESS // Long press
          };
          queue_add_blocking(&input_queue, &button_input);
        }
      }
    }

    process_led_queue(pcf8575);
    sleep_ms(1);
  }
}

bool getFromQueue(input_type &input){
  return queue_try_remove(&input_queue, &input);
}

void launchCore1(){
  queue_init(&input_queue, sizeof(input_type), 2);
  queue_init(&led_queue, sizeof(uint8_t), 16); // 16 LEDs
  queue_init(&tcp_fail_queue, sizeof(size_t), 2); // 2 slots for TCP fail signals
  multicore_launch_core1(core1_entry);
}
