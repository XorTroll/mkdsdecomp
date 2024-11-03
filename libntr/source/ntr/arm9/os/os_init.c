#include <ntr/arm9/os/os_init.h>
#include <ntr/arm9/os/os_valarm.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/arm9/os/os_lock.h>
#include <ntr/arm9/mem/mem_init.h>
#include <ntr/arm9/arm/arm.asm.h>
#include <ntr/arm9/bios.h>
#include <ntr/extra/extra_log.h>

int g_MainExRegionEnabled = false;

u32 g_DebugFlags = UINT32_MAX;

static NTR_ARM9_MEM_ADDR_DTCM Os_ThreadQueue g_IrqThreadQueue;

typedef enum Os_GbaSlotFlag {
    Os_GbaSlotFlag_MagicNINTENDO = NTR_BIT(24),
    Os_GbaSlotFlag_NoMagicNINTENDO = NTR_BIT(25)
} Os_GbaSlotFlag;

typedef enum Os_DebugFlag {
    Os_DebugFlag_IsDebugger = NTR_BIT(0),
    Os_DebugFlag_Unk1 = NTR_BIT(28),
    Os_DebugFlag_GbaSlotMagicNINTENDO = NTR_BIT(29),
    Os_DebugFlag_SioConnection = NTR_BIT(30),
    Os_DebugFlag_Unk3 = NTR_BIT(31)
} Os_DebugFlag;

bool Os_ExistsSioConnection(void) {
    // Check if the ARM7 BIOS CRC value is set by the BIOS
    // Depending on that, check one address or the copy?
    // I guess that depending on ARM7/9 the CRC value will be set or not

    if(NTR_ARM9_BIOS_SHARED_REGION->nds7_bios_crc != 0) {
        return NTR_ARM9_BIOS_SHARED_REGION->sio_debug_connection_exists_copy == 1;
    }
    else {
        return NTR_ARM9_BIOS_SHARED_REGION->sio_debug_connection_exists == 1;
    }
}

bool Os_IsEmulator(void) {
    return false;
}

u32 Os_GetDebugFlags(void) {
    // Can't default them to zero
    if(g_DebugFlags == UINT32_MAX) {
        u32 gba_flags = 0;
        // u32 gba_flags = <get some flags related to what's in the gba slot>();

        u32 base_flags;
        if(Os_IsEmulator()) {
            base_flags = gba_flags | Os_DebugFlag_Unk1;
        }
        else if(Os_ExistsSioConnection()) {
            base_flags = gba_flags | Os_DebugFlag_SioConnection;
        }
        else if(gba_flags & Os_GbaSlotFlag_MagicNINTENDO) {
            base_flags = gba_flags | Os_DebugFlag_GbaSlotMagicNINTENDO;
        }
        else {
            base_flags = gba_flags | Os_DebugFlag_Unk3;
        }

        // Thus, bit0 == IsDebugger
        g_DebugFlags = base_flags | NTR_ARM9_BIOS_SHARED_REGION->nds7_swi_is_debugger_check_scratch_addr;
    }

    return g_DebugFlags;
}

void Os_InitializeSvcStack(void) {
    // Set stack magics at the stack top/bottom
    *(u32*)(NTR_ARM9_MEM_ADDR_DTCM_IRQ_STACK_BOTTOM - sizeof(u32)) = NTR_ARM9_OS_THREAD_STACK_BOTTOM_MAGIC;
    *(u32*)(NTR_ARM9_MEM_ADDR_DTCM_IRQ_STACK_TOP) = NTR_ARM9_OS_THREAD_STACK_TOP_MAGIC;
}

void Os_InitializeIrqThreadQueue(void) {
    Os_ThreadQueue_Initialize(&g_IrqThreadQueue);
}

void *Os_ComputeMemoryRegionStartAddress(Os_MemoryRegion region) {
    uintptr_t addr;

    switch(region) {
        case Os_MemoryRegion_MainRam:
            addr = NTR_ARM9_MEM_ADDR_MAIN_RAM_START;
            break;
        case Os_MemoryRegion_MainRamEx:
            if(g_MainExRegionEnabled && ((Os_GetDebugFlags() & 0b11) != 1)) {
                addr = NTR_ARM9_MEM_ADDR_MAIN_RAM_EX_START;
            }
            else {
                addr = 0;
            }
            break;
        case Os_MemoryRegion_Itcm:
            addr = NTR_ARM9_MEM_ADDR_ITCM_START;
            break;
        case Os_MemoryRegion_Dtcm:
            addr = NTR_ARM9_MEM_ADDR_DTCM_AUTOLOAD_START;
            break;
        case Os_MemoryRegion_Shared:
            addr = NTR_ARM9_MEM_ADDR_SHARED_START;
            break;
        case Os_MemoryRegion_WramMain:
            addr = NTR_ARM9_MEM_ADDR_WRAM_MAIN_START;
            break;
        default:
            addr = 0;
            break;
    }

    return (void*)addr;
}

