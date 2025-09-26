/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_demo_multi_sen_pro.h
 *
 * @brief   gh3x2x adt
 *
 * @version ref gh3x2x_drv_version.h
 *
 */


#ifndef _gh_demo_multi_sen_pro_H_
#define _gh_demo_multi_sen_pro_H_


#include "gh_drv.h"

/// wear on event en set
#define  WEAR_ON_EVENT_EN_CONFIG(en)        do {\
                                                GH3X2X_WriteRegBitField(GH3X2X_INT_CR2_REG_ADDR, \
                                                    GH3X2X_WEAR_ON_EVENT_EN_LSB, GH3X2X_WEAR_ON_EVENT_EN_MSB, (en)); \
                                            } while (0)

/// wear off event en set
#define  WEAR_OFF_EVENT_EN_CONFIG(en)        do {\
                                                GH3X2X_WriteRegBitField(GH3X2X_INT_CR2_REG_ADDR, \
                                                    GH3X2X_WEAR_OFF_EVENT_EN_LSB, GH3X2X_WEAR_OFF_EVENT_EN_MSB, (en)); \
                                            } while (0)



#define SOFT_WEAR_INPUT_DATA_CHNL0_INDEX   (0)
#define SOFT_WEAR_INPUT_DATA_ACCX_INDEX    (1)
#define SOFT_WEAR_INPUT_DATA_ACCY_INDEX    (2)
#define SOFT_WEAR_INPUT_DATA_ACCZ_INDEX    (3)
#define SOFT_WEAR_INPUT_COLOR_INDEX        (4)
#define SOFT_WEAR_INPUT_DATA_ADT_INDEX     (5) //ir  adt data
#define SOFT_WEAR_INPUT_DATA_BG_INDEX      (6)
#define SOFT_WEAR_INPUT_DATA_CAP0_INDEX    (7)
#define SOFT_WEAR_INPUT_DATA_CAP1_INDEX    (8)
#define SOFT_WEAR_INPUT_DATA_CAP2_INDEX    (9)
#define SOFT_WEAR_INPUT_DATA_CAP3_INDEX    (10)


#define SOFT_WEAR_INPUT_DATA_INDEX_TOTAL (11)
#define SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL (8)


#define SOFT_WEAR_OUTPUT_WEAR_STATE_BIT  (3)   


#define NADTCAP_BYTE_TRUE       (GU8)1
#define NADTCAP_BYTE_FALSE      (GU8)0



#define NADT_SAMPLE_FREQ  25
#define NADT_WWD_IIR_LEN  4

#define NADT_WEAR_LIVE_DETECTION
#ifdef  NADT_WEAR_LIVE_DETECTION // this is wear live dection, full wear dection,can distinguish body and other thing; more Ram
#define NADT_PPG_BUFF_LEN (4) //4
#else
#define NADT_PPG_BUFF_LEN (2) //lite
#endif // NADT_WEAR_LIVE_DETECTION

#define NADT_WWD_FILTER_BUFF_LEN (NADT_PPG_BUFF_LEN*NADT_SAMPLE_FREQ)
#define NADT_MATH_ROUND(v) round(v)
#define NADT_MATH_ABSF(v)  fabs(v)
#define NADT_MATH_ABS(v)   abs(v)
#define NADT_MATH_ROUNDF(v) roundf(v)
#define NADT_ABS_GS32(v) abs((GS32)(v))
#define NADT_MIN(a,b) ((a) > (b) ? (b) : (a))



#define NADT_PV_PEAK (1)
#define NADT_PV_VALLEY (2)
#define NADT_PV_PV_INTER (4)
#define NADT_ACC_TRUNCATE (5)

#define GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_ON            0   //index of member in linked list for gh senosr wear on
#define GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_OFF           1   //index of member in linked list for gh senosr wear off
#define GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE               2   //index of member in linked list for gsensor move
#define GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE_TIME_OUT      3   //index of member in linked list for gsenosr move time out

#define GH3X2X_MULTI_SENSOR_EVENT_NUM_MAX           4
#define GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID     255

#define GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON        (1 << GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_ON)        // gh sensor wear on
#define GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_OFF       (1 << GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GH_WEAR_OFF)       // gh sensor wear off
#define GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE           (1 << GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE)           // gsensor move
#define GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT  (1 << GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_GS_MOVE_TIME_OUT)  //gs  move time out





typedef struct
{
    GS32 *pData;
    GS32 nLength;
    GS32 nPos;
}NADT_QUEUE_BUFFER_S32;

typedef struct
{
    GS16 *pData;
    GS32 nLength;
    GS32 nPos;
}NADT_QUEUE_BUFFER_S16;

typedef struct
{
    const GF32 *pfCoefB;
    const GF32 *pfCoefA;
    GF32 *pfXbuff;
    GF32 *pfYbuff;
    GS32 lLen;
    GF32 fThr;
}NADT_ST_IIR_PARAM;

typedef struct
{
    GS32 *pfFftValue;
    GS16 *psPeakIndex;
    GS16 *psValleyIndex;
    GS16 *psPeakValleyIndex;
    GS32 lPeakValleyLen;
    GS32 lPeakLen;
    GS32 lValleyLen;
    GS32 lDataLen;
}NADT_ST_PEAK_VALLEY_INFO;

typedef struct
{
    GBOOL bSearchMax;
    GS32 fExtremumValue;
    GS32 lPreExtremumPos;
    GS32 lExtremumPos;
    GS32 lDataCount;
}NADT_ST_PEAKX_DET;





void GhMultSensorWearEventManagerInit(void);
void GhMultSensorWearEventManagerHookRegister(void (*WearEventHook) (GU32 uchNewEvent));
void GhMultSensorWearEventManagerEnable(void);
void GhMultSensorWearEventManagerDisable(void);
GU8 GhMultSensorWearEventManagerIsEnable(void);
void GhMultSensorWearEventSend(GU32 unEvent);
GU8 GhMultiSensorConditionCheckIsNew (GU32 unEventNew, GU32 unEventOld);
GU8 GhMultiSensorConditionCheckInWindow (GU32 unEventComb, GU32 unWindow, GU8 uchNowTimeNeedInWin);





#define GH_MULTI_SENSOR_TIMER_USER_TIME_STAMP 0
#define GH_MULTI_SENSOR_TIMER_USER_MOVE_DET   1

#define GH_MULTI_SENSOR_TIMER_USER_NUM_MAX 2


typedef struct
{
	void (*TimerCreate)(GU32 unTimerPeriodMs);
    void (*TimerStart)(void);
	void (*TimerStop)(void);
    void (*TimerDelete)(void);
}STGhMultiSensorTimerOps;

typedef struct
{
	void (*TimerHandle)(GU32 unTimeIntervalMs);
}STGhMultiSensorTimerUser;


typedef struct
{
	const STGhMultiSensorTimerOps *pstTimerOps;
	STGhMultiSensorTimerUser stTimerUsers[GH_MULTI_SENSOR_TIMER_USER_NUM_MAX];
}STGhMultiSensorTimer;




void GhMultiSensorTimerInit(void);
void GhMultiSensorTimerStart(void);
void GhMultiSensorTimerStop(void);
void GhMultiSensorTimerOpsRegister(const STGhMultiSensorTimerOps *pstTimerOps);
void GhMultiSensorTimerOpsUnregister(void);
void GhMultiSensorTimerHandle(void);



void GhGsMoveDetecterInit(void);
void GhGsMoveDetecterEnable(void);
void GhGsMoveDetecterDisable(void);
GU8 GhGsMoveDetecterIsEnable(void);

#endif /* _GH3X2X_DRV_SOFT_LEAD_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

