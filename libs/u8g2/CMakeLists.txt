cmake_minimum_required(VERSION 3.28)

add_compile_definitions(U8G2_USE_DYNAMIC_ALLOC)

set(COMPONENT_ADD_INCLUDEDIRS csrc)
# file(GLOB COMPONENT_SRCS csrc/*.c)
file(GLOB COMPONENT_HDRS csrc/*.h)

set(COMPONENT_SRCS
        csrc/u8x8_d_ssd1306_128x64_noname.c
#     csrc/u8x8_setup.c
    csrc/mui.c
    csrc/mui_u8g2.c
#     csrc/u8g2_d_memory.c
    csrc/u8g2_hvline.c
#     csrc/u8x8_message.c
#     csrc/u8x8_input_value.c
#     csrc/u8g2_polygon.c
    csrc/u8x8_8x8.c
#     csrc/u8x8_debounce.c
    csrc/u8g2_setup.c
    csrc/u8x8_setup.c
    csrc/u8x8_display.c
    csrc/u8g2_box.c
    csrc/u8x8_byte.c
#     csrc/u8log_u8x8.c
#     csrc/u8g2_message.c
#     csrc/u8g2_line.c
    csrc/u8g2_intersection.c
#     csrc/u8g2_fonts.c
    csrc/u8g2_cleardisplay.c
    csrc/u8g2_font.c
    csrc/u8x8_cad.c
#     csrc/u8x8_string.c
    csrc/u8g2_ll_hvline.c
#     csrc/u8x8_fonts.c
#     csrc/u8x8_d_stdio.c
    csrc/u8g2_circle.c
    csrc/u8g2_buffer.c
    csrc/u8g2_kerning.c
    csrc/u8g2_button.c
#     csrc/u8g2_bitmap.c
    csrc/u8x8_gpio.c
#     csrc/u8g2_d_setup.c
     csrc/u8x8_capture.c
     csrc/u8x8_u16toa.c
     csrc/u8x8_u8toa.c

)

if(COMMAND idf_component_register)
    idf_component_register(SRCS "${COMPONENT_SRCS}" INCLUDE_DIRS csrc)
    return()
endif()

project(u8g2)

if(COMMAND register_component)
    set(COMPONENT_NAME ".")
    register_component()
else()
    add_library(u8g2 ${COMPONENT_SRCS})
    target_include_directories(u8g2 PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/csrc>" "$<INSTALL_INTERFACE:include>")
endif()

install(TARGETS u8g2
        EXPORT u8g2-targets
        ARCHIVE DESTINATION lib)

install(FILES ${COMPONENT_HDRS}
        DESTINATION include/u8g2)

install(EXPORT u8g2-targets
        DESTINATION lib/cmake/u8g2)

install(FILES ${u8g2_SOURCE_DIR}/pkg/u8g2-config.cmake
        DESTINATION lib/cmake/u8g2)
