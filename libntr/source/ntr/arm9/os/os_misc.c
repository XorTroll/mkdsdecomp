#include <ntr/arm9/os/os_misc.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/extra/extra_log.h>

void Os_ReferSymbol(const char *symbol) {
    // Nothing is done here
    // This is probably just used to force symbol strings to appear in the final game, or is just a debug functionality
}

void Os_Terminate(void) {
    NTR_EXTRA_DEBUGLOGF("TERMINATED!!!");
    while(true) {
        Os_DisableIrq();
        Os_Halt();
    }
}
