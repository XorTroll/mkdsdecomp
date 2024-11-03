#include <ntr/arm9/os/os_thread.h>
#include <ntr/arm9/os/os_mutex.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/arm9/os/os_misc.h>
#include <ntr/arm9/bios.h>
#include <ntr/arm9/mem/mem_addr.h>
#include <ntr/arm9/mem/mem_cpu.h>
#include <ntr/arm9/arm/arm_ctx.h>
#include <ntr/arm/arm_proc.h>
#include <ntr/extra/extra_log.h>

u32 g_CurrentThreadId;

u32 g_ThreadDtorSpValue = 0;
u32 g_ThreadRescheduleCount = 0;
Os_ThreadSwitchFn g_SystemThreadSwitchFn = NULL; // ???

Os_ThreadInfo g_Os_ThreadInfo;
Os_Thread **g_CurrentThreadRef;

#define _NTR_ARM9_OS_THREAD_LAUNCHER_THREAD_PRIORITY 16
Os_Thread g_LauncherThread;

#define _NTR_ARM9_OS_THREAD_IDLE_THREAD_PRIORITY_TEMP 31
#define _NTR_ARM9_OS_THREAD_IDLE_THREAD_PRIORITY_REAL 32
#define _NTR_ARM9_OS_THREAD_IDLE_THREAD_STACK_SIZE 200
u8 g_IdleThreadStack[_NTR_ARM9_OS_THREAD_IDLE_THREAD_STACK_SIZE];
Os_Thread g_IdleThread;

static void Os_IdleThreadMain(void*) {
    Os_EnableIrq();
    while(true) {
        Os_Halt();
    }
}

static u32 Os_AllocateThreadId() {
    // Note: this is not inlined, this is it's own function
    return ++g_CurrentThreadId;
}

static void Os_InsertThread(Os_Thread *thr) {
    // Search last thread whose priority <= the input thread's one
    // (ensure the list is priority-sorted)
    Os_Thread *thr_prio_le_this = NULL;
    Os_Thread *cur_thr;
    for(cur_thr = g_Os_ThreadInfo.prio_thr_list; (cur_thr != NULL) && (cur_thr->priority < thr->priority); cur_thr = cur_thr->next_thr) {
        thr_prio_le_this = cur_thr;
    }

    if(thr_prio_le_this != NULL) {
        // Insert between threads with lower/higher priority
        thr->next_thr = thr_prio_le_this->next_thr;
        thr_prio_le_this->next_thr = thr;
    }
    else {
        // No thread with smaller priority, thus insert first
        thr->next_thr = g_Os_ThreadInfo.prio_thr_list;
        g_Os_ThreadInfo.prio_thr_list = thr->next_thr;
    }
}

static inline void Os_SetThreadStackMagic(Os_Thread *thr) {
    *(u32*)(thr->stack_bottom - sizeof(u32)) = NTR_ARM9_OS_THREAD_STACK_BOTTOM_MAGIC;
    *(u32*)(thr->stack_top) = NTR_ARM9_OS_THREAD_STACK_TOP_MAGIC;
}

static Os_Thread *Os_FindReadyThread(void) {
    // Iterate through priority list until a ready thread is found
    Os_Thread *thr;
    for(thr = g_Os_ThreadInfo.prio_thr_list; (thr != NULL) && (thr->state != Os_ThreadState_Ready); thr = thr->next_thr);
    return thr;
}

static void Os_RescheduleThread(void) {
    if(g_ThreadRescheduleCount == 0) {
        if((g_Os_ThreadInfo.irq_depth != 0) || (Arm_GetProcessorMode() == Arm_ProcessorMode_Irq)) {
            g_Os_ThreadInfo.needs_rescheduling = true;
        }
        else {
            Os_Thread *next_thr = Os_FindReadyThread();
            if((g_Os_ThreadInfo.cur_thr != next_thr) && (next_thr != NULL) && ((g_Os_ThreadInfo.cur_thr->state == Os_ThreadState_Terminated) || Arm_Context_Save(&g_Os_ThreadInfo.cur_thr->ctx))) {
                if(g_SystemThreadSwitchFn) {
                    g_SystemThreadSwitchFn(g_Os_ThreadInfo.cur_thr, next_thr);
                }

                if(g_Os_ThreadInfo.thr_switch_fn) {
                    g_Os_ThreadInfo.thr_switch_fn(g_Os_ThreadInfo.cur_thr, next_thr);
                }

                g_Os_ThreadInfo.cur_thr = next_thr;
                Arm_Context_Load(&next_thr->ctx);
            }
        }
    }
}

