#include "dcc_connection_menu.h"

#include <lwip/ip_addr.h>
#include <lwip/tcp.h>
#include <stdio.h>

#include <muipp_u8g2.hpp>
#include <sstream>

#include "../config.h"
#include "defines.h"
#include "display_controls.h"
// #include "lwip/apps/mdns.h"
// #include "lwip/err.h"
// #include "lwip/ip_addr.h"
// #include "lwip/netif.h"
// #include "lwip/prot/dns.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "dcc_menu.h"
#include "wifi_control.h"
#include "mdns_scan.h"

DCCConnectionMenu::DCCConnectionMenu(
    std::shared_ptr<DisplayControls> displayControls)
    : MuiMenu(displayControls) {
}

void DCCConnectionMenu::showMenu() {
  displayControls->showScreen(shared_from_this(),
                              [this](u8g2_t &u8g2) { buildMenu(u8g2); });
}

void DCCConnectionMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
  auto mdnsScan = MDNSScan::getInstance();
  selectedIndex = -1;
  muiItemId root_page =
      makePage("DCC Connect");  // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();

  auto list = new MuiItem_U8g2_DynamicScrollList(
      u8g2,            // U8g2 object to draw to
      scroll_list_id,  // ID for the item
      [this, mdnsScan](size_t index) {
        auto addresses = mdnsScan->getAddresses();
        auto item = addresses.at(index);
        str = item.name;
        return str.c_str();
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [mdnsScan]() { return mdnsScan->getAddresses().size(); },
      [this,mdnsScan](size_t index) {
        printf("Selected: %s\n", mdnsScan->getAddresses().at(index).name.c_str());
        selectedIndex = index;
      },
      MAIN_MENU_Y_SHIFT,
      MAIN_MENU_ROWS,  // offset for each line of text and total number of lines
                       // in menu to dispaly
      MAIN_MENU_X_OFFSET, MAIN_MENU_Y_OFFSET,  // x,y cursor
      MAIN_MENU_FONT3,
      MAIN_MENU_FONT3  // font to use printin highlighted / non-highlighted item
  );

  // let's set specific options for this meuItem

  /*
    dynamic list will act as page selector,
    it means that on ''enter'' event the highlighted scroll item's text label
    will be used as a key to search and switch to the page with same 'name'
  */
  list->listopts.page_selector = false;

  // this flag means that last item of a list will act as 'back' event, and will
  // try to retuen to the previous page/item
  list->listopts.back_on_last = true;
  // scroller here is the only active element on a page,
  // thre is no other items where we can move our cursor to
  // so let's set that an attempt to unselect this item will genreate "menuQuit"
  // event
  list->on_escape = mui_event_t::quitMenu;

  // add list menu object to our root page
  addMuippItem(list, root_page);

  // this is the only active Item on a page, so  let's autselect it
  // it means that whenever this page is loaded, the item on this page with
  // specified id will be focused and selected so it will receive all cursor
  // events by default
  pageAutoSelect(root_page, scroll_list_id);

  // start our menu from root page
  menuStart(root_page);
}

bool DCCConnectionMenu::doAction(mui_event event) {
  if (selectedIndex != -1) {
    auto mdnsScan = MDNSScan::getInstance();
    auto addresses = mdnsScan->getAddresses();

    if (selectedIndex == addresses.size() - 1) {
      displayControls->exitMenu();
      return false;
    }
    auto item = addresses.at(selectedIndex);

    printf("Connecting to server %s %s:%s\n", item.name.c_str(), item.address.c_str(),item.port.c_str());
    displayControls->showConnectTo(item.name.c_str());

    // Function to initiate a connection to the server
    auto wifiControl = WifiControl::getInstance();
    int port = std::stoi(item.port); // Convert port string to integer
    if (wifiControl->connectToServer(item.address.c_str(), port)) {
      printf("Connected to server\n");
      displayControls->setDCCConnectionEntry(item);
      clearAction();
      dccMenu = std::make_shared<DCCMenu>(displayControls);
      dccMenu->showMenu();
    } else {
      displayControls->showDCCFailedConnection("failed");
      printf("Failed to connect to server\n");
      sleep_ms(2000);
      displayControls->exitMenu();
      return false;
    }
  }
  return true;
}

void DCCConnectionMenu::clearAction() {
  printf("Clearing action\n");
  selectedIndex = -1;
}



