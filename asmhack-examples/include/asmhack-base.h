#ifndef ASMHACK_BASE_H
#define ASMHACK_BASE_H

#include <mkds-eu-decomp.h>
#include <debug.h>

#include <stddef.h>
#include <stdbool.h>

#define DebugPrintf(fmt, ...) { \
    char tmp_buf[DEBUG_LOG_BUF_SIZE]; \
    Util_SNPrintF(tmp_buf, sizeof(tmp_buf) - 1, fmt, ##__VA_ARGS__); \
    tmp_buf[sizeof(tmp_buf) - 1] = 0; \
    DebugPrint(tmp_buf); \
}

void IntToBinaryString(int value, char *out_str, size_t size);

#endif
