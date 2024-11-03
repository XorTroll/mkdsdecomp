#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Arm_MathContext_Load
    ldr r12, [r0]
    ldr r3, [r0, #4]
    ldr r2, =0x04000290 @ REG_IO_DIV_NUMER
    ldr r1, =0x04000298 @ REG_IO_DIV_DENOM
    str r12, [r2]
    str r3, [r2, #4]
    ldr r12, [r0, #0x8]
    ldr r2, [r0, #0xC]
    ldr r3, =0x04000280 @ REG_IO_DIVCNT
    str r12, [r1]
    str r2, [r1, #4]
    ldrh r12, [r0, #0x18]
    ldr r2, =0x040002B8 @ REG_IO_SQRTPARAM
    ldr r1, =0x040002B0 @ REG_IO_SQRTCNT
    strh r12, [r3]
    ldr r12, [r0, #0x10]
    ldr r3, [r0, #0x14]
    str r12, [r2]
    str r3, [r2, #4]
    ldrh r0, [r0, #0x1A]
    strh r0, [r1]
    bx lr

NTR_BEGIN_ASM_FN Arm_MathContext_Save
    ldr r1, =0x04000290 @ REG_IO_DIV_NUMER
    ldr r2, =0x04000298 @ REG_IO_DIV_DENOM
    ldr r12, [r1]
    ldr r3, [r1, #4]
    ldr r1, =0x04000280 @ REG_IO_DIVCNT
    str r12, [r0]
    str r3, [r0, #0x4]
    ldr r12, [r2]
    ldr r3, [r2, #4]
    ldr r2, =0x040002B8 @ REG_IO_SQRTPARAM
    str r12, [r0, #0x8]
    str r3, [r0, #0xC]
    ldrh r3, [r1]
    ldr r1, =0x040002B0 @ REG_IO_SQRTCNT
    and r3, r3, #3
    strh r3, [r0, #0x18]
    ldr r3, [r2]
    ldr r2, [r2, #4]
    str r3, [r0, #0x10]
    str r2, [r0, #0x14]
    ldrh r1, [r1]
    and r1, r1, #1
    strh r1, [r0, #0x1A]
    bx lr

NTR_BEGIN_ASM_FN Arm_Context_Create
    add r1, r1, #4
    str r1, [r0, #0x40]
    str r2, [r0, #0x44]
    sub r2, r2, #0x40 @ SVC stack size?
    str r2, [r0, #0x38]
    ands r1, r1, #1
    movne r1, #0x3F
    moveq r1, #0x1F
    str r1, [r0]
    mov r1, #0
    str r1, [r0, #0x4]
    str r1, [r0, #0x8]
    str r1, [r0, #0xC]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    str r1, [r0, #0x18]
    str r1, [r0, #0x1C]
    str r1, [r0, #0x20]
    str r1, [r0, #0x24]
    str r1, [r0, #0x28]
    str r1, [r0, #0x2C]
    str r1, [r0, #0x30]
    str r1, [r0, #0x34]
    str r1, [r0, #0x3C]
    bx lr

NTR_BEGIN_ASM_FN Arm_Context_Load
    push {r0,lr}
    add r0, r0, #0x48 @ ctx->math_ctx
    ldr r1, =Arm_MathContext_Load
    blx r1
    pop {r0,lr}
    mrs r1, cpsr
    bic r1, r1, #0x1F
    orr r1, r1, #0xD3
    msr cpsr_c, r1
    ldr r1, [r0], #4
    msr spsr_cxsf, r1
    ldr sp, [r0, #0x40]
    ldr lr, [r0, #0x3C]
    ldm r0, {r0-lr}^
    nop
    subs pc, lr, #4

NTR_BEGIN_ASM_FN Arm_Context_Save
    push {r0,lr}
    add r0, r0, #0x48 @ ctx->math_ctx
    ldr r1, =Arm_MathContext_Save
    blx r1
    pop {r0,lr}
    mov r1, r0
    mrs r2, cpsr
    str r2, [r1], #4
    mov r0, #0xD3
    msr cpsr_c, r0
    str sp, [r1, #0x40]
    msr cpsr_c, r2
    mov r0, #1+
    stm r1, {r0-lr}
    add r0, pc, #8
    str r0, [r1, #0x3C]
    mov r0, #0
    bx lr
