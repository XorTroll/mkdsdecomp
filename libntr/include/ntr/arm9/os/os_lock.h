#ifndef _NTR_ARM9_OS_LOCK_H
#define _NTR_ARM9_OS_LOCK_H

#include <ntr/base.h>

typedef struct Os_Lock {
    u32 lock_flag;
    u16 owner_id;
    u16 extension;
} Os_Lock;
_Static_assert(sizeof(Os_Lock) == 0x8);

typedef void(*Os_LockControlFn)(void);

typedef enum Os_LockResult {
    Os_LockResult_Success = 0,
    Os_LockResult_InvalidLockId = -2
} Os_LockResult;

void Os_SpinWait(u32 cycle_count);

int Os_Lock_TryLock(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn);
int Os_Lock_Unlock(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn);
u16 Os_Lock_GetOwnerId(Os_Lock *lock);

void Os_InitializeLock(void);

int Os_AllocateLockId(void);
void Os_ReleaseLockId(int lock_id);

#endif
