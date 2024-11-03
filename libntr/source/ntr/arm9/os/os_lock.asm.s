#include <ntr/base.asm.h>

.text
.arm

NTR_BEGIN_ASM_FN Os_SpinWait
    subs r0, r0, #4
    bcs Os_SpinWait
    bx lr
