#include "dcc_connection_menu.h"

#include <lwip/ip_addr.h>
#include <lwip/tcp.h>
#include <stdio.h>

#include <muipp_u8g2.hpp>
#include <sstream>

#include "../config.h"
#include "defines.h"
#include "display_controls.h"
#include "lwip/apps/mdns.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/prot/dns.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "dcc_menu.h"
#include "wifi_control.h"

DCCConnectionMenu::DCCConnectionMenu(
    std::shared_ptr<DisplayControls> displayControls)
    : MuiMenu(displayControls) {
  parseDCCEXAddresses(DCCEX_ADDRESSES);
  discoverMDNSWithrottle();
}

void DCCConnectionMenu::parseDCCEXAddresses(const std::string &input) {
  std::stringstream ss(input);
  std::string item;
  addresses.clear();
  while (std::getline(ss, item, ',')) {
    std::stringstream item_ss(item);
    std::string name, address, port;
    std::getline(item_ss, name, '|');
    std::getline(item_ss, address, '|');
    std::getline(item_ss, port, '|');
    addresses.push_back({name, address, port});
  }
  addresses.push_back({"Back", "0.0.0.0", "0"});
  printf("Parsed addresses:\n");
  for (const auto& entry : addresses) {
    printf("  %s | %s | %s\n", entry.name.c_str(), entry.address.c_str(), entry.port.c_str());
  }
}

void DCCConnectionMenu::showMenu() {
  displayControls->showScreen(shared_from_this(),
                              [this](u8g2_t &u8g2) { buildMenu(u8g2); });
}

void DCCConnectionMenu::buildMenu(u8g2_t &u8g2) {
  // create root page
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
      [this](size_t index) {
        auto item = addresses.at(index);
        str = item.name;
        return str.c_str();
      },
      // next callback is called by DynamicScrollList to find out the size
      // (total number) of elements in a list
      [this]() { return addresses.size(); },
      [this](size_t index) {
        printf("Selected: %s\n", addresses.at(index).name.c_str());
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

bool DCCConnectionMenu::doAction() {
  if (selectedIndex != -1) {
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
  mdns_search_stop(mdns_request_id);
  mdns_resp_remove_netif(&cyw43_state.netif[0]);
  addresses.clear();
  selectedIndex = -1;
}

std::string DCCConnectionMenu::dns_label_to_string(const char *label,
                                                   int maxlen) {
  std::string result;
  int idx = 0;
  while (idx < maxlen) {
    uint8_t len = label[idx];
    if (len == 0 || idx + len + 1 > maxlen) break;
    if (!result.empty()) result += ".";
    result.append(&label[idx + 1], len);
    idx += len + 1;
  }
  return result;
}

void DCCConnectionMenu::print_dns_txt(const char *txt, int len) {
  int idx = 0;
  while (idx < len) {
    uint8_t entry_len = txt[idx];
    if (entry_len == 0 || idx + 1 + entry_len > len) break;
    printf("TXT: %.*s\n", entry_len, &txt[idx + 1]);
    idx += entry_len + 1;
  }
}

void DCCConnectionMenu::discoverMDNSWithrottle() {
  // Optionally clear previous results
  // addresses.clear();

  // Register the netif for mDNS
  mdns_resp_add_netif(&cyw43_state.netif[0], HOSTNAME);

  // mDNS search callback
  auto mdns_result_cb = [](struct mdns_answer *answer, const char *varpart,
                           int varlen, int flags, void *arg) {
    if (!answer) return;

    DCCConnectionMenu *self = static_cast<DCCConnectionMenu *>(arg);
    if (answer->info.type == DNS_RRTYPE_PTR && varpart && varlen > 0) {
      // The target hostname is in varpart, in DNS label format
      std::string target = self->dns_label_to_string(varpart, varlen);
      printf("SRV target (hostname): %s\n", target.c_str());
    }
    // Optionally, print TXT records from varpart
    if (answer->info.type == DNS_RRTYPE_TXT && varpart && varlen > 0) {
      self->print_dns_txt(varpart, varlen);
    }
    if (answer->info.type == DNS_RRTYPE_SRV && varpart && varlen > 6) {
      // SRV: varpart contains priority(2), weight(2), port(2), target (DNS
      // label)
      if(!self->mdns_search_result){
        self->mdns_search_result = std::make_unique<DCCConnectionEntry>();
      }
      self->mdns_search_result->port = std::to_string(((uint8_t)varpart[4] << 8) | (uint8_t)varpart[5]);
      self->mdns_search_result->name = self->dns_label_to_string(varpart + 6, varlen - 6);
      printf("SRV: target: %s, port: %u\n", self->mdns_search_result->name.c_str(),
             self->mdns_search_result->port);
      self->checkAddHost();
    }
    if (answer->info.type == DNS_RRTYPE_A && varpart && varlen == 4) {
      char ipbuf[16];
      snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u",
              (uint8_t)varpart[0], (uint8_t)varpart[1],
              (uint8_t)varpart[2], (uint8_t)varpart[3]);
      self->mdns_search_result->address = ipbuf;
                             
      // Print the IP address in human-readable format
      printf("A record: s\n", self->mdns_search_result->address.c_str());
      self->checkAddHost();
    }

    // Optionally handle MDNS_SEARCH_RESULT_LAST to know when search is done
    if (flags & MDNS_SEARCH_RESULT_LAST) {
      printf("mDNS search for _withrottle._tcp finished.\n");
    }
  };

  err_t err =
      mdns_search_service(NULL,             // name (NULL for any instance)
                          "_withrottle",    // service type (without ._tcp)
                          DNSSD_PROTO_TCP,  // protocol
                          &cyw43_state.netif[0],  // netif (NULL for all)
                          mdns_result_cb,         // callback
                          this,                   // user arg
                          &mdns_request_id);

  if (err != ERR_OK) {
    printf("mDNS search failed: %d\n", err);
  }
}

void DCCConnectionMenu::checkAddHost() {
  if (!mdns_search_result) return;

  if (mdns_search_result->name.empty()) return;
  if (mdns_search_result->address.empty()) return;
  if (mdns_search_result->port.empty()) return;

  // Check if mdns_search_ip is already present in the addresses list
  for (const auto& entry : addresses) {
    if(entry.address == mdns_search_result->address) {
      // IP already exists, do not add
      printf("IP already exists: %s\n", mdns_search_result->address.c_str());
      mdns_search_result.reset();
      return;
    }
  }
  

  addresses.insert(addresses.begin(), {mdns_search_result->name, mdns_search_result->address, mdns_search_result->port}); // Push to the front
  printf("Added address: %s %s:%s\n", 
         mdns_search_result->name.c_str(),
         mdns_search_result->address.c_str(),
         mdns_search_result->port.c_str());
  mdns_search_result.reset();
  // Update the display
  displayControls->setRedraw();
}
