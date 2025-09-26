/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_mp_user.c
 * 
 * @brief   gh3x2x mp code must implemented interface by user
 * 
 * @author  Goodix Product Tools Development Team
 * 
 */

/* includes */
#include "stdint.h"
#include "string.h"
#include "gh3x2x_mp_config.h"
#include "gh3x2x_mp_common.h"

#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )

	//use drv lib some interface
	
#else //__GH3X2X_MP_USE_DRV_INTERFACE__ == 0, use mp lib interface define

#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_MP_INTERFACE_I2C__ )

/* i2c interface */
/**
 * @fn     void gh3x2x_mp_i2c_init(void)
 * 
 * @brief  i2c init for gh3x2x mp
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_i2c_init(void)
{

    /* code implement by user */
    GH3X2X_MP_LOG_L2("gh3x2x_mp i2c start init!");
	
	GOODIX_MP_PLATFORM_I2C_INIT_ENTITY();
	
	
}

/**
 * @fn    GU8 gh3x2x_mp_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length)
 * 
 * @brief  i2c write for gh3x2x mp
 *
 * @attention   
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 gh3x2x_mp_i2c_write(GU8 device_id, const GU8 write_buffer[], GU16 length);
{
    GU8 ret = 1;

    /* code implement by user */
	
	GOODIX_MP_PLATFORM_I2C_WRITE_ENTITY(device_id, write_buffer, length);

    return ret;
}


/**
 * @fn     uint8_t gh3x2x_mp_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length,
 *                            uint8_t read_buffer[], uint16_t read_length)
 * 
 * @brief  i2c read for gh3x2x mp
 *
 * @attention   
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
GU8 gh3x2x_mp_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length)
{
    GU8 ret = 1;

    /* code implement by user */
	
	GOODIX_MP_PLATFORM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length);
    
	return ret;
}

/**
 * @fn    GU8 gh3x2x_mp_i2c_send_cmd(GU8 device_id, GU8 uchCmd)
 * 
 * @brief  i2c send cmd to gh3x2x mp
 *
 * @attention  
 *
 * @param[in]   GU8 device_id, GU8 uchCmd
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 gh3x2x_mp_i2c_send_cmd(GU8 device_id, GU8 uchCmd)
{
	GU8 ret = 1;
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_I2C_SEND_CMD_ENTITY(device_id, uchCmd);
	
	return ret;
}

#else // __GH3X2X_MP_INTERFACE__ == __GH3X2X_MP_INTERFACE_SPI__

/* spi interface */
/**
 * @fn     void gh3x2x_mp_spi_init(void)
 * 
 * @brief  spi init for gh3x2x mp
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_spi_init(void)
{
	/* code implement by user */
	GH3X2X_MP_LOG_L2("gh3x2x_mp spi start init! \r\n");
	
	GOODIX_MP_PLATFORM_SPI_INIT_ENTITY();
}

/**
 * @fn     GU8 gh3x2x_mp_spi_write(GU8 write_buffer[], GU16 length)
 * 
 * @brief  spi write for gh3x2x mp
 *
 * @param[in]   write_buffer    write data buffer
 * @param[in]   length          write data len
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 gh3x2x_mp_spi_write(GU8 write_buffer[], GU16 length)
{
	GU8 ret = 1;
	/* code implement by user */

	
	GOODIX_MP_PLATFORM_SPI_WRITE_ENTITY(write_buffer, length);

    return ret;
}

 
/**
 * @fn     GU8 gh3x2x_mp_spi_read(GU8 uchWriteBuffer[], GU16 usWriteLength, GU8 uchReadBuffer[], GU16 usReadLength)
 * 
 * @brief  spi read for gh3x2x mp
 *
 * @attention   
 *
 * @param[in]   GU8 uchWriteBuffer[], GU16 usWriteLength
 * @param[out]  GU8 uchReadBuffer[], GU16 usReadLength
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 gh3x2x_mp_spi_read(GU8 uchWriteBuffer[], GU16 usWriteLength, GU8 uchReadBuffer[], GU16 usReadLength)
{
	GU8 ret = 1;
	
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_SPI_READ_ENTITY(uchWriteBuffer, usWriteLength, uchReadBuffer, usReadLength);

    return ret;
}


/**
 * @fn    GU8 gh3x2x_mp_spi_send_cmd(GU8 uchCmd)
 * 
 * @brief  spi send cmd to gh3x2x
 *
 * @attention  
 *
 * @param[in]   GU8 uchCmd
 * @param[out]  None
 *
 * @return  status
 * @retval  #1      return successfully
 * @retval  #0      return error
 */
