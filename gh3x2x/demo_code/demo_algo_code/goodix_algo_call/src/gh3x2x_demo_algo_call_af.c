#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_hba.h"
#include "goodix_hrv.h"
#include "goodix_af.h"

#if (__USE_GOODIX_HRV_ALGORITHM__ && __USE_GOODIX_AF_ALGORITHM__)

extern GU32 g_unHrvLastHrResult;
extern goodix_hrv_ret goodix_hrv_init_func(GU32 fs);
extern goodix_hrv_ret goodix_hrv_deinit_func(void);

GS8 GH3x2xAfAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);    
    for (GU8 uchChMapCnt = 0; uchChMapCnt < g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_AF]->pstFunctionInfo->uchChnlNum; ++uchChMapCnt)
    {
        g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_RESP]->puchFrameLastGain[uchChMapCnt] = GH3X2X_GAIN_VALUE_INVALID;
    }
    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,\r\n", __FUNCTION__, g_nAfParamsArr[0]);
    if ((AFInit((GS32)g_nAfParamsArr[0]) == GX_ALGO_HBA_SUCCESS) &&\
        (goodix_hrv_init_func(pstFrameInfo ->pstFunctionInfo ->usSampleRate) == GX_ALGO_HBA_SUCCESS))
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0xFF;
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("af deinit error!\r\n");
    }
    g_unHrvLastHrResult = 0;
    return chRet;
}

GS8 GH3x2xAfAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;


    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    if ((goodix_hrv_deinit_func() == GX_ALGO_HBA_SUCCESS) && (AFDeInit() == GX_ALGO_HBA_SUCCESS))
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("af deinit error!\r\n");
    }
    return chRet;
}

GS8 GH3x2xAfAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo )
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
    GU32 unResult[3] = {0};
    GS32 nOutputData[6]   = {0};
    GS32 nAfInputData[6] = {0};
    ST_HRV_INPUT_INFO siRawdataInput;
    memset((GU8*)&siRawdataInput, 0, sizeof(ST_HRV_INPUT_INFO));
    GU8 uchGainValue = 0;

    for (int nRawdatacnt = 0 ; nRawdatacnt< HRV_CHNL_MAX_NUM ; nRawdatacnt ++)
    {
        siRawdataInput.ppg_green[nRawdatacnt] = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(pstFrameInfo->punFrameRawdata[nRawdatacnt]);
        siRawdataInput.ppg_green[nRawdatacnt] = GH3X2X_GET_RAWDATA_FOR_ALGO(siRawdataInput.ppg_green[nRawdatacnt]);
        uchGainValue = GH3X2X_GET_LOW_4BITS(pstFrameInfo->punFrameAgcInfo[nRawdatacnt]);
        if ((pstFrameInfo->puchFrameLastGain[nRawdatacnt] != uchGainValue) && \
            (pstFrameInfo->puchFrameLastGain[nRawdatacnt] != GH3X2X_GAIN_VALUE_INVALID))
        {
            siRawdataInput.green_gain_adj_flag[nRawdatacnt] = 1;
            pstFrameInfo->puchFrameLastGain[nRawdatacnt] = uchGainValue;
        }
            
        siRawdataInput.green_cur_adj_flag[nRawdatacnt] = ((pstFrameInfo->punFrameFlag[0]) & 0x01);
    }
    siRawdataInput.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    siRawdataInput.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    siRawdataInput.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    siRawdataInput.frame_id = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        g_unHrvLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    siRawdataInput.hr = g_unHrvLastHrResult;
    
    GU8 uchHrvUpdateFlag = HRVCalculateAlgo(&siRawdataInput, nOutputData);

    if (uchHrvUpdateFlag && (nOutputData[5] != 0))
    {
        for (int i = 0 ; i < 6 ; i ++)
        {
            nAfInputData[i] = nOutputData[i];
        }

        pstFrameInfo->pstAlgoResult->uchUpdateFlag = AF_Detection_Module(nAfInputData, unResult);

        if(pstFrameInfo->pstAlgoResult->uchUpdateFlag == 1)
        {
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)unResult[0];
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)unResult[1];
            pstFrameInfo->pstAlgoResult->snResult[2] = (GS32)unResult[2];
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)nOutputData[0];
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)nOutputData[1];
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)nOutputData[2];
            pstFrameInfo->pstAlgoResult->snResult[6] = (GS32)nOutputData[3];
            pstFrameInfo->pstAlgoResult->snResult[7] = (GS32)nOutputData[4];
            pstFrameInfo->pstAlgoResult->usResultBit = 0xFF;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            GH3X2X_DEBUG_ALGO_LOG("[%s]af = %d,UpdateFlag = %d\r\n",__FUNCTION__,(int)pstFrameInfo->pstAlgoResult->snResult[0],(int)pstFrameInfo->pstAlgoResult->uchUpdateFlag);
            //GH3X2X_HrAlgorithmResultReport(pstFrameInfo->pstAlgoResult,stRawdata.frameid);
        }
    }
    return GH3X2X_RET_OK;
}

#endif

