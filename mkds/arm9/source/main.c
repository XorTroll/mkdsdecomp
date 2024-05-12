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

    // <screen brightness setup>();

    g_GlobalHeapHolder.some_ptr = (void*)(0x02000000);
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

extern void PrintInit();
extern void PrintString(size_t len, const char *str);

void SetExecuteStart(ExecutionContextFunction start_fn) {
    ExecutionContext_SetFunction(&g_StartExecutionContext, start_fn);

    // Unused dummy argument
    ExecutionContext_Execute(&g_StartExecutionContext, (void*)0xBEEF);
}

#define LOOP_PRINT(str) \
    PrintInit(); \
    PrintString(__builtin_strlen(str), str); \
    while(1);

void NtrMain(void) {
    InitializeEverything();
    SetExecuteStart(StartExecuteFunction);
}
