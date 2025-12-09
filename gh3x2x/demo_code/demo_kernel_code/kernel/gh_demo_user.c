/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_user.c
 * 
 * @brief   gh3x2x driver lib demo code that user defined
 * 
 * @author  
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh_drv.h"
#include "gh_demo_config.h"
#include "gh_demo_inner.h"
#include "gh_demo.h"
//#include "drivers/hrm/gh3x2x/hrm_gh3x2x.h"

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x2x_demo_algo_call.h"
#endif

#if (__GH3X2X_MP_MODE__)
#include "gh3x2x_mp_common.h"
#endif

#if ( __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
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
void hal_gh3x2x_i2c_init(void)
{
    /* code implement by user */
    GOODIX_PLATFORM_I2C_INIT_ENTITY();

    extern void gh3026_i2c_init(void);
    gh3026_i2c_init();
}

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
GU8 hal_gh3x2x_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
{
    uint8_t ret = 1;

    /* code implement by user */
    GOODIX_PLATFORM_I2C_WRITE_ENTITY(device_id, write_buffer,length);

    extern void gh3026_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length);
    gh3026_i2c_write(device_id, write_buffer, length);

    return ret;
}

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
GU8 hal_gh3x2x_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    uint8_t ret = 1;

    /* code implement by user */
    GOODIX_PLATFORM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length);

    extern void gh3026_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length);
    gh3026_i2c_read(device_id, write_buffer, write_length, read_buffer, read_length);

    return ret;
}

#else // __GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
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

void hal_gh3x2x_spi_init(void)
{
    /* init spi and cs pin */
    GOODIX_PLATFORM_SPI_INIT_ENTITY();
}

