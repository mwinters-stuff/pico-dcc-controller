#ifndef CAB_CONTROL_H
#define CAB_CONTROL_H

#include <memory>
#include "display_controls.h"
#include "mui_menu.h"
#include <DCCEXLoco.h>
#include "pico/util/queue.h"

class CabControl: public MuiMenu, public std::enable_shared_from_this<CabControl> {
  enum class SpeedActionFrom{
    NONE,
    ROTARY,
    KEYBOARD,
    POT,
  };

  enum class PotState { BELOW, MATCHED, ABOVE };

  public:
    CabControl(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls) {
      queue_init(&speed_update_queue, sizeof(uint8_t), 1);
    };
    ~CabControl(){
      queue_free(&speed_update_queue);
    };

    MenuList getMenu() override { return MenuList::CAB_CONTROL; };
    MenuList backMenu() override { return MenuList::SHOW_ROSTER; };
    
    void buildMenu(u8g2_t &u8g2) override;
    void showMenu() override;
    bool doAction() override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;
    void doPotAction(uint16_t value) override;
    void doButtonAction(uint8_t action, uint8_t value)override;
    void loop() override;

    std::string getName() override { return "CabControl"; };
    
    void setLoco(DCCExController::Loco *loco) {
      this->loco = loco;
      if (loco != nullptr) {
        printf("CabControl: setLoco: %d\n", loco->getAddress());
      } else {
        printf("CabControl: setLoco: nullptr\n");
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
    DCCExController::Loco *loco;
    muiItemId speed_idx;
    uint8_t speed{0};
    std::string speed_str;
    std::string direction_str;
    SpeedActionFrom speedActionFrom{SpeedActionFrom::NONE};
    PotState potState = PotState::MATCHED;
    bool potReady = false;


    void setSpeedWithDelay(uint8_t set_speed);
};

#endif