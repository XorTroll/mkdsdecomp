#ifndef _NTR_OS_ALARM_H
#define _NTR_OS_ALARM_H

#include <ntr/base.h>

typedef struct Os_Alarm {
    void (*handler_fn)(void*);
    void *handler_arg;
    u32 tag;
    u32 fire_tick;
    u32 fire_tick_2;
    Os_Alarm *alarm_prev;
    Os_Alarm *alarm_next;
    u32 period_l;
    u32 period_h;
    u32 start;
    u32 start_2;
    u32 unk;
} Os_Alarm;

void Os_Alarm_Create(Os_Alarm *alarm);

#endif
