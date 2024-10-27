#include <ntr/os/os_alarm.h>
#include <ntr/os/os_timer.h>
#include <ntr/os/os_irq.h>

static u16 g_AlarmInitialized = false;
static Os_AlarmQueue g_AlarmQueue;

void Os_InitializeAlarm(void) {
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
