#include "storage_interface.h"
#include "loco_controller.h"
#define STORAGE_LOCO_BUTTON_ADDRESS 0x00
#define STORAGE_LOCO_BUTTON_LENGTH 4



bool StorageInterface::readLocoButton(std::vector<DCCExController::Loco* > locos){
  uint8_t data[STORAGE_LOCO_BUTTON_LENGTH];
  if (!read(STORAGE_LOCO_BUTTON_ADDRESS, data, STORAGE_LOCO_BUTTON_LENGTH)) {
    return false; // Read failed
  }
  

  for (size_t i = 0; i < locos.size() && i < STORAGE_LOCO_BUTTON_LENGTH; ++i) {
    if (data[i] != 0xff) { // Check if button is assigned
      auto loco = DCCExController::Loco::getByAddress(data[i]);
      if (loco != nullptr) {
        LocoController::getInstance()->setLoco(i, loco);
      }
    }
  }
  return true;
}

bool StorageInterface::writeLocoButton(std::vector<DCCExController::Loco* > locos){
  uint8_t data[STORAGE_LOCO_BUTTON_LENGTH];
  for (size_t i = 0; i < STORAGE_LOCO_BUTTON_LENGTH; ++i) {
    if (i < locos.size() && locos[i] != nullptr) {
      data[i] = locos[i]->getAddress(); // Store loco address
    } else {
      data[i] = 0xff; // No loco assigned to this button
    }
  }
  return write(STORAGE_LOCO_BUTTON_ADDRESS, data, STORAGE_LOCO_BUTTON_LENGTH);
}


