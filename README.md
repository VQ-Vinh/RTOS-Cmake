# CustomRTOS

Custom Real-Time Operating System for STM32 microcontrollers.

## Project Structure

```
CustomRTOS_V3/
├── CMakeLists.txt           # CMake build configuration
├── arm-toolchain.cmake      # ARM toolchain settings
├── stm32_f103.ld            # Linker script for STM32F1
├── stm32_f407.ld            # Linker script for STM32F4
├── build.bat                # Build script (Windows)
├── Source/
│   ├── Kernel/              # RTOS kernel (hardware-independent)
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
│   │   ├── STM32/          # STM32F1 hardware port
│   │   └── STM32F4/        # STM32F4 hardware port
│   ├── App/                # Application layer
│   │   ├── main.c
│   │   └── task1/
│   └── Driver/             # Peripheral drivers
```

## Supported Hardware

| MCU | CPU | Flash | RAM |
|-----|-----|-------|-----|
| STM32F103CB | Cortex-M3 | 128KB | 20KB |
| STM32F407VG | Cortex-M4 | 512KB | 192KB |

## Features

- **Task Scheduler**: Cooperative multitasking with priority support
- **Timer**: Software timers with callback
- **Semaphore**: Binary and counting semaphores
- **Hardware Abstraction**: Clean separation between kernel and port layers

## Building

### STM32F1
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### STM32F4
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Or simply run `build.bat` on Windows.

## Output Files

- `CustomRTOS.elf` - ELF executable with debug info
- `CustomRTOS.hex` - Intel HEX format for flashing
- `CustomRTOS.bin` - Raw binary

## Flashing

```bash
st-flash write CustomRTOS.bin 0x08000000
```

## Requirements

- ARM GCC toolchain (arm-none-eabi-gcc)
- CMake 3.15+