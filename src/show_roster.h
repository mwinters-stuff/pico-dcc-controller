#ifndef _SHOW_ROSTER_MENU_H
#define _SHOW_ROSTER_MENU_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "DCCEXLoco.h"

class DisplayControls;

#define NAME_MAX_LENGTH 30



class ShowRosterMenu : public MuiMenu, public std::enable_shared_from_this<ShowRosterMenu> {
 private:
  struct menuItem {
    uint8_t value;
    char label[NAME_MAX_LENGTH];
    DCCExController::Loco *loco;
  };

 protected:
  menuItem *selectedItem = nullptr;
  std::vector<menuItem> menu;
  std::shared_ptr<MuiMenu> subMenu;
  uint8_t highlightIndex = 0xff; // 0xff means no highlight

 public:
  ShowRosterMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~ShowRosterMenu() {};


      MenuList getMenu() override { return MenuList::SHOW_ROSTER; };
    MenuList backMenu() override { return MenuList::DCC_MENU; };

  void showMenu() override;
  std::string getLocoName(uint8_t locoIndex);
  void buildMenu(u8g2_t& u8g2) override;
  bool doAction(mui_event event) override;
  bool doLongPressAction() override;
  void clearAction() override;
  std::string getName() override  { return "ShowRoster"; };
  void doButtonAction(uint8_t action, uint8_t value) override;
  bool doKeyAction(int8_t action) override;

  void getLocoIndexNames();

  void showButtonLED(int menuIndex);
};

#endif