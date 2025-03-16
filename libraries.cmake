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


endfunction()