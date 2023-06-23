#ifndef _MKDS_OS_H
#define _MKDS_OS_H

#include "base.h"

u32 Os_DisableIRQ();
void Os_RestoreIRQ(u32 prev_state);

#endif