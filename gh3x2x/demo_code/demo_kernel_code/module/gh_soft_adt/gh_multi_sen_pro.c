/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_adt.c
 *
 * @brief   gh3x2x adt functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#include <math.h>
#include "gh_demo_config.h"
#include "gh_demo_inner.h"
#include "gh_drv.h"
#include "gh_multi_sen_pro.h"

#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__

STGhMultiSensorTimer g_stGhMultiSensorTimer;

void GhMultiSensorTimerInit(void)
{
    GH3X2X_Memset((GU8*)(&g_stGhMultiSensorTimer), 0, sizeof(g_stGhMultiSensorTimer));
}

void GhMultiSensorTimerOpsRegister(const STGhMultiSensorTimerOps *pstTimerOps)
{
    g_stGhMultiSensorTimer.pstTimerOps = pstTimerOps;

    if(g_stGhMultiSensorTimer.pstTimerOps)
    {
        g_stGhMultiSensorTimer.pstTimerOps->TimerCreate(__GH_MULTI_SENSOR_TIMER_PERIOD_MS__);
    }
}

void GhMultiSensorTimerOpsUnregister(void)
{
    if(g_stGhMultiSensorTimer.pstTimerOps)
    {
        g_stGhMultiSensorTimer.pstTimerOps->TimerDelete();
    }
    g_stGhMultiSensorTimer.pstTimerOps = 0;
}

void GhMultiSensorTimerStart(void)
{
    if(g_stGhMultiSensorTimer.pstTimerOps)
    {
        g_stGhMultiSensorTimer.pstTimerOps->TimerStart();
    }
}

void GhMultiSensorTimerStop(void)
{
    if(g_stGhMultiSensorTimer.pstTimerOps)
    {
        g_stGhMultiSensorTimer.pstTimerOps->TimerStop();
    }
}

void GhMultiSensorTimerUserRegister(GU8 uchUserId, void (*TimerHandle) (GU32))
{
    if(uchUserId < GH_MULTI_SENSOR_TIMER_USER_NUM_MAX)
    {
        g_stGhMultiSensorTimer.stTimerUsers[uchUserId].TimerHandle = TimerHandle;
    }
}

void GhMultiSensorTimerUserUnregister(GU8 uchUserId)
{
    if(uchUserId < GH_MULTI_SENSOR_TIMER_USER_NUM_MAX)
    {
        g_stGhMultiSensorTimer.stTimerUsers[uchUserId].TimerHandle = 0;
    }
}

void GhMultiSensorTimerHandle(void)
{
    for(GU8 uchUserCnt = 0; uchUserCnt < GH_MULTI_SENSOR_TIMER_USER_NUM_MAX; uchUserCnt ++)
    {
        if(g_stGhMultiSensorTimer.stTimerUsers[uchUserCnt].TimerHandle)
        {
            g_stGhMultiSensorTimer.stTimerUsers[uchUserCnt].TimerHandle(__GH_MULTI_SENSOR_TIMER_PERIOD_MS__);
        }
    }
}

typedef struct 
{
    GU64 unnTimeStamp;    //64bit unix timestamp
    GU8  uchNext;        // 0~254 point to newer event  255：invalid  
}STMultiSensorWearEvent;

typedef struct 
{
    GU8 En;       // 0: Manager is disabled    1:  Manger is enabled
    GU8 uchHead;   // linked list head
    volatile GU64 unnCurrentTimeStamp;
    STMultiSensorWearEvent  pstMultiSensorWearEvent[GH3X2X_MULTI_SENSOR_EVENT_NUM_MAX];   //linked list members
    void (*WearEventHook) (GU32 uchNewEvent);
}STMultiSensorWearEventManager;  //linked list

STMultiSensorWearEventManager g_stMultiSensorWearEventManager;

/// @brief get current timestamp
/// @param  
/// @return 
static GU64 GhMultSensorWearEventManagerGetCurTs(void)
{
    return g_stMultiSensorWearEventManager.unnCurrentTimeStamp;  //it maybe is not atomic opration, this function must add lock !!!
}

