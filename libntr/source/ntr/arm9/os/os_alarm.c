#include <ntr/arm9/os/os_alarm.h>
#include <ntr/arm9/os/os_timer.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/arm9/os/os_misc.h>
#include <ntr/arm9/mem/mem_reg.h>
#include <ntr/arm/arm_proc.h>

static Os_AlarmQueue g_AlarmQueue;

static void Os_SetAlarmTimer(Os_Alarm *alarm);

static void Os_InsertAlarm(Os_Alarm *alarm, Os_Tick fire) {
    if(alarm->period != 0) {
        Os_Tick tick = Os_GetTick();
        fire = alarm->start;
        if(alarm->start < tick) {
            fire += alarm->period * (((tick - alarm->start) / alarm->period) + 1);
        }
    }

    alarm->fire = fire;
    if(g_AlarmQueue.prev_alarm != NULL) {
        Os_Alarm *cur_alarm = g_AlarmQueue.prev_alarm;
        while((fire - cur_alarm->fire) >= 0) {
            cur_alarm = cur_alarm->queue.next_alarm;
            if(cur_alarm != NULL) {
                alarm->queue.next_alarm = NULL;
                Os_Alarm *prev_next_alarm = g_AlarmQueue.next_alarm;
                g_AlarmQueue.next_alarm = alarm;
                prev_next_alarm->queue.prev_alarm = prev_next_alarm;
                if(prev_next_alarm != NULL) {
                    prev_next_alarm->queue.next_alarm = NULL;
                }
                else {
                    g_AlarmQueue.prev_alarm = alarm;
                    g_AlarmQueue.next_alarm = alarm;
                    Os_SetAlarmTimer(alarm);
                }
                return;
            }
        }

        // Insert alarm between queue.prev and queue.next
        alarm->queue.prev_alarm = cur_alarm->queue.prev_alarm;
        cur_alarm->queue.prev_alarm = alarm;
        alarm->queue.next_alarm = cur_alarm;
        if(alarm->queue.prev_alarm != NULL) {
            alarm->queue.prev_alarm->queue.next_alarm = alarm;
        }
        else {
            g_AlarmQueue.prev_alarm = alarm;
            Os_SetAlarmTimer(alarm);
        }
    }
    else {
        alarm->queue.next_alarm = NULL;
        Os_Alarm *prev_next_alarm = g_AlarmQueue.next_alarm;
        g_AlarmQueue.next_alarm = alarm;
        prev_next_alarm->queue.prev_alarm = prev_next_alarm;
        if(prev_next_alarm != NULL) {
            prev_next_alarm->queue.next_alarm = NULL;
        }
        else {
            g_AlarmQueue.prev_alarm = alarm;
            g_AlarmQueue.next_alarm = alarm;
            Os_SetAlarmTimer(alarm);
        }
    }
}

static void Os_AlarmHandlerImpl() {
    NTR_ARM9_MEM_REG_TIMER_1_CNT = 0;
    Os_DisableIrqHandler(Os_IrqFlag_Timer_1);
    Os_SetIrqCheckFlag(Os_IrqFlag_Timer_1);

    Os_Tick tick = Os_GetTick();
    if(g_AlarmQueue.prev_alarm != NULL) {
        if(tick < g_AlarmQueue.prev_alarm->fire) {
            Os_SetAlarmTimer(g_AlarmQueue.prev_alarm);
            return;
        }

        if(g_AlarmQueue.prev_alarm->queue.next_alarm != NULL) {
            g_AlarmQueue.next_alarm = NULL;
        }

        Os_AlarmHandlerFn handler_fn = g_AlarmQueue.prev_alarm->handler_fn;
        if(g_AlarmQueue.prev_alarm->period == 0) {
            g_AlarmQueue.prev_alarm->handler_fn = NULL;
        }
        if(handler_fn != NULL) {
            handler_fn(g_AlarmQueue.prev_alarm->handler_arg);
        }

        if(g_AlarmQueue.prev_alarm->period != 0) {
            g_AlarmQueue.prev_alarm->handler_fn = handler_fn;
            Os_InsertAlarm(g_AlarmQueue.prev_alarm, 0);
        }

        // In the block above, the insertion might have changed prev_alarm
        if(g_AlarmQueue.prev_alarm != NULL) {
            Os_SetAlarmTimer(g_AlarmQueue.prev_alarm);
        }
    }
}

