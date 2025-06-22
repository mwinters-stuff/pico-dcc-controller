#ifndef _LOCO_CONTROLLER_H
#define _LOCO_CONTROLLER_H
#include <memory>
#include <string>
#include <vector>
#include "action_interface.h"
#include <DCCEXLoco.h>
#include "loco_control.h"


class LocoController: public ActionInterface, public std::enable_shared_from_this<LocoController> {
  private:
    std::vector<std::shared_ptr<LocoControl>> locos {nullptr,nullptr,nullptr,nullptr};  // Vector to hold loco indices
    uint8_t current_loco_index;
    static std::shared_ptr<LocoController> instance;

    LocoController() : current_loco_index(0xff) {
      // Initialize locos with nullptr
      for (int i = 0; i < locos.size(); ++i) {
        locos[i] = std::make_shared<LocoControl>(i);
      }
    };
  public:
    virtual ~LocoController() {};

    static std::shared_ptr<LocoController> initInstance(){
      instance = std::shared_ptr<LocoController>(new LocoController());
      return instance;
    }

    static std::shared_ptr<LocoController> getInstance(){
      return LocoController::instance;
    }

    bool doAction() override;
    bool doLongPressAction() override;
    bool doMoveLeftAction() override;
    bool doMoveRightAction() override;
    bool doKeyAction(int8_t action) override;
    void doPotAction(uint16_t value) override;
    void doButtonAction(uint8_t action, uint8_t value) override;
    void loop() override;

    void setLoco(uint8_t index, DCCExController::Loco *loco);
    DCCExController::Loco *getLoco(uint8_t index);

    uint8_t getLocoButtonIndex(DCCExController::Loco *loco);
};

#endif