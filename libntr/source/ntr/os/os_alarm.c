#include <ntr/os/os_alarm.h>

void Os_Alarm_Create(Os_Alarm *alarm) {
    alarm->handler_fn = NULL;
    alarm->tag = 0;
}
