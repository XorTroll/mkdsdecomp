#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Mem_CpuFill8
    bxeq lr
    tst r0, #1
    beq loc_214D194
    ldrh r12, [r0, #-1]
    and r12, r12, #0xFF
    orr r3, r12, r1, lsl#8
    strh r3, [r0, #-1]
    add r0, r0, #1
    subs r2, r2, #1
    bxeq lr
loc_214D194:
    cmp r2, #2
    bcc loc_214D1DC
    orr r1, r1, r1, lsl#8
    tst r0, #2
    beq loc_214D1B4
    strh r1, [r0], #2
    subs r2, r2, #2
    bxeq lr
loc_214D1B4:
    orr r1, r1, r1, lsl#0x10
    bics  r3, r2, #3
    beq loc_214D1D4
    sub r2, r2, r3
    add r12, r3, r0
loc_214D1C8:
    str r1, [r0], #4
    cmp r0, r12
    bcc loc_214D1C8
loc_214D1D4:
    tst r2, #2
    strneh r1, [r0], #2
loc_214D1DC:
    tst r2, #1
    bxeq lr
    ldrh r3, [r0]
    and r3, r3, #0xFF00
    and r1, r1, #0xFF
    orr r1, r1, r3
    strh r1, [r0]
    bx lr
