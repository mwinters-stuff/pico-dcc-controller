
#include "loco_controller.h"
#include <stdio.h>
#include "defines.h"
#include "pico/stdlib.h"
#include <DCCEXProtocol.h>
#include "wifi_control.h"

std::shared_ptr<LocoController> LocoController::instance;

bool LocoController::doAction(){
  return true;   
}

void LocoController::doButtonAction(uint8_t action, uint8_t value) {
  switch(action){
    case INPUT_BUTTON_LOCO_1:
      current_loco_index = 0;
      break;
    case INPUT_BUTTON_LOCO_2:
      current_loco_index = 1;
      break;
    case INPUT_BUTTON_LOCO_3:
      current_loco_index = 2;
      break;
    case INPUT_BUTTON_LOCO_4:
      current_loco_index = 3;
      break;
    case INPUT_BUTTON_STOP_ALL:
      // Handle stop all action
      // WifiControl::getInstance()->dccProtocol()->stopAll();
      break;
  }
}

void LocoController::loop() {
  // Handle loop actions
  for(auto &loco : locos) {
    if (loco != nullptr) {
      loco->loop();
    }
  }
}
void LocoController::setLoco(uint8_t index, DCCExController::Loco *loco) {
  if (index < locos.size() && locos[index] != nullptr) {
    locos[index]->setLoco(loco);
    current_loco_index = index;
  }
}

DCCExController::Loco *LocoController::getLoco(uint8_t index) {
  if (index < locos.size() && locos[index] != nullptr) {
    return locos[index]->getLoco();
  }
  return nullptr;
}


uint8_t LocoController::getLocoButtonIndex(DCCExController::Loco *loco) {
  for (uint8_t i = 0; i < locos.size(); ++i) {
    if (locos[i] && locos[i]->getLoco() == loco) {
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

