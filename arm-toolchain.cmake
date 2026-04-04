# CMake Toolchain for ARM Cortex-M (STM32F1/F4)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Find programs
find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc)
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy)

# Prevent CMake from testing the compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# No operating system
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)