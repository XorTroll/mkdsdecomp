#ifndef _MKDS_MAIN_H
#define _MKDS_MAIN_H

#include <ntr/mem.h>

typedef struct GlobalHeapHolder {
    u32 unk_maybe_fn;
    void *some_ptr;
    void *heap_start_addr;
    Mem_HeapHandle global_heap_hnd;
    char *main_proc_str;
} GlobalHeapHolder;

Mem_HeapHandle GetGlobalHeapHandle(void);

#endif