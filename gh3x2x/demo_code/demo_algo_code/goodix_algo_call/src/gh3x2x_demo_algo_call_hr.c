/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algo_call_hr.c
 *
 * @brief   gh3x2x algo hr interface
 *
 */

#include <stdio.h>
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_hba.h"

#if (__USE_GOODIX_HR_ALGORITHM__)

void Gh3x2xSetHbaMode(GS32 nHbaScenario)
{
    g_unHrParamsArr[1] = nHbaScenario;
}

goodix_hba_ret goodix_hba_init_func(GU32 fs)
{
    goodix_hba_config stHbCfg;
    goodix_hba_ret stAlgoRet = GX_ALGO_HBA_SUCCESS;
    GCHAR uchHrVersion[120] = {0};

    goodix_hba_ret ret = goodix_hba_version((uint8_t *)uchHrVersion);
    if (GX_ALGO_HBA_SUCCESS != ret)
    {
        GH3X2X_DEBUG_ALGO_LOG("hba algorithm init fail : 0x%x\r\n", ret);
        return ret;
    }

    GH3X2X_HbaAlgoChnlMapDefultSet();

    GH3X2X_DEBUG_ALGO_LOG("hba algorithm version : %s\r\n", uchHrVersion);
    GH3X2X_DEBUG_ALGO_LOG("hba algorithm legal chnl num : %d\r\n", g_stHbaAlgoChnlMap.uchNum);

    if (g_stHbaAlgoChnlMap.uchNum > Gh3x2xGetHrAlgoSupportChnl())
    {
        GH3X2X_DEBUG_ALGO_LOG("ERROR: hr algorithm legal chnl num is too big, check cfg !!! \r\n");
        return GX_ALGO_HBA_RWONG_INPUT;
    }

    goodix_hba_config *p_cfg = (goodix_hba_config *)goodix_hba_config_get_arr();
    GH3X2X_Memcpy((void *)&stHbCfg, p_cfg, sizeof(goodix_hba_config));
    stHbCfg.fs = fs;
    stHbCfg.valid_ch_num = g_stHbaAlgoChnlMap.uchNum;

#if (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_BASIC || __GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM)
    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          stHbCfg.mode,
                          stHbCfg.fs,
                          stHbCfg.valid_ch_num,
                          stHbCfg.hba_earliest_output_time,
                          stHbCfg.hba_latest_output_time,
                          stHbCfg.raw_ppg_scale);
#elif (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM)
    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          stHbCfg.mode,
                          stHbCfg.fs,
                          stHbCfg.valid_ch_num,
                          stHbCfg.hba_earliest_output_time,
                          stHbCfg.hba_latest_output_time,
                          stHbCfg.raw_ppg_scale,
                          stHbCfg.valid_score_scale);
#elif (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE)
    GH3X2X_INFO_ALGO_LOG("[%s]:params = %d,%d,%d,%d,%d,%d,%d,%d\r\n", __FUNCTION__,
                          stHbCfg.mode,
                          stHbCfg.fs,
                          stHbCfg.valid_ch_num,
                          stHbCfg.hba_earliest_output_time,
                          stHbCfg.hba_latest_output_time,
                          stHbCfg.raw_ppg_scale,
                          stHbCfg.delay_time,
                          stHbCfg.valid_score_scale);
#endif

    const void *p_CfgInstance = (void *)&stHbCfg;
    char chVer[HBA_INTERFACE_VERSION_LEN_MAX] = {0};

    goodix_hba_config_get_version(chVer, HBA_INTERFACE_VERSION_LEN_MAX);
    const char *p_InterfaceVer = chVer;

    stAlgoRet = goodix_hba_init(p_CfgInstance, sizeof(stHbCfg), p_InterfaceVer);
    GH3X2X_INFO_ALGO_LOG("[%s]:cfg_instance = 0x%x, cfg_size:%d, interface_ver:%s\r\n", __FUNCTION__,
                         p_CfgInstance, sizeof(stHbCfg), p_InterfaceVer);

    return stAlgoRet;
}

goodix_hba_ret goodix_hba_deinit_func(void)
{
    return goodix_hba_deinit();
}

GS8 GH3x2xHrAlgoInit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    goodix_hba_ret ret = goodix_hba_init_func(pstFrameInfo->pstFunctionInfo->usSampleRate);

    if (GX_ALGO_HBA_SUCCESS == ret)
    {
        chRet = GH3X2X_RET_OK;
        GH3X2X_INFO_ALGO_LOG("[%s]GH3X2X_HbaInit success!!\r\n", __FUNCTION__);
    }
    else
    {
        GH3X2X_INFO_ALGO_LOG("hba init error! error code:0x%x\r\n", ret);
    }

    pstFrameInfo->pstAlgoRecordResult->uchUpdateFlag = 0;
    pstFrameInfo->pstAlgoRecordResult->snResult[0] = 0;

    return chRet;
}

