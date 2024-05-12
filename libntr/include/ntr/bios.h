#ifndef _NTR_BIOS_H
#define _NTR_BIOS_H

#include "os/os_thread.h"

// Definitions for all the various fields set by the BIOS in the RAM

// TODO: consider defining a struct for this?

#define NTR_BIOS_DTCM_STACK_SIZE 0xC00

typedef struct Bios_DtcmRegion {
    Os_ThreadQueue irq_queue;
    void *irq_handlers[22];
    u32 unk[712];
    u8 launcher_thr_stack[0x2800]; // This size is probably not fixed
    u8 svc_stack[NTR_BIOS_DTCM_STACK_SIZE];
    // <more unknown stuff>
} Bios_DtcmRegion;

extern void *__dtcm_start;
#define NTR_BIOS_DTCM_REGION ((volatile Bios_DtcmRegion*)(__dtcm_start))

typedef enum Bios_BootIndicatorType {
    Bios_BootIndicatorType_Normal = 1,
    Bios_BootIndicatorType_DownloadPlay = 2
} Bios_BootIndicatorType;

#define NTR_BIOS_DEBUG_SIO_EXISTS *(volatile u16*)(0x27FF814)
#define NTR_BIOS_ARM7_BIOS_CRC *(volatile u16*)(0x27FFC10)
#define NTR_BIOS_DEBUG_SIO_EXISTS_COPY *(volatile u16*)(0x27FFC14)
#define NTR_BIOS_BOOT_INDICATOR *(volatile u16*)(0x27FFC40)
#define NTR_BIOS_SWI_IS_DEBUGGER_ARM7_SCRATCH *(volatile u16*)(0x27FFFFA)
#define NTR_BIOS_SWI_IS_DEBUGGER_ARM9_SCRATCH *(volatile u16*)(0x27FFFF8)

#endif
