#include "cab_control_menu.h"

#include <stdio.h>

#include <muipp_tpl.hpp>
#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "wifi_control.h"
#include "loco_controller.h"
#include "mui_menu_custom_items.h"

void CabControlMenu::showMenu() {
  displayControls->showScreen(shared_from_this(),
                              [this](u8g2_t &u8g2) { buildMenu(u8g2); });
}

void CabControlMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
  if (loco == nullptr) {
    return;
  }
  muiItemId root_page;

  if (loco->getName() != nullptr && strlen(loco->getName())) {
    root_page = makePage(loco->getName());
  } else {
    root_page = makePage(std::to_string(loco->getAddress()).c_str());
  }

  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  speed_idx = nextIndex();

  // Set font and calculate y position
  u8g2_SetFont(&u8g2, PAGE_TITLE_FONT_SMALL);
  int item_y_offset = u8g2_GetMaxCharHeight(&u8g2);

  addMuippItem(new Custom_U8g2_ValuesList(
                   u8g2, speed_idx, "Speed",
                   [&]() {
                     speed_str = std::to_string(loco->getSpeed());
                     return speed_str.data();
                   },
                   0, 
                   u8g2_GetDisplayWidth(&u8g2),
                   item_y_offset,  // cursor point
                   PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,
                   text_align_t::top),  // justify value to the right
               root_page);

  u8g2_SetFont(&u8g2, PAGE_TITLE_FONT);
  item_y_offset += u8g2_GetMaxCharHeight(&u8g2);

  addMuippItem(
      new Custom_U8g2_ValuesList(
          u8g2, nextIndex(), "Driving",
          [&]() {
            direction_str = loco->getDirection() == DCCExController::Forward
                                ? "Forward"
                                : "Reverse";
            return direction_str.data();
          },  
          0, 
          u8g2_GetDisplayWidth(&u8g2), 
          item_y_offset,  // cursor point
          PAGE_TITLE_FONT, text_align_t::left, text_align_t::right,
          text_align_t::top),  // justify value to the right
      root_page);

  item_y_offset += u8g2_GetMaxCharHeight(&u8g2);

  auto functionsIndex = nextIndex();
  addMuippItem(
      new Custom_U8g2_ActionButton(u8g2, functionsIndex,
      [this]() {
        // go to functions menu
        printf("Functions button pressed\n");
        displayControls->showFunctionsMenu(shared_from_this(), loco);
      },
       "Functions", MAIN_MENU_FONT1,
      0, u8g2_GetDisplayHeight(&u8g2), text_align_t::left, text_align_t::bottom)
      , root_page);

  pageAutoSelect(root_page, functionsIndex);

  addMuippItem(
      new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back", MAIN_MENU_FONT1),
      root_page);

  // start our menu from root page
  menuStart(root_page);
}

void CabControlMenu::setLoco(DCCExController::Loco *loco) {
  this->loco = loco;
  if (loco != nullptr) {
    printf("CabControlMenu: setLoco: %d\n", loco->getAddress());
  } else {
    printf("CabControlMenu: setLoco: nullptr\n");
  }
}

bool CabControlMenu::doAction(mui_event event) { 
  return true; 
}

bool CabControlMenu::doLongPressAction() {
  displayControls->exitMenu();
  return false;
}

bool CabControlMenu::doMoveLeftAction() { return true; }

bool CabControlMenu::doMoveRightAction() { return true; }

bool CabControlMenu::doKeyAction(int8_t action) {
  if (action == 'B') {
    displayControls->exitMenu();
    return false;
  }
  return true;
}

void CabControlMenu::loop() { displayControls->setRedraw(); }
