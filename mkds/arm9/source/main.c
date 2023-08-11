#include <ntr/os.h>
#include "exec.h"
#include "main.h"

GlobalHeapHolder g_GlobalHeapHolder;

ExecutionContext g_StartExecutionContext;

Mem_HeapHandle GetGlobalHeapHandle() {
    return g_GlobalHeapHolder.global_heap_hnd;
}

void MemoryRegionAddressAccess() {
    // No clue about the purpose of this, but it's called
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam);
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Unknown3);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Unknown3);
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Unknown4);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Unknown4);
}

void InitializeSomeStuff() {
    Os_DefaultDisableForceDebugMode();
    Os_Initialize();
    // TODO: much more to add here
}

void InitializeEverything() {
    InitializeSomeStuff();

    // <screen brightness setup>();

    g_GlobalHeapHolder.some_ptr = (void*)0x02000000;
    g_GlobalHeapHolder.heap_start_addr = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);

    MemoryRegionAddressAccess();

    void *heap_start = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);
    void *heap_end = Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam);
    g_GlobalHeapHolder.global_heap_hnd = Mem_CreateExpHeap(heap_start, heap_end - heap_start);

    ExecutionContext_Create(&g_StartExecutionContext, NULL);

    // This string appears to be unused
    g_GlobalHeapHolder.main_proc_str = "MainProc";

    // <function that inits some object>();
}

void SetExecuteStart(ExecutionContextFunction start_fn) {
    ExecutionContext_SetFunction(&g_StartExecutionContext, start_fn);

    // Unused dummy argument
    ExecutionContext_Execute(&g_StartExecutionContext, (void*)0xBEEF);
}

void StartExecuteFunction(void*) {
    // TODO...
    while(1);
} 

void NtrMain(void) {
    InitializeEverything();
    SetExecuteStart(StartExecuteFunction);
}