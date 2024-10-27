#ifndef _NTR_EXTRA_LOG_H
#define _NTR_EXTRA_LOG_H

#include <ntr/extra/extra_log.asm.h>
#include <ntr/base.h>

// Custom nocash-like logging (emulators like melonDS pick these up in console)

// Uncomment to disable debug logging

#define NTR_EXTRA_ENABLE_DEBUGLOG

extern void Extra_DebugLog(const char *msg);

// Note: preceding all logs with the identifier below to make them more identifiable in all the console output emulators generate

#ifdef NTR_EXTRA_ENABLE_DEBUGLOG
#define NTR_EXTRA_DEBUGLOGF(fmt, ...) { \
    char tmp_buf[NTR_EXTRA_DEBUGLOG_BUF_SIZE]; \
    snprintf(tmp_buf, sizeof(tmp_buf) - 1, "{ExtraLog} " fmt "\n", ##__VA_ARGS__); \
    Extra_DebugLog(tmp_buf); \
}
#else
#define NTR_EXTRA_DEBUGLOGF(fmt, ...)
#endif

#endif
