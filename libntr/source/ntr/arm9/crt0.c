#include <ntr/arm9/mem/mem_cpu.h>
#include <ntr/extra/extra_log.h>

extern uintptr_t __bss_start__;
extern uintptr_t __bss_end__;

#include <string.h>

void Crt0_CleanBss(void) {
    NTR_EXTRA_DEBUGLOGF("CLEAN BSS: %p to %p", &__bss_start__, &__bss_end__);
    memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
    // Mem_CpuFill8();
}
