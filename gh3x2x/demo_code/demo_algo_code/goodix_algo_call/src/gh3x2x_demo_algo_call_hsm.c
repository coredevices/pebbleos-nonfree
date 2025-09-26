#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_hsm.h"

#if (__USE_GOODIX_HSM_ALGORITHM__)

GS8 GH3x2xHsmAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    int32_t nRet = GH3X2X_RET_GENERIC_ERROR;
#if (GH3X2X_ALGORITHM_HSM_SUPPORT)


    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    //g_uchLastGainOfChannel[0] = GH3X2X_GAIN_VALUE_INVALID;
    //int nHSMFs = (int)pstFrameInfo ->pstFunctionInfo ->usSampleRate;
    int32_t nHSMFs = 25;
    nRet = HSMAlgoInit(&nHSMFs);
    if (nRet != GH3X2X_RET_OK)
    {
        GH3X2X_DEBUG_ALGO_LOG("hsm process error! error code : %d\r\n", (int)nRet);
    }
    else
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x1;
    }
    g_unHsmLastHrResult = 0;
#else
    nRet = GH3X2X_RET_ALGO_NO_SUPPORTED;
    GH3X2X_DEBUG_ALGO_LOG("hsm algorithm no supported error!\r\n");
#endif
    return (GS8)nRet;
}

GS8 GH3x2xHsmAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
#if (GH3X2X_ALGORITHM_HSM_SUPPORT)
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;


    ST_HSM_DATA_OUT stHSMDataOut;
    ST_HSM_SCORE stHSMScore;
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    GU8 uchHsmResultSendBuffer[235] = {0};
    GU8 uchHsmPackLen = 140;
    if (HSMLastDataProc(&stHSMDataOut, &stHSMScore) == GH3X2X_RET_OK)
    {    
        uchHsmResultSendBuffer[4] = 2;
        if (stHSMDataOut.uchStageDataFlag != 0)
        {
            GH3X2X_Memcpy(uchHsmResultSendBuffer + 1 + 4, (GU8*)&stHSMScore, 18);
            uchHsmResultSendBuffer[23] = stHSMDataOut.uchStageOutNum;
            GH3X2X_Memcpy(uchHsmResultSendBuffer + 1 + 4 + 19, stHSMDataOut.puchHSMStageOut, 120);
        }
        GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_HSM_RESULT_UPDATE, uchHsmResultSendBuffer,
                            &uchHsmResultSendBuffer[4], uchHsmPackLen);
        Gh3x2xDemoSendProtocolData(uchHsmResultSendBuffer,usRespondLen);
        chRet = HSMAlgoDeInit();
    }
    else
    {
        chRet = HSMAlgoDeInit();
        GH3X2X_RET_ERROR_CHECK(chRet);
        chRet = GH3X2X_RET_GENERIC_ERROR;
    }
#else
    GS8 chRet = GH3X2X_RET_ALGO_NO_SUPPORTED;
    GH3X2X_DEBUG_ALGO_LOG("hsm algorithm no supported error!\r\n");
#endif
    return chRet;
}

GS8 GH3x2xHsmAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return 0;
    }
#if (GH3X2X_ALGORITHM_HSM_SUPPORT)
    ST_HSM_DATA_IN stHSMDataIn;
    ST_HSM_DATA_OUT stHSMDataOut;
    ST_HSM_SCORE stHSMScore;

    if(GH3X2X_RET_OK != GH3x2xCheckAndFreeMem(pstFrameInfo))
    {
        return;
    }

    stHSMDataIn.IPpgRaw = (int)GH3X2X_GET_RAWDATA_FOR_ALGO((int)pstFrameInfo->punFrameRawdata[0] - GH3X2X_RAWDATA_BASE_VALUE);
    stHSMDataIn.IACCx = pstFrameInfo->pusFrameGsensordata[0];
    stHSMDataIn.IACCy = pstFrameInfo->pusFrameGsensordata[1];
    stHSMDataIn.IACCz = pstFrameInfo->pusFrameGsensordata[2];    
    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->usResultBit = 0x1;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)g_unHsmLastHrResult;
        g_unHsmLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    stHSMDataIn.IHeart = g_unHsmLastHrResult;

    if (HSMProc(&stHSMDataIn, &stHSMDataOut, &stHSMScore) == GH3X2X_RET_OK)
    {
        if (stHSMDataOut.uchStageDataFlag != 0)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->snResult[0] = 1;
            GU8 uchHsmResultSendBuffer[235] = {0};
            GU8 uchHsmPackLen = 140;
            uchHsmResultSendBuffer[4] = stHSMDataOut.uchStageDataFlag;
            GH3X2X_Memcpy(uchHsmResultSendBuffer + 1 + 4, (GU8*)&stHSMScore, 18);
            uchHsmResultSendBuffer[23] = stHSMDataOut.uchStageOutNum;
            GH3X2X_Memcpy(uchHsmResultSendBuffer + 1 + 4 + 19, stHSMDataOut.puchHSMStageOut, 120);
            GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_HSM_RESULT_UPDATE, uchHsmResultSendBuffer,
                                                    &uchHsmResultSendBuffer[4], uchHsmPackLen);
            Gh3x2xDemoSendProtocolData(uchHsmResultSendBuffer,usRespondLen);
        }
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("hsm process error!\r\n");
    }
#else
    return;
#endif
}

#endif

