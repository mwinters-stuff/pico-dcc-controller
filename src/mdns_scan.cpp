#include "mdns_scan.h"

#include <lwip/ip_addr.h>
#include <lwip/tcp.h>
#include <stdio.h>


#include <sstream>

#include "../config.h"
#include "defines.h"

#include "lwip/apps/mdns.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/prot/dns.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "dcc_menu.h"
#include "wifi_control.h"

std::shared_ptr<MDNSScan> MDNSScan::instance = nullptr;

MDNSScan::MDNSScan() {
  parseDCCEXAddresses(DCCEX_ADDRESSES);
  discoverMDNSWithrottle();
}

void MDNSScan::parseDCCEXAddresses(const std::string &input) {
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
  printf("MDNS  Parsed addresses:\n");
  for (const auto& entry : addresses) {
    printf("  %s | %s | %s\n", entry.name.c_str(), entry.address.c_str(), entry.port.c_str());
  }
}



MDNSScan::~MDNSScan() {
  printf("MDNS destructor\n");
  mdns_search_stop(mdns_request_id);
  mdns_resp_remove_netif(&cyw43_state.netif[0]);
  addresses.clear();
}

std::string MDNSScan::dns_label_to_string(const char *label,
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

void MDNSScan::print_dns_txt(const char *txt, int len) {
  int idx = 0;
  while (idx < len) {
    uint8_t entry_len = txt[idx];
    if (entry_len == 0 || idx + 1 + entry_len > len) break;
    printf("TXT: %.*s\n", entry_len, &txt[idx + 1]);
    idx += entry_len + 1;
  }
}

void MDNSScan::discoverMDNSWithrottle() {
  // Optionally clear previous results
  // addresses.clear();

  // Register the netif for mDNS
  mdns_resp_add_netif(&cyw43_state.netif[0], HOSTNAME);

  // mDNS search callback
  auto mdns_result_cb = [](struct mdns_answer *answer, const char *varpart,
                           int varlen, int flags, void *arg) {
    if (!answer) return;

    MDNSScan *self = static_cast<MDNSScan *>(arg);
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

void MDNSScan::checkAddHost() {
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
  printf("mDNS Added address: %s %s:%s\n", 
         mdns_search_result->name.c_str(),
         mdns_search_result->address.c_str(),
         mdns_search_result->port.c_str());
  mdns_search_result.reset();
  // Update the display

}
