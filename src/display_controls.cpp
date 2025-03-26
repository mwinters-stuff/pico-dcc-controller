#include "display_controls.h"

#include <u8g2.h>

#include "defines.h"
#include "u8g2functions.h"

#define MAIN_MENU_FONT u8g2_font_bauhaus2015_tr
#define SMALL_TEXT_FONT u8g2_font_glasstown_nbp_t_all

DisplayControls::DisplayControls() {
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c,
                                         u8x8_gpio_and_delay_hw_i2c);
};

DisplayControls::~DisplayControls() {};

void DisplayControls::begin() {
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_SendBuffer(&u8g2);
}

void DisplayControls::drawScreen() {
  if (!redrawDisplay || !currentMenu) return;

  u8g2_ClearBuffer(&u8g2);
  printf("Render menu:%lu ms\n", to_ms_since_boot(get_absolute_time()));
  currentMenu->render();
  u8g2_SendBuffer(&u8g2);
  redrawDisplay = false;
}

void DisplayControls::rotateAction(uint8_t action) {
  if (!currentMenu) return;

  switch (action) {
    case ROTARY_LEFT:
      currentMenu->muiEvent(mui_event(mui_event_t::moveDown));
      break;
    case ROTARY_RIGHT:
      currentMenu->muiEvent(mui_event(mui_event_t::moveUp));
      break;
    case ROTARY_PRESS:
      auto e = currentMenu->muiEvent(mui_event(mui_event_t::enter));
      // check if menu has quit in responce to button event,
      // if quit, then I'll destroy menu object
      if (e.eid == mui_event_t::quitMenu) {
        // _menu.release();
        printf("menu object destroyed\n");
      }
      break;
  }
  redrawDisplay = true;
}

void DisplayControls::keyAction(uint8_t action) {
  
}
  

void DisplayControls::showScreen(std::shared_ptr<MuiMenu> menu, BuildPassEntryFunction buildPassEntryFunction) {
  currentMenu = menu;
  buildPassEntryFunction(u8g2, this);
  redrawDisplay = true;
}

void DisplayControls::showScreen(BuildPassEntryFunction buildPassEntryFunction) {
  buildPassEntryFunction(u8g2, this);
  redrawDisplay = true;
}

void DisplayControls::setRedraw() {
  printf("setRedraw\n");
  redrawDisplay = true;
}

void DisplayControls::SetPowerSave(bool on) {
  u8g2_SetPowerSave(&u8g2, on);
}
