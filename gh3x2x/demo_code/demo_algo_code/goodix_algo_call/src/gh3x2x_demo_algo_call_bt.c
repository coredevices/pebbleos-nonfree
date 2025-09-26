#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_call.h"
#include "goodix_mem.h"
#include "goodix_algo.h"
#include "goodix_algo_bt.h"

#if (__USE_GOODIX_BT_ALGORITHM__)

#define NTCSecondResult  0

/*****体温全局变量*****/
//NTC R-T table
#define NTC_TYPE  CB62K552D
#if (CB62K928D==NTC_TYPE)
GU16 RT_Table[] = {25992,24742,23559,22437,21375,20367,19413,18507,17648,16833,16060,15325,14628,13965,13336,12738,12169,11629,11115,10625,10160,9717,9295,8894,8512,8147,7800,7469,7154,6853,6566,6293,6032,5783,5545,5318,5101,4894,4696,4507};
#elif(CB62K552D==NTC_TYPE)
GU16 RT_Table[] = {25837,24594,23418,22303,21247,20246,19297,18397,17543,16733,15964,15234,14540,13882,13256,12662,12097,11559,11048,10562,10099,9659,9240,8841,8461,8099,7754,7425,7111,6812,6527,6255,5996,5748,5512,5286,5071,4865,4668,4480};
#elif(DT104D3950==NTC_TYPE)
GU16 RT_Table[] = {26630,25340,24130,22980,21890,20850,19880,18950,18070,17240,16450,15700,14990,14320,13680,13070,12490,11940,11420,10920,10450,10000,9572,9165,8777,8408,8056,7720,7401,7096,6805,6528,6263,6011,5769,5539,5319,5109,4908,4717};
#endif
GU16 unSensorNtcRT_Len = sizeof(RT_Table)/sizeof(RT_Table[0]);
GU16 unSensorNtcRef = 3000; // NTC ref resistance (unit : 10ohm)
GS16 unTempRangeLower = 4*100;
GS16 unTempRangeUpper = 43*100;
GU8 unNTCChnalRes = 156;
GU32 g_unBtLastHrResult = 0;
GU16 uFs =0;

#if !__USE_GOODIX_DRV_NTC_Convert
STGoodixAlgorithmBtControlConfigParametersBuffer gstBtCFG;
STGoodixAlgorithmBtInputRef                      gstBtInputRef;
STGoodixAlgorithmBtCalculateOutputResult         gstBtAlgoOUT;
//STGoodixAlgorithmBtFinalOutputResult             gstBtAlgoFinal;
STGoodixAlgorithmInfo                            gstBtAlgoInfo;
/**
 * @brief memory malloc function
 * @param size
 * @return pointer of memory
*/
void* GoodixSensorAlgoMalloc(GS32 size)
{
    return goodix_mem_malloc(size);
}

/**
 * @brief memory free function
 * @param addr
*/
void GoodixSensorAlgoFree(void* addr)
{
    goodix_mem_free(addr);
}

/**
 * @brief algorithm log function
 * @param addr
*/
void GoodixSensorAlgoLog(GCHAR* pLogStr)
{
    //GH3X2X_DEBUG_ALGO_LOG("[%s]:%s\n", __FUNCTION__, pLogStr);
}

#else
STGH3x2xDrvBtOutputResult                        gstDrvBtOutRes;

GU16 GetRTIndex(GS32 R, GU16* pR, GU16 len)
{
	GU16 i =0;
	while(R <= pR[i]*10)
	{
		i++;
		if(len == i)
		{
			i = 0xFFFF;
			break;
		}
	}
	return i;
}

/**
 * @fn   GS16 GH3x2xBtRTConvert(GS32* pnRawdata,STGH3x2xDrvBtOutputResult* pstDrvBtOutRes)
 *
 * @brief  calc byte check rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata  one rawdata addr IDEL REF NTC0 NTC1
 * @param[out]  None
 *
 * @return  target rawdata
 */
