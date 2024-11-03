#ifndef _NTR_ARM9_OS_ALARM_H
#define _NTR_ARM9_OS_ALARM_H

#include <ntr/base.h>
#include <ntr/arm9/os/os_timer.h>

typedef void(*Os_AlarmHandlerFn)(void*);

typedef struct Os_Alarm Os_Alarm;

typedef struct Os_AlarmQueue {
    Os_Alarm *prev_alarm;
    Os_Alarm *next_alarm;
} Os_AlarmQueue;
_Static_assert(sizeof(Os_AlarmQueue) == 0x8);

struct Os_Alarm {
    Os_AlarmHandlerFn handler_fn;
    void *handler_arg;
    u32 tag;
    Os_Tick fire;
    Os_AlarmQueue queue;
    Os_Tick period;
    Os_Tick start;
};
_Static_assert(sizeof(Os_Alarm) == 0x30);

void Os_InitializeAlarm(void);

void Os_Alarm_Create(Os_Alarm *alarm);
void Os_Alarm_Set(Os_Alarm *alarm, Os_Tick tick, Os_AlarmHandlerFn handler_fn, void *handler_arg);
void Os_Alarm_Cancel(Os_Alarm *alarm);

inline void Os_AlarmQueue_Initialize(Os_AlarmQueue *queue) {
    queue->prev_alarm = NULL;
    queue->next_alarm = NULL;
}

#endif