GU8 gh3x2x_mp_spi_send_cmd(GU8 uchCmd)
{
	GU8 ret = 1;
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_SPI_SEND_CMD_ENTITY(uchCmd);
	
	return ret;
	
}

#endif // __GH3X2X_MP_INTERFACE__


/**
 * @fn     void gh3x2x_mp_reset_pin_init(void)
 * 
 * @brief  reset pin init for gh3x2x mp
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_reset_pin_init(void)
{
    /* code implement by user */
	GH3X2X_MP_LOG_L2("gh3x2x_mp reset pin start init! \r\n");
	
	GOODIX_MP_PLATFORM_RESET_PIN_INIT_ENTITY();
}

/**
 * @fn     void gh3x2x_mp_reset_pin_ctrl(GU8 pin_level)
 * 
 * @brief  reset pin ctrl for gh3x2x
 *
 * @attention   pin level set 1 [high level] or 0 [low level]
 *
 * @param[in]   pin_level     reset pin level
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_reset_pin_ctrl(GU8 pin_level)
{
   /* code implement by user */
   
   GOODIX_MP_PLATFORM_RESET_PIN_CTRL_ENTITY(pin_level);	
	
}

/**
 * @fn     void gh3x2x_mp_int_init(void)
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
void gh3x2x_mp_int_init(void)
{
    /* code implement by user */
	GH3X2X_MP_LOG_L2("gh3x2x_mp int pin start init! \r\n");
	
	GOODIX_MP_PLATFORM_INT_INIT_ENTITY();

}

/**
 * @fn     void gh3x2x_mp_int_handler_call_back(void)
 * 
 * @brief  call back of gh3x2x int handler, mp lib need make GH3x2xMp_IntFlag = 1
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_int_handler_call_back(void)
{
/****************** DO NOT MODITY ****************************************************************/
	GH3x2xMp_IntFlag = 1; //need do this
/****************** DO NOT MODITY ****************************************************************/
	
	GOODIX_MP_PLATFORM_INT_HANDLER_CALL_BACK_ENTITY();//do nothing currently
}

#endif //__GH3X2X_MP_USE_DRV_INTERFACE__

/**
 * @fn     void gh3x2x_mp_led_power_pin_init(void)
 * 
 * @brief  init led power pin
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_led_power_pin_init()
{
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_LED_POWER_PIN_INIT_ENTITY();
}

/**
 * @fn     gh3x2x_mp_led_power_on(void)
 * 
 * @brief  led power on
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_led_power_on()
{
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_LED_POWER_ON_ENTITY();
}

/**
 * @fn     void gh3x2x_mp_led_power_off(void)
 * 
 * @brief  led power off
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_led_power_off(void)
{
	/* code implement by user */
	
	GOODIX_MP_PLATFORM_LED_POWER_OFF_ENTITY();
}

/**
 * @fn     void gh3x2x_mp_prompt_message(GU8 uchMsg[], GU16 usMsglen)
 * 
 * @brief  prompt message to do something
 *
 * @attention   None
 *
 * @param[in]   GU8 uchMsg[], GU16 usMsglen
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_prompt_message(GU8 uchMsg[], GU16 usMsglen)
{
	/* code implement by user */
	
	GH3X2X_MP_LOG_L2("gh3x2x_mp prompt message: %s! \r\n", uchMsg);
	
	if(0 == memcmp(uchMsg, PPG_PROMPT_UP_RUBBER_MSG, usMsglen))
		GOODIX_MP_PLATFORM_PROMPT_UP_RUBBER_ENTITY(); /* code implement by user */
	
	//test is OK msg
	if(0 == memcmp(uchMsg, GH3X2X_MP_TEST_OK_MSG, usMsglen))
		GOODIX_MP_PLATFORM_PROMPT_TEST_OK_ENTITY(); /* code implement by user */
	
	//test is NG msg
	if(0 == memcmp(uchMsg, GH3X2X_MP_TEST_NG_MSG, usMsglen))
		GOODIX_MP_PLATFORM_PROMPT_TEST_NG_ENTITY(); /* code implement by user */
}

