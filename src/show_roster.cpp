#include "show_roster.h"

#include <stdio.h>

#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "wifi_control.h"
#include "cab_control_menu.h"
#include "loco_controller.h"
#include "main_core1.h"
#include "mui_menu_custom_items.h"


ShowRosterMenu::ShowRosterMenu(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls), menu(){
  auto loco = DCCExController::Loco::getFirst();
  while (loco != nullptr) {
    menuItem item;
    item.value = loco->getAddress();
    strncpy(item.label, loco->getName(), NAME_MAX_LENGTH - 1);
    item.loco = loco;
    menu.push_back(item);
    loco = loco->getNext();
  }
  printf("Roster size: %d\n", menu.size());
  for (const auto& item : menu) {
    printf("Roster item: %d, %s\n", item.value, item.label);
  }
  menuItem item;
  item.value = 0xff;
  strncpy(item.label, "Back", NAME_MAX_LENGTH - 1);
  menu.push_back(item);
}

void ShowRosterMenu::showMenu() {

  displayControls->showScreen(shared_from_this(),
    [this](u8g2_t &u8g2) {
      buildMenu(u8g2);
    });

}

void ShowRosterMenu::getLocoIndexNames() {
  for (size_t i = 0; i < menu.size() - 1; ++i) {
    auto loco = menu.at(i).loco;
    if (loco != nullptr) {
      uint8_t buttonIndex = LocoController::getInstance()->getLocoButtonIndex(loco);
      if (buttonIndex != 0xff) {
        snprintf(menu.at(i).label, NAME_MAX_LENGTH, "%d: %s", buttonIndex + 1, loco->getName());
      } else {
        snprintf(menu.at(i).label, NAME_MAX_LENGTH, "%s", loco->getName());
      }
    }
  }

}

void ShowRosterMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
  muiItemId root_page = makePage("Roster");  // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  getLocoIndexNames();

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();


  

  auto list = new Custom_U8g2_DynamicScrollList(
      u8g2,            // U8g2 object to draw to
      scroll_list_id,  // ID for the item
      [this](size_t index) {
        return menu.at(index).label;  // callback to get label for each item
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [this]() { return menu.size(); },
      [this](size_t index) {
          selectedItem = &menu.at(index);
          printf("Selected: %s\n", selectedItem->label);
      },
      [this](size_t index) {
        highlightIndex = index;
        auto highlighted = &menu.at(index);
        printf("Highlighted: %s\n", highlighted->label);
        // if highlighted item is a loco, we can show its LED
        showButtonLED(index);
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

bool ShowRosterMenu::doAction(){
  switch(selectedItem->value){
    case 0xff:
      // back button pressed
      displayControls->exitMenu();
      return false;
      break;
    default:
      // selectedItem.value is a loco address
      printf("Selected loco: %d\n", selectedItem->value);

      auto menu = std::make_shared<CabControlMenu>(displayControls);
      
      menu->setLoco(selectedItem->loco);
      LocoController::getInstance()->driveLoco(selectedItem->loco);  // Set loco at index 0
      menu->showMenu();
      break;
  }
  return true;
}

void ShowRosterMenu::clearAction(){
  selectedItem = nullptr;
}

bool ShowRosterMenu::doLongPressAction(){
  displayControls->exitMenu();
  return false;
}


void ShowRosterMenu::doButtonAction(uint8_t action, uint8_t value) {

}

bool ShowRosterMenu::doKeyAction(int8_t action) {
  // Handle key actions
 if (action >= '1' && action <= '4') {
    // Handle loco selection by key
    uint8_t locoIndex = action - '1';
    if (locoIndex < 4 && highlightIndex < menu.size() - 1) {
      auto loco = menu.at(highlightIndex).loco;
      printf("Selected loco: %s Key %d\n", loco->getName(), locoIndex);
      LocoController::getInstance()->setLoco(locoIndex, loco);
      // Show the button LED for the selected loco
      showButtonLED(highlightIndex);
      showMenu(); // Refresh the menu to show updated button indices
    }
  }
  return true;
}

void ShowRosterMenu::showButtonLED(int menuIndex){
  // Show button LED for the selected loco
  turn_off_loco_leds(); // Turn off all LEDs first
  auto loco = menu.at(menuIndex).loco;
  uint8_t buttonIndex = LocoController::getInstance()->getLocoButtonIndex(loco);  
  if (buttonIndex != 0xff) {
    printf("Showing LED for button %d\n", buttonIndex);
    queue_led_command(get_loco_led_from_input(buttonIndex), true);
  } else {
    printf("No button assigned for this loco\n");
  }

}
