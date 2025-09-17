#include "mui_menu_custom_items.h"




void Custom_U8g2_DynamicScrollList::render(const MuiItem* parent){
  // printf("DynScrl print lst of %u items\n", _size_cb());

  setCursor(_x, _y);
  // specified cursor position will be the top reference for the Title text
  u8g2_SetFontPosTop(&_u8g2);

  // normalize index position which items should be displayed
  int visible_idx = muipp::clamp(_index - _num_of_rows/2, 0, static_cast<int>(_size_cb()-1) );
  for (int i = 0; i != _num_of_rows; ++i){
    // change font for active/inactive row
    if (visible_idx == _index && _font)
      u8g2_SetFont(&_u8g2, _font);
    else if (_font2)
      u8g2_SetFont(&_u8g2, _font2);

    // draw button
    // printf("draw Dynlist_btn %u:%s\n", visible_idx, _cb(visible_idx));
    u8g2_DrawButtonUTF8(&_u8g2, _x, _y + _y_shift*i, visible_idx == _index ? U8G2_BTN_INV : 0, 0, 1, 1, _cb(visible_idx));

    if (_select_cb)
        _select_cb(_index);  // call select callback if it was assigned

    if (++visible_idx >= _size_cb())
      return;
  }
}


mui_event Custom_U8g2_DynamicScrollList::muiEvent(mui_event e){
  // printf("DynamicScrollList::muiEvent %u\n", static_cast<uint32_t>(e.eid));
  switch(e.eid){
    // cursor actions - move to previous position in a list
    case mui_event_t::moveUp :
    case mui_event_t::moveLeft : {
      if (_index)
        --_index;
      else
        _index = _size_cb() - 1;
      if (_select_cb)
        _select_cb(_index);  // call select callback if it was assigned
      break;
    }
    // cursor actions - move to next position in a list
    case mui_event_t::moveDown :
    case mui_event_t::moveRight : {
      ++_index;
      if (_index == _size_cb() )
        _index = 0;
      if (_select_cb)
        _select_cb(_index);  // call select callback if it was assigned
      break;
    }

    case mui_event_t::enter : {
      // if dynamic list works as page seletor, we will stich pages here
      if (listopts.page_selector){
        // last item in a list acts as "go to previous page"
        if (listopts.back_on_last && (_index == _size_cb() - 1))
          return mui_event(mui_event_t::prevPage);
        else  // otherwise - switch to page by it's label
          return mui_event(mui_event_t::goPageByName, 0, static_cast<void*>(const_cast<char*>(_cb(_index))) );
      }
      // otherwise we call a callback function to decide next event action
      if (_action)
        _action(_index);
      break;
    }

    // enter acts as escape to release selection
    case mui_event_t::escape :
      return mui_event(on_escape);
  }

  return {};
}



//  ***
//  MuiItem_U8g2_ValuesList

void Custom_U8g2_ValuesList::render(const MuiItem* parent){
  if (_font)
    u8g2_SetFont(&_u8g2, _font);

  // draw label
  auto a = getXoffset(name);
  u8g2_DrawUTF8(&_u8g2, a, _y, name);

  // value must be printed right after end of label
  if (_x == _xval)
    _xval = getX();

  // draw button - https://github.com/olikraus/u8g2/wiki/u8g2reference#drawbuttonutf8
  u8g2_uint_t flags = 0;
  if (selected)
    flags |= U8G2_BTN_INV;
  else if (focused)
    flags |= U8G2_BTN_BW1;

  //if (_val_align == text_align_t::center)
  //  flags |= U8G2_BTN_HCENTER;

  // calc cursor's offset for value
  const char* s = _getCurrent();
  auto vxoff = getXoffset(_xval, _val_halign, v_align, s);
  // draw button with frame or inversion
  u8g2_DrawButtonUTF8(&_u8g2, vxoff, _y, flags, 0, 2, 2, s);
}

mui_event Custom_U8g2_ValuesList::muiEvent(mui_event e){
  // switch(e.eid){
  //   // cursor actions - decr value
  //   case mui_event_t::moveUp :
  //   case mui_event_t::moveLeft : 
  //     _onPrev();
  //     break;
  //   // cursor actions - incr value
  //   case mui_event_t::moveDown :
  //   case mui_event_t::moveRight :
  //     _onNext();
  //     break;

  //   // enter acts as escape to release selection
  //   case mui_event_t::enter :
  //     return mui_event(on_escape);
  // }

  // by default, return noop
  return {};
}