/// @brief update timestamp for wear event manager, this function will be call at timer handle
/// @param unTimeIntervalMs the interval of timer (unit: ms)
static void GhMultSensorWearEventManagerUpdataCurTs(GU32 unTimeIntervalMs)
{
    g_stMultiSensorWearEventManager.unnCurrentTimeStamp += unTimeIntervalMs;  //it maybe is not atomic opration, this function must add lock !!!
}

static void GhMultSensorWearEventManagerTimerHandle(GU32 unTimeIntervalMs)
{
    GH3X2X_INFO_LOG("GhMultSensorWearEventManagerTimerHandle.\r\n");
    GhMultSensorWearEventManagerUpdataCurTs(unTimeIntervalMs);
}

/// @brief Init multi-sensor wear event manager
/// @param  
void GhMultSensorWearEventManagerInit(void)
{
    GH3X2X_Memset((GU8*)(&g_stMultiSensorWearEventManager), 0, sizeof(STMultiSensorWearEventManager));
    g_stMultiSensorWearEventManager.uchHead = GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID;  // point to NULL
    //clear linked list members
    for(GU8 uchEventCnt = 0; uchEventCnt < GH3X2X_MULTI_SENSOR_EVENT_NUM_MAX; uchEventCnt ++)
    {
        g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[uchEventCnt].uchNext = GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID;
    }

}

/// @brief register hook function for wear event hook
/// @param WearEventHook hook funcion
void GhMultSensorWearEventManagerHookRegister(void (*WearEventHook) (GU32 uchNewEvent))
{
    g_stMultiSensorWearEventManager.WearEventHook = WearEventHook;
}

/// @brief enable wear event manager
/// @param  
void GhMultSensorWearEventManagerEnable(void)
{
    g_stMultiSensorWearEventManager.En = 1; 
    GhMultiSensorTimerUserRegister(GH_MULTI_SENSOR_TIMER_USER_TIME_STAMP, GhMultSensorWearEventManagerTimerHandle); //register timer user
}

/// @brief disable wear event manager
/// @param  
void GhMultSensorWearEventManagerDisable(void)
{
    GhMultSensorWearEventManagerInit();
    GhMultiSensorTimerUserUnregister(GH_MULTI_SENSOR_TIMER_USER_TIME_STAMP);
}

GU8 GhMultSensorWearEventManagerIsEnable(void)
{
    return g_stMultiSensorWearEventManager.En;
}

/// @brief get the pointer of next member
/// @param[in] uchCurEventPointer this point points current event member's uchNext
/// @return the pointer of next member, this point points next event member's uchNext
static GU8* GetNextEventPointer(GU8* uchCurEventPointer)
{
    if(0 != uchCurEventPointer)
    {
        return &(g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[*uchCurEventPointer].uchNext);
    }
    else
    {
        return 0;
    }
}

/// @brief get the pointer of previous member of event uchEvtOffset
/// @param uchEvtOffset offset of event
/// @return the pointer of previous member of event uchEvtOffset, this point points uchHead or previous member's uchNext. 0: can not find this member
static GU8* GetPreviousEventPointer(GU8 uchEvtOffset)
{
    GU8* pTemp = 0;

    GU8* pNext = &(g_stMultiSensorWearEventManager.uchHead);

    do{
        if(GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID == *pNext)
        {
            break;
        }
        if(uchEvtOffset == (*pNext))
        {
            pTemp = pNext;
            break;
        }
        pNext = GetNextEventPointer(pNext);
    }while(1);

    return pTemp;
}

/// @brief get the pointer of tail in linker list 
/// @param  
/// @return the pointer of tail in linker list, the poiniter points to g_stMultiSensorWearEventManager.uchHead or pstMultiSensorWearEvent[N].uchNext
static GU8* GetTailEventPointer(void)
{
    GU8* pTemp = &(g_stMultiSensorWearEventManager.uchHead);

    do{
        if(GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID == *pTemp)
        {
            break;
        }
        pTemp = GetNextEventPointer(pTemp);
    }while(1);

    return pTemp;
}

