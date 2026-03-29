#include "task.h"
#include <string.h>
#include <stddef.h>

Scheduler scheduler;

void schedulerInit(void) {
    scheduler.taskCount = 0;
    scheduler.tickCount = 0;
    scheduler.currentTask = NULL;
    listInit(&scheduler.readyList);
    listInit(&scheduler.blockedList);
}

void taskInit(void) {
    schedulerInit();
}

void taskCreate(TaskFunction function, uint32_t priority) {
    if (scheduler.taskCount >= MAX_TASKS) return;

    Task *task = &scheduler.tasks[scheduler.taskCount];
    task->function = function;
    task->priority = priority;
    task->state = TASK_READY;
    task->delayTicks = 0;
    
    // Initialize stack pointer to end of stack
    task->stackPointer = &task->stack[TASK_STACK_SIZE - 1];
    memset(task->stack, 0, sizeof(task->stack));
    
    // Add to ready list
    listInsertEnd(&scheduler.readyList, &task->node);
    scheduler.taskCount++;
}

void taskDelay(uint32_t ticks) {
    Task *current = scheduler.currentTask;
    if (current == NULL) return;
    
    current->delayTicks = ticks;
    current->state = TASK_BLOCKED;
    
    // Remove from ready list
    listRemove(&current->node);
    // Add to blocked list
    listInsertEnd(&scheduler.blockedList, &current->node);
}

void taskSchedule(void) {
    // Update blocked tasks
    ListNode *node = listGetFirst(&scheduler.blockedList);
    while (node != NULL) {
        ListNode *nextNode = node->next;
        Task *task = (Task *)node;
        
        if (task->delayTicks > 0) {
            task->delayTicks--;
        } else {
            listRemove(node);
            task->state = TASK_READY;
            listInsertEnd(&scheduler.readyList, node);
        }
        
        node = nextNode;
    }
    
    // Get next ready task
    if (!listIsEmpty(&scheduler.readyList)) {
        ListNode *first = listGetFirst(&scheduler.readyList);
        scheduler.currentTask = (Task *)first;
    }
}

Task *taskGetCurrent(void) {
    return scheduler.currentTask;
}

void taskYield(void) {
    // Move current task to end of ready list if still ready
    if (scheduler.currentTask != NULL && scheduler.currentTask->state == TASK_RUNNING) {
        ListNode *node = &scheduler.currentTask->node;
        listRemove(node);
        listInsertEnd(&scheduler.readyList, node);
    }
    taskSchedule();
}
