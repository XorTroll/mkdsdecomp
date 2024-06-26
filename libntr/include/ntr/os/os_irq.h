#ifndef _NTR_OS_IRQ_H
#define _NTR_OS_IRQ_H

#include <ntr/base.h>

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

void Os_EnableIrqHandler(Os_IrqFlag irq_flag);
void Os_DisableIrqHandler(Os_IrqFlag irq_flag);

#endif
