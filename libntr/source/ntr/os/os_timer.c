#include <ntr/os/os_timer.h>

static u16 g_ReservedTimerMask = 0;

void Os_ReserveTimer(u8 timer_no) {
    g_ReservedTimerMask |= NTR_BIT(timer_no);
}
