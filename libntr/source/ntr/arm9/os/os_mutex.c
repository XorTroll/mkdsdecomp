#include <ntr/arm9/os/os_mutex.h>
#include <ntr/arm9/os/os_thread.h>
#include <ntr/arm9/os/os_irq.h>

extern Os_ThreadInfo g_Os_ThreadInfo;

struct Os_Mutex {
    Os_ThreadQueue thr_queue;
    Os_Thread *thr;
    u32 count;
    Os_MutexQueue queue;
};
_Static_assert(sizeof(Os_Mutex) == 0x18);

Os_Mutex *Os_MutexQueue_Pop(Os_MutexQueue *queue) {
    Os_Mutex *pop_mutex = queue->prev_mutex;

    if(pop_mutex != NULL) {
        queue->prev_mutex = pop_mutex->queue.prev_mutex;
        if(pop_mutex->queue.prev_mutex != NULL) {
            pop_mutex->queue.prev_mutex->queue.next_mutex = NULL;
        }
        else {
            queue->next_mutex = NULL;
        }
    }

    return pop_mutex;
}

void Os_Mutex_Initialize(Os_Mutex *mutex) {
    Os_ThreadQueue_Initialize(&mutex->thr_queue);
    mutex->thr = NULL;
    mutex->count = 0;
}

static void Os_Mutex_EnqueueThread(Os_Thread *thr, Os_Mutex *mutex) {
    Os_Mutex *next_mutex = thr->mutex_queue.next_mutex;
    if(next_mutex != NULL) {
        next_mutex->queue.prev_mutex = mutex;
    }
    else {
        thr->mutex_queue.prev_mutex = mutex;
    }

    mutex->queue.next_mutex = next_mutex;
    mutex->queue.prev_mutex = NULL;
    thr->mutex_queue.next_mutex = mutex;
}

static void Os_Mutex_DequeueThread(Os_Thread *thr, Os_Mutex *mutex) {
    Os_Mutex *next_mutex = mutex->queue.prev_mutex;
    Os_Mutex *prev_mutex = mutex->queue.next_mutex;
    if(next_mutex != NULL) {
        next_mutex->queue.next_mutex = prev_mutex;
    }
    else {
        thr->mutex_queue.next_mutex = prev_mutex;
    }
    if(prev_mutex != NULL) {
        prev_mutex->queue.prev_mutex = next_mutex;
    }
    else {
        thr->mutex_queue.prev_mutex = next_mutex;
    }   
}

void Os_Mutex_Lock(Os_Mutex *mutex) {
    u32 old_state = Os_DisableIrq();

    Os_Thread *cur_thr = g_Os_ThreadInfo.cur_thr;
    while(true) {
        Os_Thread *mutex_thr = mutex->thr;
        if(mutex_thr == NULL) {
            mutex->thr = cur_thr;
            ++mutex->count;
            Os_Mutex_EnqueueThread(cur_thr, mutex);
            break;
        }
        else if(mutex_thr == cur_thr) {
            ++mutex->count;
            break;
        }
        else {
            cur_thr->mutex = mutex;
            Os_Thread_Sleep(&mutex->thr_queue);
            cur_thr->mutex = NULL;
        }
    }

    Os_RestoreIrq(old_state);
}

bool Os_Mutex_TryLock(Os_Mutex *mutex) {
    u32 old_state = Os_DisableIrq();
    bool is_locked = false;

    Os_Thread *cur_thr = g_Os_ThreadInfo.cur_thr;
    if(mutex->thr != NULL) {
        if(mutex->thr == cur_thr) {
            // If we are already locked
            is_locked = true;
            ++mutex->count;
        }
        else {
            // If we are locked by another thread, we cannot lock
            is_locked = false;
        }
    }
    else {
        // Not locked yet, lock in this thread
        mutex->thr = cur_thr;
        ++mutex->count;
        Os_Mutex_EnqueueThread(cur_thr, mutex);
        is_locked = true;
    }

    Os_RestoreIrq(old_state);
    return is_locked;
}

void Os_Mutex_Unlock(Os_Mutex *mutex) {
    u32 old_state = Os_DisableIrq();

    if((mutex->thr == g_Os_ThreadInfo.cur_thr) && (--mutex->count == 0)) {
        Os_Mutex_DequeueThread(g_Os_ThreadInfo.cur_thr, mutex);
        mutex->thr = NULL;
        Os_Thread_WakeUp(&mutex->thr_queue);
    }

    Os_RestoreIrq(old_state);
}

void Os_UnlockAllMutexes(Os_Thread *thr) {
    while(thr->mutex_queue.prev_mutex != NULL) {
        Os_Mutex *mutex = Os_MutexQueue_Pop(&thr->mutex_queue);
        mutex->count = 0;
        mutex->thr = NULL;
        Os_Thread_WakeUp(&mutex->thr_queue);
    }
}
