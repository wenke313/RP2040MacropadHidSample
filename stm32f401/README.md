# STM32F401 + WS2812 Port

This directory contains the STM32F401 port of the RP2040 Macropad HID Sample.

## Hardware Requirements

- STM32F401 microcontroller (e.g., STM32F401CCU6 "Black Pill" board)
- WS2812B LED strip (12 LEDs recommended)
- 12 push buttons (with pull-up resistors)
- Rotary encoder
- USB connection

## Pin Configuration

### Default Pin Mapping:
- **WS2812 Data Pin**: PA8 (TIM1_CH1 for PWM)
- **Keys**: PB0-PB11 (12 keys)
- **Rotary Encoder A**: PA0
- **Rotary Encoder B**: PA1
- **Rotary Encoder Button**: PA2
- **USB**: PA11 (USB_DM), PA12 (USB_DP)

You can modify these pin definitions in `Inc/config.h`.

## Build Instructions

### Prerequisites

1. Install ARM GCC toolchain:
```bash
sudo apt-get install gcc-arm-none-eabi
```

2. Install CMake (>= 3.13):
```bash
sudo apt-get install cmake
```

3. Install STM32CubeMX (optional, for regenerating initialization code)

### Building the Project

1. Clone this repository and navigate to the `stm32f401` directory:
```bash
cd stm32f401
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure and build:
```bash
cmake ..
make
```

4. Flash the firmware:
```bash
# Using ST-Link
st-flash write macropad_stm32.bin 0x08000000

# Or using OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program macropad_stm32.elf verify reset exit"
```

## Features

- USB HID Keyboard functionality
- LampArray HID support for RGB control
- 12 programmable keys
- 2 layers controlled by rotary encoder
- WS2812B RGB LED control
- Windows Dynamic Lighting support

## WS2812 Implementation

The WS2812 driver uses Timer1 and DMA for efficient bit-banging:
- Timer1 generates PWM signals with precise timing
- DMA transfers color data without CPU intervention
- Supports up to 256 LEDs (configurable)

## USB Configuration

This port uses TinyUSB library configured for STM32F401:
- Full-speed USB (12 Mbps)
- HID Keyboard device
- LampArray HID device
- Compatible with Windows Dynamic Lighting

## Customization

Edit `Inc/config.h` to customize:
- Number of LEDs
- Pin assignments
- Key mappings
- Timing parameters

## Troubleshooting

1. **LEDs not working**: Check WS2812 data pin connection and power supply
2. **USB not recognized**: Verify USB D+/D- connections and 1.5kΩ pull-up on D+
3. **Keys not responding**: Check GPIO configuration and pull-up resistors

## 📚 Additional Documentation

- **[SETUP_GUIDE.md](SETUP_GUIDE.md)** - 详细的安装和编译指南 (Detailed setup in Chinese/English)
- **[HARDWARE_GUIDE.md](HARDWARE_GUIDE.md)** - 硬件连接和接线图 (Hardware wiring diagrams)
- **[PORTING_NOTES.md](PORTING_NOTES.md)** - RP2040与STM32实现对比 (RP2040 vs STM32 comparison)
- **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** - 项目总结和技术细节 (Project summary and technical details)
- **[examples/](examples/)** - 测试程序示例 (Example test programs)

## 🎯 Quick Links

- [Build Instructions](#build-instructions) - Start building
- [Hardware Setup](HARDWARE_GUIDE.md) - Connect your hardware
- [Troubleshooting](#troubleshooting) - Fix common issues
- [Pin Configuration](#pin-configuration) - Customize pins

## 🤝 Contributing

Issues and pull requests are welcome! See the main repository for contribution guidelines.

## License

Same as the original project - MIT License
