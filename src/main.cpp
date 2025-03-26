#include <stdio.h>

#include <chrono>

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <Versatile_RotaryEncoder.h>

#include "defines.h"
#include "display_controls.h"
#include "keypad_scan.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "wifi_scan.h"
#include "main_core1.h"

#include "../config.h"

int main() {
  stdio_init_all();


  // Initialise the Wi-Fi chip
  if (cyw43_arch_init()) {
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
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_AP, WIFI_PASS,
                                         WIFI_AUTH, 30000)) {
    printf("failed to connect.\n");
    return 1;
  } else {
    printf("Connected.\n");
    // Read the ip address in a human readable way
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1],
           ip_address[2], ip_address[3]);
  }

  launchCore1();

  DisplayControls displayControls;
  displayControls.begin();

  auto wifiScan = std::make_shared<WifiScan>();
  wifiScan->scanWifi();

  displayControls.showScreen(
      wifiScan, [wifiScan](u8g2_t &u8g2, DisplayControls *displayControls) {
        wifiScan->buildMenu(u8g2, displayControls);
      });

  // Track the last activity time
  absolute_time_t last_activity_time = get_absolute_time();
  bool is_display_sleeping = false;

  while (true) {
    displayControls.drawScreen();

    input_type input_value;
    if (getFromQueue(input_value)) {
      // Update last activity time
      last_activity_time = get_absolute_time();

      // Wake up the display if it is sleeping
      if (is_display_sleeping) {
        printf("Waking up the display\n");
        displayControls.SetPowerSave(false);
        is_display_sleeping = false;
      }
      if (input_value.input_source == INPUT_ROTARY) {
        displayControls.rotateAction(input_value.value);
        switch (input_value.value) {
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
            printf("Unknown rotary event %d\n", input_value.value);
            break;
        }
      } else if (input_value.input_source == INPUT_KEYPAD) {
        printf("Key pressed: %c\n", input_value.value);
        displayControls.keyAction(input_value.value);
      }

      // Check if 1 minute has passed since the last activity
      if (!is_display_sleeping &&
          absolute_time_diff_us(last_activity_time, get_absolute_time()) >
              60000000) {
        printf("Putting the display to sleep\n");
        is_display_sleeping = true;
        displayControls.SetPowerSave(true);
      }

      sleep_ms(10);  // Small delay to avoid busy looping
    }
  }
}
