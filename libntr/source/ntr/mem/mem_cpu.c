#include <ntr/mem.h>


void Mem_CpuFill32(int val, void *src, size_t size) {
    u32 *cur_32 = (u32*)src;
    u32 *end_32 = (u32*)(src + size);

    while(cur_32 < end_32) {
        *cur_32 = val;
        cur_32++;
    }
}
