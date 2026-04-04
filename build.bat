@echo off
REM Build script for CustomRTOS on STM32F103
REM Requirements: CMake, ARM GCC toolchain (arm-none-eabi-gcc)

setlocal enabledelayedexpansion

echo ========================================
echo CustomRTOS Build for STM32F4
echo ========================================

REM Check if CMake exists
where cmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found. Please install CMake.
    pause
    exit /b 1
)

REM Check if ARM GCC exists
where arm-none-eabi-gcc >nul 2>&1
if errorlevel 1 (
    echo ERROR: arm-none-eabi-gcc not found. Please install ARM toolchain.
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build

REM Configure with CMake
echo.
echo Configuring with CMake...
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
if errorlevel 1 (
    echo CMake configuration failed
    pause
    exit /b 1
)

REM Build project
echo.
echo Building project...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed
    pause
    exit /b 1
)

REM Optional: Flash to device
echo.
echo Build completed successfully!
echo.
echo Output files:
echo - CustomRTOS.elf
echo - CustomRTOS.hex (use this to flash with ST-Link)
echo - CustomRTOS.bin (use this to flash with other tools)
echo.
echo To flash using st-flash:
echo   st-flash write CustomRTOS.bin 0x08000000
echo.
echo To flash using openocd:
echo   openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program CustomRTOS.elf verify reset exit"
echo.

pause
