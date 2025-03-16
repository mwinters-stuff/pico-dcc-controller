#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
// #include "hardware/dma.h"
// #include "hardware/pio.h"
// #include "hardware/interp.h"
// #include "hardware/timer.h"
// #include "hardware/watchdog.h"
// #include "hardware/clocks.h"
#include "pico/cyw43_arch.h"
// #include "hardware/uart.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"

#include <Versatile_RotaryEncoder.h>

#include "wifi_scan.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
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

queue_t rotary_encoder_queue;

void core1_entry()
{
    Versatile_RotaryEncoder encoder(ROTARY_clk, ROTARY_dt, ROTARY_sw);

    encoder.setHandleRotate([](int direction)
                            {
        if (direction == Versatile_RotaryEncoder::left) {
            int8_t val = ROTARY_LEFT;
            queue_add_blocking(&rotary_encoder_queue, &val);
        } else if (direction == Versatile_RotaryEncoder::right) {
            int8_t val = ROTARY_RIGHT;
            queue_add_blocking(&rotary_encoder_queue, &val);
        } });
    encoder.setHandlePressRelease([]()
                           {
        int8_t val = ROTARY_PRESS;
        queue_add_blocking(&rotary_encoder_queue, &val); });
    encoder.setHandleDoublePress([]()
                                 {
        int8_t val = ROTARY_DOUBLEPRESS;
        queue_add_blocking(&rotary_encoder_queue, &val); });
    encoder.setHandleLongPressRelease([]()
                               {
        int8_t val = ROTARY_DOUBLEPRESS;
        queue_add_blocking(&rotary_encoder_queue, &val); });

    while (true)
    {
        encoder.ReadEncoder();
        sleep_ms(1);
    }
}

int main()
{
    stdio_init_all();

    queue_init(&rotary_encoder_queue, sizeof(uint8_t), 2);

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("WINTERS", "tesskatt", CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        return 1;
    }
    else
    {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    multicore_launch_core1(core1_entry);

    WifiScan wifiScan;
    wifiScan.scanWifi();

    while (true)
    {
        uint8_t val;
        queue_remove_blocking(&rotary_encoder_queue, &val);
        switch (val)
        {
        case ROTARY_LEFT:
            printf("Rotary left\n");
            break;
        case ROTARY_RIGHT:
            printf("Rotary right\n");
            break;
        case ROTARY_PRESS:
            printf("Rotary press\n");
            break;
        case ROTARY_DOUBLEPRESS:
            printf("Rotary double press\n");
            break;
        case ROTARY_LONGPRESS:
            printf("Rotary long press\n");
            break;
        default:
            printf("Unknown rotary event %d\n", val);
            break;
        }
    }
}
