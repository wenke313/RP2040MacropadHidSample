# STM32F401 vs RP2040 Implementation Differences

This document outlines the key differences between the original RP2040 implementation and the STM32F401 port.

## Architecture Overview

### RP2040 (Original)
- **Microcontroller**: Raspberry Pi RP2040 (Dual Cortex-M0+)
- **SDK**: Pico SDK
- **WS2812 Driver**: PIO (Programmable I/O) state machine
- **USB Stack**: TinyUSB (built into Pico SDK)
- **Build System**: CMake with Pico SDK integration

### STM32F401 (Port)
- **Microcontroller**: STM32F401 (Cortex-M4F)
- **HAL**: STM32 HAL (Hardware Abstraction Layer)
- **WS2812 Driver**: Timer1 PWM + DMA
- **USB Stack**: TinyUSB (standalone)
- **Build System**: CMake or Makefile

## WS2812 LED Control

### RP2040 Implementation
```c
// Uses PIO state machine
Controller.pio = NEOPIXEL_PIO;
Controller.sm = pio_claim_unused_sm(Controller.pio, true);
pio_sm_put_blocking(Controller.pio, Controller.sm, grb_value);
```

- **Hardware**: PIO peripheral (unique to RP2040)
- **File**: `src/Lighting/neopixel.pio` - Assembly code for PIO
- **Advantages**: 
  - Precise timing without CPU intervention
  - Dedicated hardware for protocol implementation
  - Very efficient

### STM32F401 Implementation
```c
// Uses Timer PWM + DMA
WS2812_Init(&htim1);
WS2812_SetPixel(index, r, g, b);
WS2812_Update();  // Triggers DMA transfer
```

- **Hardware**: TIM1 (PWM) + DMA2
- **Files**: `stm32f401/Src/ws2812_stm32.c`
- **Method**: PWM duty cycle encoding (0: ~30%, 1: ~70%)
- **Advantages**:
  - Available on any STM32 with timers
  - DMA reduces CPU load
  - Configurable for different LED counts

## GPIO Configuration

### RP2040
```c
gpio_pull_up(KEY1);
gpio_get(KEY1);
```

### STM32F401
```c
// Configuration in GPIO_Init()
GPIO_InitStruct.Pin = KEY1_PIN;
GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull = GPIO_PULLUP;
HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);

// Reading
HAL_GPIO_ReadPin(KEY_PORT, KEY1_PIN);
```

## Timer/Interrupts

### RP2040
```c
struct repeating_timer lightingTimer;
add_repeating_timer_ms(10, lightingTimerCallback, NULL, &lightingTimer);
```

### STM32F401
```c
// Timer configured for 10ms interrupts
htim2.Init.Prescaler = 4199;
htim2.Init.Period = 99;
HAL_TIM_Base_Start_IT(&htim2);

// Interrupt handler
void TIM2_IRQHandler(void) {
    lightingUpdateFlag = 1;
    keyboardUpdateFlag = 1;
}
```

## Clock Configuration

### RP2040
- Automatic clock configuration by Pico SDK
- Typically runs at 125 MHz

### STM32F401
- Manual clock configuration required
- Configured to run at 84 MHz
- Uses HSE (25MHz) + PLL
- Code in `SystemClock_Config()`

## Memory Layout

### RP2040
- 264KB SRAM
- 2MB Flash (external)
- Bootloader in ROM

### STM32F401CC
- 64KB SRAM
- 256KB Flash (internal)
- Bootloader in system memory

## Build Dependencies

### RP2040
```bash
# Pico SDK (includes everything)
git clone https://github.com/raspberrypi/pico-sdk
export PICO_SDK_PATH=/path/to/pico-sdk
```

### STM32F401
```bash
# Multiple dependencies
./setup_dependencies.sh  # Downloads:
# - STM32CubeF4 (HAL + CMSIS)
# - TinyUSB (standalone)
# - Linker scripts and startup code
```

## File Structure Comparison

### RP2040
```
src/
├── macropad.c              # Main application
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── Lighting/
│   ├── neopixel.pio        # PIO assembly
│   └── Neopixel.c
└── Keys/Keys.c
```

### STM32F401
```
stm32f401/
├── Src/
│   ├── main.c              # Main application
│   ├── ws2812_stm32.c      # WS2812 driver
│   └── neopixel_stm32.c
├── Inc/
│   ├── config.h            # Pin definitions
│   └── stm32f4xx_hal_conf.h
├── CMakeLists.txt
├── Makefile
└── setup_dependencies.sh
```

## Flashing/Programming

### RP2040
1. Hold BOOTSEL button
2. Connect USB
3. Copy .uf2 file to mounted drive
4. Auto-reboot

### STM32F401
1. Connect ST-Link debugger
2. Use st-flash or OpenOCD:
```bash
st-flash write firmware.bin 0x08000000
```

## Code Reuse

The following components are **shared** between both implementations:
- `src/usb_descriptors.c` - USB HID descriptors
- `src/Lighting/LampArray.c` - LampArray HID logic
- `src/Lighting/LampArrayHidStructs.h` - Data structures
- `src/Keys/keymap.h` - Key mapping definitions

The following components are **platform-specific**:
- WS2812 driver implementation
- GPIO and timer configuration
- Main application initialization
- Build system configuration

## Migration Notes

If you want to port code from RP2040 to STM32F401:

1. **GPIO**: Replace `gpio_*` with `HAL_GPIO_*`
2. **Timers**: Replace Pico timer API with HAL timer API
3. **WS2812**: Use PWM+DMA instead of PIO
4. **Clocks**: Add explicit clock configuration
5. **Interrupts**: Use HAL interrupt handlers
6. **Build**: Update CMakeLists.txt with HAL sources

## Performance Comparison

| Feature | RP2040 | STM32F401 |
|---------|--------|-----------|
| CPU Speed | 125 MHz | 84 MHz |
| WS2812 Method | PIO (dedicated) | PWM+DMA |
| CPU Load (LEDs) | Very Low | Low |
| USB Speed | Full Speed | Full Speed |
| Key Scan Rate | 10ms | 10ms |
| LED Update Rate | 10ms | 10ms |

## Advantages of Each Platform

### RP2040 Advantages
- Simpler setup (Pico SDK includes everything)
- PIO makes WS2812 control very easy
- More SRAM (264KB)
- UF2 bootloader (no debugger needed)

### STM32F401 Advantages
- More common/available platform
- Cortex-M4F with FPU
- Lower cost in volume
- Extensive STM32 ecosystem
- Many board options (Black Pill, etc.)

## Conclusion

Both implementations provide the same functionality:
- ✅ USB HID keyboard
- ✅ LampArray HID support
- ✅ WS2812 RGB control
- ✅ Rotary encoder
- ✅ 12 programmable keys

Choose based on:
- **RP2040**: If you want easiest development and have Adafruit Macropad
- **STM32F401**: If you want to use STM32 or build custom hardware