/// @brief get oldest and newest event(in range of unEventComb) information
/// @param unEventComb Event combination you need check
/// @param[out] pOldestTs pointer of oldest event timestamp
/// @param[out] pNewestTs pointer of newest event timestamp
/// @param[out] pOldestEvt pointer of oldest event,  *pOldestEvt = 0 means this event cannot find 
/// @param[out] pONewestEvt pointer of newest event, *pONewestEvt = 0 means this event cannot find 
/// @return 
static void GetOldestAndNewestEvtInfo(GU32 unEventComb, GU64* pOldestTs, GU64* pNewestTs, GU32* pOldestEvt, GU32* pONewestEvt)
{
    GU32 unCurEvt;
    GU32 unRemainEventComb = unEventComb;
    (*pOldestEvt) = 0;
    (*pONewestEvt) = 0;

    GU8* pTemp = &(g_stMultiSensorWearEventManager.uchHead);

    do{
        if(GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID == *pTemp)
        {
            break;
        }
        unCurEvt = ((GU32)1 << (*pTemp));
        if(unEventComb & unCurEvt)
        {
            if(unEventComb == unRemainEventComb)  //first event we finded
            {
                (*pOldestTs) = (g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[*pTemp].unnTimeStamp);
                (*pOldestEvt) = unCurEvt;
            }
            (*pNewestTs) = (g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[*pTemp].unnTimeStamp);
            (*pONewestEvt) = unCurEvt;
            unRemainEventComb &= (~unCurEvt);  //clear event we finded
            if(0 == unRemainEventComb)   //last event we finded
            {
                break;
            }
        }
        pTemp = GetNextEventPointer(pTemp);
    }while(1);
}

/// @brief print all events in linked list
/// @param  
static void GhMultiSensorPrintAllEvt(void)
{
    GU8* pTemp = &(g_stMultiSensorWearEventManager.uchHead);
    GU8 uchEvtNum = 0;
    GH3X2X_INFO_LOG("[MultiSensor] print events start...\r\n");
    do{
        if(GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID == *pTemp)
        {
            break;
        }
        GH3X2X_INFO_LOG("[MultiSensor] EvtPosi = %d, Evt = 0x%x, Ts = %llu.\r\n", (int)uchEvtNum, (((GU32)1) << (*pTemp)), g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[*pTemp].unnTimeStamp);
        uchEvtNum ++;
        pTemp = GetNextEventPointer(pTemp);
    }while(1);
    GH3X2X_INFO_LOG("[MultiSensor] print events end, uchEvtNum = %d.\r\n", (int)uchEvtNum);
}

