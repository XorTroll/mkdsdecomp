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

#endif

#endif
