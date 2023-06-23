#ifndef _MKDS_MEM_H
#define _MKDS_MEM_H

#include "base.h"

typedef struct Mem_HeapHead Mem_HeapHead;
typedef Mem_HeapHead *Mem_HeapHandle;

Mem_HeapHandle Mem_CreateExpHeap(void *ptr, size_t size);
void Mem_DestroyHeap(Mem_HeapHandle heap_handle);

void *Mem_AllocateHeap(Mem_HeapHandle heap_handle, size_t size);
void *Mem_AllocateHeapAligned(Mem_HeapHandle heap_handle, size_t size, ssize_t align);
void Mem_FreeHeap(Mem_HeapHandle heap_handle, void *ptr);

void Mem_SetExpHeapGroupId(Mem_HeapHandle heap_handle, u16 group_id);
size_t Mem_GetExpHeapTotalFreeSize(Mem_HeapHandle heap_handle);
void Mem_ResizeExpHeap(Mem_HeapHandle heap_handle, void *ptr, size_t size);

int Mem_CreateFrameHeapState(Mem_HeapHandle heap_handle, u32 state_id);
int Mem_RestoreFrameHeapState(Mem_HeapHandle heap_handle, u32 state_id);

Mem_HeapHandle Mem_CreateChildExpHeap(Mem_HeapHandle parent_heap_handle);
Mem_HeapHandle Mem_CreateChildFrameHeapFromHead(Mem_HeapHandle parent_heap_handle, size_t size);
Mem_HeapHandle Mem_CreateChildFrameHeapFromTail(Mem_HeapHandle parent_heap_handle, size_t size);

#endif