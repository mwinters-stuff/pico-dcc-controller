#include <hardware/adc.h>
#include <pico/stdlib.h>


#include "pot_reader.h"


PotReader::PotReader(uint8_t pin) : _pin(pin) {
    adc_init();
    adc_gpio_init(_pin);
    adc_select_input(0); // Select ADC input 0
}

int PotReader::readValue() {
    adc_select_input(0); // Ensure we are reading from the correct ADC input
    uint32_t sum = 0;
    const int samples = 50;
    for (int i = 0; i < samples; ++i) {
        sum += adc_read();
        sleep_us(100); // Small delay between samples (optional, adjust as needed)
    }
    uint16_t avg_value = sum / samples;
    return avg_value; // Return the averaged value
}



