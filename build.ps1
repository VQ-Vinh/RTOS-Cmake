# PowerShell Build Script for CustomRTOS on STM32F103
# Requirements: CMake, ARM GCC toolchain (arm-none-eabi-gcc)

param(
    [switch]$Clean = $false,
    [switch]$Release = $true,
    [string]$BuildType = "Release"
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "CustomRTOS Build for STM32F103" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if CMake exists
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakePath) {
    Write-Host "ERROR: CMake not found. Please install CMake." -ForegroundColor Red
    Write-Host "Download from: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

# Check if ARM GCC exists
$armGccPath = Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue
if (-not $armGccPath) {
    Write-Host "ERROR: arm-none-eabi-gcc not found. Please install ARM toolchain." -ForegroundColor Red
    Write-Host "Download from: https://developer.arm.com/tools-and-software/open-source-software/gnu-toolchain/gnu-rm" -ForegroundColor Yellow
    exit 1
}

Write-Host "✓ CMake found: $($cmakePath.Source)" -ForegroundColor Green
Write-Host "✓ ARM GCC found: $($armGccPath.Source)" -ForegroundColor Green
Write-Host ""

# Clean build directory if requested
if ($Clean) {
    if (Test-Path "build") {
        Write-Host "Cleaning build directory..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force "build"
        Write-Host "✓ Build directory cleaned" -ForegroundColor Green
        Write-Host ""
    }
}

# Create build directory
if (-not (Test-Path "build")) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path "build" | Out-Null
    Write-Host "✓ Build directory created" -ForegroundColor Green
    Write-Host ""
}

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
Push-Location "build"

$toolchainPath = (Resolve-Path "..\arm-toolchain.cmake").Path
Write-Host "  Toolchain: $toolchainPath"
Write-Host "  Build Type: $BuildType"

cmake -DCMAKE_TOOLCHAIN_FILE="$toolchainPath" -DCMAKE_BUILD_TYPE="$BuildType" ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: CMake configuration failed" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host "✓ CMake configuration successful" -ForegroundColor Green
Write-Host ""

# Build project
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build . --config "$BuildType" --parallel 4

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Build failed" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host "✓ Build completed successfully" -ForegroundColor Green
Write-Host ""

Pop-Location

# Check output files
Write-Host "Output Files:" -ForegroundColor Cyan
$files = @("build\CustomRTOS.elf", "build\CustomRTOS.hex", "build\CustomRTOS.bin")
foreach ($file in $files) {
    if (Test-Path $file) {
        $size = (Get-Item $file).Length
        Write-Host "  ✓ $file ($size bytes)" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "Flashing Instructions:" -ForegroundColor Cyan
Write-Host ""
Write-Host "Method 1: Using st-flash" -ForegroundColor Yellow
Write-Host "  st-flash write build\CustomRTOS.bin 0x08000000"
Write-Host ""
Write-Host "Method 2: Using STM32CubeProgrammer (GUI)" -ForegroundColor Yellow
Write-Host "  1. Open STM32CubeProgrammer"
Write-Host "  2. File → Open File → Select build\CustomRTOS.hex"
Write-Host "  3. Connect ST-Link and click Download"
Write-Host ""
Write-Host "Method 3: Using openocd" -ForegroundColor Yellow
Write-Host "  openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg ``"
Write-Host "    -c `"program build/CustomRTOS.elf verify reset exit`""
Write-Host ""

# Option to flash immediately
$flash = Read-Host "Flash to device now? (y/n) [default: n]"
if ($flash -eq "y" -or $flash -eq "yes") {
    $toolChoice = Read-Host "Choose tool: (1) st-flash, (2) openocd, (3) skip"
    
    switch ($toolChoice) {
        "1" {
            Write-Host "Flashing with st-flash..." -ForegroundColor Yellow
            st-flash write build\CustomRTOS.bin 0x08000000
            if ($LASTEXITCODE -eq 0) {
                Write-Host "✓ Flash successful!" -ForegroundColor Green
            } else {
                Write-Host "ERROR: Flash failed" -ForegroundColor Red
            }
        }
        "2" {
            Write-Host "Flashing with openocd..." -ForegroundColor Yellow
            openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg `
                -c "program build/CustomRTOS.elf verify reset exit"
            if ($LASTEXITCODE -eq 0) {
                Write-Host "✓ Flash successful!" -ForegroundColor Green
            } else {
                Write-Host "ERROR: Flash failed" -ForegroundColor Red
            }
        }
        default {
            Write-Host "Skipping flash" -ForegroundColor Yellow
        }
    }
}

Write-Host ""
Write-Host "Build and configuration complete! 🚀" -ForegroundColor Cyan
