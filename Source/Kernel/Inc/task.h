#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "list.h"

#define TASK_STACK_SIZE 256
#define MAX_TASKS 8

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

typedef void (*TaskFunction)(void);

typedef struct {
    uint32_t stack[TASK_STACK_SIZE];
    uint32_t *stackPointer;
    TaskFunction function;
    uint32_t priority;
    TaskState state;
    ListNode node;
    uint32_t delayTicks;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    uint32_t taskCount;
    Task *currentTask;
    uint32_t tickCount;
    List readyList;
    List blockedList;
} Scheduler;

extern Scheduler scheduler;

void taskInit(void);
void taskCreate(TaskFunction function, uint32_t priority);
void taskDelay(uint32_t ticks);
void taskSchedule(void);
Task *taskGetCurrent(void);
void taskYield(void);
void schedulerInit(void);

#endif // TASK_H
