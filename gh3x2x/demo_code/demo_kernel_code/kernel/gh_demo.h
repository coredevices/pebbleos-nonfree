/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x_demo.h
 *
 * @brief   header file of gh3x2x driver lib demo
 *
 * @version GH(M)3X2X_DRV_LIB_DEMO_1.0
 *
 * @author  Gooidx Iot Team
 *
 */


#ifndef _GH_DEMO_H_
#define _GH_DEMO_H_

#include "gh_drv.h"

#include "gh_multi_sen_pro.h"

/**
 * @fn     int Gh3x2xDemoInit(void)
 *
 * @brief  Init GH3x2x module.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
int Gh3x2xDemoInit(void);

/**
 * @fn     GU8 Gh3x2xDemoInterruptProcess(void)
 *
 * @brief  Interrupt process of GH3x2x.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoInterruptProcess(void);

/**
 * @fn     void hal_gh3x2x_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x2x int handler
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void hal_gh3x2x_int_handler_call_back(void);

/**
 * @fn     GS8 Gh3x2xDemoArrayCfgSwitch(GU8 uchArrayCfgIndex)
 *
 * @brief  array cfg switch (call by user)
 *
 * @attention  you should switch array cfg manually before calling Gh3x2xDemoStartSampling
 *
 * @param[in]   uchArrayCfgIndex    0: gh3x2x_reg_list0    1: gh3x2x_reg_list0 ...
 * @param[out]  None
 *
 * @return  GH3X2X_RET_OK:switch application mode success
 * @return  GH3X2X_RET_RESOURCE_ERROR:can't find corresponding reg config array
 */
GS8 Gh3x2xDemoArrayCfgSwitch(GU8 uchArrayCfgIndex);


/**
 * @fn     void Gh3x2xDemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex)
 *
 * @brief  Start sampling of GH3x2x
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X2X_FUNCTION_ADT/GH3X2X_FUNCTION_HR/GH3X2X_FUNCTION_SPO2/GH3X2X_FUNCTION_ECG and etc.
 * @param[in]   uchArrayCfgIndex      cfg id
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoStartSamplingWithCfgSwitch(GU32 unFuncMode, GU8 uchArrayCfgIndex);

/**
 * @fn     void Gh3x2xDemoStartSampling()
 *
 * @brief  Start sampling of GH3x2x
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X2X_FUNCTION_ADT/GH3X2X_FUNCTION_HR/GH3X2X_FUNCTION_SPO2/GH3X2X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoStartSampling(GU32 unFuncMode);

/**
 * @fn     void Gh3x2xDemoStartSamplingForEngineeringMode()
 *
 * @brief  Start sampling of GH3x2x for engineering mode
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X2X_FUNCTION_ADT/GH3X2X_FUNCTION_HR/GH3X2X_FUNCTION_SPO2/GH3X2X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoStartSamplingForEngineeringMode(GU32 unFuncMode, GU8 uchArrayCfgIndex, STGh3x2xEngineeringModeSampleParam *pstSampleParaGroup , GU8 uchSampleParaGroupNum);


/**
 * @fn     void Gh3x2xDemoStopSampling(GU32 unFuncMode)
 *
 * @brief  Stop sampling of GH3x2x
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function mode that will start
 *                              GH3X2X_FUNCTION_ADT/GH3X2X_FUNCTION_HR/GH3X2X_FUNCTION_SPO2/GH3X2X_FUNCTION_ECG and etc.
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoStopSampling(GU32 unFuncMode);



/**
 * @fn     void Gh3x2xDemoStopSamplingForEngineeringMode(void)
 *
 * @brief  Stop sampling of GH3x2x for engineering mode
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoStopSamplingForEngineeringMode(void);



/**
 * @fn     void Gh3x2x_CreateMoveDetTimer(void)
 * 
 * @brief  Create a timer for adt confirm which will read gsensor data periodically
 *
 * @attention   Period of timer can be set by 
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xCreateMoveDetTimer(void);



/**
 * @fn     void Gh3x2xDemoProtocolProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen)
 *
 * @brief  Analyze protocol about GH3x2x,and pack protocol data to reply
 *
 * @attention   None
 *
 * @param[in]  puchProtocolDataBuffer   pointer to received protocol data buffer
 * @param[in]  usRecvLen                protocol data buffer length
 *
 * @return  None
 */
void Gh3x2xDemoProtocolProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen);

/**
 * @fn     void Gh3x2xDemoFunctionSampleRateSet(GU32 unFunctionID,  GU16 usSampleRate)
 *
 * @brief  
 *
 * @attention   None
 *
 * @param[in]   unFunctionID   such as: GH3X2X_FUNCTION_HR   or  GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_HRV
 * @param[in]   usSampleRate   only 25Hz times supported  (25/50/75...)
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoFunctionSampleRateSet(GU32 unFunctionID,  GU16 usSampleRate);




/**
 * @fn     void Gh3x2xDemoSetFuncionLedCurrent(GU8 uchFunctionID, GU16 usLedDrv0Current, GU16 usLedDrv1Current)
 *
 * @brief  Set function led current
 *
 * @attention   None
 *
 * @param[in]   uchFunctionID               function offset
 * @param[in]   usLedDrv0Current         0mA-200mA
 * @param[in]   usLedDrv1Current         0mA-200mA
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoSetFuncionLedCurrent(GU8 uchFunctionID, GU16 usLedDrv0Current, GU16 usLedDrv1Current);


/**
 * @fn      void Gh3x2xDemoFunctionChannelEnSet(GU32 unFunctionID,  GU32 unChnlEn)
 *
 * @brief  
 *
 * @attention   None
 *
 * @param[in]   unFunctionID   such as: GH3X2X_FUNCTION_HR   or  GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_HRV
 * @param[in]   unChnlEn   BitN = 0: channel N is disable        BitN = 1: channel N is enable
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoFunctionChannelEnSet(GU32 unFunctionID,  GU32 unChnlEn);

/**
 * @fn     void Gh3x2xInterruptModeSwitch(EMInterruptModeType emIntModeType)
 *
 * @brief  change Interrupt process mode of GH3x2x.
 *
 * @attention   do not input 2
 *
 * @param[in]   emIntModeType 0:int pin 1: polling
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xInterruptModeSwitch(GU8 uchIntModeType);
GU8 Gh3x2xGetInterruptMode(void);
void Gh3x2xSerialSendTimerHandle(void);




#if (__GH3X2X_CASCADE_EN__)
GS8 Gh3x2xEcgCascadeCommunicationTest(void);
#endif

#endif /* _GH_DEMO_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
