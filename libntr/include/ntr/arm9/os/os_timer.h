#ifndef _NTR_ARM9_OS_TIMER_H
#define _NTR_ARM9_OS_TIMER_H

#include <ntr/base.h>

typedef u64 Os_Tick;

void Os_ReserveTimer(u8 timer_no);

Os_Tick Os_GetTick(void);

#endif
