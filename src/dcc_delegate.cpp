#include "dcc_delegate.h"
#include <pico/time.h>

#include <stdio.h>
#include "loco_controller.h"

void DCCEXProtocolDelegateImpl::receivedServerVersion(int major, int minor,
                                                      int patch) {
  printf("Server Version: %d.%d.%d\n", major, minor, patch);
}

void DCCEXProtocolDelegateImpl::receivedMessage(char *message) {
  printf("Broadcast Message: %s\n", message);
}

void DCCEXProtocolDelegateImpl::receivedRosterList() {
  printf("Roster list received.\n");
  LocoController::getInstance()->loadLocosFromStorage();
}

void DCCEXProtocolDelegateImpl::receivedTurnoutList() {
  printf("Turnout list received.\n");
}

void DCCEXProtocolDelegateImpl::receivedRouteList() {
  printf("Route list received.\n");
}

void DCCEXProtocolDelegateImpl::receivedTurntableList() {
  printf("Turntable list received.\n");
}

void DCCEXProtocolDelegateImpl::receivedLocoUpdate(
    DCCExController::Loco *loco) {
  printf("Loco Update: Address=%d\n", loco->getAddress());
}

void DCCEXProtocolDelegateImpl::receivedLocoBroadcast(
    int address, int speed, DCCExController::Direction direction,
    int functionMap) {
  printf("Loco Broadcast: Address=%d, Speed=%d, Direction=%d, FunctionMap=%d\n",
         address, speed, direction, functionMap);
}

void DCCEXProtocolDelegateImpl::receivedTrackPower(
    DCCExController::TrackPower state) {
  printf("Track Power State: %d\n", state);
}

void DCCEXProtocolDelegateImpl::receivedIndividualTrackPower(
    DCCExController::TrackPower state, int track) {
  printf("Individual Track Power: Track=%d, State=%d\n", track, state);
}

void DCCEXProtocolDelegateImpl::receivedTrackType(
    char track, DCCExController::TrackManagerMode type, int address) {
  printf("Track Type: Track=%c, Type=%d, Address=%d\n", track, type, address);
}

void DCCEXProtocolDelegateImpl::receivedTurnoutAction(int turnoutId,
                                                      bool thrown) {
  printf("Turnout Action: ID=%d, Thrown=%s\n", turnoutId,
         thrown ? "true" : "false");
}

void DCCEXProtocolDelegateImpl::receivedTurntableAction(int turntableId,
                                                        int position,
                                                        bool moving) {
  printf("Turntable Action: ID=%d, Position=%d, Moving=%s\n", turntableId,
         position, moving ? "true" : "false");
}

void DCCEXProtocolDelegateImpl::receivedReadLoco(int address) {
  printf("Read Loco Address: %d\n", address);
}

void DCCEXProtocolDelegateImpl::receivedValidateCV(int cv, int value) {
  printf("Validate CV: CV=%d, Value=%d\n", cv, value);
}

void DCCEXProtocolDelegateImpl::receivedValidateCVBit(int cv, int bit,
                                                      int value) {
  printf("Validate CV Bit: CV=%d, Bit=%d, Value=%d\n", cv, bit, value);
}

void DCCEXProtocolDelegateImpl::receivedWriteLoco(int address) {
  printf("Write Loco Address: %d\n", address);
}

void DCCEXProtocolDelegateImpl::receivedWriteCV(int cv, int value) {
  printf("Write CV: CV=%d, Value=%d\n", cv, value);
}

void DCCEXProtocolDelegateImpl::receivedScreenUpdate(int screen, int row,
                                                     char *message) {
  printf("Screen Update: Screen=%d, Row=%d, Message=%s\n", screen, row,
         message);
}

uint32_t DCCEXProtocolDelegateImpl::millis() {
  return static_cast<uint32_t>(time_us_64() / 1000); // Convert microseconds to milliseconds
}
