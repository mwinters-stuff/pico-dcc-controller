#ifndef _MUI_MENU_H
#define _MUI_MENU_H

#include <u8g2.h>

#include "muiplusplus.hpp"

class DisplayControls;

class MuiMenu : public MuiPlusPlus {
 public:
  virtual ~MuiMenu() {};

  virtual void buildMenu(u8g2_t &u8g2, DisplayControls *displayControls) = 0;
};

#endif