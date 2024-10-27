#ifndef _NTR_MEM_ADDR_H
#define _NTR_MEM_ADDR_H

#include <ntr/base.h>

// Note: officially some of these stack sizes are in reality linker-emitted values (for example, MKDS has sys stack size 0x2800 but Nintendogs has 0x1400)...

#define NTR_MEM_ADDR_SVC_STACK_SIZE 0x40
#define NTR_MEM_ADDR_SYS_STACK_SIZE 0x2800
#define NTR_MEM_ADDR_IRQ_STACK_SIZE 0xC00

extern uintptr_t __end__;
extern uintptr_t __itcm_start;
extern uintptr_t __itcm_end;
extern uintptr_t __dtcm_start;
extern uintptr_t __dtcm_end;

// Main (0x02000000-0x02700000) = MainCode (0x02000000-code_end, secure + crt0 + text + rodata + data + bss) + MainRam (code_end-0x023E0000) + ??? (0x023E0000-0x02400000) + MainRamEx (0x02400000-0x02700000)

#define NTR_MEM_ADDR_MAIN_CODE_START (uintptr_t)(0x2000000)
#define NTR_MEM_ADDR_MAIN_CODE_END (uintptr_t)(&__end__)
#define NTR_MEM_ADDR_MAIN_CODE_SIZE (NTR_MEM_ADDR_MAIN_CODE_END - NTR_MEM_ADDR_MAIN_CODE_START)

#define NTR_MEM_ADDR_MAIN_RAM_START NTR_MEM_ADDR_MAIN_CODE_END
#define NTR_MEM_ADDR_MAIN_RAM_END (uintptr_t)(0x23E0000)
#define NTR_MEM_ADDR_MAIN_RAM_SIZE (NTR_MEM_ADDR_MAIN_RAM_END - NTR_MEM_ADDR_MAIN_RAM_START)

#define NTR_MEM_ADDR_MAIN_RAM_EX_START (uintptr_t)(0x2400000)
#define NTR_MEM_ADDR_MAIN_RAM_EX_END (uintptr_t)(0x2700000)
#define NTR_MEM_ADDR_MAIN_RAM_EX_SIZE (NTR_MEM_ADDR_MAIN_RAM_EX_END - NTR_MEM_ADDR_MAIN_RAM_EX_START)

#define NTR_MEM_ADDR_ITCM_START (uintptr_t)(&__itcm_start)
#define NTR_MEM_ADDR_ITCM_END (uintptr_t)(&__itcm_end)
#define NTR_MEM_ADDR_ITCM_SIZE (NTR_MEM_ADDR_ITCM_END - NTR_MEM_ADDR_ITCM_START)

#define NTR_MEM_ADDR_DTCM_START (uintptr_t)(&__dtcm_start)
#define NTR_MEM_ADDR_DTCM_END (uintptr_t)(&__dtcm_end)
#define NTR_MEM_ADDR_DTCM_SIZE (NTR_MEM_ADDR_DTCM_END - NTR_MEM_ADDR_DTCM_START)

// TODO: what is this initial 0xE0?
#define NTR_MEM_ADDR_DTCM_AUTOLOAD_START NTR_MEM_ADDR_DTCM_START + 0xE0

#define NTR_MEM_ADDR_DTCM __attribute__((used, section(".dtcm")))

#define NTR_MEM_ADDR_DTCM_SVC_STACK_BOTTOM   (NTR_MEM_ADDR_DTCM_END - NTR_MEM_ADDR_SVC_STACK_SIZE)
#define NTR_MEM_ADDR_DTCM_SVC_STACK_TOP       (NTR_MEM_ADDR_DTCM_SVC_STACK_BOTTOM - NTR_MEM_ADDR_SVC_STACK_SIZE)

#define NTR_MEM_ADDR_DTCM_IRQ_STACK_BOTTOM       NTR_MEM_ADDR_DTCM_SVC_STACK_TOP
#define NTR_MEM_ADDR_DTCM_IRQ_STACK_TOP       (NTR_MEM_ADDR_DTCM_IRQ_STACK_BOTTOM - NTR_MEM_ADDR_IRQ_STACK_SIZE)

#define NTR_MEM_ADDR_SHARED_START (uintptr_t)(0x27FF000)
#define NTR_MEM_ADDR_SHARED_END (uintptr_t)(0x27FF680)

#define NTR_MEM_ADDR_WRAM_MAIN_START (uintptr_t)(0x37F8000)
#define NTR_MEM_ADDR_WRAM_MAIN_END NTR_MEM_ADDR_WRAM_MAIN_START

#define NTR_MEM_ADDR_MEM_REGION_START_LIST_START (void**)(0x27FFDA0)
#define NTR_MEM_ADDR_MEM_REGION_END_LIST_START (void**)(0x27FFDC4)

#endif
