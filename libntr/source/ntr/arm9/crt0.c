#include <ntr/mem/mem_cpu.h>

extern uintptr_t __bss_start__;
extern uintptr_t __bss_end__;

void Crt0_CleanBss(void) {
    Mem_CpuFill8(&__bss_start__, 0, &__bss_end__ - &__bss_start__);
}
