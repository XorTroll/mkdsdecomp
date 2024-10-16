#ifndef _NTR_BASE_H
#define _NTR_BASE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "base.asm.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define NTR_BIT(n) (1 << (n))

extern void DebugLog(const char *msg);

#define DebugLogf(fmt, ...) { \
    char tmp_buf[0x200]; \
    snprintf(tmp_buf, sizeof(tmp_buf) - 1, fmt, ##__VA_ARGS__); \
    DebugLog(tmp_buf); \
}

// TODO: define registers properly

#endif
