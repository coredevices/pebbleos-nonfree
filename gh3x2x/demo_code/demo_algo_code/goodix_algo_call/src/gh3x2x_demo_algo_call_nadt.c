#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "goodix_mem.h"
#include "goodix_nadt.h"

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)

#define NADT_CONFIG_SAMPLE_RATE_TYPE                (0)
#define NADT_CONFIG_MULTI_SAMPLE_RATE_TYPE          (0)

#define SOFT_WEAR_INPUT_DATA_CHNL0_INDEX            (0)
#define SOFT_WEAR_INPUT_DATA_ACCX_INDEX             (1)
#define SOFT_WEAR_INPUT_DATA_ACCY_INDEX             (2)
#define SOFT_WEAR_INPUT_DATA_ACCZ_INDEX             (3)
#define SOFT_WEAR_INPUT_COLOR_INDEX                 (4)
#define SOFT_WEAR_INPUT_DATA_ADT_INDEX              (5) //ir  adt data
#define SOFT_WEAR_INPUT_DATA_BG_INDEX               (6)
#define SOFT_WEAR_INPUT_DATA_CAP0_INDEX             (7)
#define SOFT_WEAR_INPUT_DATA_CAP1_INDEX             (8)
#define SOFT_WEAR_INPUT_DATA_CAP2_INDEX             (9)
#define SOFT_WEAR_INPUT_DATA_CAP3_INDEX             (10)
#define SOFT_WEAR_INPUT_DATA_INDEX_TOTAL            (11)
#define SOFT_WEAR_OUTPUT_DATA_INDEX_TOTAL           (8)
#define WATCH_NUM                                   (3)

#if (WATCH_NUM == 1)
static GS32 lWearCap1 = 16400;
static GS32 lUnwearCap1 = 12400;
static GS32 lNoise1 = 300;
static GS32 lWearCap2 = 0;
static GS32 lUnwearCap2 = 0;
static GS32 lNoise2 = 0;
#elif (WATCH_NUM == 2)
static GS32 lWearCap1 = 20000;
static GS32 lUnwearCap1 = 16000;
static GS32 lNoise1 = 200;
static GS32 lWearCap2 = 3700;
static GS32 lUnwearCap2 = 1200;
static GS32 lNoise2 = 200;
#elif (WATCH_NUM == 3)
static GS32 lWearCap1 = 42500;
static GS32 lUnwearCap1 = 39500;
//static GS32 lNoise1 = 150;
static GS32 lWearCap2 = 42500;
static GS32 lUnwearCap2 = 39500;
//static GS32 lNoise2 = 150;
#endif

//static GS32 ghbrelsult[10];
//static GS32 glConfig[3] = { 0 };
static GU8  g_uchSoftAdtChannl0Color;   // 0: green   1: ir
static GU32 g_unNadtIrDefaultTimeOutRecord = 0;
static GU32 g_unNadtIrDefaultTimeOutSecondsThrd = __SOFT_ADT_IR_DETECT_TIMEOUT__;
static GU8 g_SoftWearQuality = 0;
//NADT_ST_MID_INFO stMidInfo;
extern GU8 g_SoftAdtWearState;

void Gh3x2x_SetIRDetectTimeThreshold(GU32 unIRDetectTimeThreshold)
{
    g_unNadtIrDefaultTimeOutSecondsThrd = unIRDetectTimeThreshold;
}

void SoftWearControlInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    g_SoftAdtWearState = GH3X2X_SOFT_ALGO_ADT_DEFAULT;

    goodix_nadt_ret stAlgoRet = GX_ALGO_NADT_SUCCESS;
    goodix_nadt_config stNadtCfg = {0};
    GCHAR uchNadtVersion[120] = {0};

    goodix_nadt_version((uint8_t *)uchNadtVersion);
    GH3X2X_DEBUG_ALGO_LOG("nadt algorithm version : %s\r\n", uchNadtVersion);

    goodix_nadt_config *p_cfg = (goodix_nadt_config *)goodix_nadt_config_get_arr();
    GH3X2X_Memcpy((void *)&stNadtCfg, p_cfg, sizeof(goodix_nadt_config));
    stNadtCfg.sampleRateNADT = pstFrameInfo->pstFunctionInfo->usSampleRate;

    GH3X2X_DEBUG_ALGO_LOG("[%s]nadt param0 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__, stNadtCfg.sampleRateNADT, \
        stNadtCfg.unwearDetectLevel, stNadtCfg.checkTimeoutThr, stNadtCfg.unwearCheckCntThr, stNadtCfg.wearCheckCntThr, \
        stNadtCfg.adtWearOnThr, stNadtCfg.adtWearOffThr, stNadtCfg.accStdThr, stNadtCfg.liveDetectEnable, stNadtCfg.rawPVLenThr, \
        stNadtCfg.heartBeatThrLow, stNadtCfg.heartBeatThrHigh, stNadtCfg.difRrInterThr, stNadtCfg.wearAcfScoreThr);
    GH3X2X_DEBUG_ALGO_LOG("[%s]nadt param1 : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__, stNadtCfg.baseLinePosRatioThr, \
        stNadtCfg.baseLineDiffThr, stNadtCfg.sigScaleThr, stNadtCfg.bgJumpDetectEnable, stNadtCfg.jumpBgDiffThr, stNadtCfg.bgPeakValleyNumThr, \
        stNadtCfg.bgPeriodDetectEnable, stNadtCfg.periodBgDiffThr, stNadtCfg.bgPeriodThr, stNadtCfg.bgPeriodDiffThr);

    const void *p_CfgInstance = (void *)&stNadtCfg;
    char chVer[NADT_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_nadt_config_get_version(chVer, NADT_INTERFACE_VERSION_LEN_MAX);
    const char *p_InterfaceVer = chVer;

    stAlgoRet = goodix_nadt_init(p_CfgInstance, sizeof(goodix_nadt_config), p_InterfaceVer);

    if (GX_ALGO_NADT_SUCCESS != stAlgoRet)
    {
        GH3X2X_DEBUG_ALGO_LOG("nadt init error! error code = 0x%x\r\n", stAlgoRet);
    }
}

void GH3x2xCapNadtResultPro(const STGh3x2xFrameInfo * const pstFrameInfo,GS32  pnSoftWearOffDetResult[])
{
    if (pnSoftWearOffDetResult[3])            // 电容更新标记,更新佩戴检测阈值
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        lUnwearCap1 = pnSoftWearOffDetResult[4];
        lWearCap1 = pnSoftWearOffDetResult[5];
        lUnwearCap2 = pnSoftWearOffDetResult[6];
        lWearCap2 = pnSoftWearOffDetResult[7];
    }
    if(g_SoftWearQuality != pnSoftWearOffDetResult[2])
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        g_SoftWearQuality = pnSoftWearOffDetResult[2];
    }
    if(pstFrameInfo->punFrameCnt == 0)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
    }
    if(pstFrameInfo->pstAlgoResult->uchUpdateFlag == 1)
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7;
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);      
        pstFrameInfo->pstAlgoResult->snResult[0] = pnSoftWearOffDetResult[0]; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
        pstFrameInfo->pstAlgoResult->snResult[1] = pnSoftWearOffDetResult[1]; //活体检测置信度
        pstFrameInfo->pstAlgoResult->snResult[2] = pnSoftWearOffDetResult[2]; //佩戴质量（1紧佩戴，2松佩戴，3极度松佩戴，4脱落）；
        if(pstFrameInfo->punFrameCnt == 0)
        {
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->snResult[3] = lUnwearCap1; //脱落电容值1；
            pstFrameInfo->pstAlgoResult->snResult[4] = lWearCap1; //佩戴电容值1；
            pstFrameInfo->pstAlgoResult->snResult[5] = lUnwearCap2; //脱落电容值2；
            pstFrameInfo->pstAlgoResult->snResult[6] = lWearCap2; //佩戴电容值2；
        }
        else
        {
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->snResult[3] = pnSoftWearOffDetResult[4]; //脱落电容值1；
            pstFrameInfo->pstAlgoResult->snResult[4] = pnSoftWearOffDetResult[5]; //佩戴电容值1；
            pstFrameInfo->pstAlgoResult->snResult[5] = pnSoftWearOffDetResult[6]; //脱落电容值2；
            pstFrameInfo->pstAlgoResult->snResult[6] = pnSoftWearOffDetResult[7]; //佩戴电容值2；
        }
    }
}

