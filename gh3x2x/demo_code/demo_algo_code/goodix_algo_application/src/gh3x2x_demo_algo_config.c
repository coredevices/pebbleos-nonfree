/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_algorithm_calc.c
 *
 * @brief   gh3x2x algorithm calc functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh3x2x_demo_algorithm_calc.h"
#include "gh3x2x_demo_algo_config.h"
#include "goodix_algo.h"
#include "goodix_mem.h"
#include "goodix_type.h"
#include "goodix_hba.h"
#include "goodix_hrv.h"
#include "goodix_spo2.h"
#include "goodix_nadt.h"
#include "goodix_algo_bt.h"

#if (__GOODIX_ALGO_CALL_MODE__)

/* hb algorithm */
#if (__USE_GOODIX_HR_ALGORITHM__)
GS32 g_unHrParamsArr[GOODIX_HR_PARAMS_NUM_MAX] = {
                                                        0,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        20,                                 // 预留未使用
                                                        8,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        0,                                  // 预留未使用
                                                        0                                   // 预留未使用
                                                 };
/// hbd rawdata channel map
STAlgoChnlMap g_stHbaAlgoChnlMap = {0};
#endif

/* hrv algorithm */
#if (__USE_GOODIX_HRV_ALGORITHM__)
GS32 g_nHrvParamsArr[GOODIX_HRV_PARAMS_NUM_MAX] = {
                                                        0,                                  // 需要分辨率为1ms的输出
                                                        200000, 100000, 300000, 300000      // ACC相关阈值
                                                  };
#endif

/* spo2 algorithm */
#if (__USE_GOODIX_SPO2_ALGORITHM__)
#if ((__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_BASIC) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM))
GS32 g_nSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX] = {
                                                          0, 0, -134717, -115284, 1097668,  // 校准参数
                                                          0,                                // 心率使能标志
                                                          0,                                // 佩戴模式
                                                          30, 15, 4, 3, 90,                 // ACC运动判断阈值
                                                          1, 65,                            // 备用
                                                          30, 10, 90,                       // PPG参数
                                                          253,                              // 质量评估模块使能开关
                                                          95,                               // 低氧阈值
                                                          10, 25, 3, 6, 10, 5,              // 出值策略
                                                          3                                 // 出值相关模块使能开关
                                                     };
#elif ((__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE))
GS32 g_nSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX] = {
                                                          0, 0, 0, -239878, 1111827,   // 校准参数
                                                          1,                                // 心率使能标志
                                                          0,                                // 佩戴模式
                                                          50, 25, 3, 3, 90,                 // ACC运动判断阈值
                                                          0, 160,                            // 备用
                                                          35, 25, 85,                       // PPG参数
                                                          0xFD,                             // 质量评估模块使能开关
                                                          95,                               // 低氧阈值
                                                          12, 18, 2, 7, 10, 5,              // 出值策略
                                                          3                                 // 出值相关模块使能开关
                                                     };
#endif
/// spo2 rawdata channel map
STAlgoChnlMap g_stSpo2AlgoChnlMap = {0};
#endif

/* ecg algorithm */
#if (__USE_GOODIX_ECG_ALGORITHM__)
GS32 g_nEcgParamsArr[GOODIX_ECG_PARAMS_NUM_MAX] = {
                                                         0,                                 // 采样率
                                                         1,                                 // 算法有效通道数
                                                         1,                                 // QRS检测标志
                                                         0,                                 // ECG耦合方式
                                                         0                                  // 芯片类型
                                                   };
/// ecg frame data cnt
GU32 g_unEcgFrameCnt = 0;
GU8 g_uchClassificationFlag;
GU8 g_unEcgTimeSeconds = 0;
#endif

/* bt algorithm */
#if (__USE_GOODIX_BT_ALGORITHM__)
GS32 g_nBtParamsArr[GOODIX_BT_PARAMS_NUM_MAX] = {0};
#endif

/* af algorithm */
#if (__USE_GOODIX_AF_ALGORITHM__)
GS32 g_nAfParamsArr[GOODIX_AF_PARAMS_NUM_MAX] = {45, 0};
#endif

/// new data flag
GU8 g_uchNewDataFlag = 0;
GU8 g_uchGh3x2xSleepFlag = 0;

//sensor enable
GU8 g_uchAlgoGsensorEnable = 0;
GU8 g_uchAlgoCapEnable = 0;
GU8 g_uchAlgoTempEnable = 0;

