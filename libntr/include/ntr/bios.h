#ifndef _NTR_BIOS_H
#define _NTR_BIOS_H

#include "base.h"

// TODO: consider defining a struct for this?

#define NTR_BIOS_ARM7_BIOS_CRC *(volatile u16*)(0x27FFC10)
#define NTR_BIOS_DEBUG_SIO_EXISTS *(volatile u16*)(0x27FF814)
#define NTR_BIOS_DEBUG_SIO_EXISTS_COPY *(volatile u16*)(0x27FF814)
#define NTR_BIOS_SWI_IS_DEBUGGER_ARM7_SCRAPE_ADDR *(volatile u16*)(0x27FFFFA)
#define NTR_BIOS_SWI_IS_DEBUGGER_ARM9_SCRAPE_ADDR *(volatile u16*)(0x27FFFF8)

#endif