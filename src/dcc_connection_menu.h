#ifndef _DCC_CONNECTION_MENU_H
#define _DCC_CONNECTION_MENU_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "dcc_menu.h"
#include "dcc_connection_entry.h"

class DisplayControls;

class DCCConnectionMenu : public MuiMenu, public std::enable_shared_from_this<DCCConnectionMenu> {
 protected:

  size_t selectedIndex = -1;
  std::shared_ptr<MuiMenu> dccMenu;
  std::string str = "";

 public:
  DCCConnectionMenu(std::shared_ptr<DisplayControls> displayControls);
  virtual ~DCCConnectionMenu() {};

  MenuList getMenu() override { return MenuList::DCC_CONNECTION; };
  MenuList backMenu() override { return MenuList::MAIN_MENU; };

  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction(mui_event event) override;
  void clearAction() override;
  std::string getName() override  { return "DCCConnection"; };

};

#endif