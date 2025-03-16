#include "wifi_scan.h"

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"



int WifiScan::scanResult(void *env, const cyw43_ev_scan_result_t *result) {
  if (result) {
      WifiScan *wifi_scan = (WifiScan *)env;

      for (auto r: wifi_scan->scanResults){
        // printf("Checking %-32s with %-32s\n", r.ssid, result->ssid);
        if(memcmp(r.ssid, result->ssid, 32) == 0){
          return 0;
        }
      
      }

      wifi_scan->scanResults.push_back(*result);

      // printf("ssid: ");
      // for (int i = 0; i < 32; i++) {
      //     printf("%02x", result->ssid[i]);
      // }

      // printf("    ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
      //     result->ssid, result->rssi, result->channel,
      //     result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
      //     result->auth_mode);
  }
  return 0;
}


void WifiScan::scanWifi() {
  absolute_time_t scan_time = nil_time;
  bool scan_in_progress = false;
  if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
      if (!scan_in_progress) {
          cyw43_wifi_scan_options_t scan_options = {0};
          int err = cyw43_wifi_scan(&cyw43_state, &scan_options, this, WifiScan::scanResult);
          if (err == 0) {
              printf("\nPerforming wifi scan\n");
              scan_in_progress = true;
          } else {
              printf("Failed to start scan: %d\n", err);
              scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
          }
      } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
          scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
          scan_in_progress = false; 
      }
  }
}