/**
 * @fn     GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  hal spi write for gh3x2x
 *
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__  , user need generate timming: write_buf[0](W) + write_buf[1](W) + ...
 * @attention   if __GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__  , user need generate timming: CS LOW  + write_buf[0](W) + write_buf[1](W) + ... + CS HIGH
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write(GU8 write_buffer[], GU16 length)
{
    GU8 ret = 1;
    GOODIX_PLATFORM_SPI_WRITE_ENTITY(write_buffer, length);
    return ret;
}

    #if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
/**
 * @fn     GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi read for gh3x2x
 *
 * @attention   user need generate timming: read_buf[0](R) + write_buf[1](R) + ... 
 *
 * @param[in]   read_length     read data len
 * @param[out]  read_buffer     pointer to read buffer
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;

    GOODIX_PLATFORM_SPI_READ_ENTITY(read_buffer, length);

    return ret;
}
    #elif (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_HARDWARE_CS__)
/**
 * @fn     GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
 * 
 * @brief  hal spi write F1 and read for gh3x2x
 *
 * @attention    user need generate timming: CS LOW + F1(W) + read_buf[0](R) + read_buf[1](R) + ... + CS HIGH
 *
 * @param[in]   write_buf     write data
 * @param[in]   length     write data len
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 hal_gh3x2x_spi_write_F1_and_read(GU8 read_buffer[], GU16 length)
{
    GU8 ret = 1;

    GOODIX_PLATFORM_SPI_READ_ENTITY(read_buffer, length);

    return ret;
}
    #endif

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
    #if (__GH3X2X_SPI_TYPE__ == __GH3X2X_SPI_TYPE_SOFTWARE_CS__) 
void hal_gh3x2x_spi_cs_ctrl(GU8 cs_pin_level)
{
    GOODIX_PLATFORM_SPI_CS_CTRL_ENTITY(cs_pin_level);
}
    #endif
#endif

#if __SUPPORT_HARD_RESET_CONFIG__
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
void hal_gh3x2x_reset_pin_init(void)
{
    GOODIX_PLATFORM_RESET_PIN_INIT_ENTITY();

    extern void gh3026_reset_pin_init(void);
    gh3026_reset_pin_init();
}

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
void hal_gh3x2x_reset_pin_ctrl(GU8 pin_level)
{
   GOODIX_PLATFORM_SPI_RESET_PIN_ENTITY(pin_level);

   extern void gh3026_reset_pin_ctrl(uint8_t pin_level);
   gh3026_reset_pin_ctrl(pin_level);
}
#endif

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
void hal_gh3x2x_int_init(void)
{
    GOODIX_PLATFORM_INT_INIT_ENTITY();

    extern void gh3026_int_pin_init(void);
    gh3026_int_pin_init();
}

#if (__NORMAL_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__ || __MIX_INT_PROCESS_MODE__ == __INTERRUPT_PROCESS_MODE__)
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
void hal_gh3x2x_int_handler_call_back(void)
{
    if (Gh3x2xGetInterruptMode() == __NORMAL_INT_PROCESS_MODE__)
    {
        //GH3X2X_INFO_LOG("g_uchDemoWorkMode=%d", g_uchDemoWorkMode);
        if(GH3X2X_DEMO_WORK_MODE_MPT != g_uchDemoWorkMode)
        {
            g_uchGh3x2xIntCallBackIsCalled = 1;
        }
    #if (__GH3X2X_MP_MODE__)
        GH3X2X_MP_SET_INT_FLAG();  //gh3x2x mp test must call it
    #endif
        GOODIX_PLATFORM_INT_HANDLER_CALL_BACK_ENTITY();
    }
}
#endif

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
void hal_gsensor_start_cache_data(void)
{
#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    GH3X2X_TimestampSyncAccInit();
#endif
    GOODIX_PLATFORM_INT_GS_START_CACHE_ENTITY();
}

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
void hal_gsensor_stop_cache_data(void)
{
    GOODIX_PLATFORM_INT_GS_STOP_CACHE_ENTITY();
}

void hal_cap_start_cache_data(void)
{
    GOODIX_PLATFORM_INT_CAP_START_CACHE_ENTITY();
}

void hal_cap_stop_cache_data(void)
{
    GOODIX_PLATFORM_INT_CAP_STOP_CACHE_ENTITY();
}

void hal_temp_start_cache_data(void)
{
    GOODIX_PLATFORM_INT_TEMP_START_CACHE_ENTITY();
}

void hal_temp_stop_cache_data(void)
{
    GOODIX_PLATFORM_INT_TEMP_STOP_CACHE_ENTITY();
}

void hal_gh3x2x_write_cap_to_flash(GS32 WearCap1,GS32 UnwearCap1,GS32 WearCap2,GS32 UnwearCap2)
{
    GOODIX_PLATFORM_WRITE_CAP_TO_FLASH_ENTITY(WearCap1, UnwearCap1, WearCap2, UnwearCap2);
}
void hal_gh3x2x_read_cap_from_flash(GS32* WearCap1,GS32* UnwearCap1,GS32* WearCap2,GS32* UnwearCap2)
{
    GOODIX_PLATFORM_READ_CAP_FROM_FLASH_ENTITY(WearCap1, UnwearCap1, WearCap2, UnwearCap2);
}

/**
 * @fn     void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
 * 
 * @brief  get gsensor fifo data
 *
 * @attention   When read fifo data of GH3x2x, will call this function to get corresponding cached gsensor data.
 *
 * @param[in]   None
 * @param[out]  gsensor_buffer          pointer to gsensor data buffer
 * @param[out]  gsensor_buffer_index    pointer to number of gsensor data(every gsensor data include x,y,z axis data)
 *
 * @return  None
 */