GS32 GH3x2x_Round(GF32 fValue)
{
    GS32 nRet = 0;
    float fEpsinon = 0.00001;
    if((fValue >= -fEpsinon)&&(fValue <= fEpsinon))
    {
        nRet = 0;
    }
    else if(fValue < -fEpsinon)
    {
        nRet = (GS32)(fValue - 0.5f);
    }
    else if(fValue > fEpsinon)
    {
        nRet = (GS32)(fValue + 0.5f);
    }
    return nRet;
}

GU8 GH3x2x_BitCount(GU32 n)
{
    GU8 c =0 ; // 计数器
    while (n >0)
    {
        if((n &1) ==1) // 当前位是1
            ++c ; // 计数器加1
        n >>=1 ; // 移位
    }
    return c ;
}

void GH3x2xSleepFlagSet(GU8 uchSleepFlag)
{
    g_uchGh3x2xSleepFlag = uchSleepFlag;
}


GU8 GH3x2xSleepFlagGet(void)
{
    return g_uchGh3x2xSleepFlag;
}

void GH3X2X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,
                            GU8 uchAlgoCapEnable,
                            GU8 uchAlgoTempEnable)
{
    g_uchAlgoGsensorEnable = uchAlgoGsensorEnable;
    g_uchAlgoCapEnable = uchAlgoCapEnable;
    g_uchAlgoTempEnable = uchAlgoTempEnable;
}

/**
 * @fn     GS8 GH3X2X_BTAlgorithmRTConfig(GS32 nOutputTime)
 *
 * @brief  BT algorithm RT table
 *
 * @attention   None
 *
 * @param[in]   nRTtable          
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */

GS8 GH3X2X_BTAlgorithmRTConfig(GU16 *nRTtable)
{
#if (__USE_GOODIX_BT_ALGORITHM__)

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    GH3X2X_Memcpy(g_nBtParamsArr, nRTtable, GOODIX_BT_PARAMS_NUM_MAX * sizeof(GU16));
    return GH3X2X_RET_OK;

#else
    GH3X2X_DEBUG_ALGO_LOG("BT algorithm no supported error!\r\n");
    return GH3X2X_RET_ALGO_NO_SUPPORTED;

#endif
}
/**
 * @fn     void GH3X2X_SetNewDataFlag(EMChannelMapId emFuncId, GU8 uchFlag)
 *
 * @brief  Set new sampling data flag
 *
 * @attention   None
 *
 * @param[in]   emFuncId    function id
 * @param[in]   uchFlag      new data or not
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SetNewDataFlag(GU8 emFuncId, GU8 uchFlag)
{
    if (GH3X2X_IS_NOT_NEW_RAWDATA == uchFlag)
    {
        GH3X2X_VAL_CLEAR_LEFT_BIT(g_uchNewDataFlag, emFuncId);
    }
    else
    {
        GH3X2X_VAL_SET_BIT(g_uchNewDataFlag, GH3X2X_GET_LEFT_SHIFT_VAL(emFuncId));
    }
}

/**
 * @fn     GS8 GH3X2X_HbAlgorithmScenarioConfig(GU8 uchScenario)
 *
 * @brief  Hb algorithm scenario config
 *
 * @attention   None
 *
 * @param[in]  uchScenario     @ref EMHbaScenesConfigType
 *                             others: fixed 0(default) and return GH3X2X_RET_PARAMETER_ERROR
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return paramters error
 * @retval  #GH3X2X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
GS8 GH3X2X_HbAlgorithmScenarioConfig(GU8 uchScenario)
{
#if (__USE_GOODIX_HR_ALGORITHM__)

    GS8 chRet = GH3X2X_RET_OK;
    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    if (uchScenario > GH3X2X_HBA_SCENARIO_CONFIG_MAX_VAL)
    {
        GH3X2X_DEBUG_ALGO_LOG("hb algorithm scenario param error!\r\n");
        g_unHrParamsArr[1] = HBA_SCENES_DEFAULT;
        chRet = GH3X2X_RET_PARAMETER_ERROR;
    }
    else
    {
        g_unHrParamsArr[1] = uchScenario;
    }

#else

    GS8 chRet = GH3X2X_RET_ALGO_NO_SUPPORTED;
    GH3X2X_DEBUG_ALGO_LOG("hba algorithm no supported error!\r\n");

#endif

    return chRet;
}

/**
 * @fn     GS8 GH3X2X_HbAlgorithmOutputTimeConfig(GS32 nOutputTime)
 *
 * @brief  Hb algorithm output time config
 *
 * @attention   None
 *
 * @param[in]   nOutputTime          valid 7~GS32_MAX
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_ALGO_NO_SUPPORTED       return algorithm no supported error
 */
