#include <stdio.h>
#include "gh3x2x_demo_algo_memory.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_call_version.h"
#include "goodix_algo.h"
#include "goodix_mem.h"
#include "goodix_type.h"
#include "goodix_hba.h"
#include "goodix_ecg.h"
#include "goodix_af.h"
#include "goodix_hsm.h"
#include "goodix_resp.h"
#include "goodix_hrv.h"
#include "goodix_spo2.h"
#include "goodix_nadt.h"
#include "goodix_algo_bt.h"

#if (__GOODIX_ALGO_CALL_MODE__)
extern const STGh3x2xInitConfig g_stGoodixAlgoCfgListArr[__GOODIX_ALGO_CFG_LIST_MAX_NUM__];
STGh3x2xFrameInfo ** g_pstGh3x2xAlgoFrameInfo = GH3X2X_PTR_NULL;
static GU32 g_unAlgoFuncStartedBitmap = 0;
static GU8 g_uchAlgInitFlag[GH3X2X_FUNC_OFFSET_MAX] = {0};
static const pfnAlgorithmCallFunc g_pfnAlgorithmCallFunc[GH3X2X_FUNC_OFFSET_MAX][GH3X2X_ALGO_FUNCTION_NUM_MAX] = {
//adt
#if __USE_GOODIX_ADT_ALGORITHM__
    { GH3x2xAdtAlgoInit, GH3x2xAdtAlgoExe, GH3x2xAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hr
#if __USE_GOODIX_HR_ALGORITHM__
    { GH3x2xHrAlgoInit, GH3x2xHrAlgoExe, GH3x2xHrAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hrv
#if __USE_GOODIX_HRV_ALGORITHM__
    { GH3x2xHrvAlgoInit, GH3x2xHrvAlgoExe, GH3x2xHrvAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//hsm
#if __USE_GOODIX_HSM_ALGORITHM__
    { GH3x2xHsmAlgoInit, GH3x2xHsmAlgoExe, GH3x2xHsmAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//fpbp
//pwa
#if __USE_GOODIX_BP_ALGORITHM__
    { GH3x2xBpAlgoInit, GH3x2xBpAlgoExe, GH3x2xBpAlgoDeinit },
    { GH3x2xBpAlgoInit, GH3x2xBpAlgoExe, GH3x2xBpAlgoDeinit },
#else
    { 0, 0, 0 },
    { 0, 0, 0 },
#endif
//spo2
#if __USE_GOODIX_SPO2_ALGORITHM__
    { GH3x2xSpo2AlgoInit, GH3x2xSpo2AlgoExe, GH3x2xSpo2AlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//ecg
#if __USE_GOODIX_ECG_ALGORITHM__
    { GH3x2xEcgAlgoInit, GH3x2xEcgAlgoExe, GH3x2xEcgAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//pwtt
    { 0, 0, 0 },
//soft adt green
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    { GH3x2xSoftAdtAlgoInit, GH3x2xSoftAdtAlgoExe, GH3x2xSoftAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//bt
#if __USE_GOODIX_BT_ALGORITHM__
    { GH3x2xBtAlgoInit, GH3x2xBtAlgoExe, GH3x2xBtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//resp
#if __USE_GOODIX_RESP_ALGORITHM__
    { GH3x2xRespAlgoInit, GH3x2xRespAlgoExe, GH3x2xRespAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//af
#if __USE_GOODIX_AF_ALGORITHM__ && __USE_GOODIX_HRV_ALGORITHM__
    { GH3x2xAfAlgoInit, GH3x2xAfAlgoExe, GH3x2xAfAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
//test
    { 0, 0, 0 },
    { 0, 0, 0 },
//soft adt ir
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    { GH3x2xSoftAdtAlgoInit, GH3x2xSoftAdtAlgoExe, GH3x2xSoftAdtAlgoDeinit },
#else
    { 0, 0, 0 },
#endif
};

GCHAR* GH3X2X_GetAlgoCallDemoVersion(void)
{
    return GOODIX_ALGO_CALL_VERSION_STRING;
}

void GH3X2X_RegisterFuncInfoStruct(const STGh3x2xFrameInfo * const  pstGh3x2xFrameInfo[])
{
    g_pstGh3x2xAlgoFrameInfo = (STGh3x2xFrameInfo **)pstGh3x2xFrameInfo;
}

void GH3X2X_AlgoCallConfigInit(const STGh3x2xFrameInfo * const  pstGh3x2xFrameInfo[], GU8 uchAlgoCfgIndex)
{
    if (pstGh3x2xFrameInfo != GH3X2X_PTR_NULL)
    {
        g_pstGh3x2xAlgoFrameInfo = (STGh3x2xFrameInfo **)pstGh3x2xFrameInfo;
    }
    GH3X2X_AlgoMemConfig(GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL);
    GH3X2X_DEBUG_ALGO_LOG("Algorithm mem size = %d\r\n",(int)GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL);
    if(uchAlgoCfgIndex < __GOODIX_ALGO_CFG_LIST_MAX_NUM__)
    {
        GH3X2X_LoadGoodixAlgoRegConfigArr(&g_stGoodixAlgoCfgListArr[uchAlgoCfgIndex]);
    }

}

void GH3X2X_AlgoVersion(GU8 uchFunctionID, GCHAR version[150])
{
    if (version == 0)
    {
        GH3X2X_DEBUG_ALGO_LOG("[%s]:version buffer is NULL!!!\r\n", __FUNCTION__);
        return;
    }
    GCHAR chNoVersion[10] = "no_ver";

    switch (uchFunctionID)
    {
#if __USE_GOODIX_ADT_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_ADT:
        GH3X2X_Memcpy(version, GH3x2xAdtVersion(), strlen((const GCHAR *)GH3x2xAdtVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_HR_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_HR:
        goodix_hba_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_HRV:
        goodix_hrv_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_SPO2:
        goodix_spo2_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_ECG_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_ECG:
        goodix_ecg_version((uint8_t*)version);
        break;
#endif
#if __USE_GOODIX_HSM_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_HSM:
        GH3X2X_Memcpy(version, HSMAlgoVersion(), strlen((const GCHAR *)HSMAlgoVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_AF_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_AF:
        GH3X2X_Memcpy(version, AFVersion(), strlen((const GCHAR *)AFVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_RESP_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_RESP:
        GH3X2X_Memcpy(version, RespAlgoVersion(), strlen((const GCHAR *)RespAlgoVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_SOFT_ADT_GREEN:
    case GH3X2X_FUNC_OFFSET_SOFT_ADT_IR:
        goodix_nadt_version((uint8_t *)version);
        break;
#endif
#if __USE_GOODIX_BP_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_PWA:
    case GH3X2X_FUNC_OFFSET_FPBP:
        //GH3X2X_Memcpy(version, GoodixBpAlgoVersion(), strlen((const GCHAR *)GoodixBpAlgoVersion()) + 1);
        break;
#endif
#if __USE_GOODIX_BT_ALGORITHM__
    case GH3X2X_FUNC_OFFSET_BT:
#if!__USE_GOODIX_DRV_NTC_Convert
        GH3X2X_Memcpy(version, GoodixBtAlgoVersion(), strlen((const GCHAR *)GoodixBtAlgoVersion()) + 1);
#endif
        break;
#endif
    default:
        GH3X2X_Memcpy(version, (GCHAR *)chNoVersion,\
        strlen((const GCHAR *)chNoVersion) + 1);
        break;
    }
    //GH3X2X_INFO_ALGO_LOG("version = %s\n", version);
}

GS8 GH3X2X_AlgoInit(GU32 unFunctionID)
{
    GU8 chRet = GH3X2X_RET_OK;

    if(GH3X2X_PTR_NULL == g_pstGh3x2xAlgoFrameInfo)
    {
        GH3X2X_INFO_ALGO_LOG("[%s]: Warning!!! g_pstGh3x2xAlgoFrameInfo is not init!\r\n", __FUNCTION__);
        return GH3X2X_RET_OK;
    }
    
    /* set started bit */
    g_unAlgoFuncStartedBitmap |= unFunctionID;

    /* init mem pool */
    if (GH3X2X_GetAlgoMemStatus() != ALGO_MEM_INIT_OK)
    {
        return -1;
    }

    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X2X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* int algo */
            const STGh3x2xFrameInfo * const pstFrameInfo = g_pstGh3x2xAlgoFrameInfo[uchFunCnt];
            if (1 == g_uchAlgInitFlag[uchFunCnt])
            {
                GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) already init!\r\n", __FUNCTION__, (int)uchFunCnt);
            }
            else
            {
                if (g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_INIT_FUNCTION] != GH3X2X_PTR_NULL)
                {
                    chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_INIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH3X2X_RET_OK)
                    {
                        GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) init fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                    }
                    else
                    {
                        GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) init success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        g_uchAlgInitFlag[uchFunCnt] = 1;
                    }
                }
                else
                {
                    GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) init func null!\r\n", __FUNCTION__, (int)uchFunCnt);
                }
                Gh3x2xOutputValueStrategyInit(unFunctionID);
            }
            GH3X2X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH3X2X_AlgoDeinit(GU32 unFunctionID)
{
    GS32 chRet = GH3X2X_RET_OK;
    
    if(GH3X2X_PTR_NULL == g_pstGh3x2xAlgoFrameInfo)
    {
        GH3X2X_INFO_ALGO_LOG("[%s]: Warning!!! g_pstGh3x2xAlgoFrameInfo is not init!\r\n", __FUNCTION__);
        return GH3X2X_RET_OK;
    }
    
    /* clear started bit */
    g_unAlgoFuncStartedBitmap &= (~unFunctionID);

    /* deint algo */
    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X2X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* deint algo */
            const STGh3x2xFrameInfo * const pstFrameInfo = g_pstGh3x2xAlgoFrameInfo[uchFunCnt];
            if (0 == g_uchAlgInitFlag[uchFunCnt])
            {
                GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) didn't init!\r\n", __FUNCTION__, (int)uchFunCnt);
            }
            else
            {
                if (g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_DEINIT_FUNCTION] != GH3X2X_PTR_NULL)
                {
                    chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_DEINIT_FUNCTION](pstFrameInfo);
                    if (chRet != GH3X2X_RET_OK)
                    {
                        GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) deinit fail! errcode = %d\r\n", __FUNCTION__, (int)uchFunCnt, (int)chRet);
                    }
                    else
                    {
                        GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) deinit success!\r\n", __FUNCTION__, (int)uchFunCnt);
                        g_uchAlgInitFlag[uchFunCnt] = 0;
                    }
                }
                else
                {
                    GH3X2X_DEBUG_ALGO_LOG("[%s]:Function(function id = %d) deinit func null!\r\n", __FUNCTION__, (int)uchFunCnt);
                }
            }
            GH3X2X_RET_ERROR_CHECK(chRet);
        }
    }
    return chRet;
}

GS8 GH3X2X_AlgoCalculate(GU32 unFunctionID)
{
    if (g_unAlgoFuncStartedBitmap == GH3X2X_NO_FUNCTION)
    {
        return GH3X2X_RET_OK;
    }

    GS32 chRet = GH3X2X_RET_GENERIC_ERROR;

    for(GU8 uchFunCnt = 0; uchFunCnt < GH3X2X_FUNC_OFFSET_MAX; uchFunCnt ++)
    {
        if (((1<<uchFunCnt) & unFunctionID) == (1<<uchFunCnt))
        {
            /* calc algo */
            if (GH3X2X_CHECK_BIT_SET(g_unAlgoFuncStartedBitmap, unFunctionID))
            {
                const STGh3x2xFrameInfo * const pstFrameInfo = g_pstGh3x2xAlgoFrameInfo[uchFunCnt];
                if(GH3x2xSleepFlagGet())
                {
                    pstFrameInfo->punFrameFlag[2] |= ((GU32)1)<<3;
                }
                if (0 == g_uchAlgInitFlag[uchFunCnt])
                {
                    //GH3X2X_DEBUG_ALGO_LOG("Function(funcition id = %d) didn't init!\r\n",(int)uchFunCnt);
                }
                else
                {
                    if (g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_CALCULATE_FUNCTION] != GH3X2X_PTR_NULL)
                    {
                        chRet = g_pfnAlgorithmCallFunc[uchFunCnt][GH3X2X_ALGO_CALCULATE_FUNCTION](pstFrameInfo);
                        if( *(pstFrameInfo->punFrameCnt) == 0 && pstFrameInfo->pstAlgoResult->uchUpdateFlag == 0 )
                        {
                            GH3X2X_Memset(pstFrameInfo->pstAlgoResult,0,sizeof(STGh3x2xAlgoResult));
                            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
                            pstFrameInfo->pstAlgoResult->usResultBit = 0xFF;
                            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
                        }
                    }
                }
            }
        }
        else
        {
            chRet = GH3X2X_RET_OK;
        }
    }
    return chRet;
}

#endif

