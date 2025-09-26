#ifndef __GOODIX_HSM_H
#define __GOODIX_HSM_H


#include "goodix_type.h"
#include "goodix_algo.h"
#ifdef __cplusplus
extern "C" {
#endif

extern int32_t HSMAlgoDeInit(void);
extern int32_t HSMAlgoInit(int32_t *Fs);
extern uint8_t *HSMAlgoVersion(void);
typedef struct
{
	GU32 ulSleepInTime;
	GU32 ulSleepOutTime;
	GU16 nSleepLength;
	GU16 nDeepRatio;
	GU16 nLightRatio;
	GU16 nRemRatio;
	GU8 nWakeTimes;
	GU8 nSleepScore;
}ST_HSM_SCORE;
typedef struct
{
	int32_t IPpgRaw;
	int32_t IACCx;
	int32_t IACCy;
	int32_t IACCz;
	int32_t IHeart;
}ST_HSM_DATA_IN;
#define GH3X2X_HSM_FLAG_BUFF_LEN (120)
typedef struct
{
	uint8_t uchStageOutNum;
	uint8_t uchStageDataFlag;
	uint8_t puchHSMStageOut[GH3X2X_HSM_FLAG_BUFF_LEN];
}ST_HSM_DATA_OUT;
extern int32_t HSMProc(ST_HSM_DATA_IN* pstHSMDataIn, ST_HSM_DATA_OUT* pstHSMDataOut, ST_HSM_SCORE* pstHSMScore);
extern int32_t HSMLastDataProc(ST_HSM_DATA_OUT* pstHSMDataOut, ST_HSM_SCORE* pstHSMScore);

#ifdef __cplusplus
}
#endif

#endif
