#ifndef CAB_CONTROL_H
#define CAB_CONTROL_H

#include <memory>
#include "display_controls.h"
#include "mui_menu.h"
#include <DCCEXLoco.h>
#include "pico/util/queue.h"

class CabControlMenu: public MuiMenu, public std::enable_shared_from_this<CabControlMenu> {
  enum class SpeedActionFrom{
    NONE,
    ROTARY,
    KEYBOARD,
    POT,
  };

  enum class PotState { BELOW, MATCHED, ABOVE };

  public:
    CabControlMenu(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls) {
    };
    ~CabControlMenu(){
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

    void loop() override;

    std::string getName() override { return "CabControlMenu"; };
    
    void setLoco(DCCExController::Loco *loco) {
      this->loco = loco;
      if (loco != nullptr) {
        printf("CabControlMenu: setLoco: %d\n", loco->getAddress());
      } else {
        printf("CabControlMenu: setLoco: nullptr\n");
      }
      
    }
    
    DCCExController::Loco *getLoco() {
      return loco;
    }

  private:
    void change_direction(bool forward);
    DCCExController::Loco *loco;

    muiItemId speed_idx;

    std::string speed_str;
    std::string direction_str;
};

#endif