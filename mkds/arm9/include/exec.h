#define _MKDS_EXEC_H

#include "base.h"

typedef void (*ExecutionContextFunction)(void*);

// This execution logic is used to "sandbox" the heap as a child heap, then dispose it after the execution's done

typedef struct ExecutionContext {
    u32 unk_unused;
    ExecutionContextFunction exec_fn_ptr;
    void (*unused_fn)(void);
    Mem_HeapHandle cur_heap_hnd;
    struct ExecutionContext *prev_active_ctx;
} ExecutionContext;

void ExecutionContext_Create(ExecutionContext *ctx, ExecutionContextFunction fn_ptr);

Mem_HeapHandle ExecutionContext_GetHeapHandle(ExecutionContext *ctx);
void ExecutionContext_Execute(ExecutionContext *ctx, void *fn_arg);
void ExecutionContext_SetFunction(ExecutionContext *ctx, ExecutionContextFunction fn_ptr);

ExecutionContext *GetActiveExecutionContext(void);