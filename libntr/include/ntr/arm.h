#ifndef _NTR_ARM_H
#define _NTR_ARM_H

#include "base.h"
#include "arm.asm.h"

void Arm_FlushAll();
void Arm_FlushRange(void *ptr, size_t len);

#endif