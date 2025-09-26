/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_demo_inner.h
 *
 * @brief   gh3x2x driver lib demo inner header file
 *
 * @author  Gooidx Iot Team
 *
 */

#ifndef _GH_DEMO_INNER_H_
#define _GH_DEMO_INNER_H_
#include "gh_demo_config.h"
#include "gh_drv_control.h"


#ifdef GOODIX_DEMO_PLATFORM
#include "gh3x2x_demo_goodix_platform.h"
#else
#define GOODIX_PLATFORM_SAMPLING_START_HOOK_ENTITY()
#define GOODIX_PLATFORM_SAMPLING_STOP_HOOK_ENTITY()
#define GOODIX_PLATFORM_SPI_WRITE_ENTITY(write_buffer, length)
#define GOODIX_PLATFORM_PROTOCOL_ANALYZE()
#define GOODIX_PLATFORM_REPORT_EVRENT_ENTITY()
#define GOODIX_PLATFORM_SPI_READ_ENTITY(read_buffer, length)
#define GOODIX_PLATFORM_SPI_CS_CTRL_ENTITY(cs_pin_level)
#define GOODIX_PLATFORM_SPI_RESET_PIN_ENTITY(pin_level)
#define GOODIX_PLATFORM_INT_HANDLER_CALL_BACK_ENTITY()
#define GOODIX_PLATFORM_INT_GS_START_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_GS_STOP_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_GET_GS_DATA_ENTITY(gsensor_buffer, gsensor_buffer_index)
#define GOODIX_PLATFORM_LOG_ENTITY(log_string)
#define GOODIX_PLATFORM_DELAY_US_ENTITY(usUsec)
#define GOODIX_PLATFORM_DELAY_MS_ENTITY(usMsec)
#define GOODIX_PLATFORM_SERIAL_SEND_ENTITY(uchTxDataBuf, usBufLen)
#define GOODIX_PLATFORM_INT_INIT_ENTITY()
#define GOODIX_PLATFORM_RESET_PIN_INIT_ENTITY()
#define GOODIX_PLATFORM_SPI_INIT_ENTITY()
#define GOODIX_PLATFORM_CREAT_ADT_CONFIRM_ENTITY(unPeriodMs)
#define GOODIX_PLATFORM_START_TIMER_ENTITY()
#define GOODIX_PLATFORM_STOP_TIMER_ENTITY()
#define GOODIX_PLATFORM_DEL_TIMER_ENTITY()
#define GOODIX_PLATFORM_WEAR_ON_EVENT()
#define GOODIX_PLATFORM_WEAR_OFF_EVENT()
#define GOODIX_PLATFORM_NONLIVING_WEAR_OFF_EVENT()
#define GOODIX_PLATFORM_LEAD_ON_EVENT()
#define GOODIX_PLATFORM_LEAD_OFF_EVENT()
#define GOODIX_PLATFORM_HR_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFORM_SPO2_RESULT_REPORT_ENTITY()
#define GOODIX_PLATFORM_SERIAL_TIMER_STOP_ENTITY()
#define GOODIX_PLATFORM_SERIAL_TIMER_START_ENTITY()
#define GOODIX_PLATFORM_I2C_WRITE_ENTITY(device_id, write_buffer,length)
#define GOODIX_PLATFORM_I2C_INIT_ENTITY()
#define GOODIX_PLATFORM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length)
#define GOODIX_PLATFORM_INT_CAP_START_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_CAP_STOP_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_GET_CAP_DATA_ENTITY(cap_data_buffer, cap_buffer_index)
#define GOODIX_PLATFORM_INT_TEMP_START_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_TEMP_STOP_CACHE_ENTITY()
#define GOODIX_PLATFORM_INT_GET_TEMP_DATA_ENTITY(temp_data_buffer,temp_buffer_index)
#define GOODIX_PLATFORM_WRITE_CAP_TO_FLASH_ENTITY(WearCap1, UnwearCap1, WearCap2, UnwearCap2)
#define GOODIX_PLATFORM_READ_CAP_FROM_FLASH_ENTITY(WearCap1, UnwearCap1, WearCap2, UnwearCap2)
#define GOODIX_PLATFORM_BEFORE_WAKE_UP_HOOK_ENTITY()
#define GOODIX_PLATFORM_WAKE_UP_HOOK_ENTITY()
#define GOODIX_PLATFORM_PT_CONFIG_HOOK_ENTITY()
#define GOODIX_PLATFORM_START_WITH_CONFIRM_HOOK_ENTITY()
#define GOODIX_PLATFORM_PRESSURE_PARAS_READ_ENTITY(buffer)
#define GOODIX_PLATFORM_PRESSURE_PARAS_WRITE_ENTITY(buffer)
#define GOODIX_PLATFORM_PRINTF_ENTITY()
#define GOODIX_PLATFORM_MALLOC_USER_ENTITY(unMemSize)
#define GOODIX_PLATFORM_FREE_USER_ENTITY(MemAddr)
#define GOODIX_PLATFORM_NADT_RESULT_HANDLE_ENTITY()
#define GOODIX_PLATFORM_GOODIX_ALGO_CALL_ENTITY()
#define GOODIX_PLATFORM_INT_DIVIDE_ZERO_ENABLE_ENTITY()
#endif