void *Os_ComputeMemoryRegionEndAddress(Os_MemoryRegion region) {
    uintptr_t addr;
    
    switch(region) {
        case Os_MemoryRegion_MainRam:
            addr = NTR_ARM9_MEM_ADDR_MAIN_RAM_END;
            break;
        case Os_MemoryRegion_MainRamEx:
            if(g_MainExRegionEnabled && ((Os_GetDebugFlags() & 0b11) != 1)) {
                addr = NTR_ARM9_MEM_ADDR_MAIN_RAM_EX_END;
            }
            else {
                addr = 0;
            }
            break;
        case Os_MemoryRegion_Itcm:
            addr = NTR_ARM9_MEM_ADDR_ITCM_END;
            break;
        case Os_MemoryRegion_Dtcm:
            addr = NTR_ARM9_MEM_ADDR_DTCM_START;
            if(NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE != 0) {
                if(NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE < 0) { // TODO: what does it even mean to have a negative stack size value?
                    addr = NTR_ARM9_MEM_ADDR_DTCM_AUTOLOAD_START - NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE;
                }
                else {
                    addr = NTR_ARM9_MEM_ADDR_DTCM_IRQ_STACK_TOP - NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE;
                }
            }
            else if(NTR_ARM9_MEM_ADDR_DTCM_AUTOLOAD_START > NTR_ARM9_MEM_ADDR_DTCM_START) {
                addr = NTR_ARM9_MEM_ADDR_DTCM_AUTOLOAD_START;
            }
            break;
        case Os_MemoryRegion_Shared:
            addr = NTR_ARM9_MEM_ADDR_SHARED_END;
            break;
        case Os_MemoryRegion_WramMain:
            addr = NTR_ARM9_MEM_ADDR_WRAM_MAIN_END;
            break;
        default:
            addr = 0;
            break;
    }

    return (void*)addr;
}

// Technically the first list has a max size of 9 regions, but only indexes 0-6 are used (here)

void Os_SetMemoryRegionStartAddress(Os_MemoryRegion region, void *addr) {
    NTR_ARM9_BIOS_SHARED_REGION->mem_regions_start[(int)region] = addr;
}

void Os_SetMemoryRegionEndAddress(Os_MemoryRegion region, void *addr) {
    NTR_ARM9_BIOS_SHARED_REGION->mem_regions_end[(int)region] = addr;
}

void *Os_GetMemoryRegionStartAddress(Os_MemoryRegion region) {
    return NTR_ARM9_BIOS_SHARED_REGION->mem_regions_start[(int)region];
}

void *Os_GetMemoryRegionEndAddress(Os_MemoryRegion region) {
    return NTR_ARM9_BIOS_SHARED_REGION->mem_regions_end[(int)region];
}

// These are defined in asm

void Os_SetSystemRomPU(u32 pu_config);
void Os_SetVectorBasePU(u32 pu_config);

// Yes, N does it in this order
#define _NTR_ARM9_OS_COMPUTE_SET_REGION(i) ({ \
    void *end_addr_##i = Os_ComputeMemoryRegionEndAddress(i); \
    Os_SetMemoryRegionEndAddress(i, end_addr_##i); \
    void *start_addr_##i = Os_ComputeMemoryRegionStartAddress(i); \
    Os_SetMemoryRegionStartAddress(i, start_addr_##i); \
})

void Os_InitializeMemoryRegionAddresses(void) {
    static int g_MemoryRegionsInitialized = false;
    if(!g_MemoryRegionsInitialized) {
        g_MemoryRegionsInitialized = true;

        _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_MainRam);

        // This one is handled later separately
        Os_SetMemoryRegionStartAddress(Os_MemoryRegion_MainRamEx, 0);
        Os_SetMemoryRegionEndAddress(Os_MemoryRegion_MainRamEx, 0);

        _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Itcm);
        _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Dtcm);
        _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Shared);
        _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_WramMain);
    }
}

void Os_InitializeMainExMemoryRegionAddress(void) {
    // For some reason this is set later... maybe wait until dev has specified whether it is enabled or not?
    _NTR_ARM9_OS_COMPUTE_SET_REGION(Os_MemoryRegion_MainRamEx);

    if(!g_MainExRegionEnabled || ((Os_GetDebugFlags() & 0b11) == 1)) {
        // Adapt permissions for not using MainEx?
        Os_SetSystemRomPU(NTR_ARM9_MEM_ADDR_MAIN_CODE_START | NTR_ARM_PAGE_4M | 1);
        Os_SetVectorBasePU(NTR_ARM9_MEM_ADDR_MAIN_RAM_END | NTR_ARM_PAGE_128K | 1);
    }
}

void Os_Initialize(void) {
    Os_InitializeMemoryRegionAddresses();
    // TODO: <fifo related init>();
    Os_InitializeLock();
    Os_InitializeMainExMemoryRegionAddress();
    Os_InitializeIrqThreadQueue();
    Os_InitializeSvcStack();
    // TODO: <init exception handler>();
    Mem_Initialize();
    Os_InitializeVAlarm();
    // TODO: <some vram init>();
    Os_InitializeThread();
    // TODO: <some other fifo init>();
    // TODO: <cart init>();
    // TODO: <another cart init>();
    // TODO: <yet another fifo init>();
}

void Os_DisableMainExRegion(void) {
    g_MainExRegionEnabled = false;
}

