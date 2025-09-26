/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh2x2x_demo_algo_frame.c
 * 
 * @brief   gh3x2x algo frame info
 * 
 * @author  Gooidx Iot Team
 * 
 */

/* includes */
#include "gh3x2x_demo_algo_config.h"
#include "gh_demo_config.h"
#include "gh_drv.h"


#if __GH_SEPARATE_ALGO_EN__

#if __FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
#if __USE_GOODIX_HR_ALGORITHM__
GU32 g_pun_HR_FrameIncompleteRawdata[GH3X2X_HR_CHNL_NUM];
GU32 g_un_HR_FrameIncompleteChnlMapBit;
GU8 g_puch_HR_FrameLastGain[GH3X2X_HR_CHNL_NUM];
GU32 g_un_HR_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_HR;
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
GU32 g_pun_HRV_FrameIncompleteRawdata[GH3X2X_HRV_CHNL_NUM];
GU32 g_un_HRV_FrameIncompleteChnlMapBit;
GU8 g_puch_HRV_FrameLastGain[GH3X2X_HRV_CHNL_NUM];
GU32 g_un_HRV_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_HRV;
#endif
#if __USE_GOODIX_HSM_ALGORITHM__
GU32 g_pun_HSM_FrameIncompleteRawdata[GH3X2X_HSM_CHNL_NUM];
GU32 g_un_HSM_FrameIncompleteChnlMapBit;
GU8 g_puch_HSM_FrameLastGain[GH3X2X_HSM_CHNL_NUM];
GU32 g_un_HSM_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_HSM;
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
GU32 g_pun_SPO2_FrameIncompleteRawdata[GH3X2X_SPO2_CHNL_NUM];
GU32 g_un_SPO2_FrameIncompleteChnlMapBit;
GU8 g_puch_SPO2_FrameLastGain[GH3X2X_SPO2_CHNL_NUM];
GU32 g_un_SPO2_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_SPO2;
#endif
#if __USE_GOODIX_ECG_ALGORITHM__
GU32 g_pun_ECG_FrameIncompleteRawdata[GH3X2X_ECG_CHNL_NUM];
GU32 g_un_ECG_FrameIncompleteChnlMapBit;
GU8 g_puch_ECG_FrameLastGain[GH3X2X_ECG_CHNL_NUM];
GU32 g_un_ECG_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_ECG;
#endif
#if 0
GU32 g_pun_PWTT_FrameIncompleteRawdata[GH3X2X_PWTT_CHNL_NUM];
GU32 g_un_PWTT_FrameIncompleteChnlMapBit;
GU8 g_puch_PWTT_FrameLastGain[GH3X2X_PWTT_CHNL_NUM];
GU32 g_un_PWTT_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_PWTT;
#endif
#if __USE_GOODIX_BT_ALGORITHM__
GU32 g_pun_BT_FrameIncompleteRawdata[GH3X2X_BT_CHNL_NUM];
GU32 g_un_BT_FrameIncompleteChnlMapBit;
GU8 g_puch_BT_FrameLastGain[GH3X2X_BT_CHNL_NUM];
GU32 g_un_BT_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_BT;
#endif
#if __USE_GOODIX_RESP_ALGORITHM__
GU32 g_pun_RESP_FrameIncompleteRawdata[GH3X2X_RESP_CHNL_NUM];
GU32 g_un_RESP_FrameIncompleteChnlMapBit;
GU8 g_puch_RESP_FrameLastGain[GH3X2X_RESP_CHNL_NUM];
GU32 g_un_RESP_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_RESP;
#endif
#if __USE_GOODIX_AF_ALGORITHM__
GU32 g_pun_AF_FrameIncompleteRawdata[GH3X2X_AF_CHNL_NUM];
GU32 g_un_AF_FrameIncompleteChnlMapBit;
GU8 g_puch_AF_FrameLastGain[GH3X2X_AF_CHNL_NUM];
GU32 g_un_AF_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_AF;
#endif
#if __USE_GOODIX_BP_ALGORITHM__
GU32 g_pun_BP_FrameIncompleteRawdata[GH3X2X_BP_CHNL_NUM];
GU32 g_un_BP_FrameIncompleteChnlMapBit;
GU8 g_puch_BP_FrameLastGain[GH3X2X_BP_CHNL_NUM];
GU32 g_un_BP_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_BP;
#endif
#if 0
GU32 g_pun_TEST1_FrameIncompleteRawdata[GH3X2X_TEST_CHNL_NUM];
GU32 g_un_TEST1_FrameIncompleteChnlMapBit;
GU8 g_puch_TEST1_FrameLastGain[GH3X2X_TEST_CHNL_NUM];
GU32 g_un_TEST1_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_TEST1;
GU32 g_pun_TEST2_FrameIncompleteRawdata[GH3X2X_TEST_CHNL_NUM];
GU32 g_un_TEST2_FrameIncompleteChnlMapBit;
GU8 g_puch_TEST2_FrameLastGain[GH3X2X_TEST_CHNL_NUM];
GU32 g_un_TEST2_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_TEST2;
#endif
#else
GU32 g_pun_MAIN_FUNC_FrameIncompleteRawdata[GH3X2X_MAIN_FUNC_CHNL_NUM];
GU32 g_un_MAIN_FUNC_FrameIncompleteChnlMapBit;
GU8 g_puch_MAIN_FUNC_FrameLastGain[GH3X2X_MAIN_FUNC_CHNL_NUM];
GU32 g_un_MAIN_FUNC_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_MAIN_FUNC;
#endif
#if  __USE_GOODIX_ADT_ALGORITHM__
GU32 g_pun_ADT_FrameIncompleteRawdata[GH3X2X_ADT_CHNL_NUM];
GU32 g_un_ADT_FrameIncompleteChnlMapBit;
GU8 g_puch_ADT_FrameLastGain[GH3X2X_ADT_CHNL_NUM];
GU32 g_un_ADT_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_ADT;
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
GU32 g_pun_SOFT_ADT_GREEN_FrameIncompleteRawdata[GH3X2X_SOFT_ADT_CHNL_NUM];
GU32 g_un_SOFT_ADT_GREEN_FrameIncompleteChnlMapBit;
GU8 g_puch_SOFT_ADT_GREEN_FrameLastGain[GH3X2X_SOFT_ADT_CHNL_NUM];
GU32 g_un_SOFT_ADT_GREEN_TimeStamp;
STGh3x2xDownSampleInfo g_stGh3x2xDownSampleInfo_SOFT_ADT_GREEN;
#endif