GS8 GH3x2xHrAlgoDeinit(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);

    goodix_hba_ret ret = goodix_hba_deinit_func();
    if (GX_ALGO_HBA_SUCCESS == ret)
    {
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_INFO_ALGO_LOG("hba deinit error! error code:0x%x\r\n", ret);
    }

    return chRet;
}

//call algo 2-level interface
GS8 GH3x2xHrAlgoExe(const STGh3x2xFrameInfo *const pstFrameInfo)
{
    if (0 == pstFrameInfo)
    {
        return GH3X2X_RET_GENERIC_ERROR;
    }

    GS32 ppg_rawdata[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint16_t drv_cur[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t gain_code[3 * GH_ALGO_PPG_CHL_NUM] = {0};
    uint8_t enable_flg[GH_ALGO_EN_FLAG_MAX] = {0};

    goodix_hba_input_rawdata stRawdata = {0};
    goodix_hba_result stResult = {0};
    goodix_hba_input_unique in_unique = {0};

    stRawdata.pst_module_unique = &in_unique;
    in_unique.scence = (hba_scenes_e)g_unHrParamsArr[1];


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

    GU8 uchChMapCnt = 0;
    GU8 uchRawdataIndex = 0;
    GS8 chAlgoRet = GH3X2X_RET_OK;

    for (uchChMapCnt = 0; uchChMapCnt < 3 * GH_ALGO_PPG_CHL_NUM; ++uchChMapCnt)
    {
        if (g_stHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)] < 0xff)
        {
            uchRawdataIndex = g_stHbaAlgoChnlMap.uchAlgoChnlMap[uchChMapCnt + 4 * (uchChMapCnt / 4)];

            stRawdata.enable_flg[uchChMapCnt / GH_ALGO_BYTE_BITS] = enable_flg[uchChMapCnt / GH_ALGO_BYTE_BITS]
                                                                    | (1 << (GH_ALGO_EN_FLAG_OFFSET - (uchChMapCnt % GH_ALGO_BYTE_BITS)));

            stRawdata.ppg_rawdata[uchChMapCnt] = pstFrameInfo->punFrameRawdata[uchRawdataIndex];

            GH3X2X_DEBUG_ALGO_LOG("[%s]ppg_rawdata = %d, enable_flg[0] = 0x%x, [1] = 0x%x\r\n", __FUNCTION__, \
                (int)stRawdata.ppg_rawdata[uchChMapCnt], (int)stRawdata.enable_flg[0], (int)stRawdata.enable_flg[1]);
        }
    }

    /* call algorithm, update result */
    goodix_hba_ret ret = goodix_hba_calc(&stRawdata, &stResult);

    if (GX_ALGO_HBA_SUCCESS == ret)
    {
        if (1 == stResult.hba_out_flag)
        {
            pstFrameInfo->pstAlgoResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoResult->snResult[0] = (GS32)stResult.hba_out;
            pstFrameInfo->pstAlgoResult->snResult[1] = (GS32)stResult.valid_score;
            pstFrameInfo->pstAlgoResult->snResult[2] = GH3X2X_ALGORITHM_GF32_2_GS32(stResult.hba_snr);
            pstFrameInfo->pstAlgoResult->snResult[3] = (GS32)stResult.valid_level;
            pstFrameInfo->pstAlgoResult->snResult[4] = (GS32)stResult.hba_acc_info;
            pstFrameInfo->pstAlgoResult->snResult[5] = (GS32)stResult.hba_reg_scence;

            GH3X2X_INFO_ALGO_LOG("[%s]hr = %d,UpdateFlag = %d\r\n", __FUNCTION__, \
                (int)pstFrameInfo->pstAlgoResult->snResult[0], (int)pstFrameInfo->pstAlgoResult->uchUpdateFlag);

            pstFrameInfo->pstAlgoResult->usResultBit = 0x3F;
            pstFrameInfo->pstAlgoResult->uchResultNum = GH3x2x_BitCount(pstFrameInfo->pstAlgoResult->usResultBit);
#if __GH3X2X_HR_OUTPUT_VALUE_STRATEGY_EN__
            Gh3x2xHrOutputValueStrategyPro(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
#endif
            GH3X2X_HrAlgorithmResultReport(pstFrameInfo->pstAlgoResult,pstFrameInfo->punFrameCnt[0]);
            
            pstFrameInfo->pstAlgoRecordResult->uchUpdateFlag = (GU8)stResult.hba_out_flag;
            pstFrameInfo->pstAlgoRecordResult->snResult[0] = (GS32)stResult.hba_out;
        }
    }
    else
    {
        chAlgoRet = GH3X2X_RET_RESOURCE_ERROR;
        GH3X2X_INFO_ALGO_LOG("GH3x2xHrAlgoExe error! error code = 0x%x\r\n", ret);
        GH3X2X_INFO_ALGO_LOG("please feedback to GOODIX!\r\n");
    }

    return chAlgoRet;
}

#endif

