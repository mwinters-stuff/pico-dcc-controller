
#include "loco_controller.h"

#include <DCCEXProtocol.h>
#include <stdio.h>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "wifi_control.h"
#include "storage_at24cx.h"
#include "main_core1.h"
#include "cab_control_menu.h"

std::shared_ptr<LocoController> LocoController::instance;

LocoController::LocoController()
    : current_loco_index(0xff), locoControl(LocoControl::initInstance()),
      storage(std::make_shared<StorageAT24Cx>()) {
};

std::shared_ptr<LocoController> LocoController::initInstance(
    std::shared_ptr<DisplayControls> displayControls) {
  instance = std::shared_ptr<LocoController>(new LocoController());
  instance->init(displayControls);
  return instance;
}

void LocoController::init(std::shared_ptr<DisplayControls> displayControls) {
  locoControl->init(displayControls);
  displayControls->addActionInterface(shared_from_this());
  this->displayControls = displayControls;
}

bool LocoController::doAction(mui_event event) { return true; }

void LocoController::doButtonAction(uint8_t action, uint8_t value) {
  switch (action) {
    case INPUT_BUTTON_LOCO_1:
      driveLocoLED(0);  // Drive the first loco
      break;
    case INPUT_BUTTON_LOCO_2:
      driveLocoLED(1);  // Drive the second loco
      break;
    case INPUT_BUTTON_LOCO_3:
      driveLocoLED(2);  // Drive the third loco
      break;
    case INPUT_BUTTON_LOCO_4:
      driveLocoLED(3);  // Drive the fourth loco
      break;
    case INPUT_BUTTON_STOP_ALL:
      // Handle stop all action
      // WifiControl::getInstance()->dccProtocol()->stopAll();
      break;
  }
  displayControls->setRedraw();  // Request a redraw of the display
}

void LocoController::loop() {
  // Handle loop actions
  locoControl->loop();
}

void LocoController::loadLocosFromStorage() {
  storage->readLocoButton(locos);
}

void LocoController::setLoco(uint8_t index, DCCExController::Loco *loco) {
  clearLoco(loco);  // Clear previous loco

  if (index < locos.size()) {
    locos[index] = loco;
    current_loco_index = index;
  }
  storage->writeLocoButton(locos);  // Save loco button states
  printf("LocoController: setLoco: loco %d set at index %d\n", loco->getAddress(), index);
}

DCCExController::Loco *LocoController::getLoco(uint8_t index) {
  if (index < locos.size() && locos[index] != nullptr) {
    return locos[index];
  }
  return nullptr;
}

void LocoController::clearLoco(DCCExController::Loco *loco) {
  for (auto &l : locos) {
    if (l == nullptr) {
      continue;  // Skip null entries
    }
    if (l->getAddress() == loco->getAddress()) {
      printf("LocoController: clearLoco: clearing loco %d\n",
             loco->getAddress());
      l = nullptr;  // Clear the loco
      if (locoControl->getLoco() == loco) {
        locoControl->setLoco(nullptr);  // Clear the loco in LocoControl
      }
      // Optionally, reset current_loco_index if this was the current loco
      if (current_loco_index < locos.size() &&locos[current_loco_index] == loco) {
        current_loco_index = 0xff;  // Reset to an invalid index
      }
      storage->writeLocoButton(locos);  // Save the updated loco button states
      printf("LocoController: clearLoco: loco %d cleared\n", loco->getAddress());
      return;  // Exit after clearing the loco
    }
  }
}

void LocoController::driveLoco(DCCExController::Loco *loco) {
  if (loco == nullptr) {
    printf("LocoController: driveLoco: loco is nullptr\n");
    return;
  }

  printf("LocoController: driveLoco: driving loco %d\n", loco->getAddress());
  if(displayControls->getMenuName() == "ShowRoster") {
    // If we are on the roster menu, update the loco control
    locoControl->setLoco(loco);
  } else {
    // Otherwise, just set the loco in loco control
    locoControl->setLoco(loco);
  }
  displayControls->setRedraw();  // Request a redraw of the display
  locoControl->setLoco(loco);
  
}

void LocoController::driveLocoLED(uint8_t index) {
  auto loco = getLoco(index);
  if (loco == nullptr) {
    printf("LocoController: driveLoco: loco at index %d is nullptr\n",
           index);
    return;
  }
  printf("LocoController: driveLoco: driving loco %d\n", loco->getAddress());
  locoControl->setLoco(loco);
  current_loco_index = index;  // Update current loco index
  driveLocoMenu(loco);
}

void LocoController::driveLocoMenu(DCCExController::Loco *loco) {
  turn_off_loco_leds(); // Turn off all LEDs first
  uint8_t buttonIndex = LocoController::getInstance()->getLocoButtonIndex(loco);  
  if (buttonIndex != 0xff) {
    printf("Showing LED for button %d\n", buttonIndex);
    queue_led_command(get_loco_led_from_input(buttonIndex), true);
  } else {
    printf("No button assigned for this loco\n");
  }

  auto menu = std::make_shared<CabControlMenu>(displayControls);
  
  menu->setLoco(loco);
  driveLoco(loco);  // Set loco at index 0
  menu->showMenu();

}

uint8_t LocoController::getLocoButtonIndex(DCCExController::Loco *loco) {
  for (uint8_t i = 0; i < locos.size(); ++i) {
    if (locos[i] && locos[i] == loco) {
      return i;
    }
  }
  return 0xff;  // Not found
}

bool LocoController::doLongPressAction() { return true; }

bool LocoController::doMoveLeftAction() { return true; }

bool LocoController::doMoveRightAction() { return true; }

bool LocoController::doKeyAction(int8_t action) { return true; }

void LocoController::doPotAction(uint16_t value) {}
