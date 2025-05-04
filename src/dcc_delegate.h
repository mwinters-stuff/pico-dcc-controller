#ifndef _DCC_DELEGATE_H
#define _DCC_DELEGATE_H


#include <DCCEXProtocol.h>
#include <stdio.h>

class DCCEXProtocolDelegateImpl : public DCCExController::DCCEXProtocolDelegate {
public:
    DCCEXProtocolDelegateImpl() {}
    virtual ~DCCEXProtocolDelegateImpl(){}

    void receivedServerVersion(int major, int minor, int patch) override;

    void receivedMessage(char *message) override;

    void receivedRosterList() override;

    void receivedTurnoutList() override;

    void receivedRouteList() override;

    void receivedTurntableList() override;

    void receivedLocoUpdate(DCCExController::Loco *loco) override;

    void receivedLocoBroadcast(int address, int speed, DCCExController::Direction direction, int functionMap) override;

    void receivedTrackPower(DCCExController::TrackPower state) override;

    void receivedIndividualTrackPower(DCCExController::TrackPower state, int track) override;

    void receivedTrackType(char track, DCCExController::TrackManagerMode type, int address) override;

    void receivedTurnoutAction(int turnoutId, bool thrown) override;

    void receivedTurntableAction(int turntableId, int position, bool moving) override;

    void receivedReadLoco(int address) override;

    void receivedValidateCV(int cv, int value) override;

    void receivedValidateCVBit(int cv, int bit, int value) override;

    void receivedWriteLoco(int address) override;

    void receivedWriteCV(int cv, int value) override;

    void receivedScreenUpdate(int screen, int row, char *message) override;

    uint32_t millis() override;
};

#endif