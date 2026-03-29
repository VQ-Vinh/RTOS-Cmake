# Quick Start Guide - CustomRTOS STM32F103

## 1. Prerequisites

Install these tools:

### Windows

- **ARM Toolchain**: Download from https://developer.arm.com/tools-and-software/open-source-software/gnu-toolchain/gnu-rm
- **CMake**: https://cmake.org/download
- **ST-Link Tools**: https://github.com/stlink-org/stlink (or use STM32CubeProgrammer)

### Linux

```bash
sudo apt-get install build-essential cmake
sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi
```

### macOS

```bash
brew install cmake arm-none-eabi-gcc stlink
```

## 2. Quick Build

### On Windows - Just click!

```bash
# From project root, double-click:
build.bat
```

### On Linux/Mac

```bash
chmod +x build.sh
./build.sh
```

### Or use Make (all platforms)

```bash
make
```

## 3. Flash to Device

### Option A: st-flash (Recommended)

```bash
st-flash write build/CustomRTOS.bin 0x08000000
```

### Option B: STM32CubeProgrammer (GUI)

1. File → Open File
2. Select: `build/CustomRTOS.hex`
3. Connect ST-Link
4. Download

### Option C: openocd

```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg \
  -c "program build/CustomRTOS.elf verify reset exit"
```

## 4. Test Your Board

Connect STM32F103 board:

1. LED on PC13 should start blinking (500ms toggle)
2. If using hardware debugger, can observe tasks running

## 5. Modify for Your Needs

### Change LED Blink Rate

Edit `Source/App/main.c`:

```c
ledToggle();
delay_ms(500);  // ← Change this value (milliseconds)
```

### Add Your Own Task

```c
void myTask(void) {
    while(1) {
        ledToggle();
        taskDelay(1000);  // Wait 1000 ticks
    }
}

// In main():
taskCreate(myTask, 1);  // Add this before taskSchedule()
```

### Change System Tick Rate

Edit `Source/Port/STM32/port.c`:

```c
#define SysTick_Frequency 1000  // Change to 100 or 10000, etc.
```

## 6. File Locations

| What You Need        | Where to Find            |
| -------------------- | ------------------------ |
| Build instructions   | README.md                |
| Technical details    | IMPLEMENTATION.md        |
| Kernel APIs          | Source/Kernel/Inc/\*.h   |
| LED control          | Source/Port/STM32/port.c |
| Main app logic       | Source/App/main.c        |
| Linker configuration | stm32_f103.ld            |

## 7. Common Issues & Solutions

### "arm-none-eabi-gcc not found"

```bash
# Add ARM toolchain to PATH
# Windows: System → Environment Variables → PATH → Add toolchain/bin
# Linux: export PATH=$PATH:/path/to/arm/toolchain/bin
```

### Build fails with linker errors

- Make sure `stm32_f103.ld` is in project root
- Check CMakeLists.txt path is correct

### LED not blinking after flashing

1. Check your STM32F103 board uses PC13 for LED
2. Verify LED is connected correctly (output high = LED off, active low)
3. Try uploading again with device erase first

### Device keeps crashing

1. Check TASK_STACK_SIZE is not too small (in `Source/Kernel/Inc/task.h`)
2. Verify system clock speed matches code (in `Source/Port/STM32/port.c`)

## 8. Debug with GDB (Optional)

Terminal 1 - Start GDB server:

```bash
st-util
```

Terminal 2 - Connect and debug:

```bash
arm-none-eabi-gdb
(gdb) target remote localhost:4242
(gdb) file build/CustomRTOS.elf
(gdb) load
(gdb) break main
(gdb) continue
```

## 9. View Build Size

After building:

```bash
arm-none-eabi-size build/CustomRTOS.elf
```

Shows FLASH used, RAM used, data size.

## 10. Clean Build

Remove old build files:

```bash
# Windows:
rmdir /s /q build

# Linux/Mac:
rm -rf build

# Or use Make:
make clean
```

## 11. Documentation

- **README.md** - Full project documentation
- **IMPLEMENTATION.md** - Technical architecture details
- **Source/Kernel/Inc/\*.h** - API headers (read these for available functions)
- **Datasheets** - See README.md References section

---

**You're ready to go! 🚀 Start with `build.bat` (Windows) or `make` (all platforms)**