#if __USE_GOODIX_HR_ALGORITHM__
GU8 g_pch_HR_ChnlMap[GH3X2X_HR_CHNL_NUM];
STGh3x2xFunctionInfo g_st_HR_FuncitonInfo;
STGh3x2xAlgoResult g_st_HR_AlgoRecordResult;
#endif
#if __USE_GOODIX_HRV_ALGORITHM__
GU8 g_pch_HRV_ChnlMap[GH3X2X_HRV_CHNL_NUM];
STGh3x2xFunctionInfo g_st_HRV_FuncitonInfo;
STGh3x2xAlgoResult g_st_HRV_AlgoRecordResult;
#endif
#if __USE_GOODIX_HSM_ALGORITHM__
GU8 g_pch_HSM_ChnlMap[GH3X2X_HSM_CHNL_NUM];
STGh3x2xFunctionInfo g_st_HSM_FuncitonInfo;
#endif
#if __USE_GOODIX_SPO2_ALGORITHM__
GU8 g_pch_SPO2_ChnlMap[GH3X2X_SPO2_CHNL_NUM];
STGh3x2xFunctionInfo g_st_SPO2_FuncitonInfo;
STGh3x2xAlgoResult g_st_SPO2_AlgoRecordResult;
#endif
#if __USE_GOODIX_ECG_ALGORITHM__
GU8 g_pch_ECG_ChnlMap[GH3X2X_ECG_CHNL_NUM];
STGh3x2xFunctionInfo g_st_ECG_FuncitonInfo;
#endif
#if 0
GU8 g_pch_PWTT_ChnlMap[GH3X2X_PWTT_CHNL_NUM];
STGh3x2xFunctionInfo g_st_PWTT_FuncitonInfo;
#endif
#if __USE_GOODIX_BT_ALGORITHM__
GU8 g_pch_BT_ChnlMap[GH3X2X_BT_CHNL_NUM];
STGh3x2xFunctionInfo g_st_BT_FuncitonInfo;
#endif
#if __USE_GOODIX_RESP_ALGORITHM__
GU8 g_pch_RESP_ChnlMap[GH3X2X_RESP_CHNL_NUM];
STGh3x2xFunctionInfo g_st_RESP_FuncitonInfo;
#endif
#if __USE_GOODIX_AF_ALGORITHM__
GU8 g_pch_AF_ChnlMap[GH3X2X_AF_CHNL_NUM];
STGh3x2xFunctionInfo g_st_AF_FuncitonInfo;
#endif
#if __USE_GOODIX_ADT_ALGORITHM__
GU8 g_pch_ADT_ChnlMap[GH3X2X_ADT_CHNL_NUM];
STGh3x2xFunctionInfo g_st_ADT_FuncitonInfo;
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
GU8 g_pch_SOFT_ADT_GREEN_ChnlMap[GH3X2X_SOFT_ADT_CHNL_NUM];
STGh3x2xFunctionInfo g_st_SOFT_ADT_GREEN_FuncitonInfo;
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
GU8 g_pch_SOFT_ADT_IR_ChnlMap[GH3X2X_SOFT_ADT_CHNL_NUM];
STGh3x2xFunctionInfo g_st_SOFT_ADT_IR_FuncitonInfo;
#endif
#if __FUNC_TYPE_BP_ENABLE__
GU8 g_pch_BP_ChnlMap[GH3X2X_BP_CHNL_NUM];
STGh3x2xFunctionInfo g_st_BP_FuncitonInfo;
#endif
#if 0
GU8 g_pch_TEST1_ChnlMap[GH3X2X_TEST_CHNL_NUM];
STGh3x2xFunctionInfo g_st_TEST1_FuncitonInfo;
GU8 g_pch_TEST2_ChnlMap[GH3X2X_TEST_CHNL_NUM];
STGh3x2xFunctionInfo g_st_TEST2_FuncitonInfo;
#endif

