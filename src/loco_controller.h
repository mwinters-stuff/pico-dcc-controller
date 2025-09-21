#ifndef _LOCO_CONTROLLER_H
#define _LOCO_CONTROLLER_H
#include <memory>
#include <string>
#include <vector>
#include "action_interface.h"
#include <DCCEXLoco.h>
#include "loco_control.h"
#include "storage_at24cx.h"

class DisplayControls;

class LocoController: public ActionInterface, public std::enable_shared_from_this<LocoController> {
  private:
    std::vector<DCCExController::Loco* > locos {nullptr,nullptr,nullptr,nullptr};  // Vector to hold loco indices
    uint8_t current_loco_index;
    static std::shared_ptr<LocoController> instance;
    std::shared_ptr<LocoControl> locoControl;
    std::shared_ptr<StorageAT24Cx> storage;
    std::shared_ptr<DisplayControls> displayControls;

    protected:
    // Private constructor to prevent instantiation from outside
    // This ensures that the class can only be instantiated through the initInstance method
    // and provides a single instance of LocoController throughout the application.   
    LocoController();
    LocoController(const LocoController&) = delete;
    LocoController& operator=(const LocoController&) = delete;
    explicit LocoController(std::shared_ptr<DisplayControls> displayControls);

  public:
    virtual ~LocoController() {};

    static std::shared_ptr<LocoController> initInstance(std::shared_ptr<DisplayControls> displayControls);

    void init(std::shared_ptr<DisplayControls> displayControls);
     

    static std::shared_ptr<LocoController> getInstance(){
      return LocoController::instance;
    }

    bool doAction(mui_event event) override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;
    void doPotAction(uint16_t value) override;
    void doButtonAction(uint8_t action, uint8_t value) override;
    void loop() override;

    void loadLocosFromStorage();

    void setLoco(uint8_t index, DCCExController::Loco *loco);
    DCCExController::Loco *getLoco(uint8_t index);
    void clearLoco(DCCExController::Loco *loco);

    void driveLocoLED(uint8_t loco_led_index);
    void driveLoco(DCCExController::Loco *loco);

    uint8_t getLocoButtonIndex(DCCExController::Loco *loco);
};

#endif