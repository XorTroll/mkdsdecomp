#include <ntr/os.h>
#include <ntr/arm.h>
#include <ntr/bios.h>

// TODO: is this common or ARM9-specific?

// Note: this is technically an int
bool g_MemoryRegionsInitialized = false;

bool g_ForceDebugMode = false;

// TODO: this size probably was not a fixed constant, rather something subject to change...? maybe compiler-emitted?
int g_SomeUnknownSize = 0x2800;

u32 g_DebugFlags = UINT32_MAX;

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

    if(NTR_BIOS_ARM7_BIOS_CRC != 0) {
        return NTR_BIOS_DEBUG_SIO_EXISTS_COPY == 1;
    }
    else {
        return NTR_BIOS_DEBUG_SIO_EXISTS == 1;
    }
}

u32 Os_GetDebugFlags(void) {
    // Can't default them to zero
    if(g_DebugFlags == UINT32_MAX) {
        u32 gba_flags = 0;
        // u32 gba_flags = <get some flags related to what's in the gba slot>();

        u32 base_flags;
        if(false /* <some function used elsewhere that just returns false>() */) {
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
        g_DebugFlags = base_flags | NTR_BIOS_SWI_IS_DEBUGGER_ARM7_SCRATCH;
    }

    return g_DebugFlags;
}

void *Os_ComputeMemoryRegionStartAddress(Os_MemoryRegion region) {
    uintptr_t addr;

    // TODO: how many of these are compiler-emitted?
    
    switch(region) {
        case Os_MemoryRegion_MainRam:
            addr = 0x21DA340;
            break;
        case Os_MemoryRegion_Unknown2:
            if(g_ForceDebugMode && ((Os_GetDebugFlags() & 0b11) != 1)) {
                addr = 0x2400000;
            }
            else {
                addr = 0;
            }
            break;
        case Os_MemoryRegion_Unknown3:
            addr = 0x1FFFFA0;
            break;
        case Os_MemoryRegion_Unknown4:
            addr = 0x27E00E0;
            break;
        case Os_MemoryRegion_Unknown5:
            addr = 0x27FF000;
            break;
        case Os_MemoryRegion_Unknown6:
            addr = 0x37F8000;
            break;
        default:
            addr = 0;
            break;
    }

    return (void*)addr;
}

void *Os_ComputeMemoryRegionEndAddress(Os_MemoryRegion region) {
    uintptr_t addr;

    // TODO: how many of these are compiler-emitted?
    
    switch(region) {
        case Os_MemoryRegion_MainRam:
            addr = 0x23E0000;
            break;
        case Os_MemoryRegion_Unknown2:
            if(g_ForceDebugMode && ((Os_GetDebugFlags() & 0b11) != 1)) {
                addr = 0x2700000;
            }
            else {
                addr = 0;
            }
            break;
        case Os_MemoryRegion_Unknown3:
            addr = 0x2000000;
            break;
        case Os_MemoryRegion_Unknown4:
            addr = 0x27E0000;

            if(g_SomeUnknownSize != 0) {
                if(g_SomeUnknownSize < 0) {
                    addr = 0x27E00E0 - g_SomeUnknownSize;
                }
                else {
                    addr = 0x27E0000 + 0x3F80 - 0xC00 - g_SomeUnknownSize;
                }
            }
            // at least one of these has to be compiler-emitted, this makes no sense as fixed constants...
            else if(0x27E0000 < 0x27E00E0) {
                addr = 0x27E00E0;
            }
            break;
        case Os_MemoryRegion_Unknown5:
            addr = 0x27FF680;
            break;
        case Os_MemoryRegion_Unknown6:
            addr = 0x37F8000;
            break;
        default:
            addr = 0;
            break;
    }

    return (void*)addr;
}

// Technically the first list has a max size of 9 regions, but only indexes 0-6 are used
#define _NTR_OS_MEM_REGION_START_ADDRESS_LIST (void**)(0x27FFDA0)
#define _NTR_OS_MEM_REGION_END_ADDRESS_LIST (void**)(0x27FFDC4)

void Os_SetMemoryRegionStartAddress(Os_MemoryRegion region, void *addr) {
    (_NTR_OS_MEM_REGION_START_ADDRESS_LIST)[(int)region] = addr;
}

void Os_SetMemoryRegionEndAddress(Os_MemoryRegion region, void *addr) {
    (_NTR_OS_MEM_REGION_END_ADDRESS_LIST)[(int)region] = addr;
}

void *Os_GetMemoryRegionStartAddress(Os_MemoryRegion region) {
    return (_NTR_OS_MEM_REGION_START_ADDRESS_LIST)[(int)region];
}

void *Os_GetMemoryRegionEndAddress(Os_MemoryRegion region) {
    return (_NTR_OS_MEM_REGION_END_ADDRESS_LIST)[(int)region];
}

// These are defined in asm
void Os_SetSystemRomPU(u32 pu_config);
void Os_SetVectorBasePU(u32 pu_config);

// Yeah, N does it in this order :P
#define _NTR_OS_COMPUTE_SET_REGION(i) ({ \
    void *end_addr_##i = Os_ComputeMemoryRegionEndAddress(i); \
    Os_SetMemoryRegionEndAddress(i, end_addr_##i); \
    void *start_addr_##i = Os_ComputeMemoryRegionStartAddress(i); \
    Os_SetMemoryRegionStartAddress(i, start_addr_##i); \
})

void Os_InitializeMemoryRegionAddresses(void) {
    if(!g_MemoryRegionsInitialized) {
        g_MemoryRegionsInitialized = true;

        _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_MainRam);

        // This one is handled later separately
        Os_SetMemoryRegionStartAddress(Os_MemoryRegion_Unknown2, 0);
        Os_SetMemoryRegionEndAddress(Os_MemoryRegion_Unknown2, 0);

        _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Unknown3);
        _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Unknown4);
        _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Unknown5);
        _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Unknown6);
    }
}

void Os_InitializeMemoryRegionAddress2(void) {
    // For some reason this is set later
    _NTR_OS_COMPUTE_SET_REGION(Os_MemoryRegion_Unknown2);

    if(!g_ForceDebugMode || ((Os_GetDebugFlags() & 0b11) == 1)) {
        // I guess this is non-debug behaviour?
        // Correct main memory from debug 8MB to normal 4MB
        // Also change 0x027XXXXX to 0x023XXXXX for vector base address

        Os_SetSystemRomPU(0x02000000 | NTR_ARM_PAGE_4M | 1);
        Os_SetVectorBasePU(0x023C0000 | NTR_ARM_PAGE_128K | 1);
    }
}

void Os_Initialize(void) {
    Os_InitializeMemoryRegionAddresses();
    // TODO: <probably fifo init>();
    // TODO: <gamecard related init>();
    Os_InitializeMemoryRegionAddress2();

    // TODO: more functions are called here
}

void Os_DefaultDisableForceDebugMode(void) {
    g_ForceDebugMode = false;
}