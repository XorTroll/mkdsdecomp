#ifndef _NTR_ARM9_OS_THREAD_H
#define _NTR_ARM9_OS_THREAD_H

#include <ntr/arm9/os/os_alarm.h>
#include <ntr/arm9/os/os_mutex.h>
#include <ntr/arm9/arm/arm_ctx.h>

#define NTR_ARM9_OS_THREAD_STACK_TOP_MAGIC 0x7BF9DD5B
#define NTR_ARM9_OS_THREAD_STACK_BOTTOM_MAGIC 0xFDDB597D

typedef enum Os_ThreadState {
    Os_ThreadState_Waiting = 0,
    Os_ThreadState_Ready = 1,
    Os_ThreadState_Terminated = 2
} Os_ThreadState;

typedef struct Os_Thread Os_Thread;

typedef struct Os_ThreadQueue {
    Os_Thread *prev_thr;
    Os_Thread *next_thr;
} Os_ThreadQueue;
_Static_assert(sizeof(Os_ThreadQueue) == 0x8);

typedef void(*Os_ThreadSwitchFn)(Os_Thread*, Os_Thread*);
typedef void(*Os_ThreadEntryFn)(void*);
typedef void(*Os_ThreadDtorFn)(void*);

struct Os_Thread {
    Arm_Context ctx;
    u32 state;
    Os_Thread *next_thr;
    u32 id;
    u32 priority;
    u32 unk_profiler;
    Os_ThreadQueue *sleep_queue_ptr;
    Os_ThreadQueue another_queue;
    Os_Mutex *mutex;
    Os_MutexQueue mutex_queue;
    void *stack_top;
    void *stack_bottom;
    u32 stack_warning_offset;
    Os_ThreadQueue join_queue;
    void *unk_specific[3];
    Os_Alarm *alarm;
    Os_ThreadDtorFn dtor_fn;
    u8 unk_extra[8];
};
_Static_assert(sizeof(Os_Thread) == 0xC0);

typedef struct Os_ThreadInfo {
    u16 needs_rescheduling;
    u16 irq_depth;
    Os_Thread *cur_thr;
    Os_Thread *prio_thr_list;
    Os_ThreadSwitchFn thr_switch_fn;
} Os_ThreadInfo;
_Static_assert(sizeof(Os_ThreadInfo) == 0x10);

inline void Os_ThreadQueue_Initialize(Os_ThreadQueue *queue) {
    queue->prev_thr = NULL;
    queue->next_thr = NULL;
}

Os_Thread *Os_ThreadQueue_Pop(Os_ThreadQueue *queue);
void Os_ThreadQueue_Queue(Os_ThreadQueue *queue, Os_Thread *thr);

void Os_Thread_Create(Os_Thread *thr, Os_ThreadEntryFn entry_fn, void *entry_arg, void *stack_bottom, size_t stack_size, int priority);
void Os_Thread_SetDestructor(Os_Thread *thr, Os_ThreadDtorFn fn);
u32 Os_Thread_GetPriority(Os_Thread *thr);
void Os_Thread_SetPriority(Os_Thread *thr, u32 priority);
void Os_Thread_WakeUpDirect(Os_Thread *thr);
bool Os_Thread_IsTerminated(Os_Thread *thr);
void Os_Thread_Join(Os_Thread *thr);

void Os_Thread_Sleep(Os_ThreadQueue *queue);
void Os_Thread_WakeUp(Os_ThreadQueue *queue);

void Os_Thread_Exit(void);

void Os_SetThreadSwitchCallback(Os_ThreadSwitchFn fn);

void Os_InitializeThread(void);

void Os_Sleep(u32 time_milli);

#endif
