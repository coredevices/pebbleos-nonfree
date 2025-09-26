#ifndef __GOODIX_AF_H
#define __GOODIX_AF_H


#include "goodix_type.h"
#include "goodix_algo.h"
#ifdef __cplusplus
extern "C" {
#endif

extern const GU8 *AFVersion(void);
extern GS32 AFInit(GS32 g_s32WaitTime);
extern GS32 AF_Detection_Module(GS32 *s32RRIRawdataArray,GU32* nResult);
extern GS32 AFDeInit(void);

#ifdef __cplusplus
}
#endif

#endif
