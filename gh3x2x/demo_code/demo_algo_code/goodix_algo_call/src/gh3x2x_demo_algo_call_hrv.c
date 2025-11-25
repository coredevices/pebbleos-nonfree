#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "goodix_hba.h"
#include "goodix_hrv.h"

#if (__USE_GOODIX_HRV_ALGORITHM__)

GU32 g_unHrvLastHrResult = 0;

//hrv
goodix_hrv_ret goodix_hrv_init_func(GU32 fs)
{
    goodix_hrv_ret stAlgoRet = GX_HRV_ALGO_OK;
    goodix_hrv_config stHrvConfig = {0};
    GCHAR uchHrvVersion[150] = {0};

    goodix_hrv_version((uint8_t *)uchHrvVersion);
    GH3X2X_DEBUG_ALGO_LOG("hrv algorithm version : %s\r\n", uchHrvVersion);

    goodix_hrv_config *p_cfg = (goodix_hrv_config *)goodix_hrv_config_get_arr();
    GH3X2X_Memcpy((void *)&stHrvConfig, p_cfg, sizeof(goodix_hrv_config));
    stHrvConfig.fs = fs;

    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,%d,%d,%d,%d,%d,\r\n", __FUNCTION__,
                                    stHrvConfig.need_ipl,
                                    stHrvConfig.fs,
                                    stHrvConfig.acc_thr[0],
                                    stHrvConfig.acc_thr[1],
                                    stHrvConfig.acc_thr[2],
                                    stHrvConfig.acc_thr[3]);

    const void *p_CfgInstance = (void *)&stHrvConfig;
    char chVer[HRV_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_hrv_config_get_version(chVer, HRV_INTERFACE_VERSION_LEN_MAX);
    const char *p_InterfaceVer = chVer;

    stAlgoRet = goodix_hrv_init(p_CfgInstance, sizeof(stHrvConfig), p_InterfaceVer);

    return stAlgoRet;
}

goodix_hrv_ret goodix_hrv_deinit_func(void)
{
    goodix_hrv_ret nRet = GX_HRV_ALGO_OK;

    nRet = goodix_hrv_deinit();
    if (GX_HRV_ALGO_OK != nRet)
    {
        GH3X2X_DEBUG_ALGO_LOG("hrv init errorcode : 0x%x\r\n", nRet);
    }

    return nRet;
}

GS8 GH3x2xHrvAlgoInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    goodix_hrv_ret ret = goodix_hrv_init_func(pstFrameInfo->pstFunctionInfo->usSampleRate);
    if (GX_HRV_ALGO_OK == ret)
    {
        pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("hrv init error! error code = 0x%x\r\n", ret);
    }
    g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag = 0;
    g_unHrvLastHrResult = 0;

    return chRet;
}

GS8 GH3x2xHrvAlgoDeinit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    if (GX_HRV_ALGO_OK == goodix_hrv_deinit_func())
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("hrv deinit error!\r\n");
    }

    return chRet;
}

