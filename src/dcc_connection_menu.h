#ifndef _DCC_CONNECTION_MENU_H
#define _DCC_CONNECTION_MENU_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include <memory>
#include <string>


class DisplayControls;

class DCCConnectionMenu : public MuiMenu {
 protected:
  size_t selectedIndex = -1;
  std::vector<std::string> addresses;
 public:
 DCCConnectionMenu(std::shared_ptr<DisplayControls> displayControls);
 void parseDCCEXAddresses(const std::string &input);
 virtual ~DCCConnectionMenu() {};

 void showMenu();
 void buildMenu(u8g2_t &u8g2);
 void clearAction();
 void doAction();

 
};

#endif