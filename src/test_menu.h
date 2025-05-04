#ifndef _TEST_MENU_H
#define _TEST_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class TestMenu : public MuiMenu {
  private:
  enum MENU_ITEM_VALUE{
    miv_None = -1,
    miv_RefreshLoco = 0,
    miv_TrackOn = 1,
    miv_TrackOff = 2,
  };
  struct menuItem{
    MENU_ITEM_VALUE value;
    const char* label;
  };
 protected:
 menuItem selectedItem;
  std::vector<menuItem> menu = {
    {miv_RefreshLoco, "Refresh Loco"},
    {miv_TrackOn, "Power On"},
    {miv_TrackOff, "Power Off"}
  };


 public:
  TestMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~TestMenu() {};
  
  void showMenu();
  void buildMenu(u8g2_t &u8g2);
  void clearAction();
  void doAction();

};

#endif