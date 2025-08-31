#ifndef _STORAGE_INTERFACE_H
#define _STORAGE_INTERFACE_H

#include <cstdint>
#include <vector>
#include <DCCEXLoco.h>


class StorageInterface {
  public:
    virtual ~StorageInterface() {}
    virtual bool init() = 0;
    virtual bool read(uint16_t address, uint8_t *data, size_t length) = 0;
    virtual bool write(uint16_t address, const uint8_t *data, size_t length) = 0;

    virtual bool readLocoButton(std::vector<DCCExController::Loco* > locos);
    virtual bool writeLocoButton(std::vector<DCCExController::Loco* > locos);
};

#endif