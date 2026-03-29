#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct {
    ListNode head;
    uint32_t itemCount;
} List;

void listInit(List *list);
void listInsertEnd(List *list, ListNode *node);
void listInsertBegin(List *list, ListNode *node);
void listRemove(ListNode *node);
ListNode *listGetFirst(List *list);
uint32_t listGetItemCount(List *list);
int listIsEmpty(List *list);

#endif // LIST_H
