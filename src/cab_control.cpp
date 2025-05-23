#include "cab_control.h"

#include <stdio.h>

#include <muipp_u8g2.hpp>
#include <muipp_tpl.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "wifi_control.h"

void CabControl::showMenu() {
  displayControls->showScreen(shared_from_this(),
                              [this](u8g2_t &u8g2) { buildMenu(u8g2); });
}

void CabControl::buildMenu(u8g2_t &u8g2) {
  // create root page
  if (loco == nullptr) {
    return;
  }
  muiItemId root_page;
  if (loco->getName() != nullptr && strlen(loco->getName())) {
    root_page = makePage(loco->getName());
  } else {
    root_page = makePage(std::to_string(loco->getAddress()).c_str());
  }

  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  speed_idx = nextIndex();


 // Set font and calculate y position
  u8g2_SetFont(&u8g2, PAGE_TITLE_FONT_SMALL);
  int item_y_offset = u8g2_GetMaxCharHeight(&u8g2);

 
  addMuippItem(
    new MuiItem_U8g2_ValuesList(u8g2, speed_idx,
      "Speed",
      [&](){ speed_str = std::to_string(loco->getSpeed()); return speed_str.data(); },         // get a string of current QC mode
      [&](){ change_speed(true); },                                           // next available voltage
      [&](){ change_speed(false); },                                          // prev available voltage
      0, u8g2_GetDisplayWidth(&u8g2), item_y_offset,                                   // cursor point
      PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,text_align_t::top),                  // justify value to the right
    root_page
  );

  u8g2_SetFont(&u8g2, PAGE_TITLE_FONT);
  item_y_offset += u8g2_GetMaxCharHeight(&u8g2);
  
  addMuippItem(
    new MuiItem_U8g2_ValuesList(u8g2, nextIndex(),
      "Driving",
      [&](){ direction_str = loco->getDirection() == DCCExController::Forward ? "Forward" : "Reverse"; return direction_str.data(); },         // get a string of current QC mode
      [&](){ loco->setDirection(loco->getDirection() == DCCExController::Forward ? DCCExController::Reverse : DCCExController::Forward); },                                           // next available voltage
      [&](){ loco->setDirection(loco->getDirection() == DCCExController::Forward ? DCCExController::Reverse : DCCExController::Forward); },                                          // prev available voltage
      0, u8g2_GetDisplayWidth(&u8g2), item_y_offset,                                   // cursor point
      PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,text_align_t::top),                  // justify value to the right
    root_page
  );


  pageAutoSelect(root_page, speed_idx);

  addMuippItem(
      new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back", MAIN_MENU_FONT1),
      root_page);

  // start our menu from root page
  menuStart(root_page);
}

void CabControl::change_speed(bool increase) {
  if (increase) {
    speed = loco->getSpeed() + 1;
    if (speed > MAX_LOCO_SPEED) {
      speed = MAX_LOCO_SPEED;
      printf("Max speed reached\n");
    }
  } else {
    speed = loco->getSpeed() - 1;
    if (speed < 0) {
      speed = 0;
      printf("Min speed reached\n");
    }
  }
  printf("Set speed: %d\n", speed);
  loco->setSpeed(speed);
  WifiControl::getInstance()->dccProtocol()->setThrottle(loco, speed, loco->getDirection());
}


void CabControl::change_direction(bool forward) {
  if(loco->getSpeed() != 0){
    return;
  }
  loco->setDirection(forward ? DCCExController::Forward : DCCExController::Reverse);
  WifiControl::getInstance()->dccProtocol()->setThrottle(loco, loco->getSpeed(), loco->getDirection());
}


bool CabControl::doAction() {
  // if(muiEvent(mui_event(mui_event_t::enter)).eid == mui_event_t::quitMenu){
  //   displayControls->endScreen();
  //   return false;
  // }

  return true;
}

bool CabControl::doLongPressAction() {
  displayControls->endScreen();
  return false;
}

bool CabControl::doMoveLeftAction() { 
  return true;
}

bool CabControl::doMoveRightAction() { return true; }

bool CabControl::doKeyAction(int8_t action) {
  if (action == 'B') {
    displayControls->endScreen();
    return false;
  } else if (action >= '0' && action <= '9') {
    // Map '0'..'9' to speed 0..126 equally
    speed = ((action - '0') * MAX_LOCO_SPEED + 4) / 9; // +4 for rounding
    loco->setSpeed(speed);
    WifiControl::getInstance()->dccProtocol()->setThrottle(loco, speed, loco->getDirection());
    printf("Set speed: %d\n", speed);
  }
  if(action == 'A'){
    if(loco->getSpeed() == 0){
      loco->setDirection(loco->getDirection() == DCCExController::Forward ? DCCExController::Reverse : DCCExController::Forward);
      printf("Set direction: %s\n", loco->getDirection() == DCCExController::Forward ? "Forward" : "Reverse");
    }
  }
  return true;
}