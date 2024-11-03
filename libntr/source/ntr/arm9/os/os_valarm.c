#include <ntr/arm9/os/os_valarm.h>
#include <ntr/arm9/os/os_alarm.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/extra/extra_log.h>

static Os_AlarmQueue g_VAlarmQueue;
static u32 g_PrevVCount = 0;
static u32 g_FrameCount = 0;

void Os_InitializeVAlarm(void) {
    static u16 g_VAlarmInitialized = false;
    if(!g_VAlarmInitialized) {
        NTR_EXTRA_DEBUGLOGF("VALARM INIT...");
        g_VAlarmInitialized = true;
        Os_AlarmQueue_Initialize(&g_VAlarmQueue);

        Os_DisableIrqHandler(Os_IrqType_Timer_1);

        g_PrevVCount = 0;
        g_FrameCount = 0;
    }
}
