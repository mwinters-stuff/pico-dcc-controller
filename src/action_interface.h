#ifndef _ACTION_INTERFACE_H
#define _ACTION_INTERFACE_H

#include <memory>
#include <string>
#include "muiplusplus.hpp"


class ActionInterface  {

 public:
  ActionInterface(){};
  virtual ~ActionInterface(){};

  virtual bool doAction(mui_event event) = 0;
  virtual bool doLongPressAction() { return true; };
  virtual bool doMoveLeftAction() { return true; };
  virtual bool doMoveRightAction() { return true; };
  virtual bool doKeyAction(int8_t action) { return true; };
  virtual void doPotAction(uint16_t value) {};
  virtual void doButtonAction(uint8_t action, uint8_t value) {};
  virtual void loop() {};

};

#endif