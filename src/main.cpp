#include <stdio.h>
#include <chrono>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "hardware/watchdog.h" // Add this include

#include "defines.h"
#include "display_controls.h"
#include "wifi_control.h"
#include "main_core1.h"
#include "main_menu.h"
#include "loco_controller.h"
#include "loco_control.h"
#include "mdns_scan.h"



// #define _WATCHDOG_

int main() {
  stdio_init_all();

  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);


  // Initialize watchdog: timeout 4 seconds, pause on debug

  auto displayControls = std::make_shared<DisplayControls>();
  
  displayControls->begin();
  
  auto wifiControl = WifiControl::initInstance(displayControls);
  if(!wifiControl->init()){
    displayControls->showWifiError();
    printf("Wi-Fi init failed\n");
    return 1;
  }

  while(!wifiControl->connect()){
    sleep_ms(1000);
  }
#ifdef _WATCHDOG_  
  watchdog_enable(4000, 1);
#endif

  launchCore1();

  auto mdnsScan = MDNSScan::initInstance(displayControls);

  auto mainMenu = std::make_shared<MainMenu>(displayControls);
  mainMenu->showMenu();

  auto locoController = LocoController::initInstance(displayControls);
  
  while (true) {
    cyw43_arch_poll();
    displayControls->loop();
    wifiControl->loop();
    if(wifiControl->dccProtocol() != nullptr) {
      locoController->loop();
    }
#ifdef _WATCHDOG_  
    watchdog_update(); // Pet the watchdog in main loop
#endif    
    sleep_ms(10);  // Small delay to avoid busy looping
  }
}
