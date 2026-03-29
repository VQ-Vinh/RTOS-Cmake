#include "list.h"
#include <stddef.h>

void listInit(List *list) {
    list->head.next = &list->head;
    list->head.prev = &list->head;
    list->itemCount = 0;
}

void listInsertEnd(List *list, ListNode *node) {
    node->next = &list->head;
    node->prev = list->head.prev;
    list->head.prev->next = node;
    list->head.prev = node;
    list->itemCount++;
}

void listInsertBegin(List *list, ListNode *node) {
    node->next = list->head.next;
    node->prev = &list->head;
    list->head.next->prev = node;
    list->head.next = node;
    list->itemCount++;
}

void listRemove(ListNode *node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

ListNode *listGetFirst(List *list) {
    if (list->itemCount == 0) return NULL;
    return list->head.next;
}

uint32_t listGetItemCount(List *list) {
    return list->itemCount;
}

int listIsEmpty(List *list) {
    return list->itemCount == 0;
}
