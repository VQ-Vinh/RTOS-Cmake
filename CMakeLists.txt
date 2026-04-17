cmake_minimum_required(VERSION 3.15)

# Project definition
project(CustomRTOS C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# =============================================================================
# Kernel sources (ANSI C - hardware independent)
# =============================================================================
set(KERNEL_SOURCES
    Source/Kernel/Src/list.c
    Source/Kernel/Src/task.c
    Source/Kernel/Src/timer.c
    Source/Kernel/Src/semaphore.c
)

# =============================================================================
# App sources
# =============================================================================
set(APP_SOURCES
    Source/App/main.c
)

# =============================================================================
# STM32F4 Build Target
# =============================================================================
set(MCU_F4 "STM32F407VG")
set(CPU_F4 "cortex-m4")
set(FPU_F4 "fpv4-sp-d16")
set(FLOAT_ABI_F4 "softfp")

# Compiler flags for STM32F4
set(CMAKE_C_FLAGS_F4 "-mcpu=${CPU_F4} -mthumb -mno-thumb-interwork")
set(CMAKE_C_FLAGS_F4 "${CMAKE_C_FLAGS_F4} -mfloat-abi=${FLOAT_ABI_F4} -mfpu=${FPU_F4}")
set(CMAKE_C_FLAGS_F4 "${CMAKE_C_FLAGS_F4} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_F4 "${CMAKE_C_FLAGS_F4} -Wall -Wextra -O2")

# Linker flags for STM32F4
set(CMAKE_EXE_LINKER_FLAGS_F4 "-mcpu=${CPU_F4} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_F4 "${CMAKE_EXE_LINKER_FLAGS_F4} -mfloat-abi=${FLOAT_ABI_F4} -mfpu=${FPU_F4}")
set(CMAKE_EXE_LINKER_FLAGS_F4 "${CMAKE_EXE_LINKER_FLAGS_F4} -Wl,--gc-sections -Wl,--print-memory-usage")

# Include directories for STM32F4
set(INCLUDE_DIRS_F4
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Inc
    ${CMAKE_SOURCE_DIR}/Source/Port/STM32F4
    ${CMAKE_SOURCE_DIR}/Source/App
)

# Port sources for STM32F4
set(PORT_SOURCES_F4
    Source/Port/STM32F4/port.c
    Source/Port/STM32F4/rcc.c
    Source/Port/STM32F4/gpio.c
    Source/Port/STM32F4/systick.c
    Source/Port/STM32F4/startup.c
)

set(APP_SOURCES_F4
    ${APP_SOURCES}
    Source/Port/STM32F4/testcase1.c
)

# Create STM32F4 executable
add_executable(CustomRTOS_F4.elf
    ${KERNEL_SOURCES}
    ${PORT_SOURCES_F4}
    ${APP_SOURCES_F4}
)

target_include_directories(CustomRTOS_F4. PRIVATE ${INCLUDE_DIRS_F4})
target_compile_options(CustomRTOS_F4. PRIVATE ${CMAKE_C_FLAGS_F4})
target_link_options(CustomRTOS_F4. PRIVATE -T${CMAKE_SOURCE_DIR}/stm32_f407.ld ${CMAKE_EXE_LINKER_FLAGS_F4})

add_custom_command(TARGET CustomRTOS_F4.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex CustomRTOS_F4.elf CustomRTOS_F4.hex
    COMMAND arm-none-eabi-objcopy -O binary CustomRTOS_F4.elf CustomRTOS_F4.bin
    COMMAND arm-none-eabi-size CustomRTOS_F4.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating STM32F4 HEX and BIN files..."
)

# =============================================================================
# STM32F1 Build Target
# =============================================================================
set(MCU_F1 "STM32F103C8")
set(CPU_F1 "cortex-m3")

# Compiler flags for STM32F1 (no FPU)
set(CMAKE_C_FLAGS_F1 "-mcpu=${CPU_F1} -mthumb -mno-thumb-interwork")
set(CMAKE_C_FLAGS_F1 "${CMAKE_C_FLAGS_F1} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS_F1 "${CMAKE_C_FLAGS_F1} -Wall -Wextra -O2")

# Linker flags for STM32F1 (no FPU)
set(CMAKE_EXE_LINKER_FLAGS_F1 "-mcpu=${CPU_F1} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS_F1 "${CMAKE_EXE_LINKER_FLAGS_F1} -Wl,--gc-sections -Wl,--print-memory-usage")

# Include directories for STM32F1
set(INCLUDE_DIRS_F1
    ${CMAKE_SOURCE_DIR}/Source/Kernel/Inc
    ${CMAKE_SOURCE_DIR}/Source/Port/STM32
    ${CMAKE_SOURCE_DIR}/Source/Driver
    ${CMAKE_SOURCE_DIR}/Source/air_quality
    ${CMAKE_SOURCE_DIR}/Source/Connectivity
    ${CMAKE_SOURCE_DIR}/Source/App
)

# Port sources for STM32F1
set(PORT_SOURCES_F1
    Source/Port/STM32/port.c
    Source/Port/STM32/rcc.c
    Source/Port/STM32/gpio.c
    Source/Port/STM32/systick.c
    Source/Port/STM32/startup.c
)

# Air quality and connectivity sources
set(APP_SOURCES_F1
    ${APP_SOURCES}
    Source/Port/STM32/testcase_f1.c
    Source/air_quality/adc.c
    Source/Connectivity/esp01.c
)

# Create STM32F1 executable
add_executable(CustomRTOS_F1.elf
    ${KERNEL_SOURCES}
    ${PORT_SOURCES_F1}
    ${APP_SOURCES_F1}
)

target_include_directories(CustomRTOS_F1. PRIVATE ${INCLUDE_DIRS_F1})
target_compile_options(CustomRTOS_F1. PRIVATE ${CMAKE_C_FLAGS_F1})
target_link_options(CustomRTOS_F1. PRIVATE -T${CMAKE_SOURCE_DIR}/stm32_f103.ld ${CMAKE_EXE_LINKER_FLAGS_F1})

add_custom_command(TARGET CustomRTOS_F1.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex CustomRTOS_F1.elf CustomRTOS_F1.hex
    COMMAND arm-none-eabi-objcopy -O binary CustomRTOS_F1.elf CustomRTOS_F1.bin
    COMMAND arm-none-eabi-size CustomRTOS_F1.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating STM32F1 HEX and BIN files..."
)

# Display build info
message(STATUS "=== CustomRTOS Build Configuration ===")
message(STATUS "STM32F4: MCU=${MCU_F4} CPU=${CPU_F4}")
message(STATUS "STM32F1: MCU=${MCU_F1} CPU=${CPU_F1}")