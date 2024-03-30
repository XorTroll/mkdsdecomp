#ifndef _NTR_OS_THREAD_H
#define _NTR_OS_THREAD_H

#include "base.h"

typedef struct {
    // TODO
} Os_Thread;

typedef struct {
    Os_Thread *prev_thr;
    Os_Thread *next_thr;
} Os_ThreadQueue;

inline void Os_ThreadQueue_Initialize(Os_ThreadQueue *queue) {
    queue->prev_thr = NULL;
    queue->next_thr = NULL;
}

#endif