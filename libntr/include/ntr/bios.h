#ifndef _NTR_BIOS_H
#define _NTR_BIOS_H

#include "base.h"

// Definitions for all the various fields set by the BIOS in the RAM

// TODO: consider defining a struct for this?

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