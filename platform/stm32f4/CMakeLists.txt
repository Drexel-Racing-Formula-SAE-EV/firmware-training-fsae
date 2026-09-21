set(CUBE_ROOT "${CMAKE_SOURCE_DIR}/third_party/STM32CubeF4")
if(NOT EXISTS "${CUBE_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/stm32f407xx.h")
  message(FATAL_ERROR
    "STM32CubeF4 is missing. Run: git submodule update --init --recursive")
endif()

set(STM32_CPU_FLAGS
  -mcpu=cortex-m4 -mthumb
  -mfpu=fpv4-sp-d16 -mfloat-abi=hard
  CACHE INTERNAL "STM32F407 compiler and linker flags")

option(TRAINING_RENODE "Build with Renode-compatible simulated ADC input" ON)

set(HAL_DIR "${CUBE_ROOT}/Drivers/STM32F4xx_HAL_Driver")
set(CMSIS_DIR "${CUBE_ROOT}/Drivers/CMSIS")
set(STM32_STARTUP_FILE
  "${CMSIS_DIR}/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s"
  CACHE INTERNAL "STM32F407 startup source")

add_library(stm32f4_platform STATIC
  src/platform.c
  src/console.c
  src/board_io.c
  src/timebase.c
  src/adc_input.c
  "${CMSIS_DIR}/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c"
  "${HAL_DIR}/Src/stm32f4xx_hal.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_cortex.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_dma.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_dma_ex.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_gpio.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_flash.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_flash_ex.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_pwr.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_pwr_ex.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_rcc.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_rcc_ex.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_uart.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_adc.c"
  "${HAL_DIR}/Src/stm32f4xx_hal_adc_ex.c")

target_include_directories(stm32f4_platform PUBLIC
  include
  "${HAL_DIR}/Inc"
  "${CMSIS_DIR}/Include"
  "${CMSIS_DIR}/Device/ST/STM32F4xx/Include")

target_compile_definitions(stm32f4_platform PUBLIC
  STM32F407xx USE_HAL_DRIVER
  TRAINING_RENODE=$<IF:$<BOOL:${TRAINING_RENODE}>,1,0>)

target_compile_options(stm32f4_platform PUBLIC
  ${STM32_CPU_FLAGS}
  -ffunction-sections -fdata-sections
  -Wall -Wextra -Wpedantic
  $<$<CONFIG:Debug>:-Og;-g3>)

function(add_training_firmware target)
  add_executable(${target}
    ${ARGN}
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/stm32f4xx_it.c"
    "${STM32_STARTUP_FILE}")
  set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/linker/STM32F407VGTx_FLASH.ld")
  set_property(TARGET ${target} PROPERTY C_STANDARD 11)
  target_link_libraries(${target} PRIVATE stm32f4_platform)
  target_compile_options(${target} PRIVATE ${STM32_CPU_FLAGS})
  target_link_options(${target} PRIVATE
    ${STM32_CPU_FLAGS}
    -T${CMAKE_CURRENT_FUNCTION_LIST_DIR}/linker/STM32F407VGTx_FLASH.ld
    --specs=nano.specs --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${target}.map
    -Wl,--print-memory-usage)
  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME ${target}
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}>
            ${CMAKE_CURRENT_BINARY_DIR}/${target}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}>
            ${CMAKE_CURRENT_BINARY_DIR}/${target}.bin
    VERBATIM)
endfunction()
