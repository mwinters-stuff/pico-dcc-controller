#ifndef _MAIN_CORE1_H
#define _MAIN_CORE1_H

#include "defines.h"

void launchCore1();
bool getFromQueue(input_type &input);
void queue_led_command(uint8_t pin, bool on);
void turn_off_loco_leds();
uint8_t get_loco_led_from_input(uint8_t input_source);

#endif