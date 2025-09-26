#ifndef __GOODIX_RESP_H
#define __GOODIX_RESP_H


#include "goodix_type.h"
#include "goodix_algo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern GS32 RespInit(GS32 *fs);
extern void RespDeInit (void);
typedef struct
{
	GS32 IPpgRaw;
	GS32 IACCx;
	GS32 IACCy;
	GS32 IACCz;
	GS32 IAdjFlag;
}RESP_ST_DATA_IN;
typedef struct
{
	GU8 nRespRate;
	GU8 nBelieve;
	GBOOL nRespOutFlag;
}RESP_ST_DATA_OUT;
extern GS32 RespProc(RESP_ST_DATA_IN* stRespDataIn, RESP_ST_DATA_OUT* stRespDataOut);
extern GU8* RespAlgoVersion (void);

#ifdef __cplusplus
}
#endif

#endif