GS8 GH3x2xSoftAdtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if (0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    GS32 ppg_rawdata[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint16_t drv_cur[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t gain_code[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t enable_flg[GH_ALGO_EN_FLAG_MAX] = {0};
    int32_t ipd[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    int32_t cap_rawdata[4] = {0};
    int32_t cap_enable[4] = {0};

    goodix_nadt_input_rawdata stRawdata = {0};
    goodix_nadt_result stResult = {0};
    GS8 chAlgoRet = GH3X2X_RET_OK;
    goodix_nadt_input_unique in_unique = {0};

    in_unique.ppg_colour_flg = g_uchSoftAdtChannl0Color;
    in_unique.pre_status = 0;
    in_unique.ch_ipd = ipd;
    in_unique.cap_rawdata = cap_rawdata;
    in_unique.cap_enable_flg = cap_enable;

    if (g_uchAlgoCapEnable == 1)
    {
        in_unique.cap_channel_num = 4;
        for (int k = 0; k < 4; k++)
        {
            in_unique.cap_rawdata[k] = pstFrameInfo->pstFrameCapdata->unCapData[k]; //  [PPGACCLEN + ALGOLEN + guchCompareNum + k]; //电容
            in_unique.cap_enable_flg[k] = 1;
        }
    }

    stRawdata.pst_module_unique = &in_unique;
    stRawdata.frame_id = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.total_ch_num = GH_ALGO_PPG_CHL_NUM;
    stRawdata.enable_flg = enable_flg;
    stRawdata.ppg_rawdata = ppg_rawdata;
    stRawdata.ch_agc_gain = gain_code;
    stRawdata.ch_agc_drv = drv_cur;
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.gyro_x = 0;
    stRawdata.gyro_y = 0;
    stRawdata.gyro_z = 0;
    stRawdata.sleep_flg = GH3x2xSleepFlagGet();
    stRawdata.bit_num = GH_ALGO_DATA_BIT_NUM;
    stRawdata.chip_type = GH_ALGO_CHIP_TYPE;
    stRawdata.data_type = GH_ALGO_DATA_TYPE;

    for (GS32 nRawdatacnt = 0; nRawdatacnt < (PPG_CHANNEL_NUM - 1); nRawdatacnt++)
    {
        stRawdata.ppg_rawdata[nRawdatacnt * GH_ALGO_PPG_CHL_NUM] = pstFrameInfo->punFrameRawdata[nRawdatacnt];
        stRawdata.ch_agc_drv[nRawdatacnt * GH_ALGO_PPG_CHL_NUM]  = ((pstFrameInfo->punFrameAgcInfo[nRawdatacnt] >> 8) & 0xFF)
                                             + ((pstFrameInfo->punFrameAgcInfo[nRawdatacnt] >> 16) & 0xFF);
        stRawdata.ch_agc_gain[nRawdatacnt * GH_ALGO_PPG_CHL_NUM]  = GH3X2X_GET_LOW_4BITS(pstFrameInfo->punFrameAgcInfo[nRawdatacnt]);

//        stRawdata.enable_flg[nRawdatacnt / GH_ALGO_BYTE_BITS] = enable_flg[nRawdatacnt / GH_ALGO_BYTE_BITS] 
//                                                                | (1 << (GH_ALGO_EN_FLAG_OFFSET - (nRawdatacnt % GH_ALGO_BYTE_BITS)));

//        GH3X2X_DEBUG_ALGO_LOG("[%s]ppg_rawdata = %d, ch_agc_drv = %d, ch_agc_gain = %d\r\n", __FUNCTION__, 
//            (int)stRawdata.ppg_rawdata[nRawdatacnt * GH_ALGO_PPG_CHL_NUM], (int)stRawdata.ch_agc_drv[nRawdatacnt * GH_ALGO_PPG_CHL_NUM], 
//            (int)stRawdata.ch_agc_gain[nRawdatacnt * GH_ALGO_PPG_CHL_NUM]);
    }

    goodix_nadt_ret ret = goodix_nadt_calc(&stRawdata, &stResult);

    GU32 unNadtIrDefaultTimeOutThrd = g_unNadtIrDefaultTimeOutSecondsThrd * pstFrameInfo->pstFunctionInfo->usSampleRate;
    if (g_unNadtIrDefaultTimeOutRecord >= unNadtIrDefaultTimeOutThrd && ((stResult.nadt_out & 0x3) == 0))
    {
        stResult.nadt_out = stResult.nadt_out | 0x1;
        g_unNadtIrDefaultTimeOutRecord = 0;
    }

    if ((stResult.nadt_out & 0x2) == 0)
    {
        if (GH3X2X_FUNCTION_SOFT_ADT_IR == pstFrameInfo->unFunctionID)
        {
            g_unNadtIrDefaultTimeOutRecord++;
        }
    }

    if (GX_ALGO_NADT_SUCCESS == ret)
    {
        if (g_uchAlgoCapEnable == 1)
        {
            //GH3x2xCapNadtResultPro(pstFrameInfo, pnSoftWearOffDetResult);
        }
        else
        {
            if (1 == stResult.nadt_out_flag)
            {
                pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
                pstFrameInfo->pstAlgoResult->usResultBit = 0x3;
                pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
                pstFrameInfo->pstAlgoResult->snResult[0] = stResult.nadt_out; // (bit0-bit1)：佩戴状态（0默认，1佩戴，2脱落，3非活体）； (bit2)：疑似脱落标记（0正常，1疑似脱落）；
                pstFrameInfo->pstAlgoResult->snResult[1] = stResult.nadt_confi; // 活体检测置信度
                GH3X2X_DEBUG_ALGO_LOG("[nadt] wear status:%d %d, confiidence = %d\r\n", pstFrameInfo->pstAlgoResult->snResult[0] & 0x3, \
                    (pstFrameInfo->pstAlgoResult->snResult[0] >> 2) & 0x1, pstFrameInfo->pstAlgoResult->snResult[1]);

                //report
                if (g_uchSoftAdtChannl0Color == 0)
                {
                    GH3X2X_SoftAdtGreenAlgorithmResultReport(pstFrameInfo->pstAlgoResult, GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
                }
                else if(g_uchSoftAdtChannl0Color == 1)
                {
                    GH3X2X_SoftAdtIrAlgorithmResultReport(pstFrameInfo->pstAlgoResult, GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt)));
                }
            }
        }
    }
    else
    {
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
        GH3X2X_DEBUG_ALGO_LOG("GH3x2xNadtAlgoExe error! error code = 0x%x\r\n", ret);
        GH3X2X_DEBUG_ALGO_LOG("please feedback to GOODIX!\r\n");
    }

    return chAlgoRet;

}


GS8 GH3x2xSoftAdtAlgoInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    if (pstFrameInfo)
    {
        g_SoftWearQuality  = 0;
        g_uchSoftAdtChannl0Color = 1 * (GH3X2X_FUNCTION_SOFT_ADT_IR == (pstFrameInfo ->unFunctionID));

        GH3X2X_DEBUG_ALGO_LOG("[SoftAdt]CH0 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[0]), \
            GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[0]));
        GH3X2X_DEBUG_ALGO_LOG("[SoftAdt]CH1 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[1]), \
            GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[1]));
        GH3X2X_DEBUG_ALGO_LOG("[SoftAdt]CH2 slot = %d, rx = %d\r\n", GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(pstFrameInfo ->pchChnlMap[2]), \
            GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(pstFrameInfo ->pchChnlMap[2]));
        GH3X2X_DEBUG_ALGO_LOG("[SoftAdt]g_uchSoftAdtChannl0Color = %d\r\n", g_uchSoftAdtChannl0Color);

        SoftWearControlInit(pstFrameInfo);
    }

    return GH3X2X_RET_OK;
}

GS8 GH3x2xSoftAdtAlgoDeinit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GH3X2X_DEBUG_ALGO_LOG("[SoftAdt]%s\r\n", __FUNCTION__);

    goodix_nadt_ret ret = goodix_nadt_deinit();
    g_unNadtIrDefaultTimeOutRecord = 0;

    if (GX_ALGO_NADT_SUCCESS != ret)
    {
        GH3X2X_DEBUG_ALGO_LOG("[%s] fail! error code = 0x%x\r\n", __FUNCTION__, ret);
        return GH3X2X_RET_GENERIC_ERROR;
    }

    return GH3X2X_RET_OK;
}

#endif
