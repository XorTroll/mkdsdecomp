#ifndef _NTR_OS_THREAD_H
#define _NTR_OS_THREAD_H

#include <ntr/base.h>

#define NTR_OS_THREAD_STACK_TOP_MAGIC 0x7BF9DD5B
#define NTR_OS_THREAD_STACK_BOTTOM_MAGIC 0xFDDB597D

typedef enum Os_ThreadState {
    Os_ThreadState_Waiting = 0,
    Os_ThreadState_Ready = 1,
    Os_ThreadState_Terminated = 2
} Os_ThreadState;

typedef struct {
    void *prev_thr;
    void *next_thr;
} Os_ThreadQueue;

inline void Os_ThreadQueue_Initialize(Os_ThreadQueue *queue) {
    queue->prev_thr = NULL;
    queue->next_thr = NULL;
}

#endif
