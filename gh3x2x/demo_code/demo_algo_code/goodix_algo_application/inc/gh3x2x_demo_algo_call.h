/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algo_call.h
 *
 * @brief   gh3x2x algo call
 *
 * @version ref gh3x2x_demo_algo_call.h
 *
 */
#ifndef _GH3X2X_DEMO_ALGO_CALL_H_
#define _GH3X2X_DEMO_ALGO_CALL_H_

#include "gh_drv.h"

extern GCHAR* GH3X2X_GetAlgoCallDemoVersion(void);
extern void GH3X2X_AlgoVersion(GU8 uchFunctionID, GCHAR version[150]);
extern GS8 GH3X2X_AlgoInit(GU32 unFunctionID);
extern GS8 GH3X2X_AlgoDeinit(GU32 unFunctionID);
extern GS8 GH3X2X_AlgoCalculate(GU32 unFunctionID);

extern void GH3X2X_AlgoCallConfigInit(const STGh3x2xFrameInfo * const  pstGh3x2xFrameInfo[], GU8 uchAlgoCfgIndex);
extern void GH3X2X_InitAlgoConfigParameters(void);
extern void GH3X2X_AlgoSensorEnable(GU8 uchAlgoGsensorEnable,GU8 uchAlgoCapEnable,GU8 uchAlgoTempEnable);

extern void GH3X2X_SoftAdtParaInit(void);
extern void GH3X2X_WriteAlgConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

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
extern GS8 GH3X2X_HbAlgorithmOutputTimeConfig(GS32 nLatestOutputTime, GS32 nEarliestOutputTime);

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
extern GS8 GH3X2X_HbAlgorithmScenarioConfig(GU8 uchScenario);
extern GS8 GH3X2X_BTAlgorithmRTConfig(GU16 *nRTtable);
extern GU8 GH3X2X_TimestampSyncGetFrameDataFlag(void);
extern void GH3X2X_TimestampSyncAccInit(void);
extern void GH3X2X_TimestampSyncPpgInit(GU32 unFunctionID);
extern void GH3X2X_TimestampSyncSetPpgIntFlag(GU8 uchPpgIntFlag);
extern void GH3X2X_TimestampSyncFillAccSyncBuffer(GU32 unTimeStamp,
                                           GS16 usAccX,
                                           GS16 usAccY,
                                           GS16 usAccZ);
extern void GH3X2X_TimestampSyncFillPpgSyncBuffer(GU32 unTimeStamp,
                                           const STGh3x2xFrameInfo * const pstFrameInfo);

#endif /* _GH3X2X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
