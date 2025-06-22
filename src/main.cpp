#include <stdio.h>

#include <chrono>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "defines.h"
#include "display_controls.h"
#include "wifi_control.h"
#include "main_core1.h"
#include "main_menu.h"
#include "loco_controller.h"
#include "loco_control.h"

int main() {
  stdio_init_all();


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
  
  launchCore1();

  auto mainMenu = std::make_shared<MainMenu>(displayControls);
  mainMenu->showMenu();

  auto locoController = LocoController::initInstance();
  
  while (true) {
    cyw43_arch_poll();
    displayControls->loop();
    wifiControl->loop();
    locoController->loop();
    sleep_ms(10);  // Small delay to avoid busy looping
  }
}
