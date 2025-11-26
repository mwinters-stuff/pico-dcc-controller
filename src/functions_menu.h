#ifndef _FUNCTIONS_MENU_H
#define _FUNCTIONS_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include <DCCEXLoco.h>

#include <memory>
#include <string>


class DisplayControls;

class FunctionsMenu : public MuiMenu, public std::enable_shared_from_this<FunctionsMenu> {
  private:
  struct menuItem{
    uint8_t value;
    std::string label;
    bool momentary{false};
    bool active{false};
  };
  DCCExController::Loco* loco;
  char title[32];
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu;


 public:
  FunctionsMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~FunctionsMenu() noexcept {};

  void setLoco(DCCExController::Loco* loco);

  MenuList getMenu() override { return MenuList::FUNCTIONS_MENU; };
  MenuList backMenu() override { return MenuList::CAB_CONTROL; };

  
  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction(mui_event event) override;
  bool doLongPressAction() override;
  void clearAction() override;
  std::string getName() override  { return "FunctionsMenu"; };


};

#endif