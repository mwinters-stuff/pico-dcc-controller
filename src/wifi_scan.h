#ifndef _WIFI_SCAN_H
#define _WIFI_SCAN_H

#include <u8g2.h>

#include <vector>

#include "mui_menu.h"
#include "pico/cyw43_arch.h"

class DisplayControls;

class WifiScan : public MuiMenu {
 protected:
  DisplayControls *displayControls;
  std::vector<cyw43_ev_scan_result_t> scanResults;
  std::vector<std::string> ssidList;
  size_t selectedIndex = -1;

  void addScanResults(cyw43_ev_scan_result_t result);

 public:
  void scanWifi();
  static int scanResult(void *env, const cyw43_ev_scan_result_t *result);

  void buildMenu(u8g2_t &u8g2, DisplayControls *displayControls);
  void buildPassEntry(u8g2_t &u8g2, DisplayControls *displayControls);
};

#endif