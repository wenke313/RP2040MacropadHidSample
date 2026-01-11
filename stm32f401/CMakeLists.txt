cmake_minimum_required(VERSION 3.13)

# Set target processor
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Project name
project(macropad_stm32 C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# MCU specific settings
set(MCU_FAMILY STM32F4xx)
set(MCU_MODEL STM32F401xC)
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_PARAMETERS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F401xC -DUSE_HAL_DRIVER")

# Debug/Release flags
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CPU_PARAMETERS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")

# Note: You need to provide the STM32 HAL library and linker script
# This is a minimal example that assumes you have:
# - STM32F4 HAL drivers in Drivers/STM32F4xx_HAL_Driver
# - CMSIS in Drivers/CMSIS
# - TinyUSB library
# - Linker script: STM32F401CCUx_FLASH.ld

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Lighting
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Keys
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Encoder
    # Add your HAL and CMSIS include paths here
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    # Add TinyUSB include paths
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/tinyusb/src
)

# Source files
set(SOURCES
    # Main application
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/neopixel_stm32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ws2812_stm32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/keys_stm32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/encoder_stm32.c
    
    # Original source files (shared)
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/usb_descriptors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Lighting/LampArray.c
    
    # Add your HAL source files here
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    
    # System and startup files
    # ${CMAKE_CURRENT_SOURCE_DIR}/Src/system_stm32f4xx.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/Src/startup_stm32f401xc.s
    
    # Add TinyUSB source files
    # See tinyusb documentation for required files
)

# Create executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Link with math library
target_link_libraries(${PROJECT_NAME}.elf m)

# Linker script
# set_target_properties(${PROJECT_NAME}.elf PROPERTIES
#     LINK_FLAGS "-T${CMAKE_CURRENT_SOURCE_DIR}/STM32F401CCUx_FLASH.ld"
# )

# Post-build commands to generate .bin and .hex files
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Building ${PROJECT_NAME}.hex and ${PROJECT_NAME}.bin"
)
