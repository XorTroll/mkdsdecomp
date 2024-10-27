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
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Itcm);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Itcm);
    Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Dtcm);
    Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Dtcm);
}

void VblankIrqHandler() {
    // TODO
}

void InitializeSDKModules(void) {
    Os_DisableMainExRegion();
    Os_Initialize();

    // TODO: <some obj value set>();
    // TODO: <os init tick>();
    Os_InitializeAlarm();
    // TODO: <math init>();
    // TODO: <gfx 2d init>();
    // TODO: <some vram init>();
    // TODO: <more vram stuff>();
    // TODO: <more gfx stuff>();
    
    NTR_MEM_REG_EBOT_DISPCNT &= ~NTR_BIT(16); // Remove green-swap bit (seems unused anyway?)
    Os_SetIrqHandler(Os_IrqFlag_VBlank, VblankIrqHandler);
    Os_EnableIrqHandler(Os_IrqFlag_VBlank);
    NTR_MEM_REG_IO_IME = 1;
    Os_EnableIrq();

    // TODO: <some gfx vblank stuff>();
    // TODO: <fs init>();
    // TODO: <some unk obj init>();
    // TODO: <maybe dwc init>();
    // TODO: <and more vram stuff>();
}

void InitializeEverything(void) {
    InitializeSDKModules();
    NTR_EXTRA_DEBUGLOGF("InitializeSDKModules done");

    NTR_EXTRA_DEBUGLOGF("os_MainRam: start=%p, end=%p", Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam), Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam));
    NTR_EXTRA_DEBUGLOGF("os_ITCM: start=%p, end=%p", Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Itcm), Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Itcm));
    NTR_EXTRA_DEBUGLOGF("os_DTCM: start=%p, end=%p", Os_GetMemoryRegionStartAddress(Os_MemoryRegion_Dtcm), Os_GetMemoryRegionEndAddress(Os_MemoryRegion_Dtcm));

    // TODO: <screen brightness contexts setup>();

    g_GlobalHeapHolder.some_ptr = NTR_MEM_ADDR_MAIN_CODE_START;
    g_GlobalHeapHolder.heap_start_addr = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);

    MemoryRegionAddressAccess();

    void *heap_start = Os_GetMemoryRegionStartAddress(Os_MemoryRegion_MainRam);
    void *heap_end = Os_GetMemoryRegionEndAddress(Os_MemoryRegion_MainRam);
    NTR_EXTRA_DEBUGLOGF("Global heap: start=%p, end=%p, size=0x%X", heap_start, heap_end, heap_end - heap_start);
    g_GlobalHeapHolder.global_heap_hnd = Mem_CreateExpHeap(heap_start, heap_end - heap_start);

    ExecutionContext_Create(&g_StartExecutionContext, NULL);

    NTR_EXTRA_DEBUGLOGF("ExecutionContext created");

    // This string appears to be unused
    g_GlobalHeapHolder.main_proc_str = "MainProc";

    // <function that inits some object>();
}

void SetExecuteStart(ExecutionContextFunction start_fn) {
    ExecutionContext_SetFunction(&g_StartExecutionContext, start_fn);

    NTR_EXTRA_DEBUGLOGF("Jumping to ExecutionContext!");

    // Unused dummy argument
    ExecutionContext_Execute(&g_StartExecutionContext, (void*)0xBEEF);
}

void NtrMain(void) {
    NTR_EXTRA_DEBUGLOGF("Hello world from MKDS!");

    NTR_EXTRA_DEBUGLOGF("MainRam: start=%p, end=%p, size=0x%X", (void*)NTR_MEM_ADDR_MAIN_RAM_START, (void*)NTR_MEM_ADDR_MAIN_RAM_END, NTR_MEM_ADDR_MAIN_RAM_SIZE);
    NTR_EXTRA_DEBUGLOGF("ITCM: start=%p, end=%p, size=0x%X", (void*)NTR_MEM_ADDR_ITCM_START, (void*)NTR_MEM_ADDR_ITCM_END, NTR_MEM_ADDR_ITCM_SIZE);
    NTR_EXTRA_DEBUGLOGF("DTCM: start=%p, end=%p, size=0x%X", (void*)NTR_MEM_ADDR_DTCM_START, (void*)NTR_MEM_ADDR_DTCM_END, NTR_MEM_ADDR_DTCM_SIZE);

    InitializeEverything();
    NTR_EXTRA_DEBUGLOGF("InitializeEverything done");
    SetExecuteStart(StartExecuteFunction);

    // This should not happen in the finished game, but while most is missing, hitting this point means my current work succeeds and I haven't messed up anything :P
    NTR_EXTRA_DEBUGLOGF("NtrMain ended!");
    while(true);
}
