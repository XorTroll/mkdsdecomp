
#include <debug.h>
@ Function for printing stuff to emulator console, grabbed from: https://github.com/Dirbaio/ASMPatchTemplate/blob/master/source/print.s

.macro NTR_BEGIN_ASM_FN name section=text
	.section .\section\().\name\(), "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm

NTR_BEGIN_ASM_FN DebugPrint
	push {r4-r9}
	ldr r4, =g_DebugLogBuffer
	mov r5, #0

CopyLoop:
	ldrb r6, [r0]
	cmp r6, #0
	beq DoPrint
	cmp r5, #DEBUG_LOG_BUF_SIZE
	beq DoPrint
	strb r6, [r4]
	
	add r0, r0, #0x1
	add r4, r4, #0x1
	add r5, r5, #0x1
	b CopyLoop

DoPrint:
	mov r7, #0
	strb r7, [r4, r5]

	mov r0, r1
	mov r1, r2
	mov r2, r3
	
@ Region of debug logging
	mov  r12, r12 @ First ID
	b PrintEnd    @ Emulator will check this, we just jump to finish the call
	.word 0x6464 @ Second ID
g_DebugLogBuffer:
	.fill DEBUG_LOG_BUF_SIZE
	.byte 0                @ ending zero (normally not required, see below)
	.align 4                @ align following code (use align 2 in thumb mode)

PrintEnd:
	pop {r4-r9}
	bx lr
