#include <ntr/os.h>
#include "exec.h"
#include "main.h"
#include "start.h"

GlobalHeapHolder g_GlobalHeapHolder;

ExecutionContext g_StartExecutionContext;

Mem_HeapHandle GetGlobalHeapHandle(void) {
    return g_GlobalHeapHolder.global_heap_hnd;
}

void MemoryRegionAddressAccess(void) {
    // No clue about the purpose of this, but it's called
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam);
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Unknown3);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Unknown3);
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Unknown4);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Unknown4);
}

void InitializeSomeStuff(void) {
    Os_DefaultDisableForceDebugMode();
    Os_Initialize();
    // TODO: much more to add here
}

void InitializeEverything(void) {
    InitializeSomeStuff();
    DebugLog("**** InitializeSomeStuff done\n");

    // <screen brightness setup>();

    g_GlobalHeapHolder.some_ptr = (void*)(0x02000000);
    g_GlobalHeapHolder.heap_start_addr = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);

    MemoryRegionAddressAccess();
    DebugLog("**** MemoryRegionAddressAccess done\n");

    void *heap_start = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);
    void *heap_end = Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam);
    g_GlobalHeapHolder.global_heap_hnd = Mem_CreateExpHeap(heap_start, heap_end - heap_start);

    DebugLog("**** Mem_CreateExpHeap done\n");

    ExecutionContext_Create(&g_StartExecutionContext, NULL);

    DebugLog("**** ExecutionContext_Create done\n");

    // This string appears to be unused
    g_GlobalHeapHolder.main_proc_str = "MainProc";

    // <function that inits some object>();
}

void SetExecuteStart(ExecutionContextFunction start_fn) {
    ExecutionContext_SetFunction(&g_StartExecutionContext, start_fn);

    DebugLog("**** Jumping to BEEF!\n");

    // Unused dummy argument
    ExecutionContext_Execute(&g_StartExecutionContext, (void*)0xBEEF);
}

void NtrMain(void) {
    DebugLog("**** Hello world!\n");
    InitializeEverything();
    DebugLog("**** InitializeEverything done\n");
    SetExecuteStart(StartExecuteFunction);
}
