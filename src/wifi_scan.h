#ifndef _WIFI_SCAN_H
#define _WIFI_SCAN_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include "pico/cyw43_arch.h"
#include <memory>


class DisplayControls;

class WifiScan : public MuiMenu {
 protected:
  std::vector<cyw43_ev_scan_result_t> scanResults;
  std::vector<std::string> ssidList;
  size_t selectedIndex = -1;

  void addScanResults(cyw43_ev_scan_result_t result);

 public:
  WifiScan(std::shared_ptr<DisplayControls> displayControls): MuiMenu(displayControls) {};
  virtual ~WifiScan() {};
  
  void scanWifi();
  static int scanResult(void *env, const cyw43_ev_scan_result_t *result);

  void buildMenu(u8g2_t &u8g2);
  void buildPassEntry(u8g2_t &u8g2);
  void clearAction();
  void doAction();
};

#endif