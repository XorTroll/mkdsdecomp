
@=================================
@ Function for printing stuff to emulator console, grabbed from: https://github.com/Dirbaio/ASMPatchTemplate/blob/master/source/print.s

.macro NTR_BEGIN_ASM_FN name section=text
	.section .\section\().\name\(), "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm


NTR_BEGIN_ASM_FN nocashPrint
	push {r4-r9}
	ldr r4, =msgData
	mov r5, #0
	
  loop:
		ldrb r6, [r0]
		cmp r6, #0
		beq printMsg
		cmp r5, #120
		beq printMsg
		strb r6, [r4]
		
		add r0, r0, #0x1
		add r4, r4, #0x1
		add r5, r5, #0x1
		b loop
		
  printMsg:
	mov r0, r1
	mov r1, r2
	mov r2, r3
	
	mov  r12,r12
	b    continue83
	.word  0x6464
  msgData:
	.fill 120
	.byte  0                @ending zero (normally not required, see below)
	.align 4               @align following code (use align 2 in thumb mode)
  continue83:

	pop {r4-r9}
bx lr
