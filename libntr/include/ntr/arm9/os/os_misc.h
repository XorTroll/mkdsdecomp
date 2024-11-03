#ifndef _NTR_ARM9_OS_MISC_H
#define _NTR_ARM9_OS_MISC_H

#include <ntr/base.h>

void Os_ReferSymbol(const char *symbol);

__attribute__((noreturn)) void Os_Terminate(void);
void Os_Halt(void);

#endif