GU32 g_punGh3x2xFrameRawdata[GH3X2X_FUNC_CHNL_NUM_MAX];
#if __GS_GYRO_ENABLE__
GS16 g_psGh3x2xFrameGsensorData[6];
#else
GS16 g_psGh3x2xFrameGsensorData[3];
#endif
GU32 g_punGh3x2xFrameAgcInfo[GH3X2X_FUNC_CHNL_NUM_MAX];
GU32 g_punGh3x2xFrameFlag[8];
STGh3x2xAlgoResult g_stGh3x2xAlgoResult;

STCapRawdata g_pstGh3x2xFrameCapData;
STTempRawdata g_pstGh3x2xFrameTempData;



//HR data info
#if __USE_GOODIX_HR_ALGORITHM__
const STGh3x2xFrameInfo g_stHR_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_HR,
    .pstFunctionInfo = &g_st_HR_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_HR_CHNL_NUM,
    .pchChnlMap = g_pch_HR_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = &g_st_HR_AlgoRecordResult,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_HR_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_HR_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_HR_FrameLastGain,
    .punFrameCnt = &g_un_HR_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_HR,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHR_FrameInfo = &g_stHR_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHR_FrameInfo = 0;
#endif
//HRV data info
#if __USE_GOODIX_HRV_ALGORITHM__
const STGh3x2xFrameInfo g_stHRV_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_HRV,
    .pstFunctionInfo = &g_st_HRV_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_HRV_CHNL_NUM,
    .pchChnlMap = g_pch_HRV_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = &g_st_HRV_AlgoRecordResult,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_HRV_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_HRV_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_HRV_FrameLastGain,
    .punFrameCnt = &g_un_HRV_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_HRV,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHRV_FrameInfo = &g_stHRV_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHRV_FrameInfo = 0;
