#ifndef _SHOW_ROSTER_MENU_H
#define _SHOW_ROSTER_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class ShowRosterMenu : public MuiMenu {
  private:
  struct menuItem{
    uint8_t value;
    const char* label;
  };
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu;


 public:
  ShowRosterMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~ShowRosterMenu() {};
  
  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  void clearAction() override;
  bool doAction() override;

  static std::shared_ptr<MuiMenu> create(std::shared_ptr<DisplayControls> displayControls) {
    auto ptr = std::make_shared<ShowRosterMenu>(displayControls);
    ptr->sharedThis = ptr;
    return ptr;
  }

};

#endif