#include <stdint.h>
#include <stddef.h>
#include "task.h"
#include "timer.h"
#include "semaphore.h"

#ifdef STM32F1
#include "testcase_f1.h"
#else
#include "testcase1.h"
#endif

// Port functions
extern void systemInit(void);
extern void sysTickInit(void);

// Callback for SysTick - delegates to testcase1
void ledControlCallback(void) {
    testcase1_led_update();
}

int main(void) {
    // Initialize system hardware
    systemInit();
    sysTickInit();
    
    // Initialize RTOS
    taskInit();
    timerInit();
    
    // Initialize test case 1 (LED blinking)
    testcase1_init();
    
    // Initialize scheduler
    taskSchedule();
    
    // Main scheduler loop
    // LED control happens via ledControlCallback() called from SysTick
    while(1) {
        Task *current = taskGetCurrent();
        if (current != NULL && current->function != NULL) {
            current->function();
        }
        taskYield();
    }
    
    return 0;
}
