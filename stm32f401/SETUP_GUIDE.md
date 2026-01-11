# STM32F401 Port Setup Guide

This guide will help you set up the development environment for the STM32F401 port.

## 概述 (Overview)

本项目将原RP2040 Macropad移植到STM32F401平台，支持WS2812 RGB LED控制。

This project ports the original RP2040 Macropad to the STM32F401 platform with WS2812 RGB LED control support.

## 硬件要求 (Hardware Requirements)

1. **STM32F401开发板** (如 "Black Pill" STM32F401CCU6)
2. **WS2812B LED灯带** (建议12颗灯珠)
3. **12个按键开关** (带上拉电阻)
4. **旋转编码器** (带按键)
5. **USB连接线**

## 软件依赖 (Software Dependencies)

### 1. ARM GCC 工具链 (ARM GCC Toolchain)

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi

# macOS
brew install gcc-arm-embedded

# Windows
# Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
```

### 2. 构建工具 (Build Tools)

```bash
# CMake
sudo apt-get install cmake

# Make
sudo apt-get install build-essential

# Git
sudo apt-get install git
```

### 3. 烧写工具 (Flashing Tools)

```bash
# ST-Link
sudo apt-get install stlink-tools

# OpenOCD (alternative)
sudo apt-get install openocd
```

## 获取依赖库 (Getting Dependencies)

本项目需要以下库：

1. **STM32 HAL库** - STM32F4 HAL drivers
2. **CMSIS** - ARM Cortex Microcontroller Software Interface Standard
3. **TinyUSB** - USB协议栈

### 自动安装脚本 (Automatic Setup Script)

运行提供的安装脚本：

```bash
cd stm32f401
chmod +x setup_dependencies.sh
./setup_dependencies.sh
```

### 手动安装 (Manual Setup)

如果自动脚本失败，可以手动下载：

#### 1. STM32CubeF4 (HAL库)

```bash
cd stm32f401
git clone https://github.com/STMicroelectronics/STM32CubeF4.git Drivers/STM32CubeF4
```

#### 2. TinyUSB

```bash
cd stm32f401
git clone https://github.com/hathach/tinyusb.git Drivers/tinyusb
cd Drivers/tinyusb
git submodule update --init
```

#### 3. 创建符号链接

```bash
cd stm32f401
ln -s Drivers/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver Drivers/STM32F4xx_HAL_Driver
ln -s Drivers/STM32CubeF4/Drivers/CMSIS Drivers/CMSIS
```

## 编译项目 (Building the Project)

### 使用 CMake

```bash
cd stm32f401
mkdir build
cd build
cmake ..
make
```

### 使用 Makefile

```bash
cd stm32f401
make
```

编译成功后会生成：
- `macropad_stm32.elf` - ELF可执行文件
- `macropad_stm32.bin` - 二进制文件
- `macropad_stm32.hex` - Intel HEX文件

## 烧写固件 (Flashing Firmware)

### 使用 ST-Link

```bash
# 方法 1: 使用 st-flash
st-flash write build/macropad_stm32.bin 0x08000000

# 方法 2: 使用 Makefile
make flash
```

### 使用 OpenOCD

```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/macropad_stm32.elf verify reset exit"
```

### 使用 STM32CubeProgrammer (图形界面)

1. 下载并安装 STM32CubeProgrammer
2. 连接ST-Link到开发板
3. 打开STM32CubeProgrammer
4. 选择 `macropad_stm32.bin` 文件
5. 设置起始地址为 `0x08000000`
6. 点击 "Start Programming"

## 引脚配置 (Pin Configuration)

默认引脚配置在 `Inc/config.h` 中定义：

| 功能 | 引脚 | 说明 |
|------|------|------|
| WS2812 数据 | PA8 | TIM1_CH1 PWM输出 |
| 按键 1-12 | PB0-PB11 | 带内部上拉 |
| 旋转编码器 A | PA0 | 带中断 |
| 旋转编码器 B | PA1 | 普通输入 |
| 旋转编码器按键 | PA2 | 普通输入 |
| USB D- | PA11 | USB数据线 |
| USB D+ | PA12 | USB数据线 |

## 调试 (Debugging)

### 使用 GDB + OpenOCD

终端 1:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

终端 2:
```bash
arm-none-eabi-gdb build/macropad_stm32.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

### 串口调试输出

如果需要调试输出，可以配置UART：

```c
// 在 main.c 中添加
#define USE_SERIAL_DEBUG 1
```

## 故障排除 (Troubleshooting)

### 问题: LED不亮

1. 检查WS2812数据线连接（PA8）
2. 检查电源供电（WS2812需要5V）
3. 检查定时器配置和时钟频率

### 问题: USB无法识别

1. 检查USB引脚连接（PA11, PA12）
2. 检查D+上是否有1.5kΩ上拉电阻
3. 检查USB时钟配置（48MHz）

### 问题: 按键无响应

1. 检查GPIO配置（需要上拉）
2. 检查按键硬件连接
3. 使用万用表测试按键

### 问题: 编译错误

1. 确认已安装所有依赖库
2. 检查CMakeLists.txt中的路径
3. 确认工具链版本兼容

## 定制化 (Customization)

### 修改LED数量

编辑 `Inc/config.h`:
```c
#define NEOPIXEL_COUNT  24  // 改为你的LED数量
```

### 修改按键映射

编辑 `../src/Keys/keymap.h` 来修改按键功能。

### 修改灯光效果

编辑 `Inc/config.h`:
```c
#define AUTONOMOUS_LIGHTING_COLOR   (LampColor){255, 0, 0}  // 红色
#define AUTONOMOUS_LIGHTING_EFFECT  SOLID  // 常亮
```

## 技术支持 (Support)

如有问题，请：
1. 查看 [Issues](https://github.com/wenke313/RP2040MacropadHidSample/issues)
2. 提交新的Issue并附上详细信息

## 许可证 (License)

与原项目相同 - MIT License
