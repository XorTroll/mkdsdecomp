#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Arm_FlushRange
    mov r12, #0
    add r1, r1, r0
    bic r0, r0, #0x1F
FlushLoop:
    mcr p15, 0, r12, c7, c10, 4
    mcr p15, 0, r0, c7, c14, 1
    add r0, r0, #0x20
    cmp r0, r1
    blt FlushLoop
    bx lr