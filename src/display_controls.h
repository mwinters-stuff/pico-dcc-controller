#ifndef _DISPLAY_CONTROLS_H
#define _DISPLAY_CONTROLS_H

#include <memory>
#include <muipp_u8g2.hpp>
#include <functional>

#include "mui_menu.h"


/*
    this class controls our buttons and
    switch display information between some regural jobs and Configuration menu.
    It's nearly same funtionality as in previous example, just wrapped under
   class object

*/
class DisplayControls {
  u8g2_t u8g2;
  
  using BuildPassEntryFunction = std::function<void(u8g2_t &, DisplayControls *)>;
  
  // screen refresh required flag
  volatile bool redrawDisplay{true};

  // a placeholder for our menu object, initially empty
  std::shared_ptr<MuiMenu> currentMenu;

  /**
   * @brief this method will dispatch events from OK button and virtual encoder
   * and forward it to the respective handlers. The thing is that buttons could
   * behave differently depending on which derived menu class is loaded.
   * Won't need it in this example, but maybe used later on
   *
   * @param arg ESP Evenloop argument
   * @param base ESP Event loop base
   * @param id event id
   * @param event_data event data pointer
   */
  // void _event_picker(void* arg, esp_event_base_t base, int32_t id, void*
  // event_data);

  /**
   * @brief button events handler
   *
   * @param e
   * @param m
   */
  // void _evt_button(ESPButton::event_t e, const EventMsg* m);
  // // encoder events handler
  // void _evt_encoder(ESPButton::event_t e, const EventMsg* m);

  /**
   * @brief this method I'll call when my "enter" button is pressed and I need
   * to pass "enter" event to menu and receive reply event from menu to
   * uderstand when mune has exit
   *
   */
  void menu_ok_action();

  /**
   * @brief this method I'll call when my "+/-" buttons are pressed and I need
   * to pass "cursor" event to menu and receive reply event from menu to
   * uderstand when mune has exit
   *
   */
  void menu_encoder_action(int8_t rotation);

  // Rotary encoder callbacks
  void handleRotate(int8_t rotation);
  void handlePressRelease();
  void handleLongPressRelease();

 public:
  // constructor
  DisplayControls();

  // destructor
  ~DisplayControls();

  // start our display control
  void begin();

  // draw something on screen, either some sily stub text, or render menu
  void drawScreen();

  void rotateAction(uint8_t action);
  void keyAction(uint8_t action);


  void showScreen(std::shared_ptr<MuiMenu> menu, BuildPassEntryFunction buildPassEntryFunction);
  void showScreen(BuildPassEntryFunction buildPassEntryFunction);

  void setRedraw();
  void SetPowerSave(bool on);
};

#endif