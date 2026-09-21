cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi.cmake"
      CACHE FILEPATH "Arm GNU toolchain file")
endif()

project(fsae_firmware_training C ASM)

set(TRAINING_PROJECT "00" CACHE STRING "Project number: 00 through 05")
set_property(CACHE TRAINING_PROJECT PROPERTY STRINGS 00 01 02 03 04 05)

set(SELECTED_DIR "")
if(TRAINING_PROJECT STREQUAL "00")
  set(SELECTED_DIR 00_bringup)
elseif(TRAINING_PROJECT STREQUAL "01")
  set(SELECTED_DIR 01_gpio_polling)
elseif(TRAINING_PROJECT STREQUAL "02")
  set(SELECTED_DIR 02_gpio_interrupt)
elseif(TRAINING_PROJECT STREQUAL "03")
  set(SELECTED_DIR 03_timer_scheduler)
elseif(TRAINING_PROJECT STREQUAL "04")
  set(SELECTED_DIR 04_uart_cli)
elseif(TRAINING_PROJECT STREQUAL "05")
  set(SELECTED_DIR 05_adc_sensor)
else()
  message(FATAL_ERROR "TRAINING_PROJECT must be 00, 01, 02, 03, 04, or 05")
endif()

add_subdirectory(platform/stm32f4)
add_subdirectory(projects/${SELECTED_DIR})
