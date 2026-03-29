#include "stdint.h"
#include "semaphore.h"
#include <stddef.h>

void semaphoreInit(Semaphore *sem, uint32_t initialCount) {
    if (sem == NULL) return;
    sem->count = initialCount;
    listInit(&sem->waitingTasks);
}

void semaphoreTake(Semaphore *sem) {
    if (sem == NULL) return;
    
    if (sem->count > 0) {
        sem->count--;
    }
}

void semaphoreGive(Semaphore *sem) {
    if (sem == NULL) return;
    sem->count++;
}

uint32_t semaphoreGetCount(Semaphore *sem) {
    if (sem == NULL) return 0;
    return sem->count;
}
