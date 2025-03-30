#ifndef _MUI_MENU_H
#define _MUI_MENU_H

#include <u8g2.h>
#include <memory>

#include "muiplusplus.hpp"

class DisplayControls;

class MuiMenu : public MuiPlusPlus {
  protected:
  std::shared_ptr<DisplayControls> displayControls;
  std::shared_ptr<MuiMenu> sharedThis;

 public:
  MuiMenu(std::shared_ptr<DisplayControls> displayControls): displayControls(displayControls), sharedThis(this) {};
  virtual ~MuiMenu(){};

  virtual void buildMenu(u8g2_t &u8g2) = 0;
  virtual void clearAction() = 0;
  virtual void doAction() = 0;
};

#endif