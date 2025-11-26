
#include "functions_menu.h"

#include <stdio.h>

#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "loco_controller.h"
#include "pico/stdlib.h"
#include "wifi_control.h"

FunctionsMenu::FunctionsMenu(std::shared_ptr<DisplayControls> displayControls)
    : MuiMenu(displayControls), menu() {}

void FunctionsMenu::setLoco(DCCExController::Loco *loco) {
  this->loco = loco;

  for (int i = 0; i < DCCExController::MAX_FUNCTIONS; i++) {
    if (loco->getFunctionName(i)[0] != '\0') {
      menuItem item;
      item.value = i;
      item.momentary = loco->isFunctionMomentary(i);
      item.active = loco->isFunctionOn(i);
      item.label =
          (std::string(item.momentary ? "*" : " ")) + loco->getFunctionName(i);
      menu.push_back(item);
    }
  }
  printf("Function size: %d\n", menu.size());

  menuItem item;
  item.value = 0xff;
  item.label = "Back";
  menu.push_back(item);
};

void FunctionsMenu::showMenu() {
  displayControls->showScreen(shared_from_this(),
                              [this](u8g2_t &u8g2) { buildMenu(u8g2); });
}

void FunctionsMenu::buildMenu(u8g2_t &u8g2) {
  // create root page

  snprintf(title, sizeof(title), "Functions: %s", loco->getName());
  muiItemId root_page = makePage(title);

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();

  auto list = new MuiItem_U8g2_DynamicScrollList(
      u8g2, scroll_list_id,
      [this](size_t index) { return menu.at(index).label.c_str(); },
      [this]() { return menu.size(); },
      [this](size_t index) {
        selectedItem = menu.at(index);
       
      },
      MAIN_MENU_Y_SHIFT,
      MAIN_MENU_ROWS,  // offset for each line of text and total number of lines
                       // in menu to dispaly
      MAIN_MENU_X_OFFSET, MAIN_MENU_Y_OFFSET,  // x,y cursor
      MAIN_MENU_FONT1,
      MAIN_MENU_FONT1  // font to use printin highlighted / non-highlighted item
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

  // addMuippItem(new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back",
  //   MAIN_MENU_FONT1), root_page);

  // start our menu from root page
  menuStart(root_page);
}

bool FunctionsMenu::doAction(mui_event event) {
  switch (selectedItem.value) {
    case 0xff:
      // back button pressed
      displayControls->exitMenu();
      LocoController::getInstance()->driveLocoMenu(loco);
      return false;
      break;
    default:
      // selectedItem.value is a function
      if(selectedItem.label.empty()) {
        printf("Function label is empty, ignoring\n");
        break;
      }
      char what[5];
        if(selectedItem.momentary) {
          // // if function is momentary, switch it off right away
          WifiControl::getInstance()->dccProtocol()->functionOn(loco, selectedItem.value);
          // WifiControl::getInstance()->dccProtocol()->functionOff(loco, selectedItem.value);
          selectedItem.active = false;
          strncpy(what, "MOM", sizeof(what));
        }else{
          if (WifiControl::getInstance()->dccProtocol()->isFunctionOn(loco, selectedItem.value)) {
            WifiControl::getInstance()->dccProtocol()->functionOff(loco, selectedItem.value);
            selectedItem.active = false;
            strncpy(what, "OFF", sizeof(what));
          } else {
            WifiControl::getInstance()->dccProtocol()->functionOn(loco, selectedItem.value);
            selectedItem.active = true;
            strncpy(what, "ON", sizeof(what));
          }
        }
        // menu.at(index).active = selectedItem.active;
        printf("Function: %s %s\n", selectedItem.label.c_str(), what);

      break;
  }
  return true;
}

void FunctionsMenu::clearAction() { selectedItem.value = -1; }

bool FunctionsMenu::doLongPressAction() {
  displayControls->exitMenu();
  LocoController::getInstance()->driveLocoMenu(loco);
  return false;
}
