#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef void (*TimerCallback)(void);

typedef struct {
    uint32_t interval;
    uint32_t counter;
    TimerCallback callback;
    int active;
} Timer;

void timerInit(void);
Timer *timerCreate(uint32_t interval, TimerCallback callback);
void timerStart(Timer *timer);
void timerStop(Timer *timer);
void timerTick(void);

#endif // TIMER_H