#define __PROTOCOL_DATA_LEN__       235

#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

typedef struct
{
    GS16 sXAxisVal; // X-Axis Value
    GS16 sYAxisVal; // Y-Axis Value
    GS16 sZAxisVal; // Z-Axis Value
#if __GS_GYRO_ENABLE__
    GS16 sXGyroVal;
    GS16 sYGyroVal;
    GS16 sZGyroVal;
#endif
}STGsensorRawdata;

#if __GH3X2X_MEM_POOL_CHECK_EN__
extern GU8  g_uchGh3x2xMemPollHaveGetCheckSum;
#endif
extern GU8 g_uchDemoWorkMode;
#if (__NORMAL_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
//hal_gh3x2x_int_handler_call_back is called, this value is 1
extern GU8 g_uchGh3x2xIntCallBackIsCalled; 
#endif
/// save function opened of driver lib
extern GU32 g_unDemoFuncMode;
/* config list array */
extern const STGh3x2xInitConfig g_stGh3x2xCfgListArr[];


extern STGh3x2xFifoMonitorInfo g_stFifoMonitorInfo;

/* i2c interface */
/**
 * @fn     void hal_gh3x2x_i2c_init(void)
 * 
 * @brief  hal i2c init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_i2c_init(void);

/**
 * @fn     uint8_t hal_gh3x2x_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length)
 * 
 * @brief  hal i2c write for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length);

/**
 * @fn     uint8_t hal_gh3x2x_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  hal i2c read for gh3x2x
 *
 * @attention   device_id is 8bits, if platform i2c use 7bits addr, should use (device_id >> 1)
 *
 * @param[in]   device_id       device addr
 * @param[in]   write_buffer    write data buffer
 * @param[in]   write_length    write data len
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length);                       

/* spi interface */
/**
 * @fn     void hal_gh3x2x_spi_init(void)
 * 
 * @brief  hal spi init for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_spi_init(void);

/**
 * @fn     GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
extern GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length);

/**
 * @fn     GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x2x
 *
 * @attention   None
 *
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
extern GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length);

extern GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buf[], GU16 length);


/**
 * @fn     void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
 * 
 * @brief  hal spi cs pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   cs_pin_level     spi cs pin level
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level);

/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_reset_pin_init(void);

/**
 * @fn     void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  hal reset pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level);

/**
 * @fn     void hal_gh3x2x_int_init(void)
 * 
 * @brief  gh3x2x int init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gh3x2x_int_init(void);

/**
 * @fn     void hal_gsensor_start_cache_data(void)
 * 
 * @brief  Start cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when start gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gsensor_start_cache_data(void);

/**
 * @fn     void hal_gsensor_stop_cache_data(void)
 * 
 * @brief  Stop cache gsensor data for gh3x2x
 *
 * @attention   This function will be called when stop gh3x2x sampling.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void hal_gsensor_stop_cache_data(void);
extern void hal_cap_start_cache_data(void);
extern void hal_cap_stop_cache_data(void);
extern void hal_temp_start_cache_data(void);
extern void hal_temp_stop_cache_data(void);
/**
 * @fn     void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
 * 
 * @brief  get gsensor fifo data
 *
 * @attention   When read fifo data of GH3x2x,user should call this function to get gsensor data.And user must
 *              assure that these gsensor data is corresponding to GH3x2x fifo data
 *
 * @param[in]   None
 * @param[out]  gsensor_buffer          pointer to gsensor data buffer
 * @param[out]  gsensor_buffer_index    pointer to number of gsensor data
 *
 * @return  None
 */
