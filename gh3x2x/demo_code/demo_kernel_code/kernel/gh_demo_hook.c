/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_hook.c
 * 
 * @brief   gh3x2x driver lib demo code for hook
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh_demo.h"
#include "gh_demo_config.h"
#include "gh_demo_inner.h"

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x2x_demo_algo_call.h"
#endif

/* hook functions */
/**
 * @fn     void gh3x2x_init_hook_func(void)
 * 
 * @brief  gh3x2x init hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_init_hook_func(void)
{
#if (__ADT_ONLY_PARTICULAR_WM_CONFIG__)
    g_usCurrentConfigListFifoWmBak = 0;
#endif

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    if(0 == GH3x2x_GetChipResetRecoveringFlag())
    {
        GH3X2X_AlgoCallConfigInit(g_pstGh3x2xFrameInfo, g_uchGh3x2xRegCfgArrIndex);
    }
#endif

#if __GH_MSG_WITH_ALGO_LAYER_EN__
    if(0 == GH3x2x_GetChipResetRecoveringFlag())
    {
        GH3X2X_SEND_MSG_ALGO_CFG_INIT(g_pstGh3x2xFrameInfo, g_uchGh3x2xRegCfgArrIndex);
    }
#endif
}

/**
 * @fn     void gh3x2x_sampling_start_hook_func(void)
 * 
 * @brief  gh3x2x start hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_sampling_start_hook_func(void)
{
    GOODIX_PLATFORM_SAMPLING_START_HOOK_ENTITY();
}

/**
 * @fn     void gh3x2x_sampling_stop_hook_func(void)
 * 
 * @brief  gh3x2x stop hook
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_sampling_stop_hook_func(void)
{
    GOODIX_PLATFORM_SAMPLING_STOP_HOOK_ENTITY();
}

/**
 * @fn     void gh3x2x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length)
 * 
 * @brief  gh3x2x get rawdata hook
 *
 * @attention   None
 *
 * @param[in]   read_buffer_ptr     pointer to read rawdata buffer
 * @param[in]   length              length
 * @param[out]  None
 *
 * @return  None
 */
 typedef struct
{
    GU32 uiAdcCode;                      //sampling rawdata of ADC
    GU8  ubSlotNo;                       //slot number
    GU8  ubAdcNo;                        //adc number
    GU8  ubFlagLedAdjIsAgc_EcgRecover;   //adj flag of ppg data or fast recover flag of ecg data
    GU8  ubFlagLedAdjAgcUp;            //adj down flag of ppg data   0: down  1:up
}StFifoDataInformation;

void gh3x2x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length)
{
#if (GH3X2X_FIFO_MONITOR_EN&(__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
     Gh3x2xFifoMonitorPro(&g_stFifoMonitorInfo, read_buffer_ptr, length);
#else
	UNUSED_VAR(read_buffer_ptr);
	UNUSED_VAR(length);	
#endif
    /* code implement by user */
    /****************** FOLLOWING CODE IS EXAMPLE **********************************/
#if 0
    StFifoDataInformation stTempFifoInfo = {0};
    if (length/4 > 0)
    {
		for (int i = 0; i < length; i += 4)
		{
			GU32 temp = 0;
			// big endian to little endian
			temp =  ((GU32)read_buffer_ptr[i+ 0]) << 24;
			temp += ((GU32)read_buffer_ptr[i+ 1])<< 16;
			temp += ((GU32)read_buffer_ptr[i+ 2]) << 8;
			temp += ((GU32)read_buffer_ptr[i+ 3]);    
			// pick rawdata and flag
			stTempFifoInfo.uiAdcCode = ((temp >> 0) & 0x00FFFFFF);
			stTempFifoInfo.ubSlotNo =  ((temp >> 29) & 0x00000007);
			stTempFifoInfo.ubAdcNo = ((temp >> 27) & 0x00000003);
			stTempFifoInfo.ubFlagLedAdjIsAgc_EcgRecover = ((temp >> 26) & 0x00000001);
			stTempFifoInfo.ubFlagLedAdjAgcUp = ((temp >> 25) & 0x00000001);
			GH3X2X_INFO_LOG("Received rawdata:slot%d,adc%d,AdcCode = %d\r\n", stTempFifoInfo.ubSlotNo, stTempFifoInfo.ubAdcNo, \
																stTempFifoInfo.uiAdcCode);
		}
    }
#endif
}

