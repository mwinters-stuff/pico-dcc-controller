#ifndef _WIFI_CONTROL_H
#define _WIFI_CONTROL_H

#include <memory>
#include <lwip/tcp.h>

class DisplayControls;

class WifiControl{
  private:
    std::shared_ptr<DisplayControls> displayControls;
    uint8_t *ip_address;
  public:
    WifiControl(std::shared_ptr<DisplayControls> displayControls);
    virtual ~WifiControl(){};

    bool connect();
    
    void failError(err_t err);
    bool connectToServer(const char *server_ip, uint16_t port);
};

#endif