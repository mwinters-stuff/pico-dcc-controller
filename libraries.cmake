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
  OVERRIDE_FIND_PACKAGE

)

# Override the default build configuration
FetchContent_GetProperties(u8g2)
if(NOT u8g2_POPULATED)
  # Copy your custom CMakeLists.txt to a temporary location
  configure_file(${CMAKE_CURRENT_LIST_DIR}/libs/u8g2/CMakeLists.txt
                 ${u8g2_SOURCE_DIR}/CMakeLists.txt
                 COPYONLY)
                 
  FetchContent_MakeAvailable(u8g2)
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