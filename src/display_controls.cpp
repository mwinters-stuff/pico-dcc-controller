#include "display_controls.h"

#include <u8g2.h>

#include <chrono>

#include "../config.h"
#include "defines.h"
#include "hardware/i2c.h"
#include "main_core1.h"
#include "u8g2functions.h"
#include "main_menu.h"
#include "dcc_connection_menu.h"
#include "show_roster.h"
#include "show_turnouts.h"
#include "dcc_menu.h"
#include "cab_control.h"

#define MAIN_MENU_FONT u8g2_font_bauhaus2015_tr
#define SMALL_TEXT_FONT u8g2_font_glasstown_nbp_t_all


// Ensure DisplayControls inherits from enable_shared_from_this in the header file

DisplayControls::DisplayControls() {
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c,
                                         u8x8_gpio_and_delay_hw_i2c);
};

DisplayControls::~DisplayControls() {

};

void DisplayControls::begin() {
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_SendBuffer(&u8g2);
}

void DisplayControls::drawScreen() {
  if (!redrawDisplay || !currentMenu) return;

  u8g2_ClearBuffer(&u8g2);
  // printf("Render menu:%lu ms\n", to_ms_since_boot(get_absolute_time()));
  currentMenu->render();
  u8g2_SendBuffer(&u8g2);
  redrawDisplay = false;
}

void DisplayControls::exitMenu() {
  currentMenu->clearAction();
  // printf("menu object destroyed\n");
  endCurrentScreen = true;
}

void DisplayControls::loop() {
  drawScreen();
  if(currentMenu) {
    currentMenu->loop();
  }

  input_type input_value;
  if (getFromQueue(input_value)) {
    // Update last activity time
    last_activity_time = get_absolute_time();

    // Wake up the display if it is sleeping
    if (is_display_sleeping) {
      printf("Waking up the display\n");
      SetPowerSave(false);
      is_display_sleeping = false;
    }
    if (input_value.input_source == INPUT_ROTARY) {
      rotateAction(input_value.value);
      switch (input_value.value) {
        case ROTARY_LEFT:
          printf("Rotary left\n");
          currentMenu->muiEvent(mui_event(mui_event_t::moveDown));
          redrawDisplay = true;
          break;
        case ROTARY_RIGHT:
          printf("Rotary right\n");
          currentMenu->muiEvent(mui_event(mui_event_t::moveUp));
          redrawDisplay = true;
          break;
        case ROTARY_PRESS: {
          printf("Rotary press\n");
          auto e = currentMenu->muiEvent(mui_event(mui_event_t::enter));
          // check if menu has quit in responce to button event,
          // if quit, then I'll destroy menu object
          if (e.eid == mui_event_t::quitMenu) {
            exitMenu();
          } else {
            if (!currentMenu->doAction()) {
              exitMenu();
            }
          }
          redrawDisplay = true;
        } break;
        case ROTARY_DOUBLEPRESS:
          printf("Rotary double press\n");
          break;
        case ROTARY_LONGPRESS: {
          printf("Rotary long press\n");
          auto e = currentMenu->muiEvent(mui_event(mui_event_t::escape));

          // Now I need to check if I received a reply with 'quitMenu' event
          // back from menu object if that is so then I need to switch to Main
          // Work Screen since menu has exited, long press will always quit
          // Menu to main screen
          if (e.eid == mui_event_t::quitMenu) {
            // release our menu object - i.e. destruct it, releasing all
            // memory
            if (currentMenu) {
              exitMenu();
            }
            // set flag to indicate we are no longer in menu
            printf("menu object destroyed\n");
          }
          redrawDisplay = true;
        } break;
        default:
          printf("Unknown rotary event %d\n", input_value.value);
          break;
      }
    } else if (input_value.input_source == INPUT_KEYPAD) {
      printf("Key pressed: %c\n", input_value.value);
      keyAction(input_value.value);
    } else if(input_value.input_source == INPUT_POT){
      potAction(input_value.value);
    } else if(input_value.input_source == INPUT_BUTTON_REVERSE){
      buttonAction(input_value.input_source, input_value.value);
    } else {
      printf("Unknown input source: %d\n", input_value.input_source);
    }
  }

  if (endCurrentScreen) {
    endCurrentScreen = false;
    if(currentMenu && currentMenu->backMenu() != MenuList::NONE) {
      printf("EndCurrentScreen Exiting menu: %s\n", currentMenu->getName().c_str());
      auto nextMenu = currentMenu->backMenu();
      currentMenu.reset();
      
      switch(nextMenu){
        case MenuList::MAIN_MENU:
          currentMenu = std::make_shared<MainMenu>(shared_from_this());
          break;
        case MenuList::DCC_CONNECTION:
          currentMenu = std::make_shared<DCCConnectionMenu>(shared_from_this());
          break;
        case MenuList::SHOW_ROSTER:
          currentMenu = std::make_shared<ShowRosterMenu>(shared_from_this());
          break;
        case MenuList::SHOW_TURNOUTS:
          currentMenu = std::make_shared<ShowTurnoutsMenu>(shared_from_this());
          break;
        case MenuList::DCC_MENU:
          currentMenu = std::make_shared<DCCMenu>(shared_from_this());
          break;
        case MenuList::CAB_CONTROL:
          currentMenu = std::make_shared<CabControl>(shared_from_this());
          break;
        default:
          printf("EndCurrentScreen Unknown menu\n");
          break;
      }
      if (currentMenu) {
        printf("EndCurrentScreen Entering menu: %s\n", currentMenu->getName().c_str());
        currentMenu->showMenu();
      } else {
        printf("EndCurrentScreen Menu not found\n");
      }

    }
    redrawDisplay = true;
  }

  // Check if 1 minute has passed since the last activity
  if (!is_display_sleeping &&
      absolute_time_diff_us(last_activity_time, get_absolute_time()) >
          getDisplaySleepMicros()) {
    printf("Putting the display to sleep\n");
    is_display_sleeping = true;
    SetPowerSave(true);
  }
}

