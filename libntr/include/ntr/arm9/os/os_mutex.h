#ifndef _NTR_ARM9_OS_MUTEX_H
#define _NTR_ARM9_OS_MUTEX_H

#include <ntr/base.h>

typedef struct Os_Mutex Os_Mutex;

typedef struct Os_Thread Os_Thread;

typedef struct Os_MutexQueue {
    Os_Mutex *prev_mutex;
    Os_Mutex *next_mutex;
} Os_MutexQueue;
_Static_assert(sizeof(Os_MutexQueue) == 0x8);

inline void Os_MutexQueue_Initialize(Os_MutexQueue *queue) {
    queue->prev_mutex = NULL;
    queue->next_mutex = NULL;
}

Os_Mutex *Os_MutexQueue_Pop(Os_MutexQueue *queue);

void Os_Mutex_Initialize(Os_Mutex *mutex);
void Os_Mutex_Lock(Os_Mutex *mutex);
bool Os_Mutex_TryLock(Os_Mutex *mutex);
void Os_Mutex_Unlock(Os_Mutex *mutex);

void Os_UnlockAllMutexes(Os_Thread *thr);

#endif