extern void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index);
extern void hal_cap_drv_get_fifo_data(STCapRawdata cap_data_buffer[], GU16 *cap_buffer_index);
extern void hal_temp_drv_get_fifo_data(STTempRawdata Temp_data_buffer[], GU16 *temp_buffer_index);
/**
 * @fn     void GH3X2X_PlatformLog(char *log_string)
 * 
 * @brief  for debug version, log
 *
 * @attention   this function must define that use debug version lib
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */
extern void GH3X2X_PlatformLog(GCHAR *log_string);
extern void GH3X2X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...));

/**
 * @fn     void Gh3x2x_BspDelayUs(GU16 usUsec)
 * 
 * @brief  Delay in us,user should register this function into driver lib
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_BspDelayUs(GU16 usUsec);

/**
 * @fn     void Gh3x2x_BspDelayMs(GU16 usMsec)
 * 
 * @brief  Delay in ms
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_BspDelayMs(GU16 usMsec);

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
extern void Gh3x2x_LeadOnEventHook(void);

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
extern void Gh3x2x_LeadOffEventHook(void);
/**
 * @fn     void Gh3x2x_BeforeWakeUpHook(void)
 * 
 * @brief  Before WakeUp Hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_BeforeWakeUpHook(void);

/**
 * @fn     void Gh3x2x_WakeUpHook(void)
 * 
 * @brief  WakeUp Hook
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_WakeUpHook(void);

/**
 * @fn     void Gh3x2x_WearEventHook(GU16 usGotEvent, GU8 uchType)
 * 
 * @brief  Wear event hook
 *
 * @attention   None        
 *
 * @param[in]   wear event
 * @param[in]   wear off type  0: no object  1: nonliving object
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_WearEventHook(GU16 usGotEvent, GU8 uchType);
#if (__GH3X2X_CASCADE_EN__)
extern void Gh3x2x_WearEventCascadeEcgHandle(GU16 usGotEvent);
#endif

/**
 * @fn     void Gh3x2x_UserHandleCurrentInfo(void)
 * 
 * @brief  handle gh3x2x chip current information for user
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2x_UserHandleCurrentInfo(void);

/**
 * @fn     void Gh3x2xDemoAlgorithmCalculate(GU8* puchReadFifoBuffer, STGsensorRawdata *pstGsAxisValueArr, 
 *                                              GU16 usGsDataNum, EMGsensorSensitivity emGsSensitivity)
 *
 * @brief  Algorithm calculate.This function will be called in Gh3x2xDemoInterruptProcess()
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer      point to gh3x2x fifo data buffer
 * @param[in]   pstGsAxisValueArr       point to gsensor data buffer
 * @param[in]   usGsDataNum             gsensor data cnt
 * @param[in]   emGsSensitivity         sensitivity of gsensor
 * @param[out]  None
 *
 * @return  None
 */

void Gh3x2xDemoFunctionProcess(GU8* puchReadFifoBuffer, GU16 usFifoBuffLen,STGsensorRawdata *pstGsAxisValueArr, GU16 usGsDataNum,
                                   STCapRawdata* pstCapValueArr,GU16 usCapDataNum,STTempRawdata* pstTempValueArr,GU16 usTempDataNum);