uint32_t DisplayControls::getDisplaySleepMicros() {
  static uint32_t displaySleepMs = DISPLAY_SLEEP * 60 * 1000 * 1000;
  return displaySleepMs;
}

void DisplayControls::rotateAction(uint8_t action) {
  switch (action) {
    case ROTARY_LEFT:
      break;
    case ROTARY_RIGHT:
      currentMenu->muiEvent(mui_event(mui_event_t::moveUp));
      break;
  }
  redrawDisplay = true;
}

void DisplayControls::keyAction(uint8_t action) {
  currentMenu->doKeyAction(action);
  redrawDisplay = true;
}

void DisplayControls::potAction(uint16_t value) {
  currentMenu->doPotAction(value);
  redrawDisplay = true;
}

void DisplayControls::buttonAction(uint8_t action, uint8_t value) {
  currentMenu->doButtonAction(action, value);
  redrawDisplay = true;
}



void DisplayControls::showScreen(
    std::shared_ptr<MuiMenu> menu,
    BuildPassEntryFunction buildPassEntryFunction) {
  printf("showScreen: %s\n", menu->getName().c_str());

  currentMenu = menu;
  // printf("showScreen: %s\n", currentMenu->getName().c_str());
  buildPassEntryFunction(u8g2);
  redrawDisplay = true;
}

void DisplayControls::showScreen2(
    BuildPassEntryFunction buildPassEntryFunction) {
  buildPassEntryFunction(u8g2);
  redrawDisplay = true;
}

void DisplayControls::setRedraw() {
  // printf("setRedraw\n");
  redrawDisplay = true;
}

void DisplayControls::SetPowerSave(bool on) { u8g2_SetPowerSave(&u8g2, on); }

void DisplayControls::showBasicMiddleLine(const char* midLine) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, MAIN_MENU_FONT);
  u8g2_DrawStr(
      &u8g2,
      (u8g2_GetDisplayWidth(&u8g2) - u8g2_GetStrWidth(&u8g2, midLine)) / 2,
      u8g2_GetDisplayHeight(&u8g2) / 2, midLine);
  u8g2_SendBuffer(&u8g2);
}

void DisplayControls::showBasicTwoLine(const char* topLine,
                                       const char* bottomLine) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, MAIN_MENU_FONT);

  // Calculate display dimensions and line positions
  int lineHeight = u8g2_GetMaxCharHeight(&u8g2);
  int displayHeight = u8g2_GetDisplayHeight(&u8g2);
  int displayWidth = u8g2_GetDisplayWidth(&u8g2);

  // Calculate vertical positions for the two lines
  int topLineY = (displayHeight / 2) -
                 (lineHeight / 2);  // Top line slightly above the middle
  int bottomLineY = (displayHeight / 2) +
                    (lineHeight / 2);  // Bottom line slightly below the middle

  // Draw the top line
  u8g2_DrawStr(&u8g2, (displayWidth - u8g2_GetStrWidth(&u8g2, topLine)) / 2,
               topLineY, topLine);

  // Draw the bottom line
  u8g2_DrawStr(&u8g2, (displayWidth - u8g2_GetStrWidth(&u8g2, bottomLine)) / 2,
               bottomLineY, bottomLine);

  u8g2_SendBuffer(&u8g2);
}

void DisplayControls::showBasicThreeLines(const char* firstLine,
                                          const char* secondLine,
                                          const char* thirdLine) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, MAIN_MENU_FONT);

  // Calculate the vertical positions for the three lines
  int lineHeight = u8g2_GetMaxCharHeight(&u8g2);
  int displayHeight = u8g2_GetDisplayHeight(&u8g2);
  int displayWidth = u8g2_GetDisplayWidth(&u8g2);

  int firstLineY =
      (displayHeight / 2) - lineHeight;   // First line above the middle
  int secondLineY = (displayHeight / 2);  // Second line at the middle
  int thirdLineY =
      (displayHeight / 2) + lineHeight;  // Third line below the middle

  // Draw the first line
  u8g2_DrawStr(&u8g2, (displayWidth - u8g2_GetStrWidth(&u8g2, firstLine)) / 2,
               firstLineY, firstLine);

  // Draw the second line
  u8g2_DrawStr(&u8g2, (displayWidth - u8g2_GetStrWidth(&u8g2, secondLine)) / 2,
               secondLineY, secondLine);

  // Draw the third line
  u8g2_DrawStr(&u8g2, (displayWidth - u8g2_GetStrWidth(&u8g2, thirdLine)) / 2,
               thirdLineY, thirdLine);

  u8g2_SendBuffer(&u8g2);
}

void DisplayControls::showWifiError() { showBasicMiddleLine("Wifi Error"); }

void DisplayControls::showWifiConnecting() {
  showBasicTwoLine("WiFi", "Connecting...");
}

void DisplayControls::showWifiConnectionFailed() {
  showBasicTwoLine("WiFi", "Connection Failed");
}

void DisplayControls::showWifiConnected() {
  showBasicTwoLine("WiFi", "Connected");
  sleep_ms(2000);
}

void DisplayControls::showDCCFailedConnection(const char* name) {
  showBasicThreeLines("Failed DCC", "Connection", name);
  sleep_ms(5000);
}

void DisplayControls::showConnectTo(const char* name) {
  showBasicTwoLine("Connecting To:", name);
}