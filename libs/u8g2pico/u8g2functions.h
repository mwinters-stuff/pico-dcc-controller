
#ifndef U8G2FUNCTIONS_H
#define U8G2FUNCTIONS_H
#include "pico/stdlib.h"
#include <u8g2.h>


uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);



#endif