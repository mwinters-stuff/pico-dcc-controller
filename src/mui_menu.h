#ifndef _MUI_MENU_H
#define _MUI_MENU_H

#include <u8g2.h>
#include <memory>
#include <string>

#include "muiplusplus.hpp"
#include "action_interface.h"

class DisplayControls;

enum class MenuList {
  NONE,
  MAIN_MENU,
  DCC_CONNECTION,
  SHOW_ROSTER,
  SHOW_TURNOUTS,
  DCC_MENU,
  CAB_CONTROL,
};

class MuiMenu : public MuiPlusPlus, public ActionInterface {
  protected:
  std::shared_ptr<DisplayControls> displayControls;

 public:
  MuiMenu(std::shared_ptr<DisplayControls> displayControls): displayControls(displayControls) {};
  virtual ~MuiMenu(){};

  virtual MenuList getMenu() = 0;
  virtual MenuList backMenu() = 0;

  virtual void buildMenu(u8g2_t &u8g2) = 0;
  virtual void showMenu() = 0;
  virtual void clearAction(){};
  virtual std::string getName() { return "MuiMenu"; };

};

#endif