#include "start.h"
#include "exec.h"
#include <ntr/arm9/bios.h>

size_t GetSoundBufferAllocationSize(u16 boot_indicator) {
    if(boot_indicator == Bios_BootIndicatorType_DownloadPlay) {
        return 0xAF000;
    }
    else {
        return 0xC400;
    }
}

size_t AllocateHeapsAndBuffers(Mem_HeapHandle base_heap_hnd, Mem_HeapHandle *out_global_obj_heap_hnd, void **out_sound_buf, Mem_HeapHandle *out_archive_heap_hnd_1, Mem_HeapHandle *out_archive_heap_hnd_2, Mem_HeapHandle *out_archive_heap_hnd_3) {
    if(NTR_ARM9_BIOS_SHARED_REGION->boot_indicator == Bios_BootIndicatorType_DownloadPlay) {
        *out_global_obj_heap_hnd = Mem_CreateChildFrameHeapFromTail(base_heap_hnd, 0x19000);
    }
    else {
        *out_global_obj_heap_hnd = Mem_CreateChildFrameHeapFromTail(base_heap_hnd, 0x1B800);
    }

    if(NTR_ARM9_BIOS_SHARED_REGION->boot_indicator == Bios_BootIndicatorType_DownloadPlay) {
        *out_archive_heap_hnd_1 = Mem_CreateChildFrameHeapFromTail(base_heap_hnd, 0x19000);
        *out_archive_heap_hnd_2 = Mem_CreateChildFrameHeapFromTail(base_heap_hnd, 0x4B000);
        *out_archive_heap_hnd_3 = Mem_CreateChildFrameHeapFromTail(base_heap_hnd, 0x1B800);
    }
    else {
        *out_archive_heap_hnd_1 = NULL;
        *out_archive_heap_hnd_2 = NULL;
        *out_archive_heap_hnd_3 = NULL;
    }

    // This is quite pointless for what's used later
    u16 boot_indicator_mod;
    if(NTR_ARM9_BIOS_SHARED_REGION->boot_indicator == Bios_BootIndicatorType_DownloadPlay) {
        boot_indicator_mod = Bios_BootIndicatorType_DownloadPlay;
    }
    else {
        boot_indicator_mod = 0;
    }

    size_t sound_buf_size = GetSoundBufferAllocationSize(boot_indicator_mod);
    *out_sound_buf = Mem_AllocateHeap(base_heap_hnd, sound_buf_size);
    return sound_buf_size;
}

void InitializeGlobalObjects(void) {
    ExecutionContext *ctx = GetActiveExecutionContext();
    Mem_HeapHandle base_heap_hnd = ExecutionContext_GetHeapHandle(ctx);

    Mem_HeapHandle global_obj_heap_hnd;
    void *sound_buf;
    Mem_HeapHandle archive_heap_hnd_1;
    Mem_HeapHandle archive_heap_hnd_2;
    Mem_HeapHandle archive_heap_hnd_3;
    size_t sound_buf_size = AllocateHeapsAndBuffers(base_heap_hnd, &global_obj_heap_hnd, &sound_buf, &archive_heap_hnd_1, &archive_heap_hnd_2, &archive_heap_hnd_3);
    NTR_EXTRA_DEBUGLOGF("AllocateHeapsAndBuffers done");

    // TODO: <a shit ton of important global objects are created then>();
}

void StartExecuteFunction(void*) {
    InitializeGlobalObjects();
    // <function that uses another exec ctx to run the main loop>();
    NTR_EXTRA_DEBUGLOGF("StartExecuteFunction done");
}
