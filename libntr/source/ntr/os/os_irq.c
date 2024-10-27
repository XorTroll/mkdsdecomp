#include <ntr/os/os_irq.h>
#include <ntr/mem/mem_reg.h>
#include <ntr/mem/mem_addr.h>

// Special handlers: first 4 for DMA IRQs, last 4 for timer IRQs

typedef struct Os_IrqSpecialHandlerEntry {
    Os_IrqHandlerFn fn;
    bool is_enabled;
    void *fn_arg;
} Os_IrqSpecialHandlerEntry;

static Os_IrqSpecialHandlerEntry g_IrqSpecialHandlerEntries[8];

static NTR_MEM_ADDR_DTCM Os_IrqHandlerFn g_IrqHandlerFns[Os_IrqType_Count];

void Os_EnableIrqHandler(Os_IrqFlag irq_flag) {
    u16 old_ime_val = NTR_MEM_REG_IO_IME;
    NTR_MEM_REG_IO_IME = 0;
    NTR_MEM_REG_IO_IE |= irq_flag;
    NTR_MEM_REG_IO_IME = old_ime_val;
}

void Os_DisableIrqHandler(Os_IrqFlag irq_flag) {
    u16 old_ime_val = NTR_MEM_REG_IO_IME;
    NTR_MEM_REG_IO_IME = 0;
    NTR_MEM_REG_IO_IE &= ~irq_flag;
    NTR_MEM_REG_IO_IME = old_ime_val;
}

static inline bool Os_IsDmaIrqType(Os_IrqType type) {
    return (type >= Os_IrqType_DmaChannel_0) && (type <= Os_IrqType_DmaChannel_3);
}

static inline Os_IrqSpecialHandlerEntry *Os_GetDmaIrqSpecialHandlerEntry(Os_IrqType dma_type) {
    // Slots 0-3, hence first 4 special entries (type is some value 8-11)
    return &g_IrqSpecialHandlerEntries[dma_type - 8];
}

static inline bool Os_IsTimerIrqType(Os_IrqType type) {
    return (type >= Os_IrqType_Timer_0) && (type <= Os_IrqType_Timer_3);
}

static inline Os_IrqSpecialHandlerEntry *Os_GetTimerIrqSpecialHandlerEntry(Os_IrqType timer_type) {
    // Slots 4-7, hence last 4 special entries (type is some value 3-6)
    return &g_IrqSpecialHandlerEntries[timer_type + 1];
}

void Os_SetIrqHandler(Os_IrqFlag irq_flag, Os_IrqHandlerFn fn) {
    int type = 0;
    do {
        if((irq_flag & 1) != 0) {
            Os_IrqSpecialHandlerEntry *special_entry_p = NULL;

            if(Os_IsDmaIrqType(type)) {
                special_entry_p = Os_GetDmaIrqSpecialHandlerEntry(type);
            }
            else {
                if(Os_IsTimerIrqType(type)) {
                    special_entry_p = Os_GetTimerIrqSpecialHandlerEntry(type);
                }
                else {
                    g_IrqHandlerFns[type] = fn;
                }
            }

            if(special_entry_p != NULL) {
                special_entry_p->fn = fn;
                special_entry_p->is_enabled = true;
                special_entry_p->fn_arg = NULL;
            }
        }
        ++type;
        irq_flag >>= 1;
    } while(type < Os_IrqType_Count);
}

Os_IrqHandlerFn Os_GetIrqHandler(Os_IrqFlag irq_flag) {
    int type = 0;
    while((irq_flag & 1) == 0) {
        ++type;
        irq_flag >>= 1;

        if(type >= Os_IrqType_Count) {
            return NULL;
        }
    }

    if(Os_IsDmaIrqType(type)) {
        return Os_GetDmaIrqSpecialHandlerEntry(type)->fn;
    }
    else if(Os_IsTimerIrqType(type)) {
        return Os_GetTimerIrqSpecialHandlerEntry(type)->fn;
    }
    else {
        return g_IrqHandlerFns[type];
    }
}

