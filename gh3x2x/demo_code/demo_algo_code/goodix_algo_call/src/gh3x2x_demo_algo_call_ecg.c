
#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_ecg.h"

#if (__USE_GOODIX_ECG_ALGORITHM__)

#define GH3X2X_SINGLE_CLASSIFICATION_TIME 32

goodix_ecg_ret goodix_ecg_init_func(GU32 fs)
{
    goodix_ecg_config stEcgCfg = {0};
    goodix_ecg_init_result stInitResult = {0};
    GCHAR uchEcgVersion[100] = {0};

    goodix_ecg_version((uint8_t*)uchEcgVersion);
    GH3X2X_DEBUG_ALGO_LOG("ecg algorithm version : %s\r\n", uchEcgVersion);

#if (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_ENABLE) || (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_DISABLE)
    if (500 != fs)
    {
        GH3X2X_DEBUG_ALGO_LOG("[%s] fail! The sampling rate(%d) does not match the macro settings.\r\n", __FUNCTION__, fs);
        return GX_ALGO_ECG_RWONG_INPUT;
    }
#elif (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_ENABLE) || (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_DISABLE)
    if (250 != fs)
    {
        GH3X2X_DEBUG_ALGO_LOG("[%s] fail! The sampling rate(%d) does not match the macro settings.\r\n", __FUNCTION__, fs);
        return GX_ALGO_ECG_RWONG_INPUT;
    }
#else
#error "please define __GOODIX_ECG_ALGO_CONFIG__ in gh3x2x_demo_algo_config.h !"
#endif

#if (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_ENABLE) || (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_ENABLE)
    stEcgCfg.qrs_flag = 1;
#elif (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_DISABLE) || (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_DISABLE)
    stEcgCfg.qrs_flag = 0;
#else
#error "please define __GOODIX_ECG_ALGO_CONFIG__ in gh3x2x_demo_algo_config.h !"
#endif

    stEcgCfg.fs = fs;
    stEcgCfg.valid_channel_num = (uint32_t)g_nEcgParamsArr[1];
    stEcgCfg.mode_type = (uint32_t)g_nEcgParamsArr[3];
    stEcgCfg.ic_type = (uint32_t)g_nEcgParamsArr[4];

    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,%d,%d,%d,%d\r\n", __FUNCTION__,
                                    stEcgCfg.fs,
                                    stEcgCfg.valid_channel_num,
                                    stEcgCfg.qrs_flag,
                                    stEcgCfg.mode_type,
                                    stEcgCfg.ic_type);

    const void *p_CfgInstance = (void *)&stEcgCfg;
    char chVer[ECG_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_ecg_config_get_version(chVer, ECG_INTERFACE_VERSION_LEN_MAX);
    const char *p_InterfaceVer = chVer;

    goodix_ecg_ret ret = goodix_ecg_init(p_CfgInstance, &stInitResult, sizeof(stEcgCfg), p_InterfaceVer);
    g_uchClassificationFlag = 0;
    g_unEcgTimeSeconds = 0;

    return ret;
}

goodix_ecg_ret goodix_ecg_deinit_func(void)
{
    return goodix_ecg_deinit();
}

GS8 GH3x2xEcgAlgoInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GU16 usSampleRate = pstFrameInfo->pstFunctionInfo->usSampleRate;

    goodix_ecg_ret ret = goodix_ecg_init_func(usSampleRate);
    if (GX_ALGO_ECG_SUCCESS == ret)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        chRet = GH3X2X_RET_PARAMETER_ERROR;
        GH3X2X_DEBUG_ALGO_LOG("ecg init error! ret = 0x%x\r\n", ret);
    }
    g_unEcgFrameCnt  = 0;
    GH3X2X_SetNewDataFlag(GH3X2X_FUNC_OFFSET_ECG, GH3X2X_IS_NEW_RAWDATA);

    *(pstFrameInfo->punFrameCnt) = 0;

    return chRet;
}

GS8 GH3x2xEcgAlgoDeinit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    goodix_ecg_ret ret = goodix_ecg_deinit_func();
    if (ret == GX_ALGO_ECG_SUCCESS)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("ecg deinit error! ret = 0x%x\r\n", ret);
    }

    return chRet;
}

