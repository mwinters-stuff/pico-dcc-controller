#ifndef CAB_CONTROL_H
#define CAB_CONTROL_H

#include <memory>
#include "display_controls.h"
#include "mui_menu.h"
#include <DCCEXLoco.h>

class CabControl: public MuiMenu, public std::enable_shared_from_this<CabControl> {
  public:
    CabControl(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls) {};
    ~CabControl(){};

    MenuList getMenu() override { return MenuList::CAB_CONTROL; };
    MenuList backMenu() override { return MenuList::SHOW_ROSTER; };
    
    void buildMenu(u8g2_t &u8g2) override;
    void showMenu() override;
    bool doAction() override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;

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

  private:
    void change_speed(bool increase);
    void change_direction(bool forward);
    DCCExController::Loco *loco;
    muiItemId speed_idx;
    int speed{0};
    std::string speed_str;
    std::string direction_str;
};

#endif