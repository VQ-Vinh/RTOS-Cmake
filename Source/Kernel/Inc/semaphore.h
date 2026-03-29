#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include "list.h"

typedef struct {
    uint32_t count;
    List waitingTasks;
} Semaphore;

void semaphoreInit(Semaphore *sem, uint32_t initialCount);
void semaphoreTake(Semaphore *sem);
void semaphoreGive(Semaphore *sem);
uint32_t semaphoreGetCount(Semaphore *sem);

#endif // SEMAPHORE_H
