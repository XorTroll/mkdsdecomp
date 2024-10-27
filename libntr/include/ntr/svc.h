#ifndef _NTR_SVC_H
#define _NTR_SVC_H

#include <ntr/base.h>

void Svc_VblankIntrWait(void);
void Svc_Delay(u32 duration);
void Svc_Copy(const void *src, void *dst, u32 flags);

#endif