GS8 GH3X2X_HbAlgorithmOutputTimeConfig(GS32 nLatestOutputTime, GS32 nEarliestOutputTime)
{
#if (__USE_GOODIX_HR_ALGORITHM__)

    GH3X2X_DEBUG_ALGO_LOG("%s\r\n", __FUNCTION__);
    g_unHrParamsArr[3] = nLatestOutputTime;
    g_unHrParamsArr[4] = nEarliestOutputTime;
    return GH3X2X_RET_OK;

#else

    GH3X2X_DEBUG_ALGO_LOG("hba algorithm no supported error!\r\n");
    return GH3X2X_RET_ALGO_NO_SUPPORTED;

#endif
}

GU16 GH3X2X_ReadSpo2CorrectionFactor(GU16 usVirtualRegAddr)
{
#if 0
    GU16 usVirtualRegData = 0;
    GU16 usValIndex = 0;

    if(usVirtualRegAddr >= GH3X2X_VSPO2_CH0_CORFAC0_L_REG_ADDR && usVirtualRegAddr <= GH3X2X_VSPO2_CH0_CORFAC2_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH3X2X_VSPO2_CH0_CORFAC0_L_REG_ADDR) / GH3X2X_REG_ADDR_SIZE;
    }
    else if(usVirtualRegAddr >= GH3X2X_VSPO2_CH0_CORFAC3_L_REG_ADDR && usVirtualRegAddr <= GH3X2X_VSPO2_CH3_CORFAC4_H_REG_ADDR)
    {
        usValIndex = (usVirtualRegAddr - GH3X2X_VSPO2_CH0_CORFAC3_L_REG_ADDR) / GH3X2X_REG_ADDR_SIZE + 6;
    }
    else
    {
        return usVirtualRegData;
    }
    
   if ((usValIndex % GH3X2X_REG_ADDR_SIZE) == 0) // low word
   {
       usVirtualRegData = GH3X2X_GET_LOW_WORD_FROM_DWORD(
                            g_nSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH3X2X_REG_ADDR_SIZE]);
   }
   else // high word
   {
       usVirtualRegData = GH3X2X_GET_HIGH_WORD_FROM_DWORD(
                            g_nSpo2ParamssArr[usValIndex / (SP02_CORR_FAC_NUM * 2)][(usValIndex % (SP02_CORR_FAC_NUM * 2)) / GH3X2X_REG_ADDR_SIZE]);
   }

   
   return usVirtualRegData;
#else
     return 0;
#endif
}

/**
 * @fn     void GH3X2X_AlgoChnlMapInit(STAlgoChnlMap stAlgoChnlMap)
 *
 * @brief  Init algo channel group struct
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X2X_AlgoChnlMapInit(STAlgoChnlMap *pstAlgoChnlMap)
{
    (*pstAlgoChnlMap).uchFlag = 0;
    (*pstAlgoChnlMap).uchNum = 0;
    for (GU32 num = 0  ; num < ALGO_CHNL_NUM ; num ++)
    {
        (*pstAlgoChnlMap).uchAlgoChnlMap[num] = 0xff;
    }
}

/**
 * @fn     void GH3X2X_HbaAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X2X_HbaAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_HR_ALGORITHM__)
    if (!g_stHbaAlgoChnlMap.uchFlag)
    {
        GH3X2X_AlgoChnlMapInit(&g_stHbaAlgoChnlMap);
        g_stHbaAlgoChnlMap.uchNum = __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
        for (GU8 cnt = 0; cnt < __HR_ALGORITHM_SUPPORT_CHNL_NUM__; cnt++)
        {
            g_stHbaAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + cnt] = cnt;
        }
    }
#endif
}

/**
 * @fn     void GH3X2X_HrvAlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X2X_HrvAlgoChnlMapDefultSet(void)
{
#if (0)
    if (!g_stHrvAlgoChnlMap.uchFlag)
    {
        GH3X2X_AlgoChnlMapInit(&g_stHrvAlgoChnlMap);
        g_stHrvAlgoChnlMap.uchNum = 1;
        g_stHrvAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + 0] = 0;
    }
#endif
}

/**
 * @fn     void GH3X2X_Spo2AlgoChnlMapDefultSet(void)
 
 *
 * @brief  Set algo channel group Defult value
 *
 * @attention   
 *
 *
 * @return  None
 */
