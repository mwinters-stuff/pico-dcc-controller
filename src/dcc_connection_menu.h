#ifndef _DCC_CONNECTION_MENU_H
#define _DCC_CONNECTION_MENU_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "test_menu.h"

class DisplayControls;

class DCCConnectionMenu : public MuiMenu, public std::enable_shared_from_this<DCCConnectionMenu> {
 protected:
  struct menuItem {
    std::string name;
    std::string address;
    std::string port;
  };

  size_t selectedIndex = -1;
  std::vector<menuItem> addresses;
  std::shared_ptr<MuiMenu> testMenu;
  std::string str = "";
  uint8_t mdns_request_id;
  std::unique_ptr<menuItem> mdns_search_result;

  void discoverMDNSWithrottle();
  void checkAddHost();

 public:
  DCCConnectionMenu(std::shared_ptr<DisplayControls> displayControls);
  void parseDCCEXAddresses(const std::string &input);
  virtual ~DCCConnectionMenu() {};

  MenuList getMenu() override { return MenuList::DCC_CONNECTION; };
  MenuList backMenu() override { return MenuList::MAIN_MENU; };

  void showMenu() override;
  void buildMenu(u8g2_t &u8g2) override;
  bool doAction() override;
  void clearAction() override;
  std::string getName() override  { return "DCCConnection"; };


  std::string dns_label_to_string(const char *label, int maxlen);
  void print_dns_txt(const char *txt, int len);

};

#endif