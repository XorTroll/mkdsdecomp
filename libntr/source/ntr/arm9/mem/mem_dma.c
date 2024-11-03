#include <ntr/arm9/mem/mem_dma.h>
#include <ntr/arm9/mem/mem_reg.h>
#include <ntr/arm9/os/os_irq.h>

void Mem_DmaStop(u32 dma_channel) {
    u32 old_state = Os_DisableIrq();

    NTR_ARM9_MEM_REG_DMA_N_CNT_H(dma_channel) = NTR_ARM9_MEM_REG_DMA_N_CNT(dma_channel) & 0xC5FF;
    NTR_ARM9_MEM_REG_DMA_N_CNT_H(dma_channel) = NTR_ARM9_MEM_REG_DMA_N_CNT(dma_channel) & 0x7FFF;

    if(dma_channel == 0) {
        NTR_ARM9_MEM_REG_DMA_0_SRC = 0;
        NTR_ARM9_MEM_REG_DMA_0_DST = 0;
        NTR_ARM9_MEM_REG_DMA_0_CNT = 0x81400001;
    }

    Os_RestoreIrq(old_state);
}