void hal_gsensor_drv_get_fifo_data(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index)
{
/**************************** WARNNING  START***************************************************/
/*  (*gsensor_buffer_index) can not be allowed bigger than __GSENSOR_DATA_BUFFER_SIZE__  ****************/
/* Be care for copying data to gsensor_buffer, length of gsensor_buffer is __GSENSOR_DATA_BUFFER_SIZE__ *****/
/**************************** WARNNING END*****************************************************/

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
    for (int i = 0; i < *gsensor_buffer_index; i++)
    {
        GU32 unTimeStamp = 0;
        /* user set time stamp code here */
        GH3X2X_TimestampSyncFillAccSyncBuffer(unTimeStamp, gsensor_buffer[i].sXAxisVal, gsensor_buffer[i].sYAxisVal, gsensor_buffer[i].sZAxisVal);
    }
#endif

    GOODIX_PLATFORM_INT_GET_GS_DATA_ENTITY(gsensor_buffer, gsensor_buffer_index);
    extern void gh3026_gsensor_data_get(STGsensorRawdata gsensor_buffer[], GU16 *gsensor_buffer_index);
    gh3026_gsensor_data_get(gsensor_buffer, gsensor_buffer_index);

/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---START***************************************************/
    if((*gsensor_buffer_index) > (__GSENSOR_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
/**************************** WARNNING: DO NOT REMOVE OR MODIFY THIS CODE   ---END***************************************************/
}

void hal_cap_drv_get_fifo_data(STCapRawdata cap_data_buffer[], GU16 *cap_buffer_index)
{
    GOODIX_PLATFORM_INT_GET_CAP_DATA_ENTITY(cap_data_buffer, cap_buffer_index)  ;
    if((*cap_buffer_index) > (__CAP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

void hal_temp_drv_get_fifo_data(STTempRawdata temp_data_buffer[], GU16 *temp_buffer_index)
{
    GOODIX_PLATFORM_INT_GET_TEMP_DATA_ENTITY(temp_data_buffer,temp_buffer_index);
    if((*temp_buffer_index) > (__TEMP_DATA_BUFFER_SIZE__))
    {
        while(1);   // Fatal error !!!
    }
}

#if (__GH3X2X_INFO_LOG_TYPE__)
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

#if __GH3X2X_INFO_LOG_TYPE__ == __GH3X2X_INFO_LOG_METHOD_0__
void GH3X2X_PlatformLog(GCHAR *log_string)
{
    GOODIX_PLATFORM_LOG_ENTITY(log_string);
}
#endif

#if __GH3X2X_INFO_LOG_TYPE__ == __GH3X2X_INFO_LOG_METHOD_1__
void GH3X2X_RegisterPrintf(int (**pPrintfUser)(const char *format, ...))
{
    //(*pPrintfUser) = printf;   //use printf in <stdio.h>  or use equivalent function in your platform
    GOODIX_PLATFORM_PRINTF_ENTITY();
}
    #endif
#endif

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
extern void delay_us(uint32_t us);
void Gh3x2x_BspDelayUs(GU16 usUsec)
{
    GOODIX_PLATFORM_DELAY_US_ENTITY(usUsec);
    
    delay_us(usUsec);
}

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
void Gh3x2x_BspDelayMs(GU16 usMsec)
{
    GOODIX_PLATFORM_DELAY_MS_ENTITY(usMsec);
    delay_us(usMsec * 1000);
}

#if (__GH_MULTI_SENSOR_EVENT_PRO_CONIG__)
void Gh3x2x_CreateMultiSensorTimer(GU32 unPeriodMs)
{
    GOODIX_PLATFORM_CREAT_ADT_CONFIRM_ENTITY(unPeriodMs);
}

/**
 * @fn     void Gh3x2x_StartMultiSensorTimer(void)
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
void Gh3x2x_StartMultiSensorTimer(void)
{
    GH3X2X_DEBUG_LOG("Gh3x2x_StartMultiSensorTimer\r\n");
    GOODIX_PLATFORM_START_TIMER_ENTITY();
}

/**
 * @fn     void Gh3x2x_StopMultiSensorTimer(void)
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
void Gh3x2x_StopMultiSensorTimer(void)
{
    GH3X2X_DEBUG_LOG("Gh3x2x_StopMultiSensorTimer\r\n");
    GOODIX_PLATFORM_STOP_TIMER_ENTITY();
}

void Gh3x2x_DeleteMultiSensorTimer(void)
{
    GOODIX_PLATFORM_DEL_TIMER_ENTITY();
}

const STGhMultiSensorTimerOps g_stGhMultiSensorTimerOps =
{
    .TimerCreate = Gh3x2x_CreateMultiSensorTimer,
    .TimerStart = Gh3x2x_StartMultiSensorTimer,
    .TimerStop = Gh3x2x_StopMultiSensorTimer,
    .TimerDelete = Gh3x2x_DeleteMultiSensorTimer,
};
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
void Gh3x2x_UserHandleCurrentInfo(void)
{
    //read or write  slotx current
    //GH3X2X_GetSlotLedCurrent(0,0); //read slot 0  drv 0
    //GH3X2X_GetSlotLedCurrent(1,0); // read  slot 1  drv 0
    //GH3X2X_SlotLedCurrentConfig(0,0,50);  //set slot0 drv0 50 LSB
    //GH3X2X_SlotLedCurrentConfig(1,0,50);  //set slot1 drv0 50 LSB
}

#if (__SUPPORT_PROTOCOL_ANALYZE__)
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
void Gh3x2x_HalSerialSendData(GU8* uchTxDataBuf, GU16 usBufLen)
{
    GOODIX_PLATFORM_SERIAL_SEND_ENTITY(uchTxDataBuf, usBufLen);
#ifdef GH3X2X_BLE_NOTIFY_DATA
	extern void gh3x2x_ble_notify(const uint8_t* p_data, uint32_t data_len);
	gh3x2x_ble_notify(uchTxDataBuf, usBufLen);
#endif
}

/**
 * @fn      void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
 *
 * @brief  Gh3x2xSerialSendTimerInit
 *
 * @attention   None
 *
 * @param[in]   uchPeriodMs    timer period (ms)
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerInit(GU8 uchPeriodMs)
{
    //UNUSED_VAR(uchPeriodMs);
	extern void gh3x2x_timer_init(uint32_t period_ms);
	gh3x2x_timer_init(uchPeriodMs);
}

/**
 * @fn     void Gh3x2xSerialSendTimerStop(void)
 *
 * @brief  Serial send timer stop
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStop(void)
{
    GOODIX_PLATFORM_SERIAL_TIMER_STOP_ENTITY();
	extern void gh3x2x_timer_stop(void);
	gh3x2x_timer_stop();
}

/**
 * @fn     void Gh3x2xSerialSendTimerStart(void)
 *
 * @brief  Serial send timer start
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xSerialSendTimerStart(void)
{
    GOODIX_PLATFORM_SERIAL_TIMER_START_ENTITY();
	extern void gh3x2x_timer_start(void);
	gh3x2x_timer_start();
}
#endif

void* Gh3x2xMallocUser(GU32 unMemSize)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
    #ifdef GOODIX_DEMO_PLATFORM
    GOODIX_PLATFORM_MALLOC_USER_ENTITY(unMemSize);
    return malloc(unMemSize);
    #else
    //return malloc(unMemSize);
	UNUSED_VAR(unMemSize);
    return 0;
    #endif
#else
	UNUSED_VAR(unMemSize);
    return 0;
#endif
}

void Gh3x2xFreeUser(void* MemAddr)
{
#if (__USER_DYNAMIC_DRV_BUF_EN__)
    GOODIX_PLATFORM_FREE_USER_ENTITY(MemAddr);
    free(MemAddr);
#else
	UNUSED_VAR(MemAddr);
#endif
}

#if ( __FUNC_TYPE_BP_ENABLE__ && (__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
/**
 * @fn     GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = rawdata_a0,
 *                          buffer[1] = rawdata_a1,
 *                          buffer[2] = pressure g value of rawdata_a1,
 *
 * @return  error code
 */
GS8 gh3x2x_write_pressure_parameters(GS32 *buffer)
{
    return GOODIX_PLATFORM_PRESSURE_PARAS_WRITE_ENTITY(buffer);
}

/**
 * @fn     GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
 * 
 * @brief  gh3x2x get pressure value
 *
 * @attention   None
 *
 * @param[out]   buffer     buffer[0] = area,
 *                          buffer[1] = rawdata_a0,
 *                          buffer[2] = rawdata_a1,
 *                          buffer[3] = pressure g value,
 *
 * @return  error code
 */
GS8 gh3x2x_read_pressure_parameters(GS32 *buffer)
{
    return GOODIX_PLATFORM_PRESSURE_PARAS_READ_ENTITY(buffer);
}
#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
