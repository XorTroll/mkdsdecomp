#include "base.asm.h"

.text
.arm

MKDS_BEGIN_ASM_FN Os_DisableIRQ
    MRS R0, CPSR
    ORR R1, R0, #0x80
    MSR CPSR_c, R1
    AND R0, R0, #0x80
    BX LR

MKDS_BEGIN_ASM_FN Os_RestoreIRQ
    MRS R1, CPSR
    BIC R2, R1, #0x80
    ORR R2, R2, R0
    MSR CPSR_c, R2
    AND R0, R1, #0x80
    BX LR