#ifndef _TEST_MENU_H
#define _TEST_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class DCCMenu : public MuiMenu, public std::enable_shared_from_this<DCCMenu> {
  private:
  enum MENU_ITEM_VALUE{
    miv_None = -1,
    miv_RefreshRoster = 0,
    miv_ShowRoster = 1,
    miv_ShowTurnouts = 2,
    miv_TrackOn = 3,
    miv_TrackOff = 4,
    miv_Back = 5,
  };
  struct menuItem{
    MENU_ITEM_VALUE value;
    const char* label;
  };
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu = {
    {miv_RefreshRoster, "Refresh Roster"},
    {miv_ShowRoster, "Show Roster"},
    {miv_ShowTurnouts, "Show Turnouts"},
    {miv_TrackOn, "Power On"},
    {miv_TrackOff, "Power Off"},
    {miv_Back, "Back"},
  };

  std::shared_ptr<MuiMenu> subMenu;
  std::string pageLabel;

 public:
  DCCMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~DCCMenu() {};
  
      MenuList getMenu() override { return MenuList::DCC_MENU; };
    MenuList backMenu() override { return MenuList::DCC_CONNECTION; };


  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction() override;
  std::string getName() override  { return "TestMenu"; };


};

#endif