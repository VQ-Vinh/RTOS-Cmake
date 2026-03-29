cmake_minimum_required(VERSION 3.15)

# Project definition
project(CustomRTOS_STM32F103 C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set MCU details
set(MCU "STM32F103CB")
set(ARCH "arm")
set(CPU "cortex-m3")
set(FPU "none")

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${CPU} -mthumb -mno-thumb-interwork")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=${CPU} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -specs=nano.specs -specs=nosys.specs")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Inc
    ${CMAKE_SOURCE_DIR}/Source/Port/STM32
    ${CMAKE_SOURCE_DIR}/Source/App
    ${CMAKE_SOURCE_DIR}/Source/App/task1
)

# Kernel sources
set(KERNEL_SOURCES
    Source/Kernel/Src/list.c
    Source/Kernel/Src/task.c
    Source/Kernel/Src/timer.c
    Source/Kernel/Src/semaphore.c
)

# Port sources
set(PORT_SOURCES
    Source/Port/STM32/port.c
    Source/Port/STM32/startup.c
)

# App sources
set(APP_SOURCES
    Source/App/main.c
    Source/App/task1/testcase1.c
)

# Create executable
add_executable(CustomRTOS.elf
    ${KERNEL_SOURCES}
    ${PORT_SOURCES}
    ${APP_SOURCES}
)

# Link with linker script
target_link_options(CustomRTOS.elf PRIVATE -T${CMAKE_SOURCE_DIR}/stm32_f103.ld)

# Generate HEX and BIN files
add_custom_command(TARGET CustomRTOS.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex CustomRTOS.elf CustomRTOS.hex
    COMMAND arm-none-eabi-objcopy -O binary CustomRTOS.elf CustomRTOS.bin
    COMMAND arm-none-eabi-size CustomRTOS.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating HEX and BIN files..."
)

# Display build info
message(STATUS "MCU: ${MCU}")
message(STATUS "CPU: ${CPU}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