#endif
//HSM data info
#if __USE_GOODIX_HSM_ALGORITHM__
const STGh3x2xFrameInfo g_stHSM_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_HSM,
    .pstFunctionInfo = &g_st_HSM_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_HSM_CHNL_NUM,
    .pchChnlMap = g_pch_HSM_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_HSM_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_HSM_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_HSM_FrameLastGain,
    .punFrameCnt = &g_un_HSM_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_HSM,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHSM_FrameInfo = &g_stHSM_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xHSM_FrameInfo = 0;
#endif
//SPO2 data info
#if __USE_GOODIX_SPO2_ALGORITHM__
const STGh3x2xFrameInfo g_stSPO2_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_SPO2,
    .pstFunctionInfo = &g_st_SPO2_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_SPO2_CHNL_NUM,
    .pchChnlMap = g_pch_SPO2_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = &g_st_SPO2_AlgoRecordResult,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_SPO2_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_SPO2_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_SPO2_FrameLastGain,
    .punFrameCnt = &g_un_SPO2_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_SPO2,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSPO2_FrameInfo = &g_stSPO2_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSPO2_FrameInfo = 0;
#endif
//ECG data info
#if __USE_GOODIX_ECG_ALGORITHM__
const STGh3x2xFrameInfo g_stECG_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_ECG,
    .pstFunctionInfo = &g_st_ECG_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_ECG_CHNL_NUM,
    .pchChnlMap = g_pch_ECG_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_ECG_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_ECG_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_ECG_FrameLastGain,
    .punFrameCnt = &g_un_ECG_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_ECG,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xECG_FrameInfo = &g_stECG_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xECG_FrameInfo = 0;
#endif
#if 0
const STGh3x2xFrameInfo g_stPWTT_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_PWTT,
    .pstFunctionInfo = &g_st_PWTT_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_PWTT_CHNL_NUM,
    .pchChnlMap = g_pch_PWTT_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_PWTT_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_PWTT_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_PWTT_FrameLastGain,
    .punFrameCnt = &g_un_PWTT_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_PWTT,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xECG_FrameInfo = &g_stECG_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xECG_FrameInfo = 0;
#endif
//BT data info
#if __USE_GOODIX_BT_ALGORITHM__
const STGh3x2xFrameInfo g_stBT_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_BT,
    .pstFunctionInfo = &g_st_BT_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_BT_CHNL_NUM,
    .pchChnlMap = g_pch_BT_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_BT_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_BT_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_BT_FrameLastGain,
    .punFrameCnt = &g_un_BT_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_BT,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xBT_FrameInfo = &g_stBT_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xBT_FrameInfo = 0;
#endif
//RESP data info
#if __USE_GOODIX_RESP_ALGORITHM__
const STGh3x2xFrameInfo g_stRESP_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_RESP,
    .pstFunctionInfo = &g_st_RESP_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_RESP_CHNL_NUM,
    .pchChnlMap = g_pch_RESP_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_RESP_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_RESP_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_RESP_FrameLastGain,
    .punFrameCnt = &g_un_RESP_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_RESP,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xRESP_FrameInfo = &g_stRESP_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xRESP_FrameInfo = 0;
#endif
//AF data info
#if  __USE_GOODIX_AF_ALGORITHM__
const STGh3x2xFrameInfo g_stAF_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_AF,
    .pstFunctionInfo = &g_st_AF_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_AF_CHNL_NUM,
    .pchChnlMap = g_pch_AF_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_AF_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_AF_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_AF_FrameLastGain,
    .punFrameCnt = &g_un_AF_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_AF,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xAF_FrameInfo = &g_stAF_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xAF_FrameInfo = 0;
#endif
#if __USE_GOODIX_BP_ALGORITHM__
const STGh3x2xFrameInfo g_stFPBP_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_FPBP,
    .pstFunctionInfo = &g_st_BP_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_BP_CHNL_NUM,
    .pchChnlMap = g_pch_BP_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_BP_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_BP_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_BP_FrameLastGain,
    .punFrameCnt = &g_un_BP_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_BP,