/**
 * @fn      void gh3x2x_algorithm_get_io_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo)
 * 
 * @brief  get algorithm input and output data
 *
 * @attention   None        
 *
 * @param[in]   pstFrameInfo
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_algorithm_get_io_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    /* algo calculate */
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    //GOODIX_PLATFORM_SAMPLING_START_HOOK_ENTITY();
    GH3X2X_AlgoCalculate(pstFrameInfo->unFunctionID);
#endif

#if __GH_MSG_WITH_ALGO_LAYER_EN__
    GH3X2X_SEND_MSG_ALGO_CAL(pstFrameInfo->unFunctionID);
#endif

#if (__SUPPORT_ALGO_INPUT_OUTPUT_DATA_HOOK_CONFIG__)
    /****************** FOLLOWING CODE IS EXAMPLE **********************************/
    #if 0
    //function id and channel num
    GH3X2X_INFO_LOG("[IO_DATA]Function ID: 0x%x, channel num = %d, frame cnt = %d\r\n",(int)(pstFrameInfo->unFunctionID),(int)(pstFrameInfo->pstFunctionInfo->uchChnlNum),(int)(pstFrameInfo->punFrameCnt[0]));
    //gsensor data
    GH3X2X_INFO_LOG("[IO_DATA]Gsensor: x = %d, y = %d, z = %d \r\n",\
                        (int)(pstFrameInfo->pusFrameGsensordata[0]),\
                        (int)(pstFrameInfo->pusFrameGsensordata[1]),\
                        (int)(pstFrameInfo->pusFrameGsensordata[2])\
                );
    //rawdata
    for(GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
    {
        GH3X2X_INFO_LOG("[IO_DATA]Ch%d rawdata= %d\r\n",(int)(uchChnlCnt),(int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
    }
    #endif
#endif

#if (__SUPPORT_ALGO_INPUT_OUTPUT_DATA_LOG__)
#define STR_MAX_LEN 128
    extern RTC_HandleTypeDef RtcHandle;
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    struct tm timeinfo;

    //get RTC time and date
    HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);
    timeinfo.tm_year = 2000 + sDate.Year - 1900; // tm_year from 1900 
    timeinfo.tm_mon  = sDate.Month - 1;          // tm_mon:0~11
    timeinfo.tm_mday = sDate.Date;
    timeinfo.tm_hour = sTime.Hours;
    timeinfo.tm_min  = sTime.Minutes;
    timeinfo.tm_sec  = sTime.Seconds;
    timeinfo.tm_isdst = 0;
    uint32_t epoch_time = mktime(&timeinfo);
    //The above code is for timestamp acquisition of stm32f412MCU. Customers need to modify it according to their own MCU
    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_HR)
    {   //hr function
        char GF_str[STR_MAX_LEN] = {0}, raw_str[STR_MAX_LEN] = {0};
        GU8 offset = 0, remain = sizeof(GF_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {    //gain & current
            GU8 n = snprintf(GF_str + offset, remain, "%d,", (int)pstFrameInfo->punFrameAgcInfo[uchChnlCnt]);
            offset += n;
            remain -= n;
        }
        offset = 0, remain = sizeof(raw_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {   //red raw;
            GU8 n = snprintf(raw_str + offset, remain, "%d,", (int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
            offset += n;
            remain -= n;
        }
        //[Unix timestample]frame id, accx, accy, accz, flag0, flag1  gain & current, raw;
        GH3X2X_IODATA_LOG("[%lu]hr_fi=%d,gs=%d,%d,%d,CF:%d,%d,GF=%sraw:%s\n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[0],
            (int)pstFrameInfo->punFrameFlag[1],
            GF_str, raw_str);
        if (pstFrameInfo->pstAlgoResult->uchUpdateFlag)
        {   //[Unix timestample]... hr_out, confidence
            GH3X2X_IODATA_LOG("[%lu]hr_data hr:%d,confi:%d \n", epoch_time,
                            (int)pstFrameInfo->pstAlgoResult->snResult[0], 
                            (int)pstFrameInfo->pstAlgoResult->snResult[1]);
        }
    }

    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_HRV)
    {   //hrv function
        char GF_str[STR_MAX_LEN] = {0}, raw_str[STR_MAX_LEN] = {0};
        GU8 offset = 0, remain = sizeof(GF_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {    //gain & current
            GU8 n = snprintf(GF_str + offset, remain, "%d,", (int)pstFrameInfo->punFrameAgcInfo[uchChnlCnt]);
            offset += n;
            remain -= n;
        }
        offset = 0, remain = sizeof(raw_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {   //red raw;
            GU8 n = snprintf(raw_str + offset, remain, "%d,", (int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
            offset += n;
            remain -= n;
        }
        //[Unix timestample]frame id, accx, accy, accz, flag0, flag1  gain & current, raw;
        GH3X2X_IODATA_LOG("[%lu]hrv_fi=%d,gs=%d,%d,%d,CF:%d,%d,GF=%sraw:%s\n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[0],
            (int)pstFrameInfo->punFrameFlag[1],
            GF_str, raw_str);
        if (pstFrameInfo->pstAlgoResult->uchUpdateFlag)
        {  //[Unix timestample]...rri0, rri1, rri_confidence. rri_valid_num
            GH3X2X_IODATA_LOG("[%lu]hrv_data rri:%d,%d,confi:%d,valid_n:%d \n", epoch_time,
                              (int)pstFrameInfo->pstAlgoResult->snResult[0], 
                              (int)pstFrameInfo->pstAlgoResult->snResult[1],(int)pstFrameInfo->pstAlgoResult->snResult[4],
                               (int)pstFrameInfo->pstAlgoResult->snResult[5]);
        }
    }

    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_SPO2)
    {   //spo2 function
        char GF_str[STR_MAX_LEN] = {0}, raw_str[STR_MAX_LEN] = {0};
        GU8 offset = 0, remain = sizeof(GF_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {    //gain & current
            GU8 n = snprintf(GF_str + offset, remain, "%d,", (int)pstFrameInfo->punFrameAgcInfo[uchChnlCnt]);
            offset += n;
            remain -= n;
        }
        offset = 0, remain = sizeof(raw_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {   //red raw;
            GU8 n = snprintf(raw_str + offset, remain, "%d,", (int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
            offset += n;
            remain -= n;
        }
        //[Unix timestample]frame id, accx, accy, accz, flag0, flag1  gain & current, raw
        GH3X2X_IODATA_LOG("[%lu]spo2_fi=%d,gs=%d,%d,%d,CF:%d,%d,GF=%sraw:%s\n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[0],
            (int)pstFrameInfo->punFrameFlag[1],
            GF_str, raw_str);
        if (pstFrameInfo->pstAlgoResult->uchUpdateFlag)
        {  //[Unix timestample]... spo2, r_val, valid_level
            GH3X2X_IODATA_LOG("[%lu]spo2_data spo2:%d,R:%d,valid:%d \n", epoch_time,
                              (int)pstFrameInfo->pstAlgoResult->snResult[0], 
                              (int)pstFrameInfo->pstAlgoResult->snResult[1],(int)pstFrameInfo->pstAlgoResult->snResult[3]);
        }
    }

    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_SOFT_ADT_IR)
    {   //SOFT_ADT_IR function
        char GF_str[STR_MAX_LEN] = {0}, raw_str[STR_MAX_LEN] = {0};
        GU8 offset = 0, remain = sizeof(GF_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {    //gain & current
            GU8 n = snprintf(GF_str + offset, remain, "%d,", (int)pstFrameInfo->punFrameAgcInfo[uchChnlCnt]);
            offset += n;
            remain -= n;
        }
        offset = 0, remain = sizeof(raw_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {   //red raw;
            GU8 n = snprintf(raw_str + offset, remain, "%d,", (int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
            offset += n;
            remain -= n;
        }
        //[Unix timestample]frame id, accx, accy, accz, flag0, flag1  gain & current, raw;
        GH3X2X_IODATA_LOG("%[%lu]NI_fi=%d,gs=%d,%d,%d,CF:%d,%d,GF=%sraw:%s\n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[0],
            (int)pstFrameInfo->punFrameFlag[1],
            GF_str, raw_str);
            if(pstFrameInfo->pstAlgoResult->uchUpdateFlag)
            {
                GH3X2X_IODATA_LOG("NI_data r0:%d,r1:%d\n", (int)pstFrameInfo->pstAlgoResult->snResult[0],
                                  (int)pstFrameInfo->pstAlgoResult->snResult[1]);
            }
    }

    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_SOFT_ADT_GREEN)
    {   //SOFT_ADT_GREEN function
        char GF_str[STR_MAX_LEN] = {0}, raw_str[STR_MAX_LEN] = {0};
        GU8 offset = 0, remain = sizeof(GF_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {    //gain & current
            GU8 n = snprintf(GF_str + offset, remain, "%d,", (int)pstFrameInfo->punFrameAgcInfo[uchChnlCnt]);
            offset += n;
            remain -= n;
        }
        offset = 0, remain = sizeof(raw_str);
        for (GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {   //red raw;
            GU8 n = snprintf(raw_str + offset, remain, "%d,", (int)(pstFrameInfo->punFrameRawdata[uchChnlCnt]));
            offset += n;
            remain -= n;
        }
        //[Unix timestample]frame id, accx, accy, accz, flag0, flag1  gain & current, raw;
        GH3X2X_IODATA_LOG("%[%lu]NG_fi=%d,gs=%d,%d,%d,CF:%d,%d,GF=%sraw:%s\n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[0],
            (int)pstFrameInfo->punFrameFlag[1],
            GF_str, raw_str);
            if (pstFrameInfo->pstAlgoResult->uchUpdateFlag)
            {
                GH3X2X_IODATA_LOG("NG_data r0:%d,r1:%d\n", (int)pstFrameInfo->pstAlgoResult->snResult[0],
                                  (int)pstFrameInfo->pstAlgoResult->snResult[1]);
            }
    }

    if (pstFrameInfo->unFunctionID & GH3X2X_FUNCTION_ECG)//ECG function
    {
        //[Unix timestample]frame id, accx, accy, accz, flag2, raw0, raw1, out0, out1, hr_bpm;
        GH3X2X_IODATA_LOG("%[%lu]ECG_fi=%d,gs=%d,%d,%d,CF:%d,raw:%d,%d,out:%d,%d,bpm:%d \n", epoch_time,
            (int)(*(pstFrameInfo->punFrameCnt) & 0xFF),
            (int)pstFrameInfo->pusFrameGsensordata[0],
            (int)pstFrameInfo->pusFrameGsensordata[1],
            (int)pstFrameInfo->pusFrameGsensordata[2],
            (int)pstFrameInfo->punFrameFlag[2],
            (int)(pstFrameInfo->punFrameRawdata[0]),
            (int)(pstFrameInfo->punFrameRawdata[1]),
            (int)pstFrameInfo->pstAlgoResult->snResult[0],
            (int)pstFrameInfo->pstAlgoResult->snResult[2],
            (int)pstFrameInfo->pstAlgoResult->snResult[1]);
    }
#endif

}

void gh3x2x_frame_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo)
{
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    if (GH3X2X_TimestampSyncGetFrameDataFlag() == 0 && GH3X2X_GetGsensorEnableFlag())
    {
        /* user set time stamp code here */
        GU32 unTimeStamp = 0;
        //GU8 uchFuncOffsetID = 0;
        //for(GU8 uchFunCnt = 0; uchFunCnt < __SYNC_FUNC_MAX__; uchFunCnt ++)
        //{
        //    if (((1<<uchFunCnt) & pstFrameInfo->unFunctionID) == (1<<uchFunCnt))
        //    {
        //        uchFuncOffsetID = uchFunCnt;
        //        break;
        //    }
        //}
        //unTimeStamp = func_ppg_timestamp[uchFuncOffsetID];
        GH3X2X_TimestampSyncFillPpgSyncBuffer(unTimeStamp, pstFrameInfo);
        //func_ppg_timestamp[uchFuncOffsetID] += 40;
    }
    else
    {
#endif
        gh3x2x_algorithm_get_io_data_hook_func(pstFrameInfo);
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    }
#endif
}

void gh3x2x_reset_by_protocol_hook(void)
{
    //Gh3x2x_BspDelayMs(20);
    //Gh3x2xDemoInterruptProcess();
    GH3X2X_INFO_LOG("[%s]:handle protocol reset\r\n", __FUNCTION__);
}

void gh3x2x_config_set_start_hook(void)
{
    /* code implement by user */
}

void gh3x2x_config_set_stop_hook(void)
{
    Gh3x2xIntPinTriggerModePro();
    GH3x2xSlotTimeInfo();
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    GH3X2X_AlgoSensorEnable(GH3X2X_GetGsensorEnableFlag(),GH3X2X_GetCapEnableFlag(),GH3X2X_GetTempEnableFlag());
#endif
}

void gh3x2x_write_algo_config_hook(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    GH3X2X_WriteAlgConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
#else
	UNUSED_VAR(usVirtualRegAddr);
	UNUSED_VAR(usVirtualRegValue);
#endif
}

#if (__FUNC_TYPE_ECG_ENABLE__)
/**
 * @fn     void Gh3x2x_LeadOnEventHook(void)
 * 
 * @brief  Lead on event hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_LeadOnEventHook(void)
{
    GOODIX_PLATFORM_LEAD_ON_EVENT();
}

/**
 * @fn     void Gh3x2x_LeadOffEventHook(void)
 * 
 * @brief  Lead off event hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_LeadOffEventHook(void)
{
    GOODIX_PLATFORM_LEAD_OFF_EVENT();
}
#endif

#if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
extern const STGhMultiSensorTimerOps g_stGhMultiSensorTimerOps;

void GhMultiSensorWearDetStop(void)
{
    GhMultiSensorTimerStop();
    GhMultiSensorTimerOpsUnregister();
    GhMultSensorWearEventManagerDisable();
    GhGsMoveDetecterDisable();
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ADT);
}

/** @brief hook when new multi sensor wear event is generated,
 you can do operation as below in this hook. 
firstly, you can check event record in Event list via API Gh3x2xMultiSensorConditionCheckInWindow or Gh3x2xMultiSensorConditionCheckIsNew;
soconly, you can disable multi sensor event manager via API GhMultSensorWearEventManagerDisable
thirdly, you can open/close function via API Gh3x2xDemoStartSampling/Gh3x2xDemoStopSampling according event record 
**/
/// @param uchNewEvent such as: GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON
void GhMultiSensorEventHook(GU32 uchNewEvent)
{
    #if 1
    if(GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT == uchNewEvent && \
       !GhMultiSensorConditionCheckInWindow(GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON, __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__, 1))
    {
        GH3X2X_DEBUG_LOG("[GhMultiSensorEventHook] gs move time out, stop adt\r\n");
        Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ADT);
    }

    if(GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE == uchNewEvent)
    {
        GH3X2X_DEBUG_LOG("[GhMultiSensorEventHook] sensor gs move, start adt\r\n");
        Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
    }

    if(GhMultiSensorConditionCheckIsNew(GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON, GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_OFF))
    {
        // if(GhMultiSensorConditionCheckInWindow(GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE, 3000, 1))
        // {
            //final wear on
            GH3X2X_INFO_LOG("[GhMultiSensorEventHook] final wear on !!!! \r\n");
            GhMultiSensorWearDetStop();
        #if (__SUPPORT_PROTOCOL_ANALYZE__)
            Gh3x2xDemoReportEvent(GH3X2X_IRQ_MSK_WEAR_ON_BIT, GH3X2X_EVENT_EX_BIT_MULTI_SENSOR);
        #endif
            /****************START:   add your handle you want after final wear on ***************/
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_ADT);
        #if (__GH_MULTI_SENSOR_WEAR_EXAMPLE_EN__&&__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)
            Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SOFT_ADT_GREEN);
        #endif

            /****************END:   add your handle you want after final wear on ***************/
        // }
    }
    #endif
}

void GhMultiSensorWearDetStart(void)
{
    GH3X2X_DEBUG_LOG("[GhMultiSensorWearDetStart] Multi Sensor Wear Detect Start\r\n");
    Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_ADT);

    GhMultiSensorTimerInit();
    GhMultiSensorTimerOpsRegister(&g_stGhMultiSensorTimerOps);
    GhMultiSensorTimerStart();

    GhMultSensorWearEventManagerInit();
    GhMultSensorWearEventManagerHookRegister(GhMultiSensorEventHook);
    GhMultSensorWearEventManagerEnable();

    GhGsMoveDetecterInit();
    GhGsMoveDetecterEnable();
}
#endif

#if (__SUPPORT_HARD_ADT_CONFIG__)
/**
 * @fn     extern void Gh3x2x_WearEventHook(GU16 usGotEvent, GU8 uchWearOffType);
 * 
 * @brief  Wear event hook
 *
 * @attention   None        
 *
 * @param[in]   wear event
 * @param[in]   wear off type  0: no object  1: nonliving object    wear on type   0: object     1: living object
 * @param[out]  None
 *
 * @return  None
 */
extern GU8 g_uchNeedStartMultiSensorWearOn;
void Gh3x2x_WearEventHook(GU16 usGotEvent, GU8 uchExentEx)
{
    if (usGotEvent & GH3X2X_IRQ_MSK_WEAR_OFF_BIT)
    {
        GOODIX_PLATFORM_WEAR_OFF_EVENT();
        if(uchExentEx&GH3X2X_EVENT_EX_BIT_WEAR_LIVING_TYPE)
        {
            GOODIX_PLATFORM_NONLIVING_WEAR_OFF_EVENT();
            GH3X2X_INFO_LOG("Wear off, no living-object!!!\r\n");
    #if (__GH_MULTI_SENSOR_WEAR_EXAMPLE_EN__&&__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)
            Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SOFT_ADT_GREEN);
            GhMultiSensorWearDetStart();
    #endif
        }
        else
        {
            GH3X2X_INFO_LOG("Wear off, no object!!!\r\n");
    #if (__GH_MULTI_SENSOR_WEAR_EXAMPLE_EN__&&__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)
            if(0 == GhMultSensorWearEventManagerIsEnable())   // multi-sensor wear detection is not opened
            {
                GH3X2X_DEBUG_LOG("[Gh3x2x_WearEventHook] no object!!! stop hr and soft_adt\r\n");
                Gh3x2xDemoStopSampling(GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SOFT_ADT_GREEN);
            }
    #endif
    #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            GhMultSensorWearEventSend(GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_OFF);
    #endif
        }
    }
    else if (usGotEvent & GH3X2X_IRQ_MSK_WEAR_ON_BIT)
    {
        GOODIX_PLATFORM_WEAR_ON_EVENT();

        if(uchExentEx&GH3X2X_EVENT_EX_BIT_WEAR_LIVING_TYPE)
        {
            GH3X2X_INFO_LOG("Wear on, living-object!!!\r\n");
        }
        else
        {
            GH3X2X_INFO_LOG("Wear on, object !!!\r\n");

    #if (__GH_MULTI_SENSOR_WEAR_EXAMPLE_EN__&&__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)
            if(0 == GhMultSensorWearEventManagerIsEnable())   // multi-sensor wear detection is not opened
            {
                Gh3x2xDemoStartSampling(GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SOFT_ADT_GREEN);  // open main function
            }
    #endif
    #if __GH_MULTI_SENSOR_EVENT_PRO_CONIG__
            GhMultSensorWearEventSend(GH3X2X_MULTI_SENSOR_EVENT_GH_WEAR_ON);
    #endif
        }
    }
}
#endif

#if (__GH3X2X_CASCADE_EN__)
void Gh3x2x_WearEventCascadeEcgHandle(GU16 usGotEvent)
{
    if (usGotEvent & GH3X2X_IRQ_MSK_WEAR_OFF_BIT)
    {
        if (GH3X2X_CascadeGetEcgEnFlag())
        {
            GH3X2X_CascadeEcgSlaverLeadDectDis();
        }
    }
    else if (usGotEvent & GH3X2X_IRQ_MSK_WEAR_ON_BIT)
    {
        if (GH3X2X_CascadeGetEcgEnFlag())
        {
            GH3X2X_CascadeEcgSlaverLeadDectEn();
        }
    }
}
#endif

void Gh3x2x_BeforeWakeUpHook(void)
{
    GOODIX_PLATFORM_BEFORE_WAKE_UP_HOOK_ENTITY();
}

void Gh3x2x_WakeUpHook(void)
{
    GOODIX_PLATFORM_WAKE_UP_HOOK_ENTITY();
}
