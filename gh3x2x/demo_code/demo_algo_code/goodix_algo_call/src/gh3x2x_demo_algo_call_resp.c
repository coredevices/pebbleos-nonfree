#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_resp.h"

#if (__USE_GOODIX_RESP_ALGORITHM__)

GS8 GH3x2xRespAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_RESP]->puchFrameLastGain[0] = GH3X2X_GAIN_VALUE_INVALID;
    GS32 nRespFs = ((GS32)(pstFrameInfo ->pstFunctionInfo ->usSampleRate) & 0xFFFF);
    chRet = RespInit(&nRespFs);
    if (chRet == GH3X2X_RET_OK)
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x3;
    }
    return chRet;
}

GS8 GH3x2xRespAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    RespDeInit();
    chRet = GH3X2X_RET_OK;
    return chRet;
}

GS8 GH3x2xRespAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
    RESP_ST_DATA_IN stRespDataIn = {0};
    RESP_ST_DATA_OUT stRespDataOut = {0};
    GU8 uchGainValue = 0;
    
    stRespDataIn.IPpgRaw = GH3X2X_GET_RAWDATA_FOR_ALGO(pstFrameInfo->punFrameRawdata[0] - GH3X2X_RAWDATA_BASE_VALUE);
    stRespDataIn.IACCx = pstFrameInfo->pusFrameGsensordata[0];
    stRespDataIn.IACCy = pstFrameInfo->pusFrameGsensordata[1];
    stRespDataIn.IACCz = pstFrameInfo->pusFrameGsensordata[2];
    
    uchGainValue = GH3X2X_GET_LOW_4BITS(pstFrameInfo->punFrameAgcInfo[0]);
    if (((pstFrameInfo->puchFrameLastGain[0] != uchGainValue) && \
        (pstFrameInfo->puchFrameLastGain[0] != GH3X2X_GAIN_VALUE_INVALID)) ||\
        ((pstFrameInfo->punFrameFlag[0]) & 0x01))
    {
        stRespDataIn.IAdjFlag = 1;
        pstFrameInfo->puchFrameLastGain[0] = uchGainValue;
    }

    GU8 chRet = RespProc(&stRespDataIn, &stRespDataOut);

    if (1 == stRespDataOut.nRespOutFlag && chRet == 0)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->snResult[0]   = (GS32)stRespDataOut.nRespRate;
        pstFrameInfo->pstAlgoResult->snResult[1]   = (GS32)stRespDataOut.nBelieve;        
        pstFrameInfo->pstAlgoResult->usResultBit = 0x3;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
        //GH3X2X_HrvAlgorithmResultReport(pstFrameInfo->pstAlgoResult,stRawdata.frameid);
    }
    return chRet;
}

#endif

