#ifndef _CONFIG_H
#define _CONFIG_H

// This file needs to be copied into "config.h" and customised.

#define HOSTNAME "pico-throttle-1"

#define WIFI_AP "ACCESS"
#define WIFI_PASS "PASS"
#define WIFI_AUTH CYW43_AUTH_WPA2_AES_PSK

#define DCCEX_ADDRESSES "Main|192.168.1.229|2560,test|192.168.1.225|2560"
#define DCCEX_PORT 2560

// minutes until display sleep.
#define DISPLAY_SLEEP 1

#endif