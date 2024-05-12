#ifndef _NTR_OS_THREAD_H
#define _NTR_OS_THREAD_H

#include <ntr/arm.h>

#define NTR_OS_THREAD_STACK_TOP_MAGIC 0x7BF9DD5B
#define NTR_OS_THREAD_STACK_BOTTOM_MAGIC 0xFDDB597D

typedef enum Os_ThreadState {
    Os_ThreadState_Waiting = 0,
    Os_ThreadState_Ready = 1,
    Os_ThreadState_Terminated = 2
} Os_ThreadState;

/*
typedef struct {
    Arm_Context ctx;
    Os_ThreadState state;
    Os_Thread *next_thread;
    u32 id;
    u32 priority;
    u32 unk_profiler;
    Os_ThreadQueue *some_queue_ref;
    Os_Thread *prev_thr;
    Os_Thread *next_thr;
    Os_Mutex *mtx;
    Os_MutexQueue mtx_queue;
    void *stack_start;
    void *stack_end;
    u32 stack_warning_offset;
    Os_ThreadQueue join_queue;
    void *unk_specific[3];
    Os_Alarm *alarm;
    void (__cdecl *dtor_fn)(void *);
    u8 unk_extra[8];
} Os_Thread;

typedef struct {
    Os_Thread *prev_thr;
    Os_Thread *next_thr;
} Os_ThreadQueue;

void Os_Thread_Join(Os_Thread *thr);
void Os_Thread_Sleep(Os_ThreadQueue *queue);

void Os_ThreadQueue_Queue(Os_ThreadQueue *queue, Os_Thread *thr);
*/

typedef struct {
    void *prev_thr;
    void *next_thr;
} Os_ThreadQueue;

inline void Os_ThreadQueue_Initialize(Os_ThreadQueue *queue) {
    queue->prev_thr = NULL;
    queue->next_thr = NULL;
}

#endif