static GS16 GH3x2xBtRTConvert(GS32* pnRawdata,STGH3x2xDrvBtOutputResult* pstDrvBtOutRes)
{
	GU8 uchIdx = 0;
	GU16 uRTIdx =0;
	GS32 nRestemp0, nRestemp1;
	GS16 nRawTemp =0;
	GU8 chRet=0;

	for(uchIdx=0;uchIdx <BT_MAX_CH_NUM;uchIdx++)
	{
		nRestemp0 = pnRawdata[1] - pnRawdata[0]; // ref - idle
		nRestemp1 = pnRawdata[2+ uchIdx] - pnRawdata[0]; // ntc - idle
		if(0==nRestemp1)
		{
			GH3X2X_DEBUG_ALGO_LOG("BT input rawdata error: ZERO! \r\n");
			nRawTemp = 0;
			chRet = EM_TEMP_OUT_RANGE_WRONG_INPUT;
		}
		else
		{
			nRestemp0 = ((GS64)nRestemp0 * (unSensorNtcRef*10 + unNTCChnalRes)) / nRestemp1 - unNTCChnalRes;
			if(nRestemp0<0)
			{
				GH3X2X_DEBUG_ALGO_LOG("BT input rawdata error: NEGATIVE! \r\n");
				nRawTemp = 0;
				chRet = EM_TEMP_OUT_RANGE_WRONG_INPUT;
			}
			else
			{
				uRTIdx = GetRTIndex(nRestemp0,RT_Table,unSensorNtcRT_Len);
				if(0 == uRTIdx)
				{
					nRawTemp = 0;
					chRet = EM_TEMP_OUT_RANGE_L;
				}
				else if(0xFFFF == uRTIdx)
				{
					nRawTemp = 0;
					chRet = EM_TEMP_OUT_RANGE_H;
				}
				else
				{
					nRawTemp = (GS16)(100*(RT_Table[uRTIdx-1]*10 - nRestemp0)/(RT_Table[uRTIdx-1]*10-RT_Table[uRTIdx]*10)) + (GS16) unTempRangeLower + (GS16)(uRTIdx-1)*100;
				}
			}
		}
		pstDrvBtOutRes->nFrameTemperature[uchIdx] = nRawTemp;
		if(0 == pstDrvBtOutRes->chFrameIndex)
		{
			pstDrvBtOutRes->stRawTemp[uchIdx].nMin = nRawTemp;
			pstDrvBtOutRes->stRawTemp[uchIdx].nMax = nRawTemp;
			pstDrvBtOutRes->stRawTemp[uchIdx].nSum = nRawTemp;
			GH3X2X_DEBUG_ALGO_LOG("[%s]:index: %d rawdata sum %d: %d \n", __FUNCTION__,pstDrvBtOutRes->chFrameIndex,uchIdx, pstDrvBtOutRes->stRawTemp[uchIdx].nSum);
		}
		else
		{
			pstDrvBtOutRes->stRawTemp[uchIdx].nMin = (pstDrvBtOutRes->stRawTemp[uchIdx].nMin>nRawTemp)?nRawTemp:pstDrvBtOutRes->stRawTemp[uchIdx].nMin;
			pstDrvBtOutRes->stRawTemp[uchIdx].nMax = (pstDrvBtOutRes->stRawTemp[uchIdx].nMax<nRawTemp)?nRawTemp:pstDrvBtOutRes->stRawTemp[uchIdx].nMax;
			pstDrvBtOutRes->stRawTemp[uchIdx].nSum += nRawTemp;
			GH3X2X_DEBUG_ALGO_LOG("[%s]:index: %d rawdata sum %d: %d \n", __FUNCTION__,pstDrvBtOutRes->chFrameIndex,uchIdx, pstDrvBtOutRes->stRawTemp[uchIdx].nSum);
		}
	}
	++pstDrvBtOutRes->chFrameIndex;
	if(pstDrvBtOutRes->chFrameIndex == uFs)
	{
		pstDrvBtOutRes->chFrameIndex=0;
		for(uchIdx=0;uchIdx <BT_MAX_CH_NUM;uchIdx++)
		{
			if(chRet)
			{
				pstDrvBtOutRes->nSecTemperature[uchIdx] = 0;
			}
			else
			{
				if(uFs>2)
				{
					GH3X2X_DEBUG_ALGO_LOG("[%s]:rawdata sum: %d \n", __FUNCTION__, pstDrvBtOutRes->stRawTemp[uchIdx].nSum);
					pstDrvBtOutRes->nSecTemperature[uchIdx] = (pstDrvBtOutRes->stRawTemp[uchIdx].nSum - pstDrvBtOutRes->stRawTemp[uchIdx].nMin - pstDrvBtOutRes->stRawTemp[uchIdx].nMax +((uFs-2)>>1))/(uFs-2);
				}
				else
				{
					pstDrvBtOutRes->nSecTemperature[uchIdx] = (pstDrvBtOutRes->stRawTemp[uchIdx].nMin + pstDrvBtOutRes->stRawTemp[uchIdx].nMax)>>1;
				}
				chRet = EM_CONVER_OK;
			}
		}
	}

	return chRet;
}
#endif

