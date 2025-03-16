#ifndef _WIFI_SCAN_H
#define _WIFI_SCAN_H

#include <vector>
#include "pico/cyw43_arch.h"

class WifiScan{
  protected:
    std::vector<cyw43_ev_scan_result_t> scanResults;

  public:
    void scanWifi();
    static int scanResult(void *env, const cyw43_ev_scan_result_t *result);

};

#endif