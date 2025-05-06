#include "dcc_connection_menu.h"

#include <stdio.h>
#include <sstream>

#include <muipp_u8g2.hpp>

#include "defines.h"
#include "../config.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "wifi_control.h"
#include "test_menu.h"

DCCConnectionMenu::DCCConnectionMenu(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls){
  parseDCCEXAddresses(DCCEX_ADDRESSES);
}

void DCCConnectionMenu::parseDCCEXAddresses(const std::string& input) {
  std::stringstream ss(input);
  std::string item;
  addresses.clear();
  while (std::getline(ss, item, ',')) {
      addresses.push_back(item);
  }
}

void DCCConnectionMenu::showMenu() {

  displayControls->showScreen(sharedThis,
    [this](u8g2_t &u8g2) {
      buildMenu(u8g2);
    });

}

void DCCConnectionMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
  selectedIndex = -1;
  muiItemId root_page = makePage("DCC Connect");  // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();

  

  auto list = new MuiItem_U8g2_DynamicScrollList(
      u8g2,            // U8g2 object to draw to
      scroll_list_id,  // ID for the item
      [this](size_t index) {
          auto fullStr = addresses.at(index);
          auto delimiterPos = fullStr.find('|');
          auto str = (delimiterPos != std::string::npos) ? fullStr.substr(0, delimiterPos) : fullStr;
          return str.c_str();
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [this]() { return addresses.size(); },
      [this](size_t index) {
          printf("Selected: %s\n", addresses.at(index).c_str());
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

  addMuippItem(new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back",
  MAIN_MENU_FONT1), root_page);

  // start our menu from root page
  menuStart(root_page);

}

void DCCConnectionMenu::clearAction() {
  selectedIndex = -1;
}

bool DCCConnectionMenu::doAction(){
  if(selectedIndex != -1){
    auto fullStr = addresses.at(selectedIndex);
    auto delimiterPos = fullStr.find('|');
    auto address = (delimiterPos != std::string::npos) ? fullStr.substr(delimiterPos + 1) : fullStr;
    auto name = (delimiterPos != std::string::npos) ? fullStr.substr(0, delimiterPos) : fullStr;

    printf("Connecting to server %s %s\n",name.c_str(), address.c_str());
    displayControls->showConnectTo(name.c_str());


// Function to initiate a connection to the server
    auto wifiControl = WifiControl::getInstance();
    if(wifiControl->connectToServer(address.c_str(), DCCEX_PORT)){
      printf("Connected to server\n");
      // testMenu = std::dynamic_pointer_cast<TestMenu>(TestMenu::create(displayControls));
      testMenu = TestMenu::create(displayControls);
      testMenu->showMenu();
    }else{
      displayControls->showDCCFailedConnection("failed");
      printf("Failed to connect to server\n");
    }
  }
  return true;

}