static void Os_EnableScheduler(void) {
    u32 old_state = Os_DisableIrq();

    if(g_ThreadRescheduleCount != 0) {
        --g_ThreadRescheduleCount;
    }

    Os_RestoreIrq(old_state);
}

static void Os_DisableScheduler(void) {
    u32 old_state = Os_DisableIrq();

    if(g_ThreadRescheduleCount != -1) {
        ++g_ThreadRescheduleCount;
    }

    Os_RestoreIrq(old_state);
}

static void Os_PopThread(Os_Thread *thr) {
    Os_Thread *prev_thr = NULL;
    Os_Thread *thr_i;
    for(thr_i = g_Os_ThreadInfo.prio_thr_list; (thr_i != NULL) && (thr_i != thr); thr_i = thr_i->next_thr) {
        prev_thr = thr_i;
    }
    
    if(prev_thr != NULL) {
        prev_thr->next_thr = thr->next_thr;
    }
    else {
        g_Os_ThreadInfo.prio_thr_list = thr->next_thr;
    }
}

static void Os_DisposeCurrentThreadImpl(void) {
    Os_Thread *cur_thr = *g_CurrentThreadRef;

    Os_DisableScheduler();
    Os_UnlockAllMutexes(cur_thr);
    Os_PopThread(cur_thr);
    cur_thr->state = Os_ThreadState_Terminated;
    Os_Thread_WakeUp(&cur_thr->join_queue);
    Os_EnableScheduler();
    Os_RescheduleThread();

    Os_Terminate();
}

static void Os_DisposeCurrentThread(void *dtor_arg) {
    Os_Thread *cur_thr = *g_CurrentThreadRef;
    Os_ThreadDtorFn dtor_fn = cur_thr->dtor_fn;
    if(dtor_fn != NULL) {
        cur_thr->dtor_fn = NULL;
        dtor_fn(dtor_arg);
        Os_DisableIrq();
    }

    Os_DisposeCurrentThreadImpl();
}

static void Os_Thread_ExitImpl(Os_Thread *thr, void *dtor_arg) {
    if(g_ThreadDtorSpValue == 0) {
        Os_DisposeCurrentThread(dtor_arg);
    }

    Arm_Context_Create(&thr->ctx, (u32)Os_DisposeCurrentThread, g_ThreadDtorSpValue);
    thr->ctx.r[0] = (u32)dtor_arg;
    thr->ctx.cpsr |= NTR_BIT(7); // Disable-interrupt bit
    thr->state = Os_ThreadState_Ready;
    Arm_Context_Load(&thr->ctx);
}

Os_Thread *Os_ThreadQueue_Pop(Os_ThreadQueue *queue) {
    Os_Thread *prev_thr = queue->prev_thr;
    if(queue->prev_thr != NULL) {
        Os_Thread *next_thr = prev_thr->another_queue.next_thr;
        queue->prev_thr = next_thr;
        if(next_thr != NULL) {
            next_thr->another_queue.prev_thr = 0;
        }
        else {
            queue->next_thr = 0;
            prev_thr->sleep_queue_ptr = 0;
        }
    }
    return prev_thr;
}

void Os_ThreadQueue_Queue(Os_ThreadQueue *queue, Os_Thread *thr) {
    Os_Thread *thr_i;
    for(thr_i = queue->prev_thr; (thr_i != NULL) && (thr_i->priority <= thr->priority); thr_i = thr_i->another_queue.next_thr) {
        if(thr_i == thr) {
            return;
        }
    }

    if(thr_i != NULL) {
        Os_Thread *prev_thr_i = thr_i->another_queue.prev_thr;
        if(prev_thr_i != NULL) {
            prev_thr_i->another_queue.next_thr = thr;
        }
        else {
            queue->prev_thr = thr;
        }

        thr->another_queue.prev_thr = prev_thr_i;
        thr->another_queue.next_thr = thr_i;
        thr_i->another_queue.prev_thr = thr;
    }
    else {
        Os_Thread *next_queue_thr = queue->next_thr;
        if(next_queue_thr != NULL) {
            next_queue_thr->another_queue.next_thr = thr;
        }
        else {
            queue->prev_thr = thr;
        }
        
        thr->another_queue.prev_thr = next_queue_thr;
        thr->another_queue.next_thr = NULL;
        queue->next_thr = thr;
    }
}