#endif
};
const STGh3x2xFrameInfo g_stPWA_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_PWA,
    .pstFunctionInfo = &g_st_BP_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_PWA_CHNL_NUM,
    .pchChnlMap = g_pch_BP_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_BP_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_BP_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_BP_FrameLastGain,
    .punFrameCnt = &g_un_BP_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_BP,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xFPBP_FrameInfo = &g_stFPBP_FrameInfo;
//const STGh3x2xFrameInfo * const  g_pstGh3x2xPWA_FrameInfo = &g_stPWA_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xFPBP_FrameInfo = 0;
//const STGh3x2xFrameInfo * const  g_pstGh3x2xPWA_FrameInfo = 0;
#endif
#if 0
const STGh3x2xFrameInfo g_stTEST1_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_TEST1,
    .pstFunctionInfo = &g_st_TEST1_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_TEST_CHNL_NUM,
    .pchChnlMap = g_pch_TEST1_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_TEST1_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_TEST1_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_TEST1_FrameLastGain,
    .punFrameCnt = &g_un_TEST1_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_TEST1,
#endif
};
const STGh3x2xFrameInfo g_stTEST2_FrameInfo =
{
    .unFunctionID = GH3X2X_FUNCTION_TEST2,
    .pstFunctionInfo = &g_st_TEST2_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_TEST_CHNL_NUM,
    .pchChnlMap = g_pch_TEST2_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .pstAlgoRecordResult = 0,
#if !__FUNC_RUN_SIMULTANEOUSLY_SUPPORT__
    .punIncompleteRawdata = g_pun_MAIN_FUNC_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_MAIN_FUNC_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_MAIN_FUNC_FrameLastGain,
    .punFrameCnt = &g_un_MAIN_FUNC_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_MAIN_FUNC,
#else
    .punIncompleteRawdata = g_pun_TEST2_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_TEST2_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_TEST2_FrameLastGain,
    .punFrameCnt = &g_un_TEST2_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_TEST2,
#endif
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xTEST1_FrameInfo = &g_stTEST1_FrameInfo;
//const STGh3x2xFrameInfo * const  g_pstGh3x2xTEST2_FrameInfo = &g_stTEST2_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xTEST1_FrameInfo = 0;
//const STGh3x2xFrameInfo * const  g_pstGh3x2xTEST2_FrameInfo = 0;
#endif
//ADT data info
#if __USE_GOODIX_ADT_ALGORITHM__
const STGh3x2xFrameInfo g_stADT_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_ADT,
    .pstFunctionInfo = &g_st_ADT_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_ADT_CHNL_NUM,
    .pchChnlMap = g_pch_ADT_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .punIncompleteRawdata = g_pun_ADT_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_ADT_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_ADT_FrameLastGain,
    .punFrameCnt = &g_un_ADT_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_ADT,
    .pstAlgoRecordResult = 0,
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xADT_FrameInfo = &g_stADT_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xADT_FrameInfo = 0;
#endif
//SOFT ADT data info
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
const STGh3x2xFrameInfo g_stSOFT_ADT_GREEN_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_SOFT_ADT_GREEN,
    .pstFunctionInfo = &g_st_SOFT_ADT_GREEN_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_SOFT_ADT_CHNL_NUM,
    .pchChnlMap = g_pch_SOFT_ADT_GREEN_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .punIncompleteRawdata = g_pun_SOFT_ADT_GREEN_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_SOFT_ADT_GREEN_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_SOFT_ADT_GREEN_FrameLastGain,
    .punFrameCnt = &g_un_SOFT_ADT_GREEN_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_SOFT_ADT_GREEN,
    .pstAlgoRecordResult = 0,
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSOFT_ADT_GREEN_FrameInfo = &g_stSOFT_ADT_GREEN_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSOFT_ADT_GREEN_FrameInfo = 0;
#endif
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
const STGh3x2xFrameInfo g_stSOFT_ADT_IR_FrameInfo = 
{
    .unFunctionID = GH3X2X_FUNCTION_SOFT_ADT_IR,
    .pstFunctionInfo = &g_st_SOFT_ADT_IR_FuncitonInfo,
    .pstAlgoResult   = &g_stGh3x2xAlgoResult,
    .uchFuntionChnlLimit = GH3X2X_SOFT_ADT_CHNL_NUM,
    .pchChnlMap = g_pch_SOFT_ADT_IR_ChnlMap,
    .punFrameRawdata = g_punGh3x2xFrameRawdata,
    .pusFrameGsensordata = g_psGh3x2xFrameGsensorData,
    .pstFrameCapdata = &g_pstGh3x2xFrameCapData,
    .pstFrameTempdata = &g_pstGh3x2xFrameTempData,
    .punFrameAgcInfo = g_punGh3x2xFrameAgcInfo,
    .punFrameFlag = g_punGh3x2xFrameFlag,
    .punIncompleteRawdata = g_pun_SOFT_ADT_GREEN_FrameIncompleteRawdata,
    .punIncompleteChnlMapBit = &g_un_SOFT_ADT_GREEN_FrameIncompleteChnlMapBit,
    .puchFrameLastGain = g_puch_SOFT_ADT_GREEN_FrameLastGain,
    .punFrameCnt = &g_un_SOFT_ADT_GREEN_TimeStamp,
    .pstDownSampleInfo = &g_stGh3x2xDownSampleInfo_SOFT_ADT_GREEN,
    .pstAlgoRecordResult = 0,
};
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSOFT_ADT_IR_FrameInfo = &g_stSOFT_ADT_IR_FrameInfo;
#else
//const STGh3x2xFrameInfo * const  g_pstGh3x2xSOFT_ADT_IR_FrameInfo = 0;
#endif


//Function data info struct
const STGh3x2xFrameInfo * const  g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_MAX] = 
{
//adt
#if __USE_GOODIX_ADT_ALGORITHM__
    &g_stADT_FrameInfo,
#else
    0,
#endif
//hr
#if __USE_GOODIX_HR_ALGORITHM__
    &g_stHR_FrameInfo,
#else
    0,
#endif
//hrv
#if __USE_GOODIX_HRV_ALGORITHM__
    &g_stHRV_FrameInfo,
#else
    0,
#endif
//hsm
#if __USE_GOODIX_HSM_ALGORITHM__
    &g_stHSM_FrameInfo,
#else
    0,
#endif
//fpbp
//pwa
#if __USE_GOODIX_BP_ALGORITHM__
    &g_stFPBP_FrameInfo,
    &g_stPWA_FrameInfo,
#else
    0,
    0,
#endif   
//spo2
#if __USE_GOODIX_SPO2_ALGORITHM__
    &g_stSPO2_FrameInfo,
#else
    0,
#endif
//ecg
#if __USE_GOODIX_ECG_ALGORITHM__
    &g_stECG_FrameInfo,
#else
    0,
#endif
//pwtt
#if 0
    &g_stPWTT_FrameInfo,
#else
    0,
#endif
//soft adt green
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    &g_stSOFT_ADT_GREEN_FrameInfo,
#else
    0,
#endif
//bt
#if __USE_GOODIX_BT_ALGORITHM__
    &g_stBT_FrameInfo,
#else
    0,
#endif
//resp
#if __USE_GOODIX_RESP_ALGORITHM__
    &g_stRESP_FrameInfo,
#else
    0,
#endif
//af
#if  __USE_GOODIX_AF_ALGORITHM__
    &g_stAF_FrameInfo,
#else
    0,
#endif
#if 0
    &g_stTEST1_FrameInfo,
    &g_stTEST2_FrameInfo,
#else
    0,
    0,
#endif
//soft adt ir
#if __USE_GOODIX_SOFT_ADT_ALGORITHM__
    &g_stSOFT_ADT_IR_FrameInfo,
#else
    0,
#endif
    0, //GH3X2X_FUNCTION_RS0
    0, //GH3X2X_FUNCTION_RS1
    0, //GH3X2X_FUNCTION_RS2
    0, //GH3X2X_FUNCTION_LEAD_DET
};



#endif


/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
