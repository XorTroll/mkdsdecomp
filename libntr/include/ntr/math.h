#ifndef _NTR_MATH_H
#define _NTR_MATH_H

#include "base.h"

#define NTR_MATH_DIV_NUMER *(volatile u64*)(0x4000290)
#define NTR_MATH_DIV_NUMER_H *(volatile u32*)(0x4000290)
#define NTR_MATH_DIV_NUMER_L *(volatile u32*)(0x4000294)

#define NTR_MATH_DIV_DENOM *(volatile u64*)(0x4000298)
#define NTR_MATH_DIV_DENOM_H *(volatile u32*)(0x4000298)
#define NTR_MATH_DIV_DENOM_L *(volatile u32*)(0x400029C)

#endif
