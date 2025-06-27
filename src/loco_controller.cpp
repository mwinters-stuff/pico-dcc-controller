
#include "loco_controller.h"
#include <stdio.h>
#include "defines.h"
#include "pico/stdlib.h"
#include <DCCEXProtocol.h>
#include "wifi_control.h"
#include "display_controls.h"

std::shared_ptr<LocoController> LocoController::instance;


LocoController::LocoController() :
                       current_loco_index(0xff), 
                       locoControl(LocoControl::initInstance()) {

};

std::shared_ptr<LocoController> LocoController::initInstance(std::shared_ptr<DisplayControls> displayControls){
  instance = std::shared_ptr<LocoController>(new LocoController());
  instance->init(displayControls);
  return instance;
}


void LocoController::init(std::shared_ptr<DisplayControls> displayControls) {
  locoControl->init(displayControls);
  displayControls->addActionInterface(shared_from_this());
}

bool LocoController::doAction(){
  return true;   
}

void LocoController::doButtonAction(uint8_t action, uint8_t value) {
  switch(action){
    case INPUT_BUTTON_LOCO_1:
      driveLoco(0);
      break;
    case INPUT_BUTTON_LOCO_2:
      driveLoco(1);
      break;
    case INPUT_BUTTON_LOCO_3:
      driveLoco(2);
      break;
    case INPUT_BUTTON_LOCO_4:
      driveLoco(3);
      break;
    case INPUT_BUTTON_STOP_ALL:
      // Handle stop all action
      // WifiControl::getInstance()->dccProtocol()->stopAll();
      break;
  }
}

void LocoController::loop() {
  // Handle loop actions
  locoControl->loop();
}

void LocoController::setLoco(uint8_t index, DCCExController::Loco *loco) {
  clearLoco(loco); // Clear previous loco

  if (index < locos.size() && locos[index] != nullptr) {
    locos[index] = loco;
    current_loco_index = index;
  }
}

DCCExController::Loco *LocoController::getLoco(uint8_t index) {
  if (index < locos.size() && locos[index] != nullptr) {
    return locos[index];
  }
  return nullptr;
}

void LocoController::clearLoco(DCCExController::Loco *loco) {
  for(auto &l : locos) {
    if (l == nullptr) {
      continue;  // Skip null entries
    }
    if (l->getAddress() == loco->getAddress()) {
      printf("LocoController: clearLoco: clearing loco %d\n", loco->getAddress());
      l = nullptr;  // Clear the loco
      if(locoControl->getLoco() == loco) {
        locoControl->setLoco(nullptr);  // Clear the loco in LocoControl
      }
      // Optionally, reset current_loco_index if this was the current loco
      if (current_loco_index < locos.size() && locos[current_loco_index] == loco) {
        current_loco_index = 0xff;  // Reset to an invalid index
      }
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
  locoControl->setLoco(loco);
}

void LocoController::driveLoco(uint8_t new_index) {
  auto loco = getLoco(new_index);  
  if(loco == nullptr) {
    printf("LocoController: driveLoco: loco at index %d is nullptr\n", new_index);
    return;
  }
  printf("LocoController: driveLoco: driving loco %d\n", loco->getAddress()); 
  locoControl->setLoco(loco);
  current_loco_index = new_index;  // Update current loco index
}



uint8_t LocoController::getLocoButtonIndex(DCCExController::Loco *loco) {
  for (uint8_t i = 0; i < locos.size(); ++i) {
    if (locos[i] && locos[i] == loco) {
      return i;  
    }
  }
  return 0xff;  // Not found
}

bool LocoController::doLongPressAction() {
  return true;  
}

bool LocoController::doMoveLeftAction() {
  return true;  
}

bool LocoController::doMoveRightAction() {
  return true;  
}

bool LocoController::doKeyAction(int8_t action) {
  return true;  
}

void LocoController::doPotAction(uint16_t value) {
  
}

