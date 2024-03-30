#include "arm.h"
#include "math.h"

void Arm_MathContext_Load(Arm_MathContext *ctx) {

}

void Arm_MathContext_Save(Arm_MathContext *ctx) {
    ctx->div_numer_param_h = NTR_MATH_DIV_NUMER_H;
    ctx->div_numer_param_l = NTR_MATH_DIV_NUMER_L;
}

void Arm_Context_Create(Arm_Context *ctx, uintptr_t pc_addr, uintptr_t sp_addr) {
    ctx->pc_p4 = pc_addr + 4;
    ctx->sp_svc = sp_addr;
    ctx->sp = sp_addr - 64;

    if(ctx->pc_p4 & 1) {
        ctx->cpsr = 0x3F;
    }
    else {
        ctx->cpsr = 0x1F;
    }

    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->r[3] = 0;
    ctx->r[4] = 0;
    ctx->r[5] = 0;
    ctx->r[6] = 0;
    ctx->r[7] = 0;
    ctx->r[8] = 0;
    ctx->r[9] = 0;
    ctx->r[10] = 0;
    ctx->r[11] = 0;
    ctx->r[12] = 0;

    ctx->lr = 0;
}

void Arm_Context_Load(Arm_Context *ctx) {

}

void Arm_Context_Save(Arm_Context *ctx) {

}
