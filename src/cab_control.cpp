#include "cab_control.h"

#include <stdio.h>

#include <muipp_tpl.hpp>
#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "pico/time.h"
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
      new MuiItem_U8g2_ValuesList(
          u8g2, speed_idx, "Speed",
          [&]() {
            speed_str = std::to_string(speed);
            return speed_str.data();
          },                               // get a string of current QC mode
          [&]() { change_speed(true); },   // next available voltage
          [&]() { change_speed(false); },  // prev available voltage
          0, u8g2_GetDisplayWidth(&u8g2), item_y_offset,  // cursor point
          PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,
          text_align_t::top),  // justify value to the right
      root_page);

  u8g2_SetFont(&u8g2, PAGE_TITLE_FONT);
  item_y_offset += u8g2_GetMaxCharHeight(&u8g2);

  addMuippItem(
      new MuiItem_U8g2_ValuesList(
          u8g2, nextIndex(), "Driving",
          [&]() {
            direction_str = loco->getDirection() == DCCExController::Forward
                                ? "Forward"
                                : "Reverse";
            return direction_str.data();
          },  // get a string of current QC mode
          [&]() {
            loco->setDirection(loco->getDirection() == DCCExController::Forward
                                   ? DCCExController::Reverse
                                   : DCCExController::Forward);
          },  // next available voltage
          [&]() {
            loco->setDirection(loco->getDirection() == DCCExController::Forward
                                   ? DCCExController::Reverse
                                   : DCCExController::Forward);
          },  // prev available voltage
          0, u8g2_GetDisplayWidth(&u8g2), item_y_offset,  // cursor point
          PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,
          text_align_t::top),  // justify value to the right
      root_page);

  pageAutoSelect(root_page, speed_idx);

  addMuippItem(
      new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back", MAIN_MENU_FONT1),
      root_page);

  // start our menu from root page
  menuStart(root_page);
}

static int64_t set_speed_callback(alarm_id_t id, void *user_data) {
  CabControl *cab = static_cast<CabControl *>(user_data);
  if (cab) {
    queue_try_add(&cab->speed_update_queue, &cab->pending_speed);
    printf("Queued speed update (timer): %d\n", cab->pending_speed);
    cab->speed_alarm_id = 0;
    cab->pending_speed = -1;
  }
  return 0;  // Do not repeat
}

void CabControl::setSpeedWithDelay(uint8_t set_speed) {
  // Cancel any existing alarm
  if (speed_alarm_id) {
    cancel_alarm(speed_alarm_id);
    speed_alarm_id = 0;
  }
  pending_speed = set_speed;
  // Set a new alarm for 100ms
  speed_alarm_id = add_alarm_in_ms(200, set_speed_callback, this, false);
  displayControls->setRedraw();
}

void CabControl::change_speed(bool increase) {
  int8_t s_speed = speed;
  if (increase) {
    s_speed++;
    if (s_speed > MAX_LOCO_SPEED) {
      s_speed = MAX_LOCO_SPEED;
      printf("Max speed reached\n");
    }
  } else {
    s_speed--;
    if (s_speed < 0) {
      s_speed = 0;
      printf("Min speed reached\n");
    }
  }
  speed = s_speed;
  printf("Set speed: %d\n", speed);
  setSpeedWithDelay(speed);
  speedActionFrom = SpeedActionFrom::ROTARY;
}

void CabControl::change_direction(bool forward) {
  if (loco->getSpeed() > 0) {
    return;
  }
  loco->setDirection(forward ? DCCExController::Forward
                             : DCCExController::Reverse);
  WifiControl::getInstance()->dccProtocol()->setThrottle(loco, loco->getSpeed(),
                                                         loco->getDirection());
}

bool CabControl::doAction() {
  // if(muiEvent(mui_event(mui_event_t::enter)).eid == mui_event_t::quitMenu){
  //   displayControls->exitMenu();
  //   return false;
  // }

  return true;
}

bool CabControl::doLongPressAction() {
  displayControls->exitMenu();
  return false;
}

bool CabControl::doMoveLeftAction() { return true; }

bool CabControl::doMoveRightAction() { return true; }

bool CabControl::doKeyAction(int8_t action) {
  if (action == 'B') {
    displayControls->exitMenu();
    return false;
  } else if (action >= '0' && action <= '9') {
    // Map '0'..'9' to speed 0..126 equally
    speed = ((action - '0') * MAX_LOCO_SPEED + 4) / 9;  // +4 for rounding
    setSpeedWithDelay(speed);
    printf("Set speed: %d\n", speed);
    speedActionFrom = SpeedActionFrom::KEYBOARD;
  }
  if (action == 'A') {
    if (loco->getSpeed() == 0) {
      loco->setDirection(loco->getDirection() == DCCExController::Forward
                             ? DCCExController::Reverse
                             : DCCExController::Forward);
      printf("Set direction: %s\n",
             loco->getDirection() == DCCExController::Forward ? "Forward"
                                                              : "Reverse");
    }
  }
  return true;
}

void CabControl::doPotAction(uint16_t value) {
  // Ignore pot input until it reaches 0 after startup
  if (!potReady) {
    if (value == 0) {
      potReady = true;
      printf("Pot ready, accepting input\n");
    } else {
      printf("Ignoring pot input until zero (current: %d)\n", value);
      return;
    }
  }

  if (speedActionFrom == SpeedActionFrom::POT) {
    speed = value;
    setSpeedWithDelay(speed);
    printf("Set Pot speed: to %d\n", speed);
    return;
  }

  if (value < speed) {
    if (potState != PotState::BELOW) {
      speed = value;
      setSpeedWithDelay(speed);
      printf("Pot takeover: speed down to %d\n", speed);
      speedActionFrom = SpeedActionFrom::POT;
    }
    potState = PotState::BELOW;
    return;
  } else if (value > speed) {
    if (potState != PotState::ABOVE) {
      speed = value;
      setSpeedWithDelay(speed);
      printf("Pot takeover: speed up to %d\n", speed);
      speedActionFrom = SpeedActionFrom::POT;
    }
    potState = PotState::ABOVE;
    return;
  } else {
    potState = PotState::MATCHED;
    printf("Potentiometer speed unchanged: %d (%d)\n", value, speed);
  }
}

void CabControl::doButtonAction(uint8_t action, uint8_t value) {
  if (action == INPUT_BUTTON_REVERSE) {
    if (value == 1) {
      change_direction(loco->getDirection() == DCCExController::Reverse);
    }
  }
}

void CabControl::loop() {
  uint8_t speed_update;
  while (queue_try_remove(&speed_update_queue, &speed_update)) {
    loco->setSpeed(speed_update);
    WifiControl::getInstance()->dccProtocol()->setThrottle(
        loco, loco->getSpeed(), loco->getDirection());
    printf("Sent speed update via WiFi: %d\n", loco->getSpeed());
  }
  displayControls->setRedraw();
}
