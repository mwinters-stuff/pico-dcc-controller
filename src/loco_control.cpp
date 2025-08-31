#include "loco_control.h"

#include <stdio.h>

#include "defines.h"
#include "main_core1.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "wifi_control.h"
#include "display_controls.h"

std::shared_ptr<LocoControl> LocoControl::instance;

    LocoControl::LocoControl(): loco(nullptr), speed(0), speedActionFrom(SpeedActionFrom::NONE), potState(PotState::MATCHED), potReady(false) {
      queue_init(&speed_update_queue, sizeof(uint8_t), 1);
    };

void LocoControl::init(std::shared_ptr<DisplayControls> displayControls) {
    displayControls->addActionInterface(shared_from_this());
    this->displayControls = displayControls;
}

void LocoControl::setLoco(DCCExController::Loco *loco) {
  this->loco = loco;
  if (loco != nullptr) {
    printf("LocoControl: setLoco: %d\n", loco->getAddress());
  } else {
    printf("LocoControl: setLoco: nullptr\n");
  }
  setSpeedLED((uint8_t)loco->getSpeed());
}

static int64_t set_speed_callback(alarm_id_t id, void *user_data) {
  LocoControl *cab = static_cast<LocoControl *>(user_data);
  if (cab) {
    queue_try_add(&cab->speed_update_queue, &cab->pending_speed);
    printf("Queued speed update (timer): %d\n", cab->pending_speed);
    cab->speed_alarm_id = 0;
    cab->pending_speed = -1;
  }
  return 0;  // Do not repeat
}

void LocoControl::setSpeedWithDelay(uint8_t set_speed) {
  // Cancel any existing alarm
  if (speed_alarm_id) {
    cancel_alarm(speed_alarm_id);
    speed_alarm_id = 0;
  }
  // Clamp set_speed to MAX_LOCO_SPEED
  if (set_speed > MAX_LOCO_SPEED) {
    set_speed = MAX_LOCO_SPEED;
  }
  pending_speed = set_speed;
  setSpeedLED(set_speed);
  // Set a new alarm for 100ms
  speed_alarm_id = add_alarm_in_ms(200, set_speed_callback, this, false);
}

void LocoControl::setSpeedLED(uint8_t set_speed) {
  // There are 8 LEDs, so divide the speed range into 8 steps
  const uint8_t led_pins[8] = {
      PCF8575_PIN_LOCO_SHOW_SPEED_1, PCF8575_PIN_LOCO_SHOW_SPEED_2,
      PCF8575_PIN_LOCO_SHOW_SPEED_3, PCF8575_PIN_LOCO_SHOW_SPEED_4,
      PCF8575_PIN_LOCO_SHOW_SPEED_5, PCF8575_PIN_LOCO_SHOW_SPEED_6,
      PCF8575_PIN_LOCO_SHOW_SPEED_7, PCF8575_PIN_LOCO_SHOW_SPEED_8};

  // Calculate which LED to turn on (0-7)
  uint8_t step = (set_speed * 8) / (MAX_LOCO_SPEED + 1);

  for (uint8_t i = 0; i < 8; ++i) {
    queue_led_command(led_pins[i], i == step && set_speed > 0);
  }
}

void LocoControl::change_speed(bool increase) {
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

void LocoControl::change_direction(bool forward) {
  if (loco->getSpeed() > 0) {
    return;
  }
  loco->setDirection(forward ? DCCExController::Forward
                             : DCCExController::Reverse);
  WifiControl::getInstance()->dccProtocol()->setThrottle(loco, loco->getSpeed(),
                                                         loco->getDirection());
}

bool LocoControl::doMoveLeftAction() { return false; }

bool LocoControl::doMoveRightAction() { return false; }

bool LocoControl::doKeyAction(int8_t action) {
  if(displayControls->isOnRosterMenu()) {
    return false; // Ignore key actions in roster menu
  }

  if (action >= '0' && action <= '9') {
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

void LocoControl::doPotAction(uint16_t value) {
    if(displayControls->isOnRosterMenu()) {
    return; // Ignore key actions in roster menu
  }

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

void LocoControl::doButtonAction(uint8_t action, uint8_t value) {
    if(displayControls->isOnRosterMenu()) {
    return; // Ignore key actions in roster menu
  }

  if (action == INPUT_BUTTON_REVERSE) {
    if (value == 1) {
      change_direction(loco->getDirection() == DCCExController::Reverse);
    }
  }
}

void LocoControl::loop() {
  uint8_t speed_update;
  while (queue_try_remove(&speed_update_queue, &speed_update)) {
    if(WifiControl::getInstance()->dccProtocol() == nullptr) {
      printf("DCCEXProtocol not initialized, skipping speed update\n");
    
    }else{
      loco->setSpeed(speed_update);
      WifiControl::getInstance()->dccProtocol()->setThrottle(
          loco, loco->getSpeed(), loco->getDirection());
      printf("Sent speed update via WiFi: %d\n", loco->getSpeed());
    }
  }
}

bool LocoControl::doAction() { return false; }
bool LocoControl::doLongPressAction() { return false; }
