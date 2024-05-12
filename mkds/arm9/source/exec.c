#include "exec.h"
#include "main.h"

// TODO: BSS? where is this NULL'd on startup?
ExecutionContext *g_ActiveExecutionContext;

void ExecutionContext_Create(ExecutionContext *ctx, ExecutionContextFunction fn_ptr) {
    ctx->unk_unused = 0;
    ctx->exec_fn_ptr = fn_ptr;
    ctx->unused_fn = NULL;
    ctx->prev_active_ctx = NULL;
    ctx->cur_heap_hnd = NULL;
}

Mem_HeapHandle ExecutionContext_GetHeapHandle(ExecutionContext *ctx) {
    // This could easily be inlined...
    return ctx->cur_heap_hnd;
}

void ExecutionContext_Execute(ExecutionContext *ctx, void *fn_arg) {
    ctx->prev_active_ctx = g_ActiveExecutionContext;

    // Prepare heap
    Mem_HeapHandle base_heap_hnd;
    if(ctx->prev_active_ctx != NULL) {
        base_heap_hnd = ctx->prev_active_ctx->cur_heap_hnd;
    }
    else {
        base_heap_hnd = GetGlobalHeapHandle();
    }
    ctx->cur_heap_hnd = Mem_CreateChildExpHeap(base_heap_hnd);

    // Execute
    g_ActiveExecutionContext = ctx;
    ctx->exec_fn_ptr(fn_arg);

    if(ctx->unused_fn != NULL) {
        ctx->unused_fn();
    } 

    // Restore previous context and dispose used heap
    g_ActiveExecutionContext = ctx->prev_active_ctx;
    Mem_DestroyHeap(ctx->cur_heap_hnd);
    ctx->cur_heap_hnd = NULL;
}

void ExecutionContext_SetFunction(ExecutionContext *ctx, ExecutionContextFunction fn_ptr) {
    ctx->exec_fn_ptr = fn_ptr;
}

ExecutionContext *GetActiveExecutionContext(void) {
    return g_ActiveExecutionContext;
}
