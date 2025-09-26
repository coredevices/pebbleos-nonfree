#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_algo.h"
#include "goodix_spo2.h"

#if (__USE_GOODIX_SPO2_ALGORITHM__)

goodix_spo2_ret goodix_spo2_init_func(GU32 fs)
{
    goodix_spo2_ret stAlgoRet = GX_ALGO_SPO2_SUCCESS;
    goodix_spo2_config stSpo2Cfg = {0};
    GCHAR uchSpo2Version[150] = {0};

    GH3X2X_Spo2AlgoChnlMapDefultSet();
    goodix_spo2_version((uint8_t *)uchSpo2Version);
    GH3X2X_DEBUG_ALGO_LOG("spo2 algorithm version : %s\r\n", uchSpo2Version);
    GH3X2X_DEBUG_ALGO_LOG("spo2 algorithm legal chnl num : %d\r\n", g_stSpo2AlgoChnlMap.uchNum);

    if (g_stSpo2AlgoChnlMap.uchNum > Gh3x2xGetSpo2AlgoSupportChnl())
    {
        GH3X2X_DEBUG_ALGO_LOG("ERROR: spo2 algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_SPO2_RWONG_INPUT;
    }

    goodix_spo2_config *p_cfg = (goodix_spo2_config *)goodix_spo2_config_get_instance();
    GH3X2X_Memcpy((void *)&stSpo2Cfg, p_cfg, sizeof(goodix_spo2_config));
    stSpo2Cfg.raw_fs = fs;
    stSpo2Cfg.valid_chl_num = g_stSpo2AlgoChnlMap.uchNum;

    GH3X2X_DEBUG_ALGO_LOG("[%s]: params1 = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          stSpo2Cfg.valid_chl_num,
                          stSpo2Cfg.raw_fs,
                          stSpo2Cfg.cali_coef_a4,
                          stSpo2Cfg.cali_coef_a3,
                          stSpo2Cfg.cali_coef_a2,
                          stSpo2Cfg.cali_coef_a1,
                          stSpo2Cfg.cali_coef_a0,
                          stSpo2Cfg.hb_en_flg,
                          stSpo2Cfg.wear_mode,
                          stSpo2Cfg.acc_thr_max,
                          stSpo2Cfg.acc_thr_min,
                          stSpo2Cfg.acc_thr_scale,
                          stSpo2Cfg.acc_thr_num,
                          stSpo2Cfg.acc_thr_angle,
                          stSpo2Cfg.ctr_en_flg,
                          stSpo2Cfg.ctr_red_thr);

    GH3X2X_DEBUG_ALGO_LOG("[%s]: params2 = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          stSpo2Cfg.ppg_jitter_thr,
                          stSpo2Cfg.ppg_noise_thr,
                          stSpo2Cfg.ppg_coeff_thr,
                          stSpo2Cfg.quality_module_key,
                          stSpo2Cfg.low_spo2_thr,
                          stSpo2Cfg.fast_out_time,
                          stSpo2Cfg.slow_out_time,
                          stSpo2Cfg.min_stable_time_high,
                          stSpo2Cfg.min_stable_time_low,
                          stSpo2Cfg.max_spo2_variation_high,
                          stSpo2Cfg.max_spo2_variation_low,
                          stSpo2Cfg.ouput_module_key);

    const void *p_CfgInstance = (void *)&stSpo2Cfg;
    char chVer[SPO2_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_spo2_config_get_version(chVer, SPO2_INTERFACE_VERSION_LEN_MAX);

    stAlgoRet = goodix_spo2_init(p_CfgInstance, sizeof(stSpo2Cfg), chVer);

    return stAlgoRet;
}

goodix_spo2_ret goodix_spo2_deinit_func(void)
{
    return goodix_spo2_deinit();
}

GS8 GH3x2xSpo2AlgoInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    goodix_spo2_ret ret = goodix_spo2_init_func(pstFrameInfo->pstFunctionInfo->usSampleRate);
    if (GX_ALGO_SPO2_SUCCESS == ret)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("spo2 init error! error code:0x%x\r\n", ret);
    }

    return chRet;
}

