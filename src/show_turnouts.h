#ifndef _SHOW_TURNOUTS_MENU_H
#define _SHOW_TURNOUTS_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class ShowTurnoutsMenu : public MuiMenu, public std::enable_shared_from_this<ShowTurnoutsMenu> {
  private:
  struct menuItem{
    uint8_t value;
    const char* label;
  };
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu;


 public:
  ShowTurnoutsMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~ShowTurnoutsMenu() {};

      MenuList getMenu() override { return MenuList::SHOW_TURNOUTS; };
    MenuList backMenu() override { return MenuList::DCC_MENU; };

  
  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction() override;
  bool doLongPressAction() override;
  void clearAction() override;
  std::string getName() override  { return "ShowTurnouts"; };


};

#endif