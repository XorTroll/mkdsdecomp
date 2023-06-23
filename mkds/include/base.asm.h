#ifndef _MKDS_BASE_ASM_H
#define _MKDS_BASE_ASM_H

#if !__ASSEMBLER__
	#error This header file is only for use in assembly files!
#endif

.macro MKDS_BEGIN_ASM_FN name section=text
	.section .\section\().\name\(), "ax", %progbits
	.global \name
	.type \name, %function
	.align 2
\name:
.endm

#endif
