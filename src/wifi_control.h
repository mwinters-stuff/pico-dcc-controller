#ifndef _WIFI_CONTROL_H
#define _WIFI_CONTROL_H

#include <DCCEXProtocol.h>
#include <lwip/tcp.h>

#include <memory>

#include "dcc_delegate.h"
#include "wifi_connection.h"

class DisplayControls;

class WifiControl {
 private:
  std::shared_ptr<DisplayControls> displayControls;
  std::shared_ptr<DCCExController::DCCEXProtocol> dccExProtocol;
  DCCEXProtocolDelegateImpl dccDelegate;
  TCPSocketStream *stream = nullptr;
  LoggingStream *logStream = nullptr;
  uint8_t *ip_address;

  bool isConnected = false;

  static std::shared_ptr<WifiControl> instance;

  // Private constructor to prevent instantiation from outside
  // This ensures that the class can only be instantiated through the
  // initInstance method and provides a single instance of WifiControl
  // throughout the application. This is a common pattern for implementing the
  // Singleton design pattern in C++.
  explicit WifiControl(const WifiControl &) = delete;
  WifiControl &operator=(const WifiControl &) = delete;
  WifiControl(std::shared_ptr<DisplayControls> displayControls);

 public:
  virtual ~WifiControl() {};

  static std::shared_ptr<WifiControl> initInstance(
      std::shared_ptr<DisplayControls> displayControls) {
    instance = std::shared_ptr<WifiControl>(new WifiControl(displayControls));
    return instance;
  }
  static std::shared_ptr<WifiControl> getInstance() { return instance; }

  bool init();
  bool connect();

  void failError(err_t err);
  bool connectToServer(const char *server_ip, uint16_t port);
  void loop();

  std::shared_ptr<DCCExController::DCCEXProtocol> dccProtocol() {
    return dccExProtocol;
  };
};

#endif