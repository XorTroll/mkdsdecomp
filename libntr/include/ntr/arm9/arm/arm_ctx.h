#ifndef _NTR_ARM9_ARM_CTX_H
#define _NTR_ARM9_ARM_CTX_H

#include <ntr/base.h>
#include <ntr/arm9/arm/arm.asm.h>

typedef struct Arm_MathContext {
    u32 div_numer_param_h;
    u32 div_numer_param_l;
    u32 div_denom_param_h;
    u32 div_denom_param_l;
    u32 sqrt_param_h;
    u32 sqrt_param_l;
    u16 divcnt_val;
    u16 sqrtcnt;
} Arm_MathContext;
_Static_assert(sizeof(Arm_MathContext) == 0x1C);

void Arm_MathContext_Load(Arm_MathContext *ctx);
void Arm_MathContext_Save(Arm_MathContext *ctx);

typedef struct Arm_Context {
    u32 cpsr;
    u32 r[13];
    u32 sp;
    u32 lr;
    u32 pc_p4;
    u32 sp_svc;
    Arm_MathContext math_ctx;
} Arm_Context;
_Static_assert(sizeof(Arm_Context) == 0x64);

void Arm_Context_Create(Arm_Context *ctx, uintptr_t pc_addr, uintptr_t sp_addr);
void Arm_Context_Load(Arm_Context *ctx);
bool Arm_Context_Save(Arm_Context *ctx);

#endif
