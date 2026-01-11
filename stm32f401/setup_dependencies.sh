#!/bin/bash

# STM32F401 Dependencies Setup Script
# This script downloads and sets up the required libraries for the STM32F401 port

set -e

echo "================================================"
echo "STM32F401 Macropad Dependencies Setup"
echo "================================================"
echo ""

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DRIVERS_DIR="${SCRIPT_DIR}/Drivers"

# Create Drivers directory if it doesn't exist
mkdir -p "${DRIVERS_DIR}"

# 1. Download STM32CubeF4 (HAL + CMSIS)
echo "Step 1: Downloading STM32CubeF4..."
if [ -d "${DRIVERS_DIR}/STM32CubeF4" ]; then
    echo "  STM32CubeF4 already exists, skipping..."
else
    cd "${DRIVERS_DIR}"
    git clone --depth 1 https://github.com/STMicroelectronics/STM32CubeF4.git
    echo "  STM32CubeF4 downloaded successfully"
fi

# 2. Download TinyUSB
echo ""
echo "Step 2: Downloading TinyUSB..."
if [ -d "${DRIVERS_DIR}/tinyusb" ]; then
    echo "  TinyUSB already exists, skipping..."
else
    cd "${DRIVERS_DIR}"
    git clone --depth 1 https://github.com/hathach/tinyusb.git
    cd tinyusb
    git submodule update --init
    echo "  TinyUSB downloaded successfully"
fi

# 3. Create symbolic links for easier access
echo ""
echo "Step 3: Creating symbolic links..."
cd "${DRIVERS_DIR}"

# Link HAL driver
if [ ! -L "STM32F4xx_HAL_Driver" ] && [ ! -d "STM32F4xx_HAL_Driver" ]; then
    ln -s STM32CubeF4/Drivers/STM32F4xx_HAL_Driver STM32F4xx_HAL_Driver
    echo "  Created link: STM32F4xx_HAL_Driver"
fi

# Link CMSIS
if [ ! -L "CMSIS" ] && [ ! -d "CMSIS" ]; then
    ln -s STM32CubeF4/Drivers/CMSIS CMSIS
    echo "  Created link: CMSIS"
fi

# 4. Copy system and startup files
echo ""
echo "Step 4: Copying system files..."
SRC_DIR="${SCRIPT_DIR}/Src"
mkdir -p "${SRC_DIR}"

# Copy system_stm32f4xx.c if not exists
if [ ! -f "${SRC_DIR}/system_stm32f4xx.c" ]; then
    cp "${DRIVERS_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c" "${SRC_DIR}/"
    echo "  Copied system_stm32f4xx.c"
fi

# Copy startup file for GCC if not exists
if [ ! -f "${SRC_DIR}/startup_stm32f401xc.s" ]; then
    cp "${DRIVERS_DIR}/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xc.s" "${SRC_DIR}/"
    echo "  Copied startup_stm32f401xc.s"
fi

# 5. Copy linker script template
echo ""
echo "Step 5: Setting up linker script..."
if [ ! -f "${SCRIPT_DIR}/STM32F401CCUx_FLASH.ld" ]; then
    # Create a basic linker script for STM32F401CC (256KB Flash, 64KB RAM)
    cat > "${SCRIPT_DIR}/STM32F401CCUx_FLASH.ld" << 'EOF'
/* Linker script for STM32F401CCUx (256KB Flash, 64KB RAM) */

ENTRY(Reset_Handler)

_estack = 0x20010000;  /* End of RAM */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 256K
  RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM.exidx :
  {
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } >FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } >RAM

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + 0x400;  /* Heap size */
    . = . + 0x800;  /* Stack size */
    . = ALIGN(8);
  } >RAM
}
EOF
    echo "  Created STM32F401CCUx_FLASH.ld"
fi

# 6. Create stm32f4xx_hal_conf.h if not exists
echo ""
echo "Step 6: Creating HAL configuration..."
INC_DIR="${SCRIPT_DIR}/Inc"
mkdir -p "${INC_DIR}"

if [ ! -f "${INC_DIR}/stm32f4xx_hal_conf.h" ]; then
    cat > "${INC_DIR}/stm32f4xx_hal_conf.h" << 'EOF'
/* STM32F4xx HAL Configuration File */
#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Uncomment the line below to enable peripheral header file inclusion */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED

/* HSE Configuration */
#define HSE_VALUE    25000000U  /* Value of the External oscillator in Hz */
#define HSE_STARTUP_TIMEOUT    100U   /* Time out for HSE start up, in ms */

/* System Configuration */
#define VDD_VALUE                    3300U /* Value of VDD in mv */
#define TICK_INT_PRIORITY            0U    /* tick interrupt priority */
#define USE_RTOS                     0U

/* Includes */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f4xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f4xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f4xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f4xx_hal_cortex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f4xx_hal_flash.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f4xx_hal_pwr.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f4xx_hal_tim.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
EOF
    echo "  Created stm32f4xx_hal_conf.h"
fi

echo ""
echo "================================================"
echo "Setup completed successfully!"
echo "================================================"
echo ""
echo "Next steps:"
echo "1. Review and update CMakeLists.txt or Makefile with correct paths"
echo "2. Build the project:"
echo "   mkdir build && cd build"
echo "   cmake .."
echo "   make"
echo ""
echo "Or using Makefile:"
echo "   make"
echo ""