static void Os_AlarmHandler(void*) {
    // Yeah, this is done in another function for some reason
    Os_AlarmHandlerImpl();
}

static void Os_SetAlarmTimer(Os_Alarm *alarm) {
    Os_Tick tick = Os_GetTick();
    NTR_ARM9_MEM_REG_TIMER_1_CNT = 0;
    Os_Tick delta = alarm->fire - tick;
    Os_SetTimerIrqHandler(1, Os_AlarmHandler, NULL);

    u16 t1_data = 0;
    if(delta < 0) {
        t1_data = (u16)~1;
    }
    else if(delta < 0x10000) {
        t1_data = (u16)(~delta);
    }
    else {
        t1_data = 0;
    }
    NTR_ARM9_MEM_REG_TIMER_1_DATA = t1_data;

    NTR_ARM9_MEM_REG_TIMER_1_CNT = NTR_ARM9_MEM_REG_TIMER_CNT_FLAG_PRESCALER_DIV64 | NTR_ARM9_MEM_REG_TIMER_CNT_FLAG_ENABLE_IRQ | NTR_ARM9_MEM_REG_TIMER_CNT_FLAG_START;

    Os_EnableIrqHandler(Os_IrqFlag_Timer_1);
}

void Os_InitializeAlarm(void) {
    static u16 g_AlarmInitialized = false;
    if(!g_AlarmInitialized) {
        g_AlarmInitialized = true;

        Os_ReserveTimer(1);
        Os_AlarmQueue_Initialize(&g_AlarmQueue);

        Os_DisableIrqHandler(Os_IrqFlag_Timer_1);
    }
}

void Os_Alarm_Create(Os_Alarm *alarm) {
    alarm->handler_fn = NULL;
    alarm->tag = 0;
}

void Os_Alarm_Set(Os_Alarm *alarm, Os_Tick tick, Os_AlarmHandlerFn handler_fn, void *handler_arg) {
    if((alarm == NULL) || (alarm->handler_fn != NULL)) {
        // Surprisingly, this is one of the few places where argument checks are done :P
        Os_Terminate();
    }

    u32 old_state = Os_DisableIrq();

    alarm->period = 0;
    alarm->handler_fn = handler_fn;
    alarm->handler_arg = handler_arg;

    Os_Tick cur_tick = Os_GetTick();
    Os_InsertAlarm(alarm, cur_tick + tick);
    Os_RestoreIrq(old_state);
}

void Os_Alarm_Cancel(Os_Alarm *alarm) {
    u32 old_state = Os_DisableIrq();

    if(alarm->handler_fn != NULL) {
        if(alarm->queue.next_alarm != NULL) {
            alarm->queue.next_alarm->queue.prev_alarm = alarm->queue.prev_alarm;
        }
        else {
            g_AlarmQueue.next_alarm = alarm->queue.prev_alarm;
        }

        if(alarm->queue.prev_alarm != NULL) {
            alarm->queue.prev_alarm->queue.next_alarm = alarm->queue.next_alarm;
        }
        else {
            g_AlarmQueue.prev_alarm = alarm->queue.next_alarm;
            if(alarm->queue.next_alarm != NULL) {
                Os_SetAlarmTimer(alarm->queue.next_alarm);
            }
        }

        alarm->handler_fn = NULL;
        alarm->period = 0;
    }
    
    Os_RestoreIrq(old_state);
}
