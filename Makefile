# Makefile for CustomRTOS - Support both STM32F1 and STM32F4

# Default target
TARGET ?= F1

# Compiler and tools
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Output directory
BUILD_DIR = build

# =============================================================================
# STM32F1 Configuration
# =============================================================================
MCU_F1 = -mcpu=cortex-m3 -mthumb
CFLAGS_F1 = $(MCU_F1) -std=c99 -O2 -DSTM32F1
CFLAGS_F1 += -ffunction-sections -fdata-sections
CFLAGS_F1 += -Wall -Wextra
LDFLAGS_F1 = $(MCU_F1) -Tstm32_f103.ld
LDFLAGS_F1 += -Wl,--gc-sections -Wl,--print-memory-usage
LDFLAGS_F1 += -specs=nano.specs -specs=nosys.specs

INCLUDE_F1 = -ISource/Port/STM32 -ISource/Driver -ISource/Sensor_Lib -ISource/Air_quality -ISource/Connectivity -ISource/App -ISource/App/Testcase -ISource/Kernel/Inc

SOURCES_F1 = \
    Source/Kernel/Src/list.c \
    Source/Kernel/Src/task.c \
    Source/Kernel/Src/timer.c \
    Source/Kernel/Src/semaphore.c \
    Source/Port/STM32/port.c \
    Source/Port/STM32/startup.c \
    Source/Driver/gpio.c \
    Source/Driver/rcc.c \
    Source/Driver/systick.c \
    Source/Driver/uart.c \
    Source/Driver/adc.c \
    Source/Sensor_Lib/mq2.c \
    Source/Sensor_Lib/dht11.c \
    Source/Air_quality/air_quality.c \
    Source/Connectivity/esp01.c \
    Source/App/Testcase/testcase_f1.c \
    Source/App/main.c

# =============================================================================
# STM32F4 Configuration
# =============================================================================
MCU_F4 = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -mfpu=fpv4-sp-d16
CFLAGS_F4 = $(MCU_F4) -std=c99 -O2 -DSTM32F4
CFLAGS_F4 += -ffunction-sections -fdata-sections
CFLAGS_F4 += -Wall -Wextra
LDFLAGS_F4 = $(MCU_F4) -Tstm32_f407.ld
LDFLAGS_F4 += -Wl,--gc-sections -Wl,--print-memory-usage
LDFLAGS_F4 += -specs=nano.specs -specs=nosys.specs

INCLUDE_F4 = -ISource/Kernel/Inc -ISource/Port/STM32F4 -ISource/Driver_f4 -ISource/Air_quality_f4 -ISource/Connectivity_f4 -ISource/App

SOURCES_F4 = \
    Source/Kernel/Src/list.c \
    Source/Kernel/Src/task.c \
    Source/Kernel/Src/timer.c \
    Source/Kernel/Src/semaphore.c \
    Source/Port/STM32F4/port.c \
    Source/Port/STM32F4/rcc.c \
    Source/Port/STM32F4/gpio.c \
    Source/Port/STM32F4/systick.c \
    Source/Port/STM32F4/startup.c \
    Source/Driver_f4/uart_f4.c \
    Source/Air_quality_f4/adc_f4.c \
    Source/Connectivity_f4/esp01_f4.c \
    Source/App/main.c

# =============================================================================
# Build Logic
# =============================================================================
ifeq ($(TARGET),F1)
    OUTPUT = $(BUILD_DIR)/CustomRTOS_F1
    CFLAGS = $(CFLAGS_F1)
    LDFLAGS = $(LDFLAGS_F1)
    INCLUDE = $(INCLUDE_F1)
    SOURCES = $(SOURCES_F1)
else ifeq ($(TARGET),F4)
    OUTPUT = $(BUILD_DIR)/CustomRTOS_F4
    CFLAGS = $(CFLAGS_F4)
    LDFLAGS = $(LDFLAGS_F4)
    INCLUDE = $(INCLUDE_F4)
    SOURCES = $(SOURCES_F4)
else
    $(error Invalid TARGET. Use TARGET=F1 or TARGET=F4)
endif

# Object files
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# =============================================================================
# Targets
# =============================================================================
.PHONY: all clean build hex bin help F1 F4

all: build hex bin

build: $(OUTPUT).elf
	@$(SIZE) $(OUTPUT).elf

$(OUTPUT).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo "Linking $@ for STM32$(TARGET)..."
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

hex: $(OUTPUT).hex

$(OUTPUT).hex: $(OUTPUT).elf
	@echo "Creating HEX file..."
	@$(OBJCOPY) -O ihex $< $@

bin: $(OUTPUT).bin

$(OUTPUT).bin: $(OUTPUT).elf
	@echo "Creating BIN file..."
	@$(OBJCOPY) -O binary $< $@

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

F1:
	@$(MAKE) TARGET=F1 all

F4:
	@$(MAKE) TARGET=F4 all

help:
	@echo "CustomRTOS Build Targets:"
	@echo "  make F1      - Build for STM32F1 (default)"
	@echo "  make F4      - Build for STM32F4"
	@echo "  make TARGET=F1 build - Build ELF for F1"
	@echo "  make TARGET=F4 build - Build ELF for F4"
	@echo "  make hex     - Generate HEX file"
	@echo "  make bin     - Generate BIN file"
	@echo "  make clean   - Clean build artifacts"