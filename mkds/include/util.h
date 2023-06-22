#ifndef _MKDS_UTIL_H
#define _MKDS_UTIL_H

#include "base.h"

#define MKDS_UTIL_ALIGN_UP(val, align) (((val) + (align - 1)) & ~(align - 1))
#define MKDS_UTIL_ALIGN_DOWN(val, align) ((val) & ~(align - 1))

typedef struct Util_IntrusiveListNode {
    void *item_prev;
    void *item_next;
} Util_IntrusiveListNode;

typedef struct Util_IntrusiveList {
    void *item_list_head;
    void *item_list_tail;
    u16 item_count;
    u16 item_list_node_offset;
} Util_IntrusiveList;

void Util_FillMemory32(int val, void *src, size_t size);

void Util_IntrusiveList_Initialize(Util_IntrusiveList *self, u16 item_list_node_offset);

void *Util_IntrusiveList_GetPrevItem(Util_IntrusiveList *self, void *item);
void *Util_IntrusiveList_GetNextItem(Util_IntrusiveList *self, void *item);
void *Util_IntrusiveList_GetAtIndex(Util_IntrusiveList *self, int idx);

void Util_IntrusiveList_InsertFirst(Util_IntrusiveList *self, void *item);
void Util_IntrusiveList_InsertLast(Util_IntrusiveList *self, void *item);
void Util_IntrusiveList_RemoveItem(Util_IntrusiveList *self, void *item);

#endif