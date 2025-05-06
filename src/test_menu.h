#ifndef _TEST_MENU_H
#define _TEST_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include "show_roster.h"

#include <memory>
#include <string>


class DisplayControls;

class TestMenu : public MuiMenu {
  private:
  enum MENU_ITEM_VALUE{
    miv_None = -1,
    miv_RefreshRoster = 0,
    miv_TrackOn = 1,
    miv_TrackOff = 2,
    miv_ShowRoster = 3,
    miv_ShowTurnouts = 4,
  };
  struct menuItem{
    MENU_ITEM_VALUE value;
    const char* label;
  };
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu = {
    {miv_RefreshRoster, "Refresh Roster"},
    {miv_TrackOn, "Power On"},
    {miv_TrackOff, "Power Off"},
    {miv_ShowRoster, "Show Roster"},
    {miv_ShowTurnouts, "Show Turnouts"},
  };

  std::shared_ptr<MuiMenu> subMenu;

 public:
  TestMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~TestMenu() {};
  
  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  void clearAction() override;
  bool doAction() override;

  static std::shared_ptr<MuiMenu> create(std::shared_ptr<DisplayControls> displayControls) {
    auto ptr = std::make_shared<TestMenu>(displayControls);
    ptr->sharedThis = ptr;
    return ptr;
  }

};

#endif