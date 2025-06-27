#ifndef _DEFINES_H
#define _DEFINES_H

// #define USE_MDNS 0
// 4 and 5 are used by the I2C bus
#define I2C_PORT i2c0
#define I2C_SDA PICO_DEFAULT_I2C_SDA_PIN
#define I2C_SCL PICO_DEFAULT_I2C_SCL_PIN

#define I2C_PORT_1 i2c1
#define I2C_SDA_1 2
#define I2C_SCL_1 3

#define ROTARY_clk 20
#define ROTARY_dt 21
#define ROTARY_sw 19

#define ROTARY_LEFT 0
#define ROTARY_RIGHT 1
#define ROTARY_PRESS 2
#define ROTARY_DOUBLEPRESS 3
#define ROTARY_LONGPRESS 4

#define KEYPAD_ROW_NUM     4
#define KEYPAD_COLUMN_NUM  4
#define KEYPAD_KEYS {'1', '2', '3', 'A'},  {'4', '5', '6', 'B'},  {'7', '8', '9', 'C'},  {'*', '0', '#', 'D'}
#define KEYPAD_COLUMN_PINS    {13, 12, 11, 10}
#define KEYPAD_ROW_PINS {9, 8, 7, 6}

#define INPUT_ROTARY 1
#define INPUT_KEYPAD 2
#define INPUT_POT 3
#define INPUT_BUTTON_REVERSE 4
#define INPUT_BUTTON_LOCO_1 5
#define INPUT_BUTTON_LOCO_2 6
#define INPUT_BUTTON_LOCO_3 7
#define INPUT_BUTTON_LOCO_4 8
#define INPUT_BUTTON_STOP_ALL 9

#define INPUT_BUTTONS_NAME {"","Rotary","Keypad","Pot","Reverse", "Loco 1", "Loco 2", "Loco 3", "Loco 4", "Stop All"}

#define POT_PIN 26


#define PCF8575_I2C_ADDR 0x20

// PCF8575 pins

#define PCF8575_PIN_LOCO_SHOW_SPEED_1 7
#define PCF8575_PIN_LOCO_SHOW_SPEED_2 6
#define PCF8575_PIN_LOCO_SHOW_SPEED_3 5
#define PCF8575_PIN_LOCO_SHOW_SPEED_4 4
#define PCF8575_PIN_LOCO_SHOW_SPEED_5 3
#define PCF8575_PIN_LOCO_SHOW_SPEED_6 2
#define PCF8575_PIN_LOCO_SHOW_SPEED_7 1
#define PCF8575_PIN_LOCO_SHOW_SPEED_8 0


#define PCF8575_PIN_LOCO_1 8
#define PCF8575_PIN_LOCO_2 9
#define PCF8575_PIN_LOCO_3 10
#define PCF8575_PIN_LOCO_4 11

#define PCF8575_PIN_LOCO_LED_1 12
#define PCF8575_PIN_LOCO_LED_2 13
#define PCF8575_PIN_LOCO_LED_3 14
#define PCF8575_PIN_LOCO_LED_4 15


// pin buttons
#define PIN_REVERSE 17
#define PIN_STOP_ALL 16

#define DEBOUNCE_TIME_MS 10 // milliseconds
#define BUTTON_PRESSED 1
#define BUTTON_RELEASED 0
#define BUTTON_LONG_PRESS 2

struct input_type{
  uint8_t input_source;
  uint8_t value;
};


#define MAINSCREEN_FONT             u8g2_font_unifont_t_cyrillic

// 12x16 wrong? small gothic font
#define SMALL_TEXT_FONT             u8g2_font_glasstown_nbp_t_all
#define SMALL_TEXT_FONT_Y_OFFSET    12

// 12x16 wrong? small gothic font
#define MAIN_MENU_FONT1             u8g2_font_glasstown_nbp_t_all

// 10x14
#define MAIN_MENU_FONT2             u8g2_font_bauhaus2015_tr    //  u8g2_font_smart_patrol_nbp_tr

// 10x14 OK! thin, quite readable
#define MAIN_MENU_FONT3             u8g2_font_unifont_t_cyrillic

// 11x15    nice bold font
#define PAGE_TITLE_FONT             u8g2_font_bauhaus2015_tr
#define PAGE_TITLE_FONT_Y_OFFSET    15

// 12x16 wrong? small gothic font
#define PAGE_TITLE_FONT_SMALL       u8g2_font_glasstown_nbp_t_all

// large numeric font for digits sliders
#define NUMERIC_FONT1               u8g2_font_profont29_tn


#define MAIN_MENU_X_OFFSET  10
#define MAIN_MENU_Y_OFFSET  15
#define MAIN_MENU_Y_SHIFT   16
#define MAIN_MENU_ROWS      3
#define MENU_LIST_Y_OFFSET  25

#define PAGE_BACK_BTN_X_OFFSET  100
#define PAGE_BACK_BTN_Y_OFFSET  55

#define MAX_LOCO_SPEED 126
#define MAX_LOCO_ADDRESS 127

#endif
