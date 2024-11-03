#ifndef _NTR_ARM9_OS_INIT_H
#define _NTR_ARM9_OS_INIT_H

#include <ntr/base.h>

typedef enum Os_MemoryRegion {
    Os_MemoryRegion_MainRam = 0,
    Os_MemoryRegion_MainRamEx = 2,
    Os_MemoryRegion_Itcm = 3,
    Os_MemoryRegion_Dtcm = 4,
    Os_MemoryRegion_Shared = 5,
    Os_MemoryRegion_WramMain = 6
} Os_MemoryRegion;

void *Os_GetMemoryRegionStartAddress(Os_MemoryRegion region);
void *Os_GetMemoryRegionEndAddress(Os_MemoryRegion region);

void Os_Initialize(void);
void Os_DisableMainExRegion(void);

#endif
