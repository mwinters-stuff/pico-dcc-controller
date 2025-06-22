#ifndef LOCO_CONTROL_H
#define LOCO_CONTROL_H

#include <memory>
#include "action_interface.h"
#include <DCCEXLoco.h>
#include "pico/util/queue.h"

class LocoControl: public ActionInterface, public std::enable_shared_from_this<LocoControl> {
  enum class SpeedActionFrom{
    NONE,
    ROTARY,
    KEYBOARD,
    POT,
  };

  enum class PotState { BELOW, MATCHED, ABOVE };

  public:
    LocoControl(uint8_t button_index):
    button_index(button_index), loco(nullptr), speed(0), speedActionFrom(SpeedActionFrom::NONE), potState(PotState::MATCHED), potReady(false) {
      queue_init(&speed_update_queue, sizeof(uint8_t), 1);
    };

    virtual ~LocoControl(){
    };

    // static std::shared_ptr<LocoControl> initInstance(){
    //   instance = std::shared_ptr<LocoControl>(new LocoControl());
    //   return instance;
    // }

    // static std::shared_ptr<LocoControl> getInstance(){
    //   return LocoControl::instance;
    // }

    bool doAction() override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;
    void doPotAction(uint16_t value) override;
    void doButtonAction(uint8_t action, uint8_t value)override;
    void loop() override;
    
    void setLoco(DCCExController::Loco *loco) {
      this->loco = loco;
      if (loco != nullptr) {
        printf("LocoControl: setLoco: %d\n", loco->getAddress());
      } else {
        printf("LocoControl: setLoco: nullptr\n");
      }
      
    }
    
    DCCExController::Loco *getLoco() {
      return loco;
    }

    alarm_id_t speed_alarm_id = 0;
    uint8_t pending_speed = 0;
    queue_t speed_update_queue;

  private:
    

    void change_speed(bool increase);
    void change_direction(bool forward);
    void setSpeedLED(uint8_t set_speed);

    // static std::shared_ptr<LocoControl> instance;
    int button_index{0};
    bool isActive{false};
    DCCExController::Loco *loco;

    uint8_t speed{0};
    std::string speed_str;
    std::string direction_str;
    SpeedActionFrom speedActionFrom{SpeedActionFrom::NONE};
    PotState potState = PotState::MATCHED;
    bool potReady = false;


    void setSpeedWithDelay(uint8_t set_speed);
};

#endif