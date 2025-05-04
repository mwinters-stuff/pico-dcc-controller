#ifndef _WIFI_CONTROL_H
#define _WIFI_CONTROL_H

#include <memory>
#include <lwip/tcp.h>
#include <DCCEXProtocol.h>
#include "dcc_delegate.h"
#include "wifi_connection.h"

class DisplayControls;

class WifiControl{
  private:
    std::shared_ptr<DisplayControls> displayControls;
    std::shared_ptr<DCCExController::DCCEXProtocol> dccExProtocol;
    DCCEXProtocolDelegateImpl dccDelegate;
    TCPSocketStream *stream;
    LoggingStream *logStream;
    uint8_t *ip_address;

    bool isConnected = false;

    static std::shared_ptr<WifiControl> instance;
  public:
  WifiControl(std::shared_ptr<DisplayControls> displayControls);
  virtual ~WifiControl(){};

    static std::shared_ptr<WifiControl> initInstance(std::shared_ptr<DisplayControls> displayControls){
      instance = std::make_shared<WifiControl>(displayControls);
      return instance;
    }
    static std::shared_ptr<WifiControl> getInstance(){
      return instance;
    }

    bool connect();
    
    void failError(err_t err);
    bool connectToServer(const char *server_ip, uint16_t port);
    void loop();

    std::shared_ptr<DCCExController::DCCEXProtocol> dccProtocol(){
      return dccExProtocol;
    };
};

#endif