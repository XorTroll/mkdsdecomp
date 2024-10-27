#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Os_SetSystemRomPU
    mcr p15, 0, r0, c6, c1, 0
    bx lr

NTR_BEGIN_ASM_FN Os_SetVectorBasePU
    mcr p15, 0, r0, c6, c2, 0
    bx lr

NTR_BEGIN_ASM_FN Os_Halt
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
