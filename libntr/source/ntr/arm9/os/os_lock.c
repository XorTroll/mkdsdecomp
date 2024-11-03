#include <ntr/arm9/os/os_lock.h>
#include <ntr/arm9/os/os_irq.h>
#include <ntr/arm9/mem/mem_cpu.h>
#include <ntr/arm9/mem/mem_reg.h>
#include <ntr/arm9/bios.h>
#include <ntr/extra/extra_log.h>

// TODO: consistency with lock_id and owner_id terms/concepts

static void Os_Lock_Wait() {
    Os_SpinWait(4096);
}

static int Os_Lock_HandleTryLock(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn, bool also_disable_fiq) {
    u32 old_state;
    if(also_disable_fiq) {
        old_state = Os_DisableIrqFiq();
    }
    else {
        old_state = Os_DisableIrq();
    }

    int last_lock_flag = (int)Mem_Swap16(lock_id, &lock->lock_flag);
    if(last_lock_flag == 0) {
        if(cnt_fn != NULL) {
            cnt_fn();
        }

        lock->owner_id = lock_id;
    }

    if(also_disable_fiq) {
        Os_RestoreIrqFiq(old_state);
    }
    else {
        Os_RestoreIrq(old_state);
    }

    return last_lock_flag;
}

static int Os_Lock_TryLockImpl(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn, bool also_disable_fiq) {
    int lock_flag;
    do {
        lock_flag = Os_Lock_HandleTryLock(lock_id, lock, cnt_fn, also_disable_fiq);
        Os_Lock_Wait();
    } while(lock_flag > 0);
    return lock_flag;
}

static int Os_Lock_UnlockImpl(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn, bool also_disable_fiq) {
    if(lock_id != lock->owner_id) {
        return Os_LockResult_InvalidLockId;
    }

    u32 old_state;
    if(also_disable_fiq) {
        old_state = Os_DisableIrqFiq();
    }
    else {
        old_state = Os_DisableIrq();
    }

    lock->owner_id = 0;
    if(cnt_fn != NULL) {
        cnt_fn();
    }
    lock->lock_flag = 0;

    if(also_disable_fiq) {
        Os_RestoreIrqFiq(old_state);
    }
    else {
        Os_RestoreIrq(old_state);
    }

    return Os_LockResult_Success;
}

int Os_Lock_TryLock(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn) {
    return Os_Lock_TryLockImpl(lock_id, lock, cnt_fn, false);
}

int Os_Lock_Unlock(u16 lock_id, Os_Lock *lock, Os_LockControlFn cnt_fn) {
    return Os_Lock_UnlockImpl(lock_id, lock, cnt_fn, false);
}

u16 Os_Lock_GetOwnerId(Os_Lock *lock) {
    return lock->owner_id;
}

void Os_InitializeLock(void) {
    static u32 g_LockInitialized = false;
    if(!g_LockInitialized) {
        NTR_EXTRA_DEBUGLOGF("LOCK INIT...");
        g_LockInitialized = true;

        // Maybe lock ID 0x7E is used to lock while everything is initialized, and 0x7F is used for post-init lock?
        
        volatile Os_Lock *init_lock = &NTR_ARM9_BIOS_SHARED_REGION->lock_init;
        init_lock->lock_flag = 0;
        Os_Lock_TryLock(0x7E, init_lock, NULL);

        while(init_lock->extension != 0) {
            Os_Lock_Wait();
        }

        NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[0] = 0xFFFFFFFF;
        NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[1] = 0xFFFF0000;

        // Clean these next 5 locks? Why not the rest...?
        Mem_CpuFill32(0, (void*)&NTR_ARM9_BIOS_SHARED_REGION->lock_VRAM_C, sizeof(Os_Lock) * 5);

        NTR_ARM9_MEM_REG_IO_EXMEMCNT |= NTR_ARM9_MEM_REG_IO_EXMEMCNT_FLAG_DS_SLOT_ARM7;
        NTR_ARM9_MEM_REG_IO_EXMEMCNT |= NTR_ARM9_MEM_REG_IO_EXMEMCNT_FLAG_GBA_SLOT_ARM7;

        Os_Lock_Unlock(0x7E, init_lock, NULL);

        Os_Lock_TryLock(0x7F, init_lock, NULL);
    }
}

int Os_AllocateLockId(void) {
    vu32 *lock_id_flag = &NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[0];
    u32 lock_flag_lz = __builtin_clz(*lock_id_flag);
    u8 val;
    if(lock_flag_lz == 32) {
        lock_id_flag = &NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[1];
        lock_flag_lz = __builtin_clz(*lock_id_flag);
        if(lock_flag_lz == 32) {
            // No space?
            return Os_LockResult_InvalidLockId;
        }

        val = 0x60;
    }
    else {
        val = 0x40;
    }

    *lock_id_flag &= ~(0x80000000 >> lock_flag_lz);
    return val + lock_flag_lz;
}

void Os_ReleaseLockId(int lock_id) {
    vu32 *lock_id_flag;
    u8 val; // ???
    if((lock_id - 0x60) < 0) {
        lock_id_flag = &NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[0];
        val = lock_id - 0x40;
    }
    else {
        lock_id_flag = &NTR_ARM9_BIOS_SHARED_REGION->lock_id_flag_mainp[1];
        val = lock_id - 0x60;
    }
    *lock_id_flag |= 0x80000000 >> val;
}