/// @brief check event combination is in this time window or not
/// @param unEventComb event combination
/// @param unWindow   time window (unit: ms)
/// @param uchNowTimeNeedInWin  current timestamp is include or not  (0: is include  1: is NOT include)
/// @return 0: no  1: yes
GU8 GhMultiSensorConditionCheckInWindow (GU32 unEventComb, GU32 unWindow, GU8 uchNowTimeNeedInWin)
{
    GU64 nnOldestTs;
    GU64 nnNewestTs;
    GU32 nnOldestEvt;
    GU32 nnNewestEvt;

    GetOldestAndNewestEvtInfo(unEventComb, &nnOldestTs, &nnNewestTs, &nnOldestEvt, &nnNewestEvt);
    if((0 == nnOldestEvt)||(0 == nnNewestEvt))   //can NOT find any event in unEventComb
    {
        return 0;
    }
    if(1 == uchNowTimeNeedInWin)
    {
        nnNewestTs = GhMultSensorWearEventManagerGetCurTs();
    }
    if((nnNewestTs - nnOldestTs) < unWindow)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/// @brief check unEventNew is new than unEventOld or not
/// @param unEventNew new event we hope for
/// @param unEventOld old event we hope for
/// @return 0: no or there is no unEventNew   1: yes or there is only unEventNew
GU8 GhMultiSensorConditionCheckIsNew (GU32 unEventNew, GU32 unEventOld)
{
    GU64 nntTs;
    GU32 unOldestEvt;
    GU32 unNewestEvt;
    GetOldestAndNewestEvtInfo(unEventNew|unEventOld, &nntTs, &nntTs, &unOldestEvt, &unNewestEvt);
    if(unNewestEvt == unEventNew)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/// @brief send evet to Multi-sensor wear event manager
/// @param unEvent such as: GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON
void GhMultSensorWearEventSend(GU32 unEvent) 
{
    GU64 unnCurTs;
    GU8*  puchPreMemberPointer; 
    GU8*  puchTailPointer; 
    GU8  uchNewEvtOffset = GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID;

    //get current time stamp
    unnCurTs = GhMultSensorWearEventManagerGetCurTs();
    //get offset of this new event
    for(GU8 uchEvtCnt = 0; uchEvtCnt < GH3X2X_MULTI_SENSOR_EVENT_NUM_MAX; uchEvtCnt ++)
    {
        if(((GU32)1 << uchEvtCnt) == unEvent)
        {
            uchNewEvtOffset = uchEvtCnt;
            break; 
        }
    }
    if((GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID == uchNewEvtOffset) || (0 == g_stMultiSensorWearEventManager.En))  //invalid event or event manager is disable
    {
        return;
    }
    //remove outdated event member in linked list
    puchPreMemberPointer = GetPreviousEventPointer(uchNewEvtOffset);
    if(puchPreMemberPointer)
    {
        (*puchPreMemberPointer) = g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[uchNewEvtOffset].uchNext;
    }
    //add new event to link tail
    puchTailPointer = GetTailEventPointer();
    (*puchTailPointer) = uchNewEvtOffset;
    //fill timestamp and next for this new event
    g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[uchNewEvtOffset].unnTimeStamp = unnCurTs;
    g_stMultiSensorWearEventManager.pstMultiSensorWearEvent[uchNewEvtOffset].uchNext = GH3X2X_MULTI_SENSOR_EVENT_LL_INDEX_INVALID;
    //print new event
    GH3X2X_INFO_LOG("[MultiSensor] new event received, Evt = 0x%x.\r\n", unEvent);
    //print all event
    GhMultiSensorPrintAllEvt();
    //call event hook
    if(g_stMultiSensorWearEventManager.WearEventHook)g_stMultiSensorWearEventManager.WearEventHook(unEvent);
}
#endif

typedef struct
{
    GS16 sXAxisVal;     /**< x-axis value */
    GS16 sYAxisVal;     /**< y-axis value */
    GS16 sZAxisVal;     /**< z-axis value */
} STGsAccRawdata;

#if __GSENSOE_MOVE_DETECT_SUPPORT__

typedef struct 
{
    GU8 uchMoveDetEnable;  //enable gs movement detect or not
    GU8 uchGsensorStatus;  // movement status of g sensor
    GU16 usGsMoveDetectCnt;  // count how many times of movement detected by g sensor 
    GU16 usGsNotMoveDetectCnt;  // count how many times of not movement detected by g sensor 
    GU8 uchGsMoveDetectNotFirstTime;  // 0: the first time of G sensor move detection  1: is not first time of G sensor move detection
    STGsAccRawdata stLastPointMoveDetGsenorRawdata;
    GU16 usNotMoveTimeCnt;   //the counter of not move, 0: time out has trigled  or initial state, 1: is moving state, >1: the conter of not move, unit: ms 
}STGsMoveDetecter;  

STGsMoveDetecter g_stGhGsMoveDetecter;

extern void GH3X2X_RedetectWearOn(void);


void GhGsMoveDetecterInit(void)
{
    GH3X2X_Memset((GU8*)(&g_stGhGsMoveDetecter), 0, sizeof(STGsMoveDetecter));

}

void GhGsMoveDetecterEnable(void)
{
    g_stGhGsMoveDetecter.uchMoveDetEnable = 1;
    #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
    GhMultiSensorTimerUserRegister(GH_MULTI_SENSOR_TIMER_USER_MOVE_DET, Gh3x2xDemoMoveDetectTimerHandler); //register timer user
    #endif
}

void GhGsMoveDetecterDisable(void)
{
    GhGsMoveDetecterInit();
    #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
    GhMultiSensorTimerUserUnregister(GH_MULTI_SENSOR_TIMER_USER_MOVE_DET);
    #endif
}

GU8 GhGsMoveDetecterIsEnable(void)
{
    return g_stGhGsMoveDetecter.uchMoveDetEnable;
}

/// function started bitmap, use for sampling control
#if 1
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__) && __USE_GOODIX_SOFT_ADT_ALGORITHM__
extern GU8 NADTWearDetectByCap(GS32 lPacketInfoArr[]);
void GH3X2X_MoveDetectByCapData(STCapRawdata* pusCapData, GU16 usCapDataCnt)
{
    if( 0 == GH3X2X_GetCapEnableFlag())
    {
        return;
    }
    GS32 lPacketInfoArr[4];
    for(GU16 i = 0; i < usCapDataCnt;i++)
    {
        lPacketInfoArr[0] = pusCapData[i].unCapData[0];
        lPacketInfoArr[1] = pusCapData[i].unCapData[1];
        lPacketInfoArr[2] = pusCapData[i].unCapData[2];
        lPacketInfoArr[3] = pusCapData[i].unCapData[3];

        if(1 == NADTWearDetectByCap(lPacketInfoArr))
        {
            return;
        }
    }
}
#else
void GH3X2X_MoveDetectByCapData(STCapRawdata* pusCapData, GU16 usCapDataCnt)
{
    UNUSED_VAR(pusCapData);
    UNUSED_VAR(usCapDataCnt);
    return;
}
#endif
#endif

/**
 * @fn      void GhMoveDetectByGsData(STGsensorRawdata* stGsData, GU16 usGsDataCnt, GU8 uchCheckWindowSize)
 
 *
 * @brief  Move detection by software.
 *
 * @attention   None
 *
 * @param[in]   stGsData        gsensor data buffer
 * @param[in]   usGsDataCnt        gsensor data count
 * @param[in]   uchCheckWindowSize        the window size of cheak moving peak num 
 * @param[out]  None
 *
 * @return  None
 */
//extern void SlaverLog(const char * lpsbLog, ...);
void GhMoveDetectByGsData(GS16* pusGsData, GU16 usGsDataCnt)
{
    GS16 sXAxisVal = 0;
    GS16 sYAxisVal = 0;
    GS16 sZAxisVal = 0;
    GU8  uchGyroEnable = g_uchGyroEnable;
    GU32 unSumOfSquare = 0;    
    GU16  usGsDataIndex = 0;
    if ((GH3X2X_PTR_NULL == pusGsData) || (0 == usGsDataCnt))
    {
        GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] return; pusGsData[0] = %d , usGsDataCnt = %d \r\n",pusGsData[0],usGsDataCnt);
        return;
    }

        {
            if (g_stGhGsMoveDetecter.uchMoveDetEnable)
            {
                    if(0 == g_stGhGsMoveDetecter.uchGsMoveDetectNotFirstTime)
                    {
                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sXAxisVal = pusGsData[0];
                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sYAxisVal = pusGsData[0+1];
                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sZAxisVal = pusGsData[0+2];
                            g_stGhGsMoveDetecter.uchGsMoveDetectNotFirstTime = 1;
                    }
                    //calculate average of x, y, z
                    for (usGsDataIndex = 0; usGsDataIndex < usGsDataCnt; usGsDataIndex++)
                    {
                            //GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] x=%d, y=%d, z =%d\r\n", pusGsData[usGsDataIndex*(3+3*uchGyroEnable)], pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 1], pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 2]);
                            sXAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable)] - g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sXAxisVal;
                            sYAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 1] - g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sYAxisVal;
                            sZAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 2] - g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sZAxisVal;

                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sXAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable)];
                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sYAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 1];
                            g_stGhGsMoveDetecter.stLastPointMoveDetGsenorRawdata.sZAxisVal = pusGsData[usGsDataIndex*(3+3*uchGyroEnable) + 2];

                            unSumOfSquare = (GU32)((GS32)sXAxisVal * sXAxisVal) + (GU32)((GS32)sYAxisVal * sYAxisVal) + (GU32)((GS32)sZAxisVal * sZAxisVal);

                            if(unSumOfSquare > ((GU32)__GSENSOR_MOVE_THRESHOLD__ * __GSENSOR_MOVE_THRESHOLD__))
                            {
                                    GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] g_stGhGsMoveDetecter.usGsMoveDetectCnt = %d \r\n", g_stGhGsMoveDetecter.usGsMoveDetectCnt);
                                    g_stGhGsMoveDetecter.usGsNotMoveDetectCnt = 0;
                                    if (g_stGhGsMoveDetecter.usGsMoveDetectCnt >= __GSENSOR_MOVE_CNT_THRESHOLD__)
                                    {
                                            GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] over __GSENSOR_MOVE_CNT_THRESHOLD__!!! \r\n");
                                            g_stGhGsMoveDetecter.uchGsensorStatus = GH3X2X_SENSOR_IS_MOVING;
                                            GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] Move detect effect!!! \r\n");
                                    }
                                    else
                                    {
                                        g_stGhGsMoveDetecter.usGsMoveDetectCnt ++;
                                    }
                            }
                            else
                            {
                                    if (g_stGhGsMoveDetecter.usGsMoveDetectCnt != 0)
                                    {
                                            GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] g_stGhGsMoveDetecter.usGsNotMoveDetectCnt = %d \r\n", g_stGhGsMoveDetecter.usGsNotMoveDetectCnt);
                                            
                                            if (g_stGhGsMoveDetecter.usGsNotMoveDetectCnt >= __GSENSOR_NOT_MOVE_CNT_THRESHOLD__)
                                            {
                                                g_stGhGsMoveDetecter.usGsMoveDetectCnt = 0;
                                                g_stGhGsMoveDetecter.uchGsensorStatus = GH3X2X_SENSOR_IS_NOT_MOVING;
                                                GH3X2X_DEBUG_LOG("[GhMoveDetectByGsData] Move detect is not effect!!! \r\n");
                                            }
                                            else
                                            {
                                                g_stGhGsMoveDetecter.usGsNotMoveDetectCnt ++;
                                            }
                                    }
                            }
                    }

                    if(GH3X2X_SENSOR_IS_MOVING == g_stGhGsMoveDetecter.uchGsensorStatus)
                    {
                        #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
                        GhMultSensorWearEventSend(GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE);
                        #endif
                        g_stGhGsMoveDetecter.usNotMoveTimeCnt = 1;
                    }
                    if((GH3X2X_SENSOR_IS_NOT_MOVING == g_stGhGsMoveDetecter.uchGsensorStatus) && (g_stGhGsMoveDetecter.usNotMoveTimeCnt >= 1))
                    {
                        GU32 unTempCnt;
                        unTempCnt = ((GU32)g_stGhGsMoveDetecter.usNotMoveTimeCnt + __GH_MULTI_SENSOR_TIMER_PERIOD_MS__);
                        if(unTempCnt > 0xFFFF)
                        {
                            unTempCnt = 0xFFFF;
                        }
                        if(unTempCnt > __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__)
                        {
                            #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
                            GhMultSensorWearEventSend(GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT);
                            #endif
                            unTempCnt = 0;
                        }
                        g_stGhGsMoveDetecter.usNotMoveTimeCnt = (GU16)unTempCnt;
                    }
            }
        }
}

/**
 * @fn     GU8 Gh3x2x_GetMoveDetStatus(void)
 *
 * @brief  Get status of movement detected
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  GH3X2X_SENSOR_IS_NOT_MOVING/GH3X2X_SENSOR_IS_MOVING
 */
GU8 Gh3x2x_GetMoveDetStatus(void)
{
    return g_stGhGsMoveDetecter.uchGsensorStatus;
}

#endif


extern  void hal_gh3x2x_write_cap_to_flash(GS32 WearCap1,GS32 UnwearCap1,GS32 WearCap2,GS32 UnwearCap2);
extern  void hal_gh3x2x_read_cap_from_flash(GS32* WearCap1,GS32* UnwearCap1,GS32* WearCap2,GS32* UnwearCap2);

void GH3x2xWriteCapToFlash(void)
{
//    hal_gh3x2x_write_cap_to_flash(lWearCap1,lUnwearCap1,lWearCap2,lUnwearCap2);
}

void GH3x2xReadCapFromFlash(void)
{
//    hal_gh3x2x_read_cap_from_flash(&lWearCap1,&lUnwearCap1,&lWearCap2,&lUnwearCap2);
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

