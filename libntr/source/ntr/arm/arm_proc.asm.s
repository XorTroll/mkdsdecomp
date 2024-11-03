#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Arm_GetProcessorMode
    mrs r0, cpsr
    and r0, r0, #0x1F @ Low 5 bits of CPSR
    bx lr
