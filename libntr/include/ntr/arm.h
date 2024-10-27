#ifndef _NTR_ARM_H
#define _NTR_ARM_H

#include "base.h"
#include "arm.asm.h"

void Arm_FlushRange(void *ptr, size_t len);

typedef struct {
    u32 div_numer_param_h;
    u32 div_numer_param_l;
    u32 div_denom_param_h;
    u32 div_denom_param_l;
    u32 sqrt_param_h;
    u32 sqrt_param_l;
    u16 divcnt_val;
    u16 sqrtcnt;
} Arm_MathContext;

typedef struct {
    u32 cpsr;
    u32 r[13];
    u32 sp;
    u32 lr;
    u32 pc_p4;
    u32 sp_svc;
    Arm_MathContext math_ctx;
} Arm_Context;

void Arm_MathContext_Load(Arm_MathContext *ctx);
void Arm_MathContext_Save(Arm_MathContext *ctx);

void Arm_Context_Create(Arm_Context *ctx, uintptr_t pc_addr, uintptr_t sp_addr);
void Arm_Context_Load(Arm_Context *ctx);
void Arm_Context_Save(Arm_Context *ctx);

#define ARM_BUS_CLOCK 33514000

inline int Arm_MillisecondsToTicks(const int ms) {
    return (ms * (ARM_BUS_CLOCK / 1000)) / 64;
}

#endif
