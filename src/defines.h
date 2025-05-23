#ifndef _DEFINES_H
#define _DEFINES_H

// #define USE_MDNS 0

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

#define ROTARY_clk 20
#define ROTARY_dt 21
#define ROTARY_sw 14

#define ROTARY_LEFT 0
#define ROTARY_RIGHT 1
#define ROTARY_PRESS 2
#define ROTARY_DOUBLEPRESS 3
#define ROTARY_LONGPRESS 4

#define KEYPAD_ROW_NUM     4
#define KEYPAD_COLUMN_NUM  4
#define KEYPAD_KEYS {'1', '2', '3', 'A'},  {'4', '5', '6', 'B'},  {'7', '8', '9', 'C'},  {'*', '0', '#', 'D'}
#define KEYPAD_COLUMN_PINS    {6, 7, 8, 9}
#define KEYPAD_ROW_PINS {10, 11, 12, 13}

#define INPUT_ROTARY 1
#define INPUT_KEYPAD 2

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
