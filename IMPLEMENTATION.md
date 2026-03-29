# CustomRTOS Implementation Summary

## Overview

Complete custom RTOS implementation for STM32F103 microcontroller, similar in concept to FreeRTOS but simplified for embedded learning.

## Kernel Architecture

### 1. **List Management** (list.h/c)

- Doubly-linked circular list implementation
- Used for task scheduling and blocking queues
- Functions: listInit, listInsertEnd, listInsertBegin, listRemove, listGetFirst

### 2. **Task Scheduler** (task.h/c)

- Context- free cooperative multitasking scheduler
- Task states: READY, RUNNING, BLOCKED, SUSPENDED
- Priority-based task creation (higher number = higher priority)
- Dynamic task delay support
- Functions: taskCreate, taskDelay, taskSchedule, taskYield

### 3. **Timer Management** (timer.h/c)

- Software timer with callback support
- Interval-based execution
- Up to 4 simultaneous timers
- Functions: timerCreate, timerStart, timerStop, timerTick

### 4. **Semaphore Synchronization** (semaphore.h/c)

- Binary and counting semaphore support
- Task synchronization primitives
- Functions: semaphoreInit, semaphoreTake, semaphoreGive

## Hardware Layer (STM32F103)

### Port Configuration (port.c)

- GPIO configuration for PC13 LED
- SysTick timer setup (1ms interrupt)
- LED control functions:
  - ledOn() - Low output (active low LED)
  - ledOff() - High output
  - ledToggle() - Toggle state
  - delay_ms() - Millisecond delay

### System Startup (startup.c)

- Vector table definition
- Reset handler with BSS/DATA initialization
- Exception vectors for STM32F103

## Memory Layout

| Component  | Size       | Location         |
| ---------- | ---------- | ---------------- |
| FLASH      | 128 KB     | 0x08000000       |
| RAM        | 20 KB      | 0x20000000       |
| ISR Vector | 256 bytes  | 0x08000000       |
| Text/Code  | Variable   | After ISR Vector |
| Data       | Variable   | RAM (0x20000000) |
| BSS        | Variable   | RAM              |
| Stack      | 20KB total | End of RAM       |

## Build System

### CMake Configuration (CMakeLists.txt)

- Cross-compiler setup for ARM Cortex-M3
- Automatic HEX and BIN file generation
- Post-build size reporting
- Linker script integration

### Toolchain File (arm-toolchain.cmake)

- Compiler settings: arm-none-eabi-gcc
- CPU flags: -mcpu=cortex-m3 -mthumb
- Optimization: -O2 (size and speed balanced)
- Memory sections: --gc-sections for unused code removal

### Linker Script (stm32_f103.ld)

- Memory regions definition
- Section layout (ISR, TEXT, DATA, BSS)
- Symbol generation (\_estack, \_sidata, etc.)

## Test Cases

### LED Blink Task

- Toggles LED every 500ms
- Demonstrates basic task execution

### LED Control (Blynk-style) Task

- Pattern sequence: ON 1s, OFF 1s, ON 2s, OFF 2s
- Simulates Blynk app LED control
- Demonstrates task timing and state management

### Idle Task

- Low-priority background task
- Keeps system running

## Key Features

1. **No Dynamic Memory Allocation**
   - All tasks pre-allocated at compile time
   - Deterministic behavior, no heap fragmentation

2. **Cooperative Multitasking**
   - Tasks must yield control
   - No preemption, simpler context switching
   - SysTick trigger taskSchedule()

3. **Periodic Scheduling**
   - SysTick interrupt every 1ms
   - Updates blocked task counters
   - Moves ready tasks to execution queue

4. **Real-time Capable**
   - 1ms system tick resolution
   - Predictable task switching
   - Interrupt-driven scheduling

## Compilation Details

### Compiler Flags

```
-mcpu=cortex-m3        # Cortex-M3 CPU
-mthumb                 # Thumb instruction set
-ffunction-sections     # Each function in separate section
-fdata-sections         # Each data in separate section
-O2                     # Optimize for performance
```

### Linker Flags

```
--gc-sections          # Remove unused sections
--print-memory-usage   # Report memory usage
-specs=nano.specs      # Use nano C library
-specs=nosys.specs     # No system calls
```

## Building & Output

### Build Command Options

1. **CMake (Recommended)**

   ```bash
   mkdir build && cd build
   cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
   cmake --build .
   ```

   Output: CustomRTOS.elf, CustomRTOS.hex, CustomRTOS.bin

2. **Make (Alternative)**

   ```bash
   make
   ```

   Same output files in build/ directory

3. **Scripts**
   - Windows: build.bat
   - Linux/Mac: build.sh

### Output File Purposes

| File           | Purpose                            | Usage                                                     |
| -------------- | ---------------------------------- | --------------------------------------------------------- |
| CustomRTOS.elf | Full executable with debug symbols | Debugging with GDB/IDE                                    |
| CustomRTOS.hex | Intel HEX format                   | Flash with STM32CubeProgrammer, generic programming tools |
| CustomRTOS.bin | Raw binary                         | Direct flash with st-flash, some programmers              |

## Flash Memory Address

- **Start Address**: 0x08000000
- **Flash Command**: `st-flash write CustomRTOS.bin 0x08000000`

## Interrupts and Priorities

| Interrupt | Priority | Handler           | Function                      |
| --------- | -------- | ----------------- | ----------------------------- |
| SysTick   | 15       | SysTick_Handler   | Scheduler tick, timer updates |
| HardFault | 0        | HardFault_Handler | Exception/crash handler       |

## Customization Points

1. **Task Stack Size**: Change TASK_STACK_SIZE in task.h
2. **Max Tasks**: Change MAX_TASKS in task.h
3. **Timer Count**: Change MAX_TIMERS in timer.c
4. **System Clock**: Modify SystemCoreClock in port.c
5. **Tick Rate**: Adjust SysTick_Frequency in port.c

## Limitations

1. Cooperative scheduling only (no preemption)
2. Fixed number of tasks (determined at compile time)
3. No inter-process communication (message queues)
4. No memory protection
5. Single-core execution only

## Future Enhancements

1. Preemptive scheduling with context save/restore
2. Dynamic memory allocation with heap
3. Message queues for task communication
4. Mutex implementation
5. Event groups
6. Software timers with dynamic creation
7. Task notifications

## Performance Characteristics

- **Context Switch Time**: ~1 microsecond (estimate)
- **Scheduler Overhead**: Minimal (simple linked list traversal)
- **Memory Overhead**: ~100 bytes base + 256 bytes per task stack
- **Real-Time Accuracy**: ±1 millisecond (SysTick dependent)

---

**Ready for STM32F103 Deployment! 🎯**
