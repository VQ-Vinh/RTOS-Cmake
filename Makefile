# Makefile for CustomRTOS on STM32F103

# Compiler and tools
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Output directory
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/CustomRTOS

# MCU settings
MCU = -mcpu=cortex-m3 -mthumb
CFLAGS = $(MCU) -std=c99 -O2
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wextra

# Include paths
INCLUDE = -ISource/Kernel/Inc -ISource/Port/STM32 -ISource/App

# Source files
SOURCES = \
    Source/Kernel/Src/list.c \
    Source/Kernel/Src/task.c \
    Source/Kernel/Src/timer.c \
    Source/Kernel/Src/semaphore.c \
    Source/Port/STM32/port.c \
    Source/Port/STM32/startup.c \
    Source/App/main.c \
    Source/App/task1/testcase1.c

# Object files
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Linker script
LINKER_SCRIPT = stm32_f103.ld
LDFLAGS = $(MCU) -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections -Wl,--print-memory-usage
LDFLAGS += -specs=nano.specs -specs=nosys.specs

# Targets
.PHONY: all clean build hex bin

all: build hex bin

build: $(OUTPUT).elf
	@$(SIZE) $(OUTPUT).elf

$(OUTPUT).elf: $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo Linking $@...
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo Compiling $<...
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

hex: $(OUTPUT).hex

$(OUTPUT).hex: $(OUTPUT).elf
	@echo Creating HEX file...
	@$(OBJCOPY) -O ihex $< $@

bin: $(OUTPUT).bin

$(OUTPUT).bin: $(OUTPUT).elf
	@echo Creating BIN file...
	@$(OBJCOPY) -O binary $< $@

clean:
	@echo Cleaning...
	@rm -rf $(BUILD_DIR)

help:
	@echo "CustomRTOS Build Targets:"
	@echo "  make build   - Build ELF file"
	@echo "  make hex     - Generate HEX file for flashing"
	@echo "  make bin     - Generate BIN file for flashing"
	@echo "  make all     - Build all (default)"
	@echo "  make clean   - Clean build artifacts"
