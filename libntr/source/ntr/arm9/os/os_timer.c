#include <ntr/arm9/os/os_timer.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/arm9/mem/mem_reg.h>

static u16 g_ReservedTimerMask = 0;
static Os_Tick g_TickCounter = 0;

void Os_ReserveTimer(u8 timer_no) {
    g_ReservedTimerMask |= NTR_BIT(timer_no);
}

Os_Tick Os_GetTick(void) {
    u32 old_state = Os_DisableIrq();

    u16 t0_data = NTR_ARM9_MEM_REG_TIMER_0_DATA;
    Os_Tick counter = g_TickCounter & 0xFFFFFFFFFFFFull;
    if(((NTR_ARM9_MEM_REG_IO_IF & 8) != 0) && ((t0_data & 0x8000) == 0)) {
        counter++;
    }

    Os_RestoreIrq(old_state);

    // Counted ticks + elapsed time on timer-0 before it ticks
    return t0_data | (counter << 16);
}
