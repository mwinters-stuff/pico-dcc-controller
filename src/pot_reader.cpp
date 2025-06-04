#include <hardware/adc.h>
#include <pico/stdlib.h>
#include <math.h>

#include "defines.h"
#include "pot_reader.h"


PotReader::PotReader(uint8_t pin) : _pin(pin), _lastValue(-1) {
    adc_init();
    adc_gpio_init(_pin);
    adc_select_input(0); // Select ADC input 0
}

int PotReader::readValue() {
    adc_select_input(0); // Ensure we are reading from the correct ADC input
    const int samples = 11; // Use an odd number for median
    uint16_t buffer[samples];

    for (int i = 0; i < samples; ++i) {
        buffer[i] = adc_read();
        sleep_us(100); // Small delay between samples
    }

    // Sort the buffer
    for (int i = 0; i < samples - 1; ++i) {
        for (int j = i + 1; j < samples; ++j) {
            if (buffer[j] < buffer[i]) {
                uint16_t tmp = buffer[i];
                buffer[i] = buffer[j];
                buffer[j] = tmp;
            }
        }
    }

    uint16_t median = buffer[samples / 2];
    int scaled = (median * MAX_LOCO_SPEED) / 4095;

    // // Deadband: only update if change is significant
    // const int threshold = 2;
    // if (_lastValue == -1 || abs(scaled - _lastValue) > threshold) {
    //     _lastValue = scaled;
    // }
    // return _lastValue;
    return scaled;
}
