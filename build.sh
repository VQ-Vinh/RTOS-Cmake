#!/bin/bash
# Build script for CustomRTOS on STM32F103
# Requirements: CMake, ARM GCC toolchain (arm-none-eabi-gcc)

echo "========================================"
echo "CustomRTOS Build for STM32F103"
echo "========================================"

# Check if CMake exists
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake."
    exit 1
fi

# Check if ARM GCC exists
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "ERROR: arm-none-eabi-gcc not found. Please install ARM toolchain."
    exit 1
fi

# Create build directory
mkdir -p build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

# Build project
echo ""
echo "Building project..."
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

# Print result
echo ""
echo "Build completed successfully!"
echo ""
echo "Output files:"
echo "- CustomRTOS.elf"
echo "- CustomRTOS.hex (use this to flash with ST-Link)"
echo "- CustomRTOS.bin (use this to flash with other tools)"
echo ""
echo "To flash using st-flash:"
echo "  st-flash write CustomRTOS.bin 0x08000000"
echo ""
echo "To flash using openocd:"
echo "  openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"program CustomRTOS.elf verify reset exit\""
