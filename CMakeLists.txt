# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Disable optimization for debug builds
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-dcc-controller C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include(libraries.cmake)
includeLibraries()
# Add executable. Default name is the project name, version 0.1

file(GLOB_RECURSE SRC 
    src/*.cpp
    libs/u8g2pico/*.cpp
    libs/fonts/*.c
)

add_executable(pico-dcc-controller ${SRC})

pico_set_program_name(pico-dcc-controller "pico-dcc-controller")
pico_set_program_version(pico-dcc-controller "0.1")

# Generate PIO header
# pico_generate_pio_header(pico-dcc-controller ${CMAKE_CURRENT_LIST_DIR}/src/keypad_scan.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-dcc-controller 1)
pico_enable_stdio_usb(pico-dcc-controller 0)

# Add the standard library to the build
target_link_libraries(pico-dcc-controller
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico-dcc-controller PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        libs/u8g2pico
)

# Add any user requested libraries
target_link_libraries(pico-dcc-controller 
        hardware_i2c
        # hardware_dma
        hardware_pio
        # hardware_interp
        hardware_timer
        # hardware_watchdog
        # hardware_clocks
        pico_multicore
        pico_cyw43_arch_lwip_poll
        Versatile_RotaryEncoder
        u8g2
        MuiPlusPlus
        DCCEXProtocol
        )

pico_add_extra_outputs(pico-dcc-controller)

