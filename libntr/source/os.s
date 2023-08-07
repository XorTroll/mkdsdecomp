#include "base_asm.h"

.text
.arm

NTR_BEGIN_ASM_FN Os_DisableIRQ
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr

NTR_BEGIN_ASM_FN Os_RestoreIRQ
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr