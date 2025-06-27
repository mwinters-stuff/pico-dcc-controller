#ifndef _MDNS_SCAN_H
#define _MDNS_SCAN_H

#include <u8g2.h>

#include <memory>
#include <string>
#include <vector>

#include "mui_menu.h"
#include "dcc_menu.h"
#include "dcc_connection_entry.h"

class MDNSScan : public std::enable_shared_from_this<MDNSScan> {
 protected:

  std::vector<DCCConnectionEntry> addresses;
  uint8_t mdns_request_id;
  std::unique_ptr<DCCConnectionEntry> mdns_search_result;
   static std::shared_ptr<MDNSScan> instance;

  void discoverMDNSWithrottle();
  void checkAddHost();
  MDNSScan();
  MDNSScan(const MDNSScan&) = delete; // Disable copy constructor
  MDNSScan& operator=(const MDNSScan&) = delete; // Disable assignment operator
 public:
  virtual ~MDNSScan();
  void parseDCCEXAddresses(const std::string &input);

  static std::shared_ptr<MDNSScan> initInstance(){
      instance = std::shared_ptr<MDNSScan>(new MDNSScan());
      return instance;
    }

    static std::shared_ptr<MDNSScan> getInstance(){
      return MDNSScan::instance;
    }


  std::string dns_label_to_string(const char *label, int maxlen);
  void print_dns_txt(const char *txt, int len);

  std::vector<DCCConnectionEntry> getAddresses() const {
    return addresses;
  }

};

#endif