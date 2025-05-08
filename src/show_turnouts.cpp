#include "show_turnouts.h"

#include <stdio.h>

#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "wifi_control.h"

ShowTurnoutsMenu::ShowTurnoutsMenu(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls), menu(){
  auto turnout = DCCExController::Turnout::getFirst();
  while (turnout != nullptr) {
    menuItem item;
    item.value = turnout->getId();
    item.label = turnout->getName();
    menu.push_back(item);
    turnout = turnout->getNext();
  }
  printf("Turnout size: %d\n", menu.size());
  for (const auto& item : menu) {
    printf("Turnout item: %d, %s\n", item.value, item.label);
  }
  menuItem item;
  item.value = 0xff;
  item.label = "Back";
  menu.push_back(item);
}

void ShowTurnoutsMenu::showMenu() {

  displayControls->showScreen(sharedThis,
    [this](u8g2_t &u8g2) {
      buildMenu(u8g2);
    });

}

void ShowTurnoutsMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
  muiItemId root_page = makePage("Turnouts");  // provide a label for the page

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
        return menu.at(index).label;
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [this]() { return menu.size(); },
      [this](size_t index) {
          selectedItem = menu.at(index);
          printf("Selected: %s\n", selectedItem.label);
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

  // addMuippItem(new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back",
  //   MAIN_MENU_FONT1), root_page);


  // start our menu from root page
  menuStart(root_page);

}

void ShowTurnoutsMenu::clearAction() {
  // selectedItem.value = miv_None;
}

bool ShowTurnoutsMenu::doAction(){
  switch(selectedItem.value){
    case 0xff:
      // back button pressed
      displayControls->endScreen();
      return false;
      break;
    default:
      // selectedItem.value is a turnout address
      printf("Selected turnout: %d\n", selectedItem.value);
      WifiControl::getInstance()->dccProtocol()->toggleTurnout(selectedItem.value);
      break;
  }
  return true;
}

bool ShowTurnoutsMenu::doLongPressAction(){
  displayControls->endScreen();
  return false;
}

