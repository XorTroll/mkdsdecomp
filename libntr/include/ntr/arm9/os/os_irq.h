#ifndef _NTR_ARM9_OS_IRQ_H
#define _NTR_ARM9_OS_IRQ_H

#include <ntr/arm9/mem/mem_addr.h>

typedef enum Os_IrqType {
    Os_IrqType_VBlank = 0,
    Os_IrqType_HBlank = 1,
    Os_IrqType_VCounterMatch = 2,
    Os_IrqType_Timer_0 = 3,
    Os_IrqType_Timer_1 = 4,
    Os_IrqType_Timer_2 = 5,
    Os_IrqType_Timer_3 = 6,
    Os_IrqType_DmaChannel_0 = 8,
    Os_IrqType_DmaChannel_1 = 9,
    Os_IrqType_DmaChannel_2 = 10,
    Os_IrqType_DmaChannel_3 = 11,
    Os_IrqType_Keypad = 12,
    Os_IrqType_GbaSlot = 13,
    // ...
    Os_IrqType_IpcSync = 16,
    Os_IrqType_IpcSend = 17,
    Os_IrqType_IpcReceive = 18,
    Os_IrqType_CardTransferCompletion = 19,
    Os_IrqType_CardIreqMc = 20,
    Os_IrqType_GeometryCommand = 21,
    Os_IrqType_Count
} Os_IrqType;

typedef enum Os_IrqFlag {
    Os_IrqFlag_VBlank = NTR_BIT(0),
    Os_IrqFlag_HBlank = NTR_BIT(1),
    Os_IrqFlag_VCounterMatch = NTR_BIT(2),
    Os_IrqFlag_Timer_0 = NTR_BIT(3),
    Os_IrqFlag_Timer_1 = NTR_BIT(4),
    Os_IrqFlag_Timer_2 = NTR_BIT(5),
    Os_IrqFlag_Timer_3 = NTR_BIT(6),
    Os_IrqFlag_DmaChannel_0 = NTR_BIT(8),
    Os_IrqFlag_DmaChannel_1 = NTR_BIT(9),
    Os_IrqFlag_DmaChannel_2 = NTR_BIT(10),
    Os_IrqFlag_DmaChannel_3 = NTR_BIT(11),
    Os_IrqFlag_Keypad = NTR_BIT(12),
    Os_IrqFlag_GbaSlot = NTR_BIT(13),
    // ...
    Os_IrqFlag_IpcSync = NTR_BIT(16),
    Os_IrqFlag_IpcSend = NTR_BIT(17),
    Os_IrqFlag_IpcReceive = NTR_BIT(18),
    Os_IrqFlag_CardTransferCompletion = NTR_BIT(19),
    Os_IrqFlag_CardIreqMc = NTR_BIT(20),
    Os_IrqFlag_GeometryCommand = NTR_BIT(21)
} Os_IrqFlag;

typedef void (*Os_IrqHandlerFn)(void);
typedef void (*Os_IrqSpecialHandlerFn)(void*);

u32 Os_EnableIrq(void);
u32 Os_DisableIrq(void);
void Os_RestoreIrq(u32 prev_state);

u32 Os_DisableIrqFiq(void);
void Os_RestoreIrqFiq(u32 prev_state);

void Os_EnableIrqHandler(Os_IrqFlag irq_flag);
void Os_DisableIrqHandler(Os_IrqFlag irq_flag);

void Os_SetIrqHandler(Os_IrqFlag irq_flag, Os_IrqHandlerFn fn);
Os_IrqHandlerFn Os_GetIrqHandler(Os_IrqFlag irq_flag);

void Os_SetTimerIrqHandler(u8 timer_no, Os_IrqSpecialHandlerFn fn, void *arg);
void Os_SetDmaIrqHandler(u8 timer_no, Os_IrqSpecialHandlerFn fn, void *arg);

inline void Os_SetIrqCheckFlag(Os_IrqFlag irq_flag) {
    NTR_ARM9_MEM_ADDR_DTCM_INTERRUPT_CHECK |= irq_flag;
}

#endif