void GH3X2X_Spo2AlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__)
    if (!g_stSpo2AlgoChnlMap.uchFlag)
    {
        GH3X2X_AlgoChnlMapInit(&g_stSpo2AlgoChnlMap);
        g_stSpo2AlgoChnlMap.uchNum = 1;
        g_stSpo2AlgoChnlMap.uchAlgoChnlMap[ALGO_RED_CHNL_POS + 0] = 0;
        g_stSpo2AlgoChnlMap.uchAlgoChnlMap[ALGO_IR_CHNL_POS + 0] = __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__;
    }
#endif
}

void GH3X2X_BpAlgoChnlMapDefultSet(void)
{
#if (__USE_GOODIX_BP_ALGORITHM__)
    if (!g_stBpAlgoChnlMap.uchFlag)
    {
        GH3X2X_AlgoChnlMapInit(&g_stBpAlgoChnlMap);
        g_stBpAlgoChnlMap.uchNum = 1;
        g_stBpAlgoChnlMap.uchAlgoChnlMap[ALGO_GREEN_CHNL_POS + 0] = 1;
        g_stBpAlgoChnlMap.uchEcgAlgoChnlMap = 0;
    }
#endif
}


void GH3X2X_WriteAlgParametersWithVirtualReg(GU8 uchParasIndex, GU32 unFunctionID, GS32* nAlgParametersBuffer, GU8 nBufferMaxLen, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex = (usVirtualRegAddr - (unFunctionID * GH3X2X_VREG_FUNCTION_OFFSET + GH3X2X_ADT_ALG_CFG_ADDR)) / GH3X2X_REG_ADDR_SIZE;
    if ((GS16)((GS16)usValIndex / (GS16)GH3X2X_REG_ADDR_SIZE - (GS16)uchParasIndex < 0) ||\
        (usValIndex / GH3X2X_REG_ADDR_SIZE - uchParasIndex >= nBufferMaxLen))
    {
        return;
    }
    if ((usValIndex % GH3X2X_REG_ADDR_SIZE) != 0) // high word
    {
        nAlgParametersBuffer[usValIndex / GH3X2X_REG_ADDR_SIZE - uchParasIndex] = \
            GH3X2X_MAKEUP_DWORD2(usVirtualRegValue, GH3X2X_GET_LOW_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH3X2X_REG_ADDR_SIZE - uchParasIndex]));

    }
    else // low word
    {
        nAlgParametersBuffer[usValIndex / GH3X2X_REG_ADDR_SIZE - uchParasIndex] = \
            GH3X2X_MAKEUP_DWORD2(GH3X2X_GET_HIGH_WORD_FROM_DWORD(nAlgParametersBuffer[usValIndex / GH3X2X_REG_ADDR_SIZE - uchParasIndex]), usVirtualRegValue);
    }
}

void GH3X2X_WriteAlgChnlMapWithVirtualReg(GU32 unFunctionID, STAlgoChnlMap* pstAlgChnlMap, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usVirtualRegAddrOffset = usVirtualRegAddr - (unFunctionID * GH3X2X_VREG_FUNCTION_OFFSET + GH3X2X_ADT_ALG_CFG_ADDR);
    switch (usVirtualRegAddrOffset)
    {
    case GH3X2X_VREG_ALGO_CHNL_MAP_OFFSET:
        GH3X2X_AlgoChnlMapInit(pstAlgChnlMap);
        pstAlgChnlMap->uchNum = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        if (pstAlgChnlMap->uchNum)
        {
            pstAlgChnlMap->uchFlag = 1;
        }
        break;
    case GH3X2X_VREG_ALGO_CHNL_MAP_OFFSET + GH3X2X_REG_ADDR_SIZE:
        pstAlgChnlMap->uchEcgAlgoChnlMap = (GU8)(usVirtualRegValue & 0x000000FF);
        break;
    default:
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH3X2X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH3X2X_REG_ADDR_SIZE]\
            = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        pstAlgChnlMap->uchAlgoChnlMap[usVirtualRegAddrOffset - GH3X2X_VREG_ALGO_CHNL_MAP_OFFSET - 2 * GH3X2X_REG_ADDR_SIZE + 1]\
            = GH3X2X_GET_HIGH_BYTE_FROM_WORD(usVirtualRegValue);
    }
}

void GH3X2X_WriteHrAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_HR_ALGORITHM__) // hr
    if (usVirtualRegAddr < GH3X2X_HR_ALGO_CHNL_NUM_ADDR)
    {
        if(!g_stHbaAlgoChnlMap.uchFlag)
        {
            GH3X2X_AlgoChnlMapInit(&g_stHbaAlgoChnlMap);
        }
        
        //GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_HR, (GS32*)&g_unHrParamsArr, GOODIX_HR_PARAMS_NUM_MAX, usVirtualRegAddr,usVirtualRegValue);
        //GH3X2X_DEBUG_ALGO_LOG("[%s]:Hba Scenario = %d\r\n", __FUNCTION__, g_unHrParamsArr[1]);
        if(HBA_SCENES_SLEEP == g_unHrParamsArr[1])
        {
            GH3x2xSleepFlagSet(1);
        }
    }
    else
    {
        GH3X2X_WriteAlgChnlMapWithVirtualReg(GH3X2X_FUNC_OFFSET_HR, &g_stHbaAlgoChnlMap, usVirtualRegAddr, usVirtualRegValue);
    }
