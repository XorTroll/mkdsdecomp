#ifndef _NTR_OS_H
#define _NTR_OS_H

#include "base.h"

u32 Os_DisableIRQ(void);
void Os_RestoreIRQ(u32 prev_state);

typedef enum Os_MemoryRegion {
    Os_MemoryRegion_MainRam = 0,
    Os_MemoryRegion_Unknown2 = 2,
    Os_MemoryRegion_Unknown3 = 3,
    Os_MemoryRegion_Unknown4 = 4,
    Os_MemoryRegion_Unknown5 = 5,
    Os_MemoryRegion_Unknown6 = 6
} Os_MemoryRegion;

void *Os_GetMemoryRegionStartAddress(Os_MemoryRegion region);
void *Os_GetMemoryRegionEndAddress(Os_MemoryRegion region);

void Os_Initialize(void);

void Os_DefaultDisableForceDebugMode(void);


typedef struct {

} Os_Thread;

u32 Os_AllocateLockId(void);

#endif
