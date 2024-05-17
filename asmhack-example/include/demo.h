#ifndef _DEMO_H
#define _DEMO_H

#include <mkds-decomp.h>

extern void DebugPrint(const char *msg);

#define DebugPrintf(fmt, ...) { \
    char tmp_buf[0x200]; \
    Util_SNPrintF(tmp_buf, sizeof(tmp_buf) - 1, fmt, ##__VA_ARGS__); \
    DebugPrint(tmp_buf); \
}

#endif // _DEMO_H