#endif
}

void GH3X2X_WriteHrvAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_HRV_ALGORITHM__) // hrv
    GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_HRV, (GS32*)g_nHrvParamsArr, GOODIX_HRV_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X2X_WriteSpo2AlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_SPO2_ALGORITHM__) // spo2
    if (usVirtualRegAddr < GH3X2X_SPO2_ALGO_CHNL_NUM_ADDR)
    {
        GH3X2X_AlgoChnlMapInit(&g_stSpo2AlgoChnlMap);
        //GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_SPO2, (GS32*)g_nSpo2ParamssArr, GOODIX_SPO2_PARAMS_NUM_MAX, usVirtualRegAddr,usVirtualRegValue);
    }
    else
    {
        GH3X2X_WriteAlgChnlMapWithVirtualReg(GH3X2X_FUNC_OFFSET_SPO2, &g_stSpo2AlgoChnlMap, usVirtualRegAddr, usVirtualRegValue);
    }
#endif
}

void GH3X2X_WriteEcgAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_ECG_ALGORITHM__) // ecg
    GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_ECG, (GS32*)g_nEcgParamsArr, GOODIX_ECG_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X2X_WriteBtAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BT_ALGORITHM__) // bt
    GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_BT, (GS32*)g_nBtParamsArr, GOODIX_BT_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X2X_WriteAfAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_AF_ALGORITHM__) // af
    GH3X2X_WriteAlgParametersWithVirtualReg(0, GH3X2X_FUNC_OFFSET_AF, (GS32*)&g_nAfParamsArr, GOODIX_AF_PARAMS_NUM_MAX, usVirtualRegAddr, usVirtualRegValue);
#endif
}

void GH3X2X_WriteBpAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__USE_GOODIX_BP_ALGORITHM__) // fpbp

#endif
}


/**
 * @fn     GU16 GH3X2X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
 *
 * @brief  Read algorithm param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[out]  None
 *
 * @return  virtual reg val
 */
GU16 GH3X2X_ReadAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr)
{
    GU16 usVirtualRegData = 0;

    return usVirtualRegData;
}

void GH3X2X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    if (usVirtualRegAddr < GH3X2X_REG_IS_VIRTUAL0X3_BIT)
    {
        return;
    }
    GU32 unFunctionID = (usVirtualRegAddr - GH3X2X_REG_IS_VIRTUAL0X3_BIT) / GH3X2X_VREG_FUNCTION_OFFSET;
    if (usVirtualRegAddr >= GH3X2X_ADT_ALG_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * unFunctionID &&\
             usVirtualRegAddr < GH3X2X_ADT_DRV_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * (unFunctionID + 1))
    {
        switch (unFunctionID)
        {
        case GH3X2X_FUNC_OFFSET_HR:
            GH3X2X_WriteHrAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_HRV:
            GH3X2X_WriteHrvAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_FPBP:
        case GH3X2X_FUNC_OFFSET_PWA:
            //GH3X2X_WriteBpAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_SPO2:
            GH3X2X_WriteSpo2AlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_ECG:
            GH3X2X_WriteEcgAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_BT:
            GH3X2X_WriteBtAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        case GH3X2X_FUNC_OFFSET_AF:
            GH3X2X_WriteAfAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
            break;
        default:
            break;
        }
    }
}

void GH3X2X_LoadGoodixAlgoRegConfigArr(const STGh3x2xInitConfig *pstGoodixAlgoCfgListArr)
{
    const STGh3x2xReg *pstRegConfigArr = pstGoodixAlgoCfgListArr->pstRegConfigArr;
    GU16 usRegConfigLen = pstGoodixAlgoCfgListArr->usConfigArrLen;
    GU16 usIndex = 0;
    if ((pstRegConfigArr != GH3X2X_PTR_NULL) && (usRegConfigLen != 0))
    {
        for (usIndex = 0; usIndex < usRegConfigLen; usIndex++) // write GH3X2X reg.
        {
            GH3X2X_WriteAlgConfigWithVirtualReg(pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
        }
    }
}

#endif











/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
