#include <stdio.h>

#include <chrono>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "defines.h"
#include "display_controls.h"
#include "wifi_control.h"
#include "main_core1.h"
#include "main_menu.h"

int main() {
  stdio_init_all();


  auto displayControls = std::make_shared<DisplayControls>();
  displayControls->begin();

  auto wifiControl = std::make_shared<WifiControl>(displayControls);
  if(!wifiControl->connect()){
    return 1;
  }
  
  launchCore1();

  auto mainMenu = std::make_shared<MainMenu>(displayControls);
  mainMenu->showMenu();
  
  while (true) {
    displayControls->loop();
    sleep_ms(10);  // Small delay to avoid busy looping
    cyw43_arch_poll();
  }
}
