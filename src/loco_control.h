#ifndef LOCO_CONTROL_H
#define LOCO_CONTROL_H

#include <memory>
#include "action_interface.h"
#include <DCCEXLoco.h>
#include "pico/util/queue.h"

class DisplayControls;

class LocoControl: public ActionInterface, public std::enable_shared_from_this<LocoControl> {
  enum class SpeedActionFrom{
    NONE,
    ROTARY,
    KEYBOARD,
    POT,
  };

  enum class PotState { BELOW, MATCHED, ABOVE };
  
    // Private constructor to prevent instantiation from outside
    // This ensures that the class can only be instantiated through the initInstance method
    // and provides a single instance of LocoControl throughout the application.
    // This is a common pattern for implementing the Singleton design pattern in C++.
    explicit LocoControl(const LocoControl&) = delete;
    LocoControl& operator=(const LocoControl&) = delete;
    LocoControl();

  public:

    virtual ~LocoControl(){
    };

    static std::shared_ptr<LocoControl> initInstance(){
      instance = std::shared_ptr<LocoControl>(new LocoControl());
      return instance;
    }

    static std::shared_ptr<LocoControl> getInstance(){
      return LocoControl::instance;
    }

    void init(std::shared_ptr<DisplayControls> displayControls);

    bool doAction(mui_event event) override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;
    void doPotAction(uint16_t value) override;
    void doButtonAction(uint8_t action, uint8_t value)override;
    void loop() override;
    
    void setLoco(DCCExController::Loco *loco) ;
    void setSpeedLED(uint8_t set_speed);
    
    DCCExController::Loco *getLoco() {
      return loco;
    }

    alarm_id_t speed_alarm_id = 0;
    uint8_t pending_speed = 0;
    queue_t speed_update_queue;

  private:
    

    void change_speed(bool increase);
    void change_direction(bool forward);

    static std::shared_ptr<LocoControl> instance;
    // int button_index{0};
    bool isActive{false};
    DCCExController::Loco *loco;

    uint8_t speed{0};
    std::string speed_str;
    std::string direction_str;
    SpeedActionFrom speedActionFrom{SpeedActionFrom::NONE};
    PotState potState = PotState::MATCHED;
    bool potReady = false;

    std::shared_ptr<DisplayControls> displayControls;


    void setSpeedWithDelay(uint8_t set_speed);
};

#endif