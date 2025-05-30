#ifndef _SHOW_ROSTER_MENU_H
#define _SHOW_ROSTER_MENU_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "DCCEXLoco.h"

class DisplayControls;

class ShowRosterMenu : public MuiMenu, public std::enable_shared_from_this<ShowRosterMenu> {
 private:
  struct menuItem {
    uint8_t value;
    const char* label;
    DCCExController::Loco *loco;
  };

 protected:
  menuItem selectedItem;
  std::vector<menuItem> menu;
  std::shared_ptr<MuiMenu> subMenu;

 public:
  ShowRosterMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~ShowRosterMenu() {};


      MenuList getMenu() override { return MenuList::SHOW_ROSTER; };
    MenuList backMenu() override { return MenuList::DCC_MENU; };

  void showMenu() override;
  void buildMenu(u8g2_t& u8g2) override;
  bool doAction() override;
  bool doLongPressAction() override;
  void clearAction() override;
  std::string getName() override  { return "ShowRoster"; };


};

#endif