GS8 GH3x2xEcgAlgoExe(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    if (0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    goodix_ecg_input_rawdata stRawdata = {0};
    goodix_ecg_result stResult;
    GS8 chAlgoRet = 0;
    GU32 unRawdata[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgOutData[ECG_CHANNEL_NUM] = {0};
    goodix_stage unEcgStage[ECG_CHANNEL_NUM] = {stage_noneffect_out, stage_noneffect_out};
    GF32 fEcgSnr[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgPowerLineOut[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgBlwOut[ECG_CHANNEL_NUM] = {0};
    GF32 fEcgBpmCh[ECG_CHANNEL_NUM] = {0};

    stResult.ecg_out = fEcgOutData;
    stResult.ecg_stage = unEcgStage;
    stResult.ecg_snr = fEcgSnr;
    stResult.ecg_powerline_out = fEcgPowerLineOut;
    stResult.ecg_blw_out = fEcgBlwOut;
    stResult.ecg_bpm_ch = fEcgBpmCh;

    GH3X2X_Memset(&stRawdata, 0, sizeof(goodix_ecg_input_rawdata));
    stRawdata.ecg_rawdata = (GS32 *)&unRawdata;
    stRawdata.frameid = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.ecg_rawdata[0] = pstFrameInfo->punFrameRawdata[0];
    stRawdata.ecg_rawdata[1] = pstFrameInfo->punFrameRawdata[1];
    stRawdata.ecg_qc_flag = GH3X2X_CHECK_BIT_IS_SET(pstFrameInfo->punFrameFlag[2], 0x01);
    stRawdata.data_type = 0;
    GH3X2X_DEBUG_ALGO_LOG("[%s]ecg ecg_rawdata = %d, %d, ecg_qc_flag = %d\r\n", __FUNCTION__, stRawdata.ecg_rawdata[0], stRawdata.ecg_rawdata[1], \
        stRawdata.ecg_qc_flag);

    if (g_unEcgFrameCnt <= ECG_FRAME_UNUSED) /* the first 5 frame data can't be used for algo */
    {
        g_unEcgFrameCnt++;
        stRawdata.ecg_qc_flag = 1;
    }

    if (1 == stRawdata.ecg_qc_flag)     /* when fast recovery,data can't be used for algo */
    {
        return chAlgoRet;
    }

    goodix_ecg_ret stRet = goodix_ecg_update(&stRawdata, &stResult);

    if (stRet == GX_ALGO_ECG_SUCCESS)
    {
        pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
        pstFrameInfo->pstAlgoResult->snResult[0] = GH3x2x_Round(stResult.ecg_out[0] * ECG_OUT_FACTOR);
        pstFrameInfo->pstAlgoResult->snResult[2] = GH3x2x_Round(stResult.ecg_out[1] * ECG_OUT_FACTOR);

        GH3X2X_DEBUG_ALGO_LOG("ecg out0 = %.3f\r\n", stResult.ecg_out[0]);
        GH3X2X_DEBUG_ALGO_LOG("ecg out1 = %.3f\r\n", stResult.ecg_out[1]);
        //GH3X2X_INFO_ALGO_LOG("bpm result = %d\r\n",(GS32)(stResult.ecg_bpm + 0.5f));

        pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)(stResult.ecg_bpm + 0.5f);
        pstFrameInfo->pstAlgoResult->usResultBit = 0x1 | \
                                                 ((0x1 << 1) * (pstFrameInfo->pstAlgoResult->snResult[1] != 0)) | \
                                                 ((0x1 << 2) * (g_nEcgParamsArr[1] == 2));
        pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);

        GH3X2X_EcgAlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("GH3x2xEcgAlgoExe error! error code:0x%x\r\n", stRet);
        GH3X2X_DEBUG_ALGO_LOG("please feedback to GOODIX!\r\n");
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
    }

    return chAlgoRet;
}

GU8 GH3x2xEcgAlgoClassification(void)
{
    goodix_ecg_classification_update((goodix_classification_flag *)&g_uchClassificationFlag);
    return g_uchClassificationFlag;
}
#else
GU8 GH3x2xEcgAlgoClassification(void)
{
    return 0;
}
#endif

