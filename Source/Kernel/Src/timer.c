#include "timer.h"
#include <stddef.h>

#define MAX_TIMERS 4

static Timer timers[MAX_TIMERS];
static uint32_t timerCount = 0;

void timerInit(void) {
    timerCount = 0;
    for (int i = 0; i < MAX_TIMERS; i++) {
        timers[i].active = 0;
        timers[i].counter = 0;
        timers[i].interval = 0;
        timers[i].callback = NULL;
    }
}

Timer *timerCreate(uint32_t interval, TimerCallback callback) {
    if (timerCount >= MAX_TIMERS) return NULL;
    
    Timer *timer = &timers[timerCount];
    timer->interval = interval;
    timer->counter = 0;
    timer->callback = callback;
    timer->active = 0;
    timerCount++;
    
    return timer;
}

void timerStart(Timer *timer) {
    if (timer != NULL) {
        timer->active = 1;
        timer->counter = 0;
    }
}

void timerStop(Timer *timer) {
    if (timer != NULL) {
        timer->active = 0;
    }
}

void timerTick(void) {
    for (uint32_t i = 0; i < timerCount; i++) {
        if (timers[i].active) {
            timers[i].counter++;
            if (timers[i].counter >= timers[i].interval) {
                timers[i].counter = 0;
                if (timers[i].callback != NULL) {
                    timers[i].callback();
                }
            }
        }
    }
}
