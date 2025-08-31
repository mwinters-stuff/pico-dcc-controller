
#ifndef _STORAGE_AT24CX_H
#define _STORAGE_AT24CX_H 
#include "storage_interface.h"
#include <sensor/AT24Cx.h>
#include <memory>

class StorageAT24Cx : public StorageInterface {
public:
    StorageAT24Cx()  {
        eeprom = std::make_shared<AT24Cx>(EEPROM_SIZE_256, PAGE_SIZE_256);
        eeprom->begin(i2c0, AT24C_ADDR); // Initialize with default I2C instance and address
    } 
    virtual ~StorageAT24Cx() {
      eeprom.reset(); // Clean up the EEPROM instance
    }
    bool init() override {
        // Initialization logic if needed
        return true;
    } 
    bool read(uint16_t address, uint8_t *data, size_t length) override {
        eeprom->readPage(address, data, length);
        return true;
    }
    bool write(uint16_t address, const uint8_t *data, size_t length) override {
        eeprom->writePage(address, const_cast<uint8_t *>(data), length);
        return true;
    }
private:
    std::shared_ptr<AT24Cx> eeprom;
};

#endif // _STORAGE_AT24CX_H