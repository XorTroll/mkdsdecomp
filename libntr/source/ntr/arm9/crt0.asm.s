#include <ntr/base.asm.h>
#include <ntr/arm.asm.h>

.arch armv5te
.cpu arm946e-s
.section ".crt0","ax"

.align 4
.arm

.global _start
_start:
    @ IME = 0
    mov r12, #0x04000000
    str r12, [r12, #0x208]
    
    bl Crt0_InitializePU

    @ Note: these SP offsets are computed here (in ASM) in official code
    @ For simplicity we borrow libnds's idea and just compute them in the ldscript
    @ (left commented the original code)

    mov r0, #0x13
    msr cpsr_c, r0

    ldr sp, = __sp_svc
/*
    ldr r0, =__dtcm_start
    add r0, r0, #0x3FC0
    mov sp, r0
*/

    mov r0, #0x12
    msr cpsr_c, r0

    ldr sp, =__sp_irq
/*
    ldr r0, =__dtcm_start
    add r0, r0, #0x3FC0
    sub r0, r0, #0x40
    sub sp, r0, #4
    ldr r1, =0xC00
    sub r1, r0, r1
*/

    mov r0, #0x1F
    msr cpsr_cxsf, r0

    ldr sp, =__sp_usr
/*
    sub sp, r1, #4
*/
    
    @ Clear DTCM
    mov r0, #0
    ldr r1, =__dtcm_start
    mov r2, #0x4000
    bl Crt0_MemorySet32

    @ Clear top screen main BG palette data
    mov r0, #0
    ldr r1, =0x05000000 @ TODO: proper register macros!
    mov r2, #0x400
    bl Crt0_MemorySet32

    @ Clear top screen OAM data
    mov r0, #0x200
    ldr r1, =0x07000000 @ TODO: proper register macros!
    mov r2, #0x400
    bl Crt0_MemorySet32

/* TODO: finish REing this object (seems to contain start/end addresses for BSS and other regions...?)
    ldr r1, =#0x02000B4C
    ldr r0, [r1, #0x14]

    @ <this subroutine is basically code decomp>
    bl Crt0_DecompressFlushCode

    @ <this subroutine copies from some addresses to others, using addresses and sizes in the mentioned object>
    bl Crt0_SomeRegionCopy

    ldr r0, =#0x02000B4C
    ldr r1, [r0, #0xC]
    ldr r2, [r0, #0x10]
    mov r3, r1
    mov r0, #0

BssCopyLoop:
    cmp r1, r2
    strcc r0, [r1], #4
    bcc BssCopyLoop
    bic r1, r3, #0x1F

InlinedFlushLoop:
    mcr p15, 0, r0, c7, c10, 4
    mcr p15, 0, r1, c7, c5, 1
    mcr p15, 0, r1, c7, c14, 1
    add r1, r1, #0x20
    cmp r1, r2
    blt InlinedFlushLoop
*/

    mov r0, #0
    @ Zero this address - is this set/used by arm7, maybe an undocumented value...?
    ldr r1, =#0x027FFF9C
    str r0, [r1]

/* TODO: what is this DTCM offset?
    ldr r1, =__dtcm_start
    add r1, r1, #0x3FC0
    add r1, r1, #0x3C
    ldr r0, =0x01FF8000 @ what even is this value?
    str r0, [r1]
*/

    @ (unofficial) clean BSS in this way for now
    bl Crt0_CleanBss

    @ TODO: RE call to function which resets some flag and some fn pointers, which are used elsewhere

    @ Call nknown empty function
    bl Crt0_UnkNullsub

    @ Run the init array
    bl Crt0_RunInitArray

    @ Finally, call main
    ldr r1, =NtrMain
    ldr lr, =#0xFFFF0000 @ arm9 exception vectors?
    bx r1

Crt0_InitializePU:
    @ Enable stuff
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, = NTR_ARM_PROTECT_ENABLE | NTR_ARM_DCACHE_ENABLE | NTR_ARM_ICACHE_ENABLE | NTR_ARM_DISABLE_TBIT | NTR_ARM_DTCM_ENABLE | NTR_ARM_DTCM_LOAD | NTR_ARM_ITCM_ENABLE | NTR_ARM_ITCM_LOAD
    bic r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0

   @ Disable cache
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0        @ Instruction cache
    mcr p15, 0, r0, c7, c6, 0        @ Data cache

    @ Wait for write buffer to empty
    mcr p15, 0, r0, c7, c10, 4

    @ Region 0 - IO registers
    ldr r0, = NTR_ARM_PAGE_64M | 0x04000000 | 1
    mcr p15, 0, r0, c6, c0, 0

    @ Region 1 - System ROM
    @ Note: 8MB is the debug size!
    ldr r0, = NTR_ARM_PAGE_8M | 0x02000000 | 1
    mcr p15, 0, r0, c6, c1, 0

    @ Region 2 - alternate vector base
    @ Note: no way to load this in one ldr instr (probably official compilation has the value already or'd from ldscript)
    ldr r0, = __dtcm_start @ why DTCM? Official code seems to mix DTCM/ITCM stuff here
    orr r0, r0, #NTR_ARM_PAGE_128K
    orr r0, r0, #1
    mcr p15, 0, r0, c6, c2, 0

    @ Region 3 - DS Accessory (GBA Cart) / DSi switchable iwram
    ldr r0, = NTR_ARM_PAGE_128M | 0x08000000 | 1
    mcr p15, 0, r0, c6, c3, 0

    @ Region 4 - ITCM
    ldr r0, = __dtcm_start @ again, why DTCM?
    orr r0, r0, #NTR_ARM_PAGE_16K
    orr r0, r0, #1
    mcr p15, 0, r0, c6, c4, 0

    @ Region 5 - DTCM
    ldr r0, = NTR_ARM_PAGE_16M | 0x01000000 | 1
    mcr p15, 0, r0, c6, c5, 0

    @ Region 6 - non cacheable main ram
    ldr r0, = NTR_ARM_PAGE_32K | 0xFFFF0000 | 1
    mcr p15, 0, r0, c6, c6, 0

    @ Region 7 - cacheable main ram
    ldr r0, = NTR_ARM_PAGE_4K | 0x027FF000 | 1
    mcr p15, 0, r0, c6, c7, 0

    @ ITCM base = 0, size = 32 MB
    mov r0, #0x20
    mcr p15, 0, r0, c9, c1, 1
    
    @ DTCM base = __dtcm_start, size = 16 KB
    ldr r0, =__dtcm_start
    orr r0, r0, #0xA
    mcr p15, 0, r0, c9, c1, 0

    @ DCache & ICache enable
    ldr r0, =0b1000010
    mcr p15, 0, r0, c2, c0, 1
    ldr r0, =0b1000010
    mcr p15, 0, r0, c2, c0, 0

    @ Write buffer enable?
    mov r0, #2
    mcr    p15, 0, r0, c3, c0, 0

    @ IAccess
    ldr r0, =0x5100011
    mcr p15, 0, r0, c5, c0, 3

    @ DAccess
    ldr r0, =0x15111011
    mcr p15, 0, r0, c5, c0, 2

    @ Enable more stuff
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, = NTR_ARM_PROTECT_ENABLE | NTR_ARM_DCACHE_ENABLE | NTR_ARM_UNK_BIT3 | NTR_ARM_UNK_BIT4 | NTR_ARM_UNK_BIT5 | NTR_ARM_UNK_BIT6 | NTR_ARM_ICACHE_ENABLE | NTR_ARM_ALT_VECTORS | NTR_ARM_ROUND_ROBIN | NTR_ARM_DTCM_ENABLE | NTR_ARM_ITCM_ENABLE
    orr r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0

    bx lr

Crt0_MemorySet32:
    add r12, r1, r2
SetLoop:
    cmp r1, r12
    stmlt r1!, {r0}
    blt SetLoop
    bx lr

NTR_BEGIN_ASM_FN Crt0_DecompressFlushCode
    cmp r0, #0
    beq Exit
    push {r4-r7}
    ldmdb r0, {r1, r2}
    add r2, r0, r2
    sub r3, r0, r1, lsr#24
    bic r1, r1, #0xFF000000
    sub r1, r0, r1
    mov r4, r2
Loop1:
    cmp r3, r1
    ble InlinedFlushRange
    ldrb r5, [r3, #-1]!
    mov r6, #8
Loop2:
    subs r6, r6, #1
    blt Loop1
    tst r5, #0x80
    bne Case1
    ldrb r0, [r3, #-1]!
    strb r0, [r2, #-1]!
    b CheckDone
Case1:
    ldrb r12, [r3, #-1]!
    ldrb r7, [r3, #-1]!
    orr r7, r7, r12, lsl#8
    bic r7, r7, #0xF000
    add r7, r7, #2
    add r12, r12, #0x20
Loop3:
    ldrb r0, [r2, r7]
    strb r0, [r2, #-1]!
    subs r12, r12, #0x10
    bge Loop3
CheckDone:
    cmp r3, r1
    mov r5, r5, lsl#1
    bgt Loop2
InlinedFlushRange:
    mov r0, #0
    bic r3, r1, #0x1F
FlushLoop:
    mcr p15, 0, r0, c7, c10, 4
    mcr p15, 0, r3, c7, c5, 1
    mcr p15, 0, r3, c7, c14, 1
    add r3, r3, #0x20
    cmp r3, r4
    blt FlushLoop
    pop {r4-r7}
Exit:
    bx lr

Crt0_RunInitArray:
/* TODO: fix or rethink this and/or ldscript (libnds's ldscript's init-array is not compatible with the way N does it)
    push {r4, lr}
    ldr r4, =__init_array_start
    b CheckEntry
RunEntryNext:
    blx r0
    add r4, r4, #4
CheckEntry:
    cmp r4, #0
    popeq {r4, lr}
    bxeq lr
    ldr r0, [r4]
    cmp r0, #0
    bne RunEntryNext
    pop {r4, lr}
*/
    bx lr

Crt0_UnkNullsub:
    bx lr