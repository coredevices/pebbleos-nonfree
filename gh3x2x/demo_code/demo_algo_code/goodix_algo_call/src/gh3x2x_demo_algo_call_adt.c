#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"

#if (__USE_GOODIX_ADT_ALGORITHM__)

#define __HARD_ADT_WAER_ON_THRD__               9000000
#define __HARD_ADT_WAER_OFF_THRD__              8800000
#define __HARD_ADT_WAER_ON_DIFF_THRD__          -8000000
#define __HARD_ADT_WAER_BIG_DIFF__              30000
#define GH3X2X_MOVINF_AVA_WINDOW_SIZE           6

static EMWearDetectType g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
static GU32 g_punGh3x2xMovingAvaFilterBuf[GH3X2X_MOVINF_AVA_WINDOW_SIZE];
static GU8  g_uchGh3x2xMovingAvaFilterPointCnt;



void Gh3x2xMovingAvaFilterInit(void)
{
    g_uchGh3x2xMovingAvaFilterPointCnt = 0;
}

GU8 Gh3x2xMovingAvaFilter(GU32 *Rawdata, GU8 uchBypass)
{
    GU32 unSum = 0;

    if(uchBypass)
    {
        return 1;
    }
    else
    {
        if(g_uchGh3x2xMovingAvaFilterPointCnt >= GH3X2X_MOVINF_AVA_WINDOW_SIZE)
        {
            for(GU8 uchCnt = 1; uchCnt < GH3X2X_MOVINF_AVA_WINDOW_SIZE; uchCnt ++)
            {
                g_punGh3x2xMovingAvaFilterBuf[uchCnt - 1] = g_punGh3x2xMovingAvaFilterBuf[uchCnt];
            }
            g_uchGh3x2xMovingAvaFilterPointCnt = GH3X2X_MOVINF_AVA_WINDOW_SIZE - 1;
        }
        g_punGh3x2xMovingAvaFilterBuf[g_uchGh3x2xMovingAvaFilterPointCnt] = Rawdata[0];
        g_uchGh3x2xMovingAvaFilterPointCnt ++;
            
        if(g_uchGh3x2xMovingAvaFilterPointCnt > 1)
        {
            for(GU8 uchCnt = 0; uchCnt < g_uchGh3x2xMovingAvaFilterPointCnt - 1; uchCnt ++)
            {
                unSum += g_punGh3x2xMovingAvaFilterBuf[uchCnt];
            }
            Rawdata[0] = unSum/(g_uchGh3x2xMovingAvaFilterPointCnt - 1);
        }
        return 1;
    }
}

#define ADT_CHECK_ALGO_VERSION "GX_HARDADT_SOFTCHECK_v0.1"

GU8* GH3x2xAdtVersion (void)
{
    return (GU8*)ADT_CHECK_ALGO_VERSION;
}

GS8  GH3x2xAdtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    Gh3x2xMovingAvaFilterInit();
    g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
    return GH3X2X_RET_OK;
}

GS8 GH3x2xAdtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    Gh3x2xMovingAvaFilterInit();
    g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
    return GH3X2X_RET_OK;
}

GS8 GH3x2xAdtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU32 unCurrentFrameRawdata = 0;
    GS32 snDiffRawdata = 0;
    GU32 unRawDataBeforeFilter = 0;

    unCurrentFrameRawdata = pstFrameInfo->punFrameRawdata[0];
    unRawDataBeforeFilter = pstFrameInfo->punFrameRawdata[0];
    GH3X2X_DEBUG_ALGO_LOG("[GH3x2xAdtAlgoExe]:punFrameRawdata = %d\r\n", unCurrentFrameRawdata);
    Gh3x2xMovingAvaFilter(&unCurrentFrameRawdata, 0);
    GH3X2X_DEBUG_ALGO_LOG("[GH3x2xAdtAlgoExe]:punFrameRawdata after filter = %d\r\n", unCurrentFrameRawdata);
    GH3X2X_DEBUG_ALGO_LOG("[GH3x2xAdtAlgoExe]:g_emAlgoWearStatus = %d, start check...\r\n",g_emAlgoWearStatus);

    snDiffRawdata = (GS32)unRawDataBeforeFilter - (GS32)unCurrentFrameRawdata;
    pstFrameInfo->pstAlgoResult->snResult[0] = ADT_ALGO_RESULT_DEFULT;


    if(WEAR_DETECT_WEAR_ON == g_emAlgoWearStatus)
    {
        #if (__HARD_ADT_WAER_OFF_THRD__ > __HARD_ADT_WAER_ON_THRD__)   //penetration model
        if(unRawDataBeforeFilter < __HARD_ADT_WAER_ON_THRD__)
        #else  //reflex model
        if((unRawDataBeforeFilter > __HARD_ADT_WAER_ON_THRD__)&&((snDiffRawdata > (GS32)__HARD_ADT_WAER_ON_DIFF_THRD__)))
        #endif
        {
            g_emAlgoWearStatus = WEAR_DETECT_WEAR_OFF;
            GH3X2X_INFO_ALGO_LOG("[GH3x2xAdtAlgoExe]:wear on event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->snResult[0] = ADT_ALGO_RESULT_WAER_ON;
        }
        else
        {
            #if (__HARD_ADT_WAER_OFF_THRD__ > __HARD_ADT_WAER_ON_THRD__)  //penetration model
            #else  //reflex model
            if(unRawDataBeforeFilter > __HARD_ADT_WAER_ON_THRD__)
            {
                GH3X2X_INFO_ALGO_LOG("[GH3x2xAdtAlgoExe]:wear on condition is not match ! r = %d, r_diff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            }
            #endif
        }
        #if (__HARD_ADT_WAER_OFF_THRD__ > __HARD_ADT_WAER_ON_THRD__)  //penetration model
        #else  //reflex model
        if(snDiffRawdata > __HARD_ADT_WAER_BIG_DIFF__)
        {
            //pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            //pstFrameInfo->pstAlgoResult->snResult[0] = ADT_ALGO_RESULT_DIFF_BIG;
            GH3X2X_INFO_ALGO_LOG("[GH3x2xAdtAlgoExe]:ppg 5Hz diff big event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
        }
        #endif
    }
    else if(WEAR_DETECT_WEAR_OFF == g_emAlgoWearStatus)
    {
        #if (__HARD_ADT_WAER_OFF_THRD__ > __HARD_ADT_WAER_ON_THRD__)  //penetration model
        if(unRawDataBeforeFilter > __HARD_ADT_WAER_OFF_THRD__)
        #else  //reflex model
        if(unRawDataBeforeFilter < __HARD_ADT_WAER_OFF_THRD__)
        #endif
        {
            g_emAlgoWearStatus = WEAR_DETECT_WEAR_ON;
            GH3X2X_INFO_ALGO_LOG("[GH3x2xAdtAlgoExe]:wear off event, Rawdata = %d, unRawdataDiff = %d\r\n",unCurrentFrameRawdata,snDiffRawdata);
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->snResult[0] = ADT_ALGO_RESULT_WAER_OFF;
        }
    }

    pstFrameInfo->pstAlgoResult->usResultBit = 0x7;
    pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
    pstFrameInfo->pstAlgoResult->snResult[1] = snDiffRawdata;
    pstFrameInfo->pstAlgoResult->snResult[2] = pstFrameInfo->punFrameCnt[0];
    GH3X2X_AdtAlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);

    return GH3X2X_RET_OK;
}

#endif