void Os_Thread_Create(Os_Thread *thr, Os_ThreadEntryFn entry_fn, void *entry_arg, void *stack_bottom, size_t stack_size, int priority) {
    u32 old_state = Os_DisableIrq();

    thr->priority = priority;
    thr->id = Os_AllocateThreadId();
    thr->state = Os_ThreadState_Waiting;
    thr->unk_profiler = 0;

    Os_InsertThread(thr);

    thr->stack_bottom = stack_bottom;
    thr->stack_top = stack_bottom - stack_size;
    Os_SetThreadStackMagic(thr);

    Os_ThreadQueue_Initialize(&thr->join_queue);

    Arm_Context_Create(&thr->ctx, (uintptr_t)entry_fn, (uintptr_t)stack_bottom - 4); // ?

    thr->ctx.r[0] = (u32)entry_arg;
    thr->ctx.lr = (u32)Os_Thread_Exit;
    
    // Clean stack (except for the initial and final magic u32's)
    Mem_CpuFill32(0, thr->stack_top + sizeof(u32), stack_size - 2 * sizeof(u32));

    Os_Thread_SetDestructor(thr, NULL);
    thr->sleep_queue_ptr = NULL;
    Os_ThreadQueue_Initialize(&thr->another_queue);
    thr->alarm = NULL;

    Os_RestoreIrq(old_state);
}

void Os_Thread_SetDestructor(Os_Thread *thr, Os_ThreadDtorFn fn) {
    thr->dtor_fn = fn;
}

u32 Os_Thread_GetPriority(Os_Thread *thr) {
    return thr->priority;
}

void Os_Thread_SetPriority(Os_Thread *thr, u32 priority) {
    Os_Thread *cur_thr = g_Os_ThreadInfo.prio_thr_list;
    Os_Thread *prev_prio_lst_thr = NULL;

    u32 old_state = Os_DisableIrq();
    while((cur_thr != NULL) && (cur_thr != thr)) {
        prev_prio_lst_thr = cur_thr;
        cur_thr = cur_thr->next_thr;
    }

    // Cannot modify idle thread priority, it's a special thread
    if((cur_thr != NULL) && (cur_thr != &g_IdleThread) && (cur_thr->priority != priority)) {
        if(prev_prio_lst_thr != NULL) {
            prev_prio_lst_thr->next_thr = thr->next_thr;
        }
        else {
            g_Os_ThreadInfo.prio_thr_list = thr->next_thr;
        }

        thr->priority = priority;
        Os_InsertThread(thr);
        Os_RescheduleThread();
    }

    Os_RestoreIrq(old_state);
}

void Os_Thread_WakeUpDirect(Os_Thread *thr) {
    u32 old_state = Os_DisableIrq();
    
    thr->state = Os_ThreadState_Ready;
    Os_RescheduleThread();

    Os_RestoreIrq(old_state);
}

bool Os_Thread_IsTerminated(Os_Thread *thr) {
    return thr->state == Os_ThreadState_Terminated;
}

void Os_Thread_Join(Os_Thread *thr) {
    u32 old_state = Os_DisableIrq();
    
    if(thr->state != Os_ThreadState_Terminated) {
        Os_Thread_Sleep(&thr->join_queue);
    }

    Os_RestoreIrq(old_state);
}

void Os_Thread_Sleep(Os_ThreadQueue *queue) {
    u32 old_state = Os_DisableIrq();
    
    Os_Thread *cur_thr = *g_CurrentThreadRef;
    if(queue != NULL) {
        cur_thr->sleep_queue_ptr = queue;
        Os_ThreadQueue_Queue(queue, *g_CurrentThreadRef);
    }

    cur_thr->state = Os_ThreadState_Waiting;
    Os_RescheduleThread();

    Os_RestoreIrq(old_state);
}

void Os_Thread_WakeUp(Os_ThreadQueue *queue) {
    u32 old_state = Os_DisableIrq();
    
    if(queue->prev_thr != NULL) {
        do {
            Os_Thread *pop_thr = Os_ThreadQueue_Pop(queue);
            pop_thr->state = Os_ThreadState_Ready;
            pop_thr->sleep_queue_ptr = NULL;
            Os_ThreadQueue_Initialize(&pop_thr->another_queue);
        } while(queue->prev_thr != NULL);

        Os_ThreadQueue_Initialize(queue);

        Os_RescheduleThread();
    }

    Os_RestoreIrq(old_state);
}

