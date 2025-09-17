#ifndef _CUSTOM_LIST_H
#define _CUSTOM_LIST_H
#include <muipp_u8g2.hpp>
#include <functional>
#include <string>
#include <vector>


class Custom_U8g2_DynamicScrollList : public Item_U8g2_Generic, public MuiItem {
  stringbyindex_cb_t _cb;
  size_cb_t _size_cb;
  index_cb_t _action;
  index_cb_t _select_cb;
  int _y_shift, _num_of_rows;
  const uint8_t *_font2;
  // current list index
  int _index{0};

public:
  /**
   * @brief Construct a Custom_U8g2_DynamicScrollList object
   * 
   * @param u8g2 reference to display object
   * @param id assigned id for the item
   * @param label_cb function that returns const char* for the specified label index
   * @param action_cb function called with index of an active element in a list when "enter" event is received
   * @param count total number of lables in a list
   * @param select_cb function called with index of an active element in a list when selection changes
   * @param y_shift pexels to shift on Y axis for every new row
   * @param num_of_rows max rows to display at once
   * @param font1 font to use for highlighted row, do not change if null
   * @param font2 font to use for other rows, same as font1 if null
   * @param x, y Coordinates of the top left corner to start printing
   */
  Custom_U8g2_DynamicScrollList(u8g2_t &u8g2,
      muiItemId id,
      stringbyindex_cb_t label_cb,
      size_cb_t count,
      index_cb_t action_cb,
      index_cb_t select_cb,
      int y_shift,
      int num_of_rows = 3,
      u8g2_uint_t x = 0, u8g2_uint_t y = 0,
      const uint8_t* font1 = nullptr,
      const uint8_t* font2 = nullptr
  )
    : Item_U8g2_Generic(u8g2, font1, x, y),
      MuiItem(id, nullptr), _cb(label_cb), _size_cb(count), _action(action_cb),_select_cb(select_cb), _y_shift(y_shift), _num_of_rows(num_of_rows), _font2(font2) {};

  // list options
  dynlist_options_t listopts;

  // event handler
  mui_event muiEvent(mui_event e) override;

  void render(const MuiItem* parent) override;
};

class Custom_U8g2_ValuesList : public Item_U8g2_Generic, public MuiItem {
  // callbacks
  string_cb_t _getCurrent;
  // cursor x position for value 
  u8g2_uint_t _xval;
  // value horizontal alignment
  text_align_t _val_halign;

public:

  Custom_U8g2_ValuesList(
    u8g2_t &u8g2,
    muiItemId id,
    const char* label,
    string_cb_t getCurrent,
    u8g2_uint_t xlbl, u8g2_uint_t xval,
    u8g2_uint_t y,
    const uint8_t* font = nullptr,
    text_align_t lbl = text_align_t::left,
    text_align_t val_halign = text_align_t::left,
    text_align_t valign = text_align_t::baseline
  ) : Item_U8g2_Generic(u8g2, font, xlbl, y, lbl, valign),
      MuiItem(id, label, {false,true}), _getCurrent(getCurrent), _xval(xval), _val_halign(val_halign) {}

  // render method
  void render(const MuiItem* parent) override;
  // event handler
  mui_event muiEvent(mui_event e) override;
};

#endif