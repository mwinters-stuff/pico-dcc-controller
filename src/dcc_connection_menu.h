#ifndef _DCC_CONNECTION_MENU_H
#define _DCC_CONNECTION_MENU_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "test_menu.h"

class DisplayControls;

class DCCConnectionMenu : public MuiMenu {
 protected:
  size_t selectedIndex = -1;
  std::vector<std::string> addresses;
  std::shared_ptr<MuiMenu> testMenu;

 public:
  DCCConnectionMenu(std::shared_ptr<DisplayControls> displayControls);
  void parseDCCEXAddresses(const std::string &input);
  virtual ~DCCConnectionMenu() {};

  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  void clearAction() override;
  bool doAction() override;

  static std::shared_ptr<MuiMenu> create(
      std::shared_ptr<DisplayControls> displayControls) {
    auto ptr = std::make_shared<DCCConnectionMenu>(displayControls);
    ptr->sharedThis = ptr;
    return ptr;
  }
};

#endif