GS8 GH3x2xSpo2AlgoDeinit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    goodix_spo2_ret ret = goodix_spo2_deinit_func();
    if (GX_ALGO_SPO2_SUCCESS == ret)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("spo2 deinit error! error code:0x%x\r\n", ret);
    }

    return chRet;
}

GS8 GH3x2xSpo2AlgoExe(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    if (0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    GS32 ppg_rawdata[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint16_t drv_cur[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t gain_code[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t enable_flg[GH_ALGO_EN_FLAG_MAX] = {0};

    goodix_spo2_input_rawdata stRawdata = {0};
    goodix_spo2_result stResult = {0};
    GS8 emAlgoRet = GH3X2X_RET_OK;

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
    stRawdata.pst_module_unique = 0;

    /* call algorithm, update result */
    for (GU8 uchChMapCnt = 0; uchChMapCnt < GH_ALGO_PPG_CHL_NUM * 3; ++uchChMapCnt)
    {
        if (g_stSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff)
        {
            GU8 uchRawdataIndex = g_stSpo2AlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            stRawdata.ch_agc_drv[uchChMapCnt]  = ((pstFrameInfo->punFrameAgcInfo[uchRawdataIndex] >> 8) & 0xFF)
                                                 + ((pstFrameInfo->punFrameAgcInfo[uchRawdataIndex] >> 16) & 0xFF);
            stRawdata.ch_agc_gain[uchChMapCnt]  = (pstFrameInfo->punFrameAgcInfo[uchRawdataIndex] & 0xF);

            stRawdata.enable_flg[uchChMapCnt / GH_ALGO_BYTE_BITS] = enable_flg[uchChMapCnt / GH_ALGO_BYTE_BITS] \
                                                                    | (1 << (GH_ALGO_EN_FLAG_OFFSET - (uchChMapCnt % GH_ALGO_BYTE_BITS)));

            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->punFrameRawdata[uchRawdataIndex];

            GH3X2X_DEBUG_ALGO_LOG("[%s] ch%d ppg_rawdata = %d, ch_agc_drv = %d, ch_agc_gain = %d\r\n", __FUNCTION__, uchChMapCnt, \
                (int)stRawdata.ppg_rawdata[uchChMapCnt], stRawdata.ch_agc_drv[uchChMapCnt], stRawdata.ch_agc_gain[uchChMapCnt]);
        }
    }

    goodix_spo2_ret ret = goodix_spo2_calc(&stRawdata, &stResult);
    // goodix_spo2_essential_info_print(&stRawdata);

    if (GX_ALGO_SPO2_SUCCESS == ret || GX_ALGO_SPO2_FRAME_UNCOMPLETE == ret || GX_ALGO_SPO2_WIN_UNCOMPLETE == ret)
    {
        if (1 == stResult.final_calc_flg)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.final_calc_flg;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)(GH3x2x_Round((float)stResult.final_spo2 / 10000));
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.final_r_val;
            pstFrameInfo->pstAlgoResult->snResult[2] = (GS32)stResult.final_confi_coeff;
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.final_valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.final_hb_mean;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.final_invalidFlg;
            pstFrameInfo->pstAlgoResult->usResultBit = 0x7F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
            GH3X2X_Spo2AlgorithmResultReport(pstFrameInfo->pstAlgoResult, pstFrameInfo->punFrameCnt[0]);
            GH3X2X_DEBUG_ALGO_LOG("[%s] final_spo2:%d, r = %d, final_valid_level:%d\r\n", __FUNCTION__, pstFrameInfo->pstAlgoResult->snResult[0], \
                pstFrameInfo->pstAlgoResult->snResult[1], pstFrameInfo->pstAlgoResult->snResult[3]);
        }
    }
    else
    {
        emAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
        GH3X2X_DEBUG_ALGO_LOG("GH3x2xSpo2AlgoExe error! error code = 0x%x\r\n", ret);
        GH3X2X_DEBUG_ALGO_LOG("please feedback to GOODIX!\r\n");
    }

    return (GS8)emAlgoRet;
}

#endif
