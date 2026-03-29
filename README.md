# CustomRTOS for STM32F103

Custom Real-Time Operating System implementation for STM32F103 microcontroller with LED control test case.

## Project Structure

```
CustomRTOS_V3/
├── CMakeLists.txt           # CMake build configuration
├── arm-toolchain.cmake      # ARM Cortex-M3 toolchain
├── Makefile                 # Alternative GNU Make configuration
├── build.bat                # Build script for Windows
├── build.sh                 # Build script for Linux/Mac
├── stm32_f103.ld           # Linker script for STM32F103
├── Source/
│   ├── Kernel/
│   │   ├── Inc/
│   │   │   ├── list.h      # Linked list data structure
│   │   │   ├── task.h      # Task scheduler
│   │   │   ├── timer.h     # Timer management
│   │   │   └── semaphore.h # Semaphore synchronization
│   │   └── Src/
│   │       ├── list.c
│   │       ├── task.c
│   │       ├── timer.c
│   │       └── semaphore.c
│   ├── Port/
│   │   └── STM32/
│   │       ├── port.c      # GPIO, SysTick configuration
│   │       └── startup.c   # STM32 startup code
│   └── App/
│       └── main.c          # Application with LED test cases
└── build/                  # Build output directory (created during build)
```

## Features

- **Kernel Components**:
  - Task scheduler with priority support
  - List data structure for task management
  - Timer support with callbacks
  - Binary semaphore for synchronization

- **Hardware Support**:
  - STM32F103 Cortex-M3 MCU
  - GPIO control for LED on PC13
  - SysTick timer for periodic scheduling

- **Test Cases**:
  - LED blink task (500ms toggle)
  - Blynk-style LED control (variable ON/OFF patterns)
  - Idle task for low-power operation

## Requirements

### Hardware

- STM32F103CB evaluation board or similar
- ST-Link V2 debugger (for flashing)

### Software

1. **ARM GCC Toolchain** - for cross-compilation
   - Download: https://developer.arm.com/tools-and-software/open-source-software/gnu-toolchain/gnu-rm
   - Extract and add `bin/` to PATH

2. **CMake** (optional, for CMake build)
   - Download: https://cmake.org/download/
   - Add to PATH

3. **Make** (optional, for Makefile build)
   - On Windows: install with MinGW or use CMake
   - On Linux/Mac: usually pre-installed

4. **ST-Link Tools** (for flashing)
   - st-flash or st-util from https://github.com/stlink-org/stlink
   - Or use openocd for more options

## Building

### Method 1: CMake (Windows)

```bash
cd CustomRTOS_V3
build.bat
```

### Method 2: CMake (Linux/Mac)

```bash
cd CustomRTOS_V3
chmod +x build.sh
./build.sh
```

### Method 3: CMake (Manual)

```bash
cd CustomRTOS_V3
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Method 4: GNU Make

```bash
cd CustomRTOS_V3
make
```

## Output Files

After successful build:

- **CustomRTOS.elf** - ELF executable (debug info included)
- **CustomRTOS.hex** - Intel HEX format (recommended for flashing)
- **CustomRTOS.bin** - Raw binary format

## Flashing to Device

### Using st-flash (STM32CubeProgrammer alternative)

```bash
st-flash write build/CustomRTOS.bin 0x08000000
```

### Using openocd

```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
  -c "program build/CustomRTOS.elf verify reset exit"
```

### Using STM32CubeProgrammer (GUI)

1. Connect ST-Link to device
2. Open STM32CubeProgrammer
3. Select your MCU (STM32F103)
4. Load build/CustomRTOS.hex
5. Click "Download"

## Configuration

### Modify System Clock

Edit `Source/Port/STM32/port.c`:

```c
#define SystemCoreClock 72000000  // Change if using different clock
```

### Adjust SysTick Frequency

Edit `Source/Port/STM32/port.c`:

```c
#define SysTick_Frequency 1000    // Change for different tick rate (Hz)
```

### Add More Tasks

Edit `Source/App/main.c`:

```c
void myTask(void) {
    while(1) {
        // Your task code here
        taskDelay(100);  // Task delay in ticks
    }
}

// In main():
taskCreate(myTask, 1);  // Create task with priority
```

### LED Control Functions

Available in `Source/Port/STM32/port.c`:

- `ledOn()` - Turn LED on
- `ledOff()` - Turn LED off
- `ledToggle()` - Toggle LED state
- `delay_ms(ms)` - Delay in milliseconds
- `getSystemTick()` - Get current system tick

## Troubleshooting

### Build Errors

1. **"arm-none-eabi-gcc not found"**: Add ARM toolchain to PATH
2. **CMake errors**: Ensure CMake is installed and in PATH
3. **Linker errors**: Check stm32_f103.ld is in project root

### Flashing Issues

1. **Device not detected**: Check ST-Link connection and drivers
2. **Flash verification failed**: Try erasing device first
3. **Permission denied (Linux/Mac)**: Use `sudo` or adjust udev rules

### Runtime Issues

1. **LED not blinking**: Check PC13 is properly configured
2. **Device crashes**: Check stack size (TASK_STACK_SIZE in task.h)
3. **Scheduler not working**: Verify SysTick interrupt is enabled

## Development Tips

1. **Add debug output**:
   - Implement UART printf for debugging
   - Use with ST-Link UART interface

2. **Optimize for size**:
   - Change CMAKE_BUILD_TYPE to MinSizeRel
   - Reduce TASK_STACK_SIZE if needed

3. **Monitor memory usage**:
   - CMake build automatically prints memory info
   - STM32F103CB has 128KB FLASH and 20KB SRAM

4. **Extend kernel**:
   - Add mutex support in kernel/
   - Implement message queues for task communication
   - Add more timer callbacks

## References

- STM32F103 Reference Manual: https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-microcontrollers-stmicroelectronics.pdf
- ARM Cortex-M3 Programming: https://developer.arm.com/documentation/dui0552/a
- CMake Documentation: https://cmake.org/cmake/help/latest/
- ST-Link Tools: https://github.com/stlink-org/stlink

## License

This project is provided as-is for educational purposes.

---

**Happy embedded development! 🚀**
