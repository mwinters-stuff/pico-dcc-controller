cmake_minimum_required(VERSION 3.16)
function (includeLibraries)

include(FetchContent)

FetchContent_Declare(
  versatile_rotaryencoder
  GIT_REPOSITORY    https://github.com/mwinters-stuff/pico-versatile_rotaryencoder.git
  GIT_TAG           main
  GIT_SHALLOW       1
  SOURCE_SUBDIR     cmake
)

FetchContent_MakeAvailable(versatile_rotaryencoder)

FetchContent_Declare(
  u8g2
  GIT_REPOSITORY    https://github.com/olikraus/u8g2.git
  # GIT_REPOSITORY    git@github.com:olikraus/u8g2.git
  GIT_TAG           master
  GIT_SHALLOW       1
)

FetchContent_MakeAvailable(u8g2)

if(TARGET u8g2)
    set(U8G2_PATCHED_SRCS
        ${u8g2_SOURCE_DIR}/csrc/u8x8_d_ssd1306_128x64_noname.c
        ${u8g2_SOURCE_DIR}/csrc/mui.c
        ${u8g2_SOURCE_DIR}/csrc/mui_u8g2.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_hvline.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_8x8.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_setup.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_setup.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_display.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_box.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_byte.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_intersection.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_cleardisplay.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_font.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_cad.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_ll_hvline.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_circle.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_buffer.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_kerning.c
        ${u8g2_SOURCE_DIR}/csrc/u8g2_button.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_gpio.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_capture.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_u16toa.c
        ${u8g2_SOURCE_DIR}/csrc/u8x8_u8toa.c
    )
    set_target_properties(u8g2 PROPERTIES SOURCES "${U8G2_PATCHED_SRCS}")
endif()

FetchContent_Declare(
  MuiPlusPlus
  GIT_REPOSITORY    https://github.com/mwinters-stuff/pico-MuiPlusPlus.git
  GIT_TAG           main
  GIT_SHALLOW       1
  SOURCE_SUBDIR     cmake
)

FetchContent_MakeAvailable(MuiPlusPlus)

FetchContent_Declare(
  DCCEXProtocol
  GIT_REPOSITORY    https://github.com/mwinters-stuff/DCCEXProtocol.git
  GIT_TAG           main
  GIT_SHALLOW       1
  # SOURCE_SUBDIR     cmake
)

FetchContent_MakeAvailable(DCCEXProtocol)



endfunction()