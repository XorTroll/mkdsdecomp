#include <ntr/arm9/mem/mem_init.h>
#include <ntr/arm9/mem/mem_reg.h>
#include <ntr/arm9/mem/mem_dma.h>

static void Mem_SetWramcnt(u8 wramcnt_val) {
    NTR_ARM9_MEM_REG_WRAM_CNT = wramcnt_val;
}

void Mem_Initialize(void) {
    Mem_SetWramcnt(NTR_ARM9_MEM_REG_WRAM_CNT_VAL_0K_32K);
    Mem_DmaStop(0);
}
