#ifndef _MAIN_MENU_H
#define _MAIN_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include "dcc_connection_menu.h"

#include <memory>
#include <string>


class DisplayControls;

class MainMenu : public MuiMenu, public std::enable_shared_from_this<MainMenu> {
  private:
  enum MENU_ITEM_VALUE{
    miv_None = -1,
    miv_ConnectToDCC = 0,
  };
  struct menuItem{
    MENU_ITEM_VALUE value;
    const char* label;
  };
 protected:
 
 menuItem selectedItem;
  std::vector<menuItem> menu = {
    {miv_ConnectToDCC, "Connect to DCC"}
  };
  std::shared_ptr<MuiMenu> connectionMenu;

 public:
  MainMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~MainMenu() {};
  
  MenuList getMenu() override { return MenuList::MAIN_MENU; };
  MenuList backMenu() override { return MenuList::NONE; };


  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction() override;
  void clearAction() override;



};

#endif