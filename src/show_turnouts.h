#ifndef _SHOW_TURNOUTS_MENU_H
#define _SHOW_TURNOUTS_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class ShowTurnoutsMenu : public MuiMenu {
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
  
  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  void clearAction() override;
  bool doAction() override;
  bool doLongPressAction() override;

  static std::shared_ptr<MuiMenu> create(std::shared_ptr<DisplayControls> displayControls) {
    auto ptr = std::make_shared<ShowTurnoutsMenu>(displayControls);
    ptr->sharedThis = ptr;
    return ptr;
  }

};

#endif