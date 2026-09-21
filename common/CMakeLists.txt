add_library(training_common STATIC
  src/training_can_protocol.c
  src/training_fault_manager.c
  src/training_ecu.c)
target_include_directories(training_common PUBLIC include)
target_link_libraries(training_common PUBLIC stm32f4_platform)

function(add_training_can_peer)
  if(TARGET training_can_peer)
    return()
  endif()
  add_training_firmware(training_can_peer
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/can_peer_main.c")
  target_link_libraries(training_can_peer PRIVATE training_common)
  set_target_properties(training_can_peer PROPERTIES OUTPUT_NAME "training_can_peer")
endfunction()

function(enable_training_freertos)
  if(TARGET freertos_kernel)
    return()
  endif()
  set(FREERTOS_ROOT "${CMAKE_SOURCE_DIR}/third_party/FreeRTOS-Kernel")
  if(NOT EXISTS "${FREERTOS_ROOT}/tasks.c")
    message(FATAL_ERROR
      "FreeRTOS-Kernel V11.3.0 is missing. Run: git submodule update --init --recursive")
  endif()
  execute_process(
    COMMAND git -C "${FREERTOS_ROOT}" describe --tags --exact-match
    OUTPUT_VARIABLE FREERTOS_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  if(NOT FREERTOS_TAG STREQUAL "V11.3.0")
    message(FATAL_ERROR
      "FreeRTOS-Kernel must be checked out at V11.3.0; found '${FREERTOS_TAG}'")
  endif()

  add_library(freertos_kernel STATIC
    "${FREERTOS_ROOT}/tasks.c"
    "${FREERTOS_ROOT}/queue.c"
    "${FREERTOS_ROOT}/list.c"
    "${FREERTOS_ROOT}/timers.c"
    "${FREERTOS_ROOT}/event_groups.c"
    "${FREERTOS_ROOT}/stream_buffer.c"
    "${FREERTOS_ROOT}/portable/GCC/ARM_CM4F/port.c"
    "${FREERTOS_ROOT}/portable/MemMang/heap_4.c")
  target_include_directories(freertos_kernel PUBLIC
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include"
    "${FREERTOS_ROOT}/include"
    "${FREERTOS_ROOT}/portable/GCC/ARM_CM4F")
  target_compile_options(freertos_kernel PRIVATE ${STM32_CPU_FLAGS}
    -ffunction-sections -fdata-sections -Wall -Wextra)
  target_compile_definitions(freertos_kernel PUBLIC TRAINING_FREERTOS=1)
  target_link_libraries(freertos_kernel PUBLIC stm32f4_platform)
endfunction()
