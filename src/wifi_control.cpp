#include "wifi_control.h"

#include <lwip/ip_addr.h>
#include <lwip/tcp.h>

#include "../config.h"
#include "display_controls.h"
#include "pico/cyw43_arch.h"
#include "wifi_connection.h"

WifiControl::WifiControl(std::shared_ptr<DisplayControls> displayControls)
    : displayControls(displayControls) {}

bool WifiControl::connect() {
  // Initialise the Wi-Fi chip
  if (cyw43_arch_init()) {
    displayControls->showWifiError();
    printf("Wi-Fi init failed\n");
    return false;
  }

  displayControls->showWifiConnecting();

  // Enable wifi station
  cyw43_arch_enable_sta_mode();

  printf("Connecting to Wi-Fi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_AP, WIFI_PASS, WIFI_AUTH,
                                         30000)) {
    displayControls->showWifiConnectionFailed();
    printf("failed to connect.\n");
    return false;
  } else {
    displayControls->showWifiConnected();
    printf("Connected.\n");
    // Read the ip address in a human readable way
    ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1],
           ip_address[2], ip_address[3]);
    return true;
  }
}

void WifiControl::failError(err_t err) {
  switch (err) {
    case ERR_MEM:
      displayControls->showDCCFailedConnection("Out of memory");
      break;
    case ERR_BUF:
      displayControls->showDCCFailedConnection("Buffer error");
      break;
    case ERR_TIMEOUT:
      displayControls->showDCCFailedConnection("Timeout");
      break;
    case ERR_RTE:
      displayControls->showDCCFailedConnection("Routing problem");
      break;
    case ERR_INPROGRESS:
      displayControls->showDCCFailedConnection("Operation in progress");
      break;
    case ERR_VAL:
      displayControls->showDCCFailedConnection("Illegal value");
      break;
    case ERR_USE:
      displayControls->showDCCFailedConnection("Operation already in progress");
      break;
    case ERR_CONN:
      displayControls->showDCCFailedConnection("Not connected");
      break;
    case ERR_IF:
      displayControls->showDCCFailedConnection("Low-level netif error");
      break;

    case ERR_ABRT:

      displayControls->showDCCFailedConnection("Connection aborted");
      break;
    case ERR_RST:
      displayControls->showDCCFailedConnection("Connection reset");
      break;
    case ERR_CLSD:
      displayControls->showDCCFailedConnection("Connection closed");
      break;
    default:
      displayControls->showDCCFailedConnection("Unknown error");
      break;
  }
}

// Function to initiate a connection to the server
bool WifiControl::connectToServer(const char *server_ip, uint16_t port) {
  ip_addr_t server_addr;
  if (!ipaddr_aton(server_ip, &server_addr)) {
    printf("Invalid IP address\n");
    return false;
  }

  struct tcp_pcb *pcb = tcp_new();
  if (pcb == nullptr) {
    printf("Failed to create PCB\n");
    return false;
  }

  // Attempt to connect
  err_t err = tcp_connect(pcb, &server_addr, port, nullptr);
  if (err != ERR_OK) {
    displayControls->showDCCFailedConnection("failed");
    printf("Failed to initiate connection: %d\n", err);
    tcp_close(pcb);
    return false;
  }

  // Poll for connection status
  printf("Connecting to server...\n");
  while (true) {
    cyw43_arch_poll();
    // Check the connection state
    if (pcb->state == ESTABLISHED) {
      printf("Connected to server: %s:%d\n", ipaddr_ntoa(&pcb->remote_ip),
             pcb->remote_port);
      return true;
    } else if (pcb->state == CLOSED || pcb->state == TIME_WAIT ||
               pcb->state == FIN_WAIT_1 || pcb->state == FIN_WAIT_2) {
      printf("Connection failed or closed\n");
      tcp_close(pcb);
      return false;
    }

    // Add a small delay to avoid busy-waiting
    sleep_ms(100);
  }
}