void Os_Thread_Exit(void) {
    Os_DisableIrq();
    Os_Thread_ExitImpl(g_Os_ThreadInfo.cur_thr, NULL);
}

void Os_SetThreadSwitchCallback(Os_ThreadSwitchFn fn) {
    u32 old_state = Os_DisableIrq();
    g_Os_ThreadInfo.thr_switch_fn = fn;
    Os_RestoreIrq(old_state);
}

extern void Crt0_CleanBss(void);

void Os_InitializeThread(void) {
    static int g_ThreadInitialized = false;
    if(!g_ThreadInitialized) {
        NTR_EXTRA_DEBUGLOGF("INIT THREAD...");

        g_Os_ThreadInfo.cur_thr = &g_LauncherThread;
        g_Os_ThreadInfo.prio_thr_list = &g_LauncherThread;

        void *launcher_thr_stack_top;
        if(NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE <= 0) {
            launcher_thr_stack_top = (void*)(NTR_ARM9_MEM_ADDR_DTCM_AUTOLOAD_START - NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE);
        }
        else {
            launcher_thr_stack_top = (void*)(NTR_ARM9_MEM_ADDR_DTCM_SVC_STACK_TOP - NTR_ARM9_MEM_ADDR_IRQ_STACK_SIZE - NTR_ARM9_MEM_ADDR_SYS_STACK_SIZE);
        }

        g_ThreadInitialized = true;

        g_LauncherThread.priority = _NTR_ARM9_OS_THREAD_LAUNCHER_THREAD_PRIORITY;
        g_LauncherThread.state = Os_ThreadState_Ready;
        g_LauncherThread.next_thr = NULL;
        g_LauncherThread.unk_profiler = 0; // ?

        g_CurrentThreadRef = &g_Os_ThreadInfo.cur_thr;

        g_LauncherThread.stack_top = launcher_thr_stack_top;
        g_LauncherThread.stack_bottom = (void*)(NTR_ARM9_MEM_ADDR_DTCM_SVC_STACK_TOP - NTR_ARM9_MEM_ADDR_IRQ_STACK_SIZE);
        g_LauncherThread.stack_warning_offset = 0; // ?
        Os_SetThreadStackMagic(&g_LauncherThread);
        Os_ThreadQueue_Initialize(&g_LauncherThread.join_queue);

        g_Os_ThreadInfo.needs_rescheduling = false;
        g_Os_ThreadInfo.irq_depth = 0;
        NTR_ARM9_BIOS_SHARED_REGION->arm9_thread_info = &g_Os_ThreadInfo;
        Os_SetThreadSwitchCallback(NULL);

        Os_Thread_Create(&g_IdleThread, Os_IdleThreadMain, 0, g_IdleThreadStack + sizeof(g_IdleThreadStack), sizeof(g_IdleThreadStack), _NTR_ARM9_OS_THREAD_IDLE_THREAD_PRIORITY_TEMP);
        g_IdleThread.priority = _NTR_ARM9_OS_THREAD_IDLE_THREAD_PRIORITY_REAL; // ???
        g_IdleThread.state = Os_ThreadState_Ready;
    }
}

static void Os_SleepAlarmHandler(void *caller_thr_ref_raw) {
    Os_Thread **caller_thr_ref = (Os_Thread**)caller_thr_ref_raw;
    Os_Thread *caller_thr = *caller_thr_ref;

    // Time slept, notify by setting the underlying ptr to NULL
    *caller_thr_ref = NULL;

    caller_thr->alarm = NULL;
    Os_Thread_WakeUpDirect(caller_thr);
}

void Os_Sleep(u32 time_milli) {
    Os_Alarm sleep_alarm;
    Os_Alarm_Create(&sleep_alarm);

    u32 old_state = Os_DisableIrq();

    Os_Thread *cur_thr = *g_CurrentThreadRef;

    u32 time_ticks = Arm_MillisecondsToTicks(time_milli);
    cur_thr->alarm = &sleep_alarm;
    Os_Alarm_Set(&sleep_alarm, time_ticks, Os_SleepAlarmHandler, &cur_thr);

    // Wait until the alarm rings and sets the thread ptr to NULL
    while(cur_thr != NULL) {
        Os_Thread_Sleep(NULL);
    }
    
    Os_RestoreIrq(old_state);
}
