#include <ntr/os/os_valarm.h>
#include <ntr/os/os_alarm.h>
#include <ntr/os/os_irq.h>
#include <ntr/base.h>

static u16 g_VAlarmInitialized = false;
static Os_AlarmQueue g_VAlarmQueue;
static u32 g_PrevVCount = 0;
static u32 g_FrameCount = 0;

void Os_InitializeVAlarm(void) {
    if(!g_VAlarmInitialized) {
        g_VAlarmInitialized = true;
        Os_AlarmQueue_Initialize(&g_VAlarmQueue);

        Os_DisableIrqHandler(Os_IrqType_Timer_1);

        g_PrevVCount = 0;
        g_FrameCount = 0;
    }
}
