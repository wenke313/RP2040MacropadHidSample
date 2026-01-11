######################################
# STM32F401 Macropad Makefile
######################################

######################################
# Target
######################################
TARGET = macropad_stm32

######################################
# Building variables
######################################
# Debug build?
DEBUG = 1
# Optimization
OPT = -Og

#######################################
# Paths
#######################################
# Build path
BUILD_DIR = build

######################################
# Source files
######################################
# C sources
C_SOURCES = \
Src/main.c \
Src/neopixel_stm32.c \
Src/ws2812_stm32.c \
Src/keys_stm32.c \
Src/encoder_stm32.c \
../src/usb_descriptors.c \
../src/Lighting/LampArray.c

# Add your STM32 HAL and system sources here
# Example:
# C_SOURCES += \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
# Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
# Src/system_stm32f4xx.c

# ASM sources
ASM_SOURCES = 
# Add startup file
# ASM_SOURCES += Src/startup_stm32f401xc.s

#######################################
# Binaries
#######################################
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# CPU
CPU = -mcpu=cortex-m4

# FPU
FPU = -mfpu=fpv4-sp-d16

# Float ABI
FLOAT-ABI = -mfloat-abi=hard

# MCU
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# AS defines
AS_DEFS = 

# C defines
C_DEFS = \
-DSTM32F401xC \
-DUSE_HAL_DRIVER

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES = \
-IInc \
-I../src \
-I../src/Lighting \
-I../src/Keys \
-I../src/Encoder

# Add your HAL and CMSIS include paths here
# Example:
# C_INCLUDES += \
# -IDrivers/STM32F4xx_HAL_Driver/Inc \
# -IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
# -IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
# -IDrivers/CMSIS/Include

# Add TinyUSB includes
# C_INCLUDES += \
# -IDrivers/tinyusb/src

# Compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# Link script
LDSCRIPT = STM32F401CCUx_FLASH.ld

# Libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# Build the application
#######################################
# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# Clean
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# Flash
#######################################
flash: all
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x08000000

#######################################
# Dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