/**
 * @fn     GU8 gh3x2x_mp_ecg_uart_send(GU8 uchSendBuffer[], GU16 usLength)
 * 
 * @brief  Communicates with the ECG signal generator board through uart
 *
 * @attention	None
 *
 * @param[in]   GU8 uchSendBuffer[], GU16 usLength
				
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_mp_ecg_uart_send(GU8 uchSendBuffer[], GU16 usLength)
{
	GU8 ret = 1;
	/* code implement by user */
	

	ret = GOODIX_MP_PLATFORM_ECG_UART_SEND_ENTITY(uchSendBuffer, usLength);
	
	return ret;
	
}

/**
 * @fn     GU8 gh3x2x_mp_ecg_uart_receive(GU8 uchRevBuffer[], GU16 Timeout_ms)
 * 
 * @brief  Communicates with the ECG signal generator board through uart
 *
 * @attention	None
 *
 * @param[in]   GU8 uchRevBuffer[], GU16 Timeout_ms
				
 * @param[out]  None
 *
 * @return  usLength: The length of the data rev
 */
GU16 gh3x2x_mp_ecg_uart_receive(GU8 uchRevBuffer[], GU16 usTimeout_ms)
{
	GU16 usRevLength = 0;
	/* code implement by user */
	
	usRevLength = GOODIX_MP_PLATFORM_ECG_UART_RECEIVE_ENTITY(uchRevBuffer, usTimeout_ms);
	
	return usRevLength;
}

/**
 * @fn     GU8 gh3x2x_mp_ecg_signal_and_io_status_set(emECG_SIGNAL_AND_IO_STATUS_SET emECG_Status, stGh3x2xMp_ECGSignalSetting stECGSignalSetting, emECG_SET_SIGNAL_SPEC uchSpec)
 * 
 * @brief  Set ECG signal and IO status according to ECG test item
 *
 * @attention
 *
 * @param[in]   emECG_SIGNAL_AND_IO_STATUS_SET emECG_Status, stGh3x2xMp_ECGSignalSetting stECGSignalSetting, emECG_SET_SIGNAL_SPEC uchSpec
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_mp_ecg_signal_and_io_status_set(emECG_SIGNAL_AND_IO_STATUS_SET emECG_Status, stGh3x2xMp_ECGSignalSetting stECGSignalSetting, emECG_SET_SIGNAL_SPEC uchSpec)
{
	GU8 ret = 1;
	/* code implement by user */
	
	ret = GOODIX_MP_PLATFORM_ECG_SIGNAL_AND_IO_STATUS_SET_ENTITY(emECG_Status, stECGSignalSetting, uchSpec);
	
	return ret;
}

/**
 * @fn     void gh3x2x_mp_log(GCHAR *log_string)
 * 
 * @brief  for print mp log
 *
 * @attention   this function must define that print mp test log
 *
 * @param[in]   log_string      pointer to log string
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_log(GCHAR *log_string)
{
	
#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		GH3X2X_PlatformLog(log_string);
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		GH3X2X_AFE_Log(log_string);
	#endif
#else
	GOODIX_MP_PLATFORM_LOG_ENTITY(log_string);
#endif

}

/**
 * @fn     void gh3x2x_mp_delay_us(GU16 usUsec)
 * 
 * @brief  Delay in us,user must register this function
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_delay_us(GU16 usUsec)
{
	
#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		Gh3x2x_BspDelayUs(usUsec);
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		hal_gh3x2x_afe_DelayUs(usUsec);
	#endif
#else
	GOODIX_MP_PLATFORM_DELAY_US_ENTITY(usUsec);
#endif
	
}


/**
 * @fn     void Gh3x2x_mp_delay_ms(GU16 usMsec)
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
void gh3x2x_mp_delay_ms(GU16 usMsec)
{
	
#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		Gh3x2x_BspDelayMs(usMsec);
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		hal_gh3x2x_afe_DelayMs(usMsec);
	#endif
#else
	GOODIX_MP_PLATFORM_DELAY_MS_ENTITY(usMsec);
#endif
	

}



/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
