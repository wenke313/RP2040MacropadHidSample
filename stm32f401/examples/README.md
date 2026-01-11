# Example Programs for STM32F401 Macropad

This directory contains example programs to help you test and verify your hardware setup.

## Available Examples

### 1. ws2812_test.c - LED Rainbow Test

A simple program that displays a rainbow animation on your WS2812 LEDs.

**Purpose**: Verify that your WS2812 LEDs are properly connected and working.

**What it does**:
- Displays a smooth rainbow pattern across all 12 LEDs
- Continuously cycles through all colors
- No USB or keyboard functionality (minimal dependencies)

**How to use**:
1. Replace `Src/main.c` with this file temporarily
2. Build and flash the firmware
3. Watch your LEDs display a rainbow animation

**Expected behavior**:
- All 12 LEDs should light up
- Colors should smoothly transition
- Pattern should continuously rotate

**Troubleshooting**:
- If LEDs don't light: Check power supply and data pin connection
- If colors are wrong: Check WS2812 wiring (GRB vs RGB)
- If pattern is erratic: Check timing/clock configuration

### Future Examples

Additional examples that could be added:

1. **button_test.c** - Test all 12 buttons
2. **encoder_test.c** - Test rotary encoder
3. **usb_test.c** - Test USB connectivity
4. **simple_macropad.c** - Minimal macropad without LampArray

## Using Examples

### Method 1: Replace main.c
```bash
cd stm32f401
cp examples/ws2812_test.c Src/main_backup.c  # Backup original
cp examples/ws2812_test.c Src/main.c
make
make flash
```

### Method 2: Modify build system
Edit `CMakeLists.txt` or `Makefile` to change the main source file.

## Contributing Examples

Feel free to add more examples! Good examples should:
- Test a specific feature
- Be well-commented
- Include expected behavior
- Provide troubleshooting tips
