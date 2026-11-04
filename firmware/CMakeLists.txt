# CITest firmware for the STM32F767ZI, built with the same compiler options as an STM32CubeIDE
# Release configuration of the real device firmware (Cortex-M7, hard float, newlib-nano, -O1).
#
#   cmake -S firmware -B build -DCMAKE_TOOLCHAIN_FILE=firmware/cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
#   cmake --build build
cmake_minimum_required(VERSION 3.22)
project(CITest_FW C)

set(MCU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)

add_executable(${PROJECT_NAME}.elf
    Core/Src/main.c
    Core/Startup/startup_stm32f767zitx.c)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32F767xx STM32F7)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    --specs=nano.specs
    -std=gnu11 -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITX_FLASH.ld
    --specs=nosys.specs --specs=nano.specs
    -static
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--start-group -lc -lm -Wl,--end-group)

# .bin / .hex next to the .elf, and the size summary CubeIDE prints after a build.
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf)
