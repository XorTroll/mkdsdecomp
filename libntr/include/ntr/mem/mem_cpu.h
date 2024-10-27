#ifndef _NTR_MEM_CPU_H
#define _NTR_MEM_CPU_H

#include <ntr/base.h>

void Mem_CpuFill8(void *ptr, u8 data, size_t size);
void Mem_CpuFill32(int val, void *ptr, size_t size);

#endif