GS8 GH3x2xHrvAlgoExe(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    if (0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    GU8 uchGainValue = 0;
    GS32 ppg_rawdata[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint16_t drv_cur[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t gain_code[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t enable_flg[GH_ALGO_EN_FLAG_MAX] = {0};

    goodix_hrv_input_rawdata stRawdata = {0};
    goodix_hrv_result stResult = {0};
    GS8 chAlgoRet = GH3X2X_RET_OK;
    goodix_hrv_input_unique in_unique = {0};

    stRawdata.pst_module_unique = &in_unique;
    if (g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->uchUpdateFlag)
    {
        g_unHrvLastHrResult = g_pstGh3x2xAlgoFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstAlgoRecordResult->snResult[0];
    }
    in_unique.hr = g_unHrvLastHrResult;

    stRawdata.frame_id = GH3X2X_GET_BYTE0_FROM_DWORD(*(pstFrameInfo->punFrameCnt));
    stRawdata.total_ch_num = GH_ALGO_PPG_CHL_NUM;
    stRawdata.enable_flg = enable_flg;
    stRawdata.ppg_rawdata = ppg_rawdata;
    stRawdata.ch_agc_gain = gain_code;
    stRawdata.ch_agc_drv = drv_cur;
    stRawdata.acc_x = pstFrameInfo->pusFrameGsensordata[0];
    stRawdata.acc_y = pstFrameInfo->pusFrameGsensordata[1];
    stRawdata.acc_z = pstFrameInfo->pusFrameGsensordata[2];
    stRawdata.groy_x = 0;
    stRawdata.groy_y = 0;
    stRawdata.groy_z = 0;
    stRawdata.sleep_flg = GH3x2xSleepFlagGet();
    stRawdata.bit_num = GH_ALGO_DATA_BIT_NUM;
    stRawdata.chip_type = GH_ALGO_CHIP_TYPE;
    stRawdata.data_type = GH_ALGO_DATA_TYPE;

    for (GS32 nRawdatacnt = 0; nRawdatacnt < PPG_CHANNEL_NUM; nRawdatacnt++)
    {
        stRawdata.ppg_rawdata[nRawdatacnt] = pstFrameInfo->punFrameRawdata[nRawdatacnt];
        uchGainValue = GH3X2X_GET_LOW_4BITS(pstFrameInfo->punFrameAgcInfo[nRawdatacnt]);
        if ((pstFrameInfo->puchFrameLastGain[nRawdatacnt] != uchGainValue) && \
            (pstFrameInfo->puchFrameLastGain[nRawdatacnt] != GH3X2X_GAIN_VALUE_INVALID))
        {
            pstFrameInfo->puchFrameLastGain[nRawdatacnt] = uchGainValue;
        }

        stRawdata.ch_agc_drv[nRawdatacnt]  = ((pstFrameInfo->punFrameAgcInfo[nRawdatacnt] >> 8) & 0xFF)
                                             + ((pstFrameInfo->punFrameAgcInfo[nRawdatacnt] >> 16) & 0xFF);
        stRawdata.ch_agc_gain[nRawdatacnt]  = uchGainValue;

        stRawdata.enable_flg[nRawdatacnt / GH_ALGO_BYTE_BITS] = enable_flg[nRawdatacnt / GH_ALGO_BYTE_BITS] \
                                                                | (1 << (GH_ALGO_EN_FLAG_OFFSET - (nRawdatacnt % GH_ALGO_BYTE_BITS)));

        GH3X2X_DEBUG_ALGO_LOG("[%s]ppg_rawdata = %d, ch_agc_drv = %d, ch_agc_gain = %d\r\n", __FUNCTION__, \
            (int)stRawdata.ppg_rawdata[nRawdatacnt], (int)stRawdata.ch_agc_drv[nRawdatacnt], \
            (int)stRawdata.ch_agc_gain[nRawdatacnt]);
    }

    goodix_hrv_ret ret = goodix_hrv_calc(&stRawdata, &stResult);
    if (GX_HRV_ALGO_OK == ret || GX_HRV_ALGO_UPDATE == ret)
    {
        if (GX_HRV_ALGO_UPDATE == ret)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = 1;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)stResult.rri[0];
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.rri[1];
            pstFrameInfo->pstAlgoResult->snResult[2] = (GS32)stResult.rri[2];
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.rri[3];
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.rri_confidence;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.rri_valid_num;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            GH3X2X_HrvAlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);

            GH3X2X_INFO_ALGO_LOG("[%s]hrv rri0 = %d, rri1 = %d, conf = %d, num = %d\r\n", __FUNCTION__, \
                (int)stResult.rri[0], (int)stResult.rri[1], stResult.rri_confidence, stResult.rri_valid_num);
        }
    }
    else
    {
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
        GH3X2X_DEBUG_ALGO_LOG("GH3x2xHrAlgoExe error! error code = 0x%x\r\n", ret);
        GH3X2X_DEBUG_ALGO_LOG("please feedback to GOODIX!\r\n");
    }

    return chAlgoRet;
}

#endif

