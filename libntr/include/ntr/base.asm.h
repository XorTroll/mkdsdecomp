#ifndef _NTR_BASE_ASM_H
#define _NTR_BASE_ASM_H

#if __ASSEMBLER__

.macro NTR_BEGIN_ASM_FN name section=text
	.section .\section\().\name\(), "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm

#define NTR_DEBUG_LOG_BUF_SIZE 0x200

#endif

#endif