GS8 GH3x2xBtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
	#if !__USE_GOODIX_DRV_NTC_Convert
    //gstBtAlgoInfo.stAlgorithmConfig.pnParametersBuffer = &gstBtCFG;
    gstBtAlgoInfo.stAlgorithmCalcOutput.pnResult = &gstBtAlgoOUT;
    gstBtAlgoInfo.stAlgorithmCalcInput.pnRefBuffer = &gstBtInputRef;
    gstBtCFG.unChannelNum = 2;

    if(BT_ERRORCODE_OK != GoodixBtAlgoInit(&gstBtAlgoInfo))
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    pstFrameInfo->pstAlgoResult->usResultBit = 0x1F;
    g_unBtLastHrResult = 0;
	uFs = pstFrameInfo ->pstFunctionInfo ->usSampleRate;
    GH3X2X_Memset(&gstBtAlgoOUT, 0, sizeof(STGoodixAlgorithmBtCalculateOutputResult));
    GH3X2X_Memset(&gstBtInputRef, 0, sizeof(STGoodixAlgorithmBtInputRef));
	#else
	//drv calculate bt
	GH3X2X_Memset(&gstDrvBtOutRes, 0, sizeof(STGH3x2xDrvBtOutputResult));
	//GH3X2X_Memcpy(RT_Table, g_nBtParamsArr, GOODIX_BT_PARAMS_NUM_MAX * sizeof(GU16));
	//unSensorNtcRT_Len = sizeof(RT_Table)/sizeof(RT_Table[0]);
	#endif
    return GH3X2X_RET_OK;
}

GS8 GH3x2xBtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
	#if !__USE_GOODIX_DRV_NTC_Convert
    if(BT_ERRORCODE_OK != GoodixBtAlgoDeinit(&gstBtAlgoInfo))
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }
	#endif
    return GH3X2X_RET_OK;
}

GS8  GH3x2xBtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS32 TempRawdata[4] = {0};
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    for(GU8 i = 0; i < 4; i++)
    {
        TempRawdata[i] = (int)(pstFrameInfo->pstFrameTempdata->unTempData[i]);
    }

    if (g_uchAlgoTempEnable == 0)
    {
        for(GU8 i = 0; i < 4; i++)
        {
            TempRawdata[i] = (int)(pstFrameInfo->punFrameRawdata[i]);
        }
    }
#if !__USE_GOODIX_DRV_NTC_Convert
    gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata = TempRawdata;

    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        g_unBtLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    gstBtInputRef.unHR = g_unBtLastHrResult;

    //GH3X2X_DEBUG_ALGO_LOG("[%s]:%d,%d,%d,%d\n", __FUNCTION__, gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[0], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[1], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[2], gstBtAlgoInfo.stAlgorithmCalcInput.pnRawdata[3]);


    if(BT_ERRORCODE_OK == GoodixBtAlgoCalculate(&gstBtAlgoInfo))
    {
        //if(gstBtAlgoOUT.uchRefresh)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            if (0!=gstBtAlgoInfo.stAlgorithmCalcOutput.pnMiddleBaseBuffer)
            {
                for (int i =0; i < 2; i++)
                {
                    pstFrameInfo->pstAlgoResult->snResult[i] = gstBtAlgoInfo.stAlgorithmCalcOutput.pnMiddleBaseBuffer[i];//ntc0
                    //pstFrameInfo->pstAlgoResult->snResult[1] = gstBtAlgoOUT.nFPCBT;//ntc1
                }
            }
            pstFrameInfo->pstAlgoResult->snResult[2] = gstBtAlgoOUT.uchState;
            pstFrameInfo->pstAlgoResult->snResult[3] = gstBtAlgoOUT.uchConfidence;
            pstFrameInfo->pstAlgoResult->snResult[4] = gstBtAlgoOUT.nCBT;
            pstFrameInfo->pstAlgoResult->snResult[5] = g_unBtLastHrResult;
        }
        chRet = GH3X2X_RET_OK;
    }
#else
	// drv calculate bt
	chRet =GH3x2xBtRTConvert( TempRawdata, &gstDrvBtOutRes);
	pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
  pstFrameInfo->pstAlgoResult->usResultBit = 0x03;
	pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
	for (int i =0; i < 2; i++)
	{
		#if NTCSecondResult
		pstFrameInfo->pstAlgoResult->snResult[i] = gstDrvBtOutRes.nSecTemperature[i];//ntc0 ntc1 per second
		GH3X2X_DEBUG_ALGO_LOG("[%s]:rawdata: %d %d %d %d.second temp: %d %d\n", __FUNCTION__, TempRawdata[0],TempRawdata[1],TempRawdata[2],TempRawdata[3],gstDrvBtOutRes.nSecTemperature[0],gstDrvBtOutRes.nSecTemperature[1]);
		#else
		pstFrameInfo->pstAlgoResult->snResult[i] = gstDrvBtOutRes.nFrameTemperature[i];//ntc0 ntc1 per frame
		GH3X2X_DEBUG_ALGO_LOG("[%s]:rawdata: %d %d %d %d.frame temp: %d %d\n", __FUNCTION__, TempRawdata[0],TempRawdata[1],TempRawdata[2],TempRawdata[3],gstDrvBtOutRes.nFrameTemperature[0],gstDrvBtOutRes.nFrameTemperature[1]);
		#endif
	}
#endif
    return chRet;
}

#endif
