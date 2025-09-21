#include "wifi_scan.h"

#include <stdio.h>

#include <muipp_u8g2.hpp>

#include "defines.h"
#include "display_controls.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int WifiScan::scanResult(void *env, const cyw43_ev_scan_result_t *result) {
  if (result) {
    WifiScan *wifi_scan = (WifiScan *)env;

    for (auto r : wifi_scan->scanResults) {
      // printf("Checking %-32s with %-32s\n", r.ssid, result->ssid);
      if (memcmp(r.ssid, result->ssid, 32) == 0) {
        return 0;
      }
    }

    if (result->ssid[0] > 0) {
      wifi_scan->addScanResults(*result);
    }
  }

  return 0;
}

void WifiScan::addScanResults(cyw43_ev_scan_result_t result) {
  printf("ssid: %-32s\n", result.ssid);
  scanResults.push_back(result);
  ssidList.push_back(
      std::string(reinterpret_cast<const char *>(result.ssid),
                  strnlen(reinterpret_cast<const char *>(result.ssid), 32)));

  if (displayControls != NULL) {
    displayControls->setRedraw();
  }
}

void WifiScan::scanWifi() {
  scanResults.clear();
  ssidList.clear();

  absolute_time_t scan_time = nil_time;
  bool scan_in_progress = false;
  if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
    if (!scan_in_progress) {
      cyw43_wifi_scan_options_t scan_options = {0};
      int err = cyw43_wifi_scan(&cyw43_state, &scan_options, this,
                                WifiScan::scanResult);
      if (err == 0) {
        printf("\nPerforming wifi scan\n");
        scan_in_progress = true;
      } else {
        printf("Failed to start scan: %d\n", err);
        scan_time = make_timeout_time_ms(10000);  // wait 10s and scan again
      }
    } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
      scan_time = make_timeout_time_ms(10000);  // wait 10s and scan again
      scan_in_progress = false;
    }
  }
}

void WifiScan::showMenu() {
  displayControls->showScreen(shared_from_this(),
    [this](u8g2_t &u8g2) {
      buildMenu(u8g2);
    });
}


void WifiScan::buildMenu(u8g2_t &u8g2) {
  // create root page
  muiItemId root_page = makePage("Wifi Scan");  // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();

  // Temperature menu options dynamic scroll list
  /*
    MuiItem_U8g2_DynamicScrollList item makes a scrolling list of text labels
    where you can set how many lines could be displayed at once and go through
    it
  */
  auto list = new MuiItem_U8g2_DynamicScrollList(
      u8g2,            // U8g2 object to draw to
      scroll_list_id,  // ID for the item
      [this](size_t index) {
        if (index == scanResults.size()) {
          return "*Scan Again*";
        } else {
          return ssidList.at(index).c_str();
        }
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [this]() { return scanResults.size() + 1; },
      [this](size_t index) {
        if (index < scanResults.size()) {
          printf("Selected: %s\n", ssidList.at(index).c_str());
          selectedIndex = index;
          this->displayControls->showScreen(shared_from_this(),
              [this](u8g2_t &u8g2) {
                this->buildPassEntry(u8g2);
              });

        } else {
          scanWifi();
        }
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
  list->listopts.back_on_last = false;
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

  //   addMuippItem(new MuiItem_U8g2_ActionButton(u8g2,
  //   nextIndex(),mui_event_t::custom1, "Rescan", MAIN_MENU_FONT1, 2,
  //   u8g2_GetDisplayHeight(&u8g2) - u8g2_GetFontSize(MAIN_MENU_FONT1),
  //   text_align_t::left,
  //   text_align_t::bottom
  // ),
  //     root_page);

  // addMuippItem(new MuiItem_U8g2_BackButton(u8g2, nextIndex(), "Back",
  // MAIN_MENU_FONT1), root_page);

  // start our menu from root page
  menuStart(root_page);
  this->displayControls = displayControls;
}

void WifiScan::buildPassEntry(u8g2_t &u8g2) {
  // create root page
  muiItemId root_page =
      makePage("Wifi AP Password");  // provide a label for the page

  // create "Page title" item and assign it to root page
  addMuippItem(
      new MuiItem_U8g2_PageTitle(u8g2, nextIndex(), PAGE_TITLE_FONT_SMALL),
      root_page);

  // create and add to main page a list with settings selection options
  muiItemId scroll_list_id = nextIndex();
  addMuippItem(new MuiItem_U8g2_StaticText(
                   u8g2, scroll_list_id,
                   ("Ap Name: " + ssidList.at(selectedIndex)).c_str(),
                   MAIN_MENU_FONT1, MAIN_MENU_X_OFFSET, MAIN_MENU_Y_OFFSET),
               root_page);

  menuStart(root_page);
  this->displayControls = displayControls;
}


bool WifiScan::doAction(mui_event event) {
  return true;
}