/**
 * @fn     void Gh3x2xDemoSamplingControl(EMUprotocolParseCmdType emSwitch)
 *
 * @brief  Start/stop sampling of gh3x2x and sensor
 *
 * @attention   None
 *
 * @param[in]   unFuncMode      function that will be started or stopped
 * @param[in]   emSwitch        stop/start sampling
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2xDemoSamplingControl(GU32 unFuncMode, EMUprotocolParseCmdType emSwitch);

/// dump mode
extern GU16 g_usDumpMode;
extern GU8 g_uchGh3x2xRegCfgArrIndex;
extern GU16 g_usCurrentConfigListFifoWmBak;
extern void GH3X2X_LedAgcProcessExDump(GU8* puchReadFifoBuffer);
extern void GH3X2X_ReadElectrodeWearDumpData(void);
extern void GH3X2X_RecordDumpData(void);
extern void gh3x2x_init_hook_func(void);
extern void gh3x2x_sampling_start_hook_func(void);
extern void gh3x2x_sampling_stop_hook_func(void);
extern void gh3x2x_get_rawdata_hook_func(GU8 *read_buffer_ptr, GU16 length);
extern void gh3x2x_algorithm_get_io_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo);
extern void gh3x2x_frame_data_hook_func(const STGh3x2xFrameInfo * const pstFrameInfo);
extern void gh3x2x_reset_by_protocol_hook(void);
extern void gh3x2x_config_set_start_hook(void);
extern void gh3x2x_config_set_stop_hook(void);
extern void gh3x2x_write_algo_config_hook(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

/**
 * @fn     void Gh3x2xDemoReportEvent(GU16 usEvent,GU8 uchWearType)
 *
 * @brief  Report event to APP/EVK
 *
 * @attention   None
 *
 * @param[in]   usEvent      GH3x2x event
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2xDemoReportEvent(GU16 usEvent,GU8 uchWearType);

/**
 * @fn     void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
 *
 * @brief  Serial send data
 *
 * @attention   None
 *
 * @param[in]   uchTxDataBuf        pointer to data buffer to be transmitted
 * @param[in]   usBufLen            data buffer length
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen);

/**
 * @fn     void Gh3x2x_StartAdtConfirmTimer(void)
 * 
 * @brief  Start time of adt confirm to get g sensor
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2xSerialSendTimerStop(void);
extern void Gh3x2xSerialSendTimerStart(void);
extern void Gh3x2x_StartMultiSensorTimer(void);
extern void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs);
/**
 * @fn     void Gh3x2x_StopAdtConfirmTimer(void)
 * 
 * @brief  Stop time of adt confirm
 *
 * @attention   None        
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
extern void Gh3x2x_StopMultiSensorTimer(void);
extern void Gh3x2xDemoSendWearOrLeadStateToUI(GU8 state);
extern void Gh3x2x_HalSerialFifoInit(void);
extern void Gh3x2xMemPoolCheckSumWrite(GU8 *lunMemPool);
extern GU16 Gh3x2xMemPoolCheckSumCheck(GU8 *lunMemPool);
extern void Gh3x2xCheckMemPollBeforeAlgoCal(void);
extern void Gh3x2xUpdataMemPollChkSumAfterAlgoCal(void);
extern void GH3x2xMaserControlFunctionLog(GU32 unFuncMode, EMUprotocolParseCmdType emCmdType);
#if ( __FUNC_TYPE_BP_ENABLE__ && (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
extern GS8 gh3x2x_write_pressure_parameters(GS32 *buffer);
extern GS8 gh3x2x_read_pressure_parameters(GS32 *buffer);
#endif
extern void* Gh3x2xMallocUser(GU32 unMemSize);
extern void Gh3x2xFreeUser(void* MemAddr);

extern void Gh3x2xDemoStopAlgoInner(GU32 unFuncMode);
extern void Gh3x2xDemoStartAlgoInner(GU32 unFuncMode);

extern void Gh3x2xDemoMoveDetectTimerHandler(GU32 unTimeIntervalMs);


#endif /* _GH3X2X_DEMO_INNER_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
