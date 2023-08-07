#ifndef _NTR_ARM_H
#define _NTR_ARM_H

#define NTR_ARM_PAGE_4K		(0b01011 << 1)
#define NTR_ARM_PAGE_8K		(0b01100 << 1)
#define NTR_ARM_PAGE_16K	(0b01101 << 1)
#define NTR_ARM_PAGE_32K	(0b01110 << 1)
#define NTR_ARM_PAGE_64K	(0b01111 << 1)
#define NTR_ARM_PAGE_128K	(0b10000 << 1)
#define NTR_ARM_PAGE_256K	(0b10001 << 1)
#define NTR_ARM_PAGE_512K	(0b10010 << 1)
#define NTR_ARM_PAGE_1M		(0b10011 << 1)
#define NTR_ARM_PAGE_2M		(0b10100 << 1)
#define NTR_ARM_PAGE_4M		(0b10101 << 1)
#define NTR_ARM_PAGE_8M		(0b10110 << 1)
#define NTR_ARM_PAGE_16M	(0b10111 << 1)
#define NTR_ARM_PAGE_32M	(0b11000 << 1)
#define NTR_ARM_PAGE_64M	(0b11001 << 1)
#define NTR_ARM_PAGE_128M	(0b11010 << 1)
#define NTR_ARM_PAGE_256M	(0b11011 << 1)
#define NTR_ARM_PAGE_512M	(0b11100 << 1)
#define NTR_ARM_PAGE_1G		(0b11101 << 1)
#define NTR_ARM_PAGE_2G		(0b11110 << 1)
#define NTR_ARM_PAGE_4G		(0b11111 << 1)

#define NTR_ARM_ITCM_LOAD	(1<<19)
#define NTR_ARM_ITCM_ENABLE	(1<<18)
#define NTR_ARM_DTCM_LOAD	(1<<17)
#define NTR_ARM_DTCM_ENABLE	(1<<16)
#define NTR_ARM_DISABLE_TBIT	(1<<15)
#define NTR_ARM_ROUND_ROBIN	(1<<14)
#define NTR_ARM_ALT_VECTORS	(1<<13)
#define NTR_ARM_ICACHE_ENABLE	(1<<12)
#define NTR_ARM_BIG_ENDIAN	(1<<7)
#define NTR_ARM_UNK_BIT6 (1<<6)
#define NTR_ARM_UNK_BIT5 (1<<5)
#define NTR_ARM_UNK_BIT4 (1<<4)
#define NTR_ARM_UNK_BIT3 (1<<3)
#define NTR_ARM_DCACHE_ENABLE	(1<<2)
#define NTR_ARM_PROTECT_ENABLE	(1<<0)

// TODO: consider splitting the file for asm and non-asm?

#if !__ASSEMBLER__

#include "base.h"

void Arm_FlushAll();
void Arm_FlushRange(void *ptr, size_t len);

#endif

#endif