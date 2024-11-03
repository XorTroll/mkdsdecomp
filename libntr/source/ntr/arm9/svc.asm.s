#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Svc_VblankIntrWait
    movs r2, #0
    svc 0x5
    bx lr

NTR_BEGIN_ASM_FN Svc_Delay
    svc 0x3
    bx lr

NTR_BEGIN_ASM_FN Svc_Copy
    svc 0xB
    bx lr
