/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_common.h
 *
 * @brief   header file of gh3x2x mp test common port
 *
 * @author  Goodix Product Tools Development Team
 *
 */
 
#ifndef _GH3X2X_MP_COMMON_H_
#define _GH3X2X_MP_COMMON_H_

#include "gh3x2x_mp_config.h"
#include "gh3x2x_mp_define.h"
#include <stdio.h>

#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		#include "gh_demo_inner.h"
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		#include "gh3x2x_afe_drv.h"
	#endif
	
#endif

#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
#define GH3X2X_MP_DRV_LIB_LOG(...)  do{\
                                            if(g_pSnprintfUserIn)\
                                            {\
                                                char gh3x2x_GH3X2X_INFO_LOG[GH3X2X_INFO_LOG_MAX_LEN] = {0};\
                                                g_pSnprintfUserIn(gh3x2x_GH3X2X_INFO_LOG, GH3X2X_INFO_LOG_MAX_LEN, __VA_ARGS__);\
                                                GH3X2X_PlatformLog((GCHAR *)gh3x2x_GH3X2X_INFO_LOG);\
                                            }\
                                            if(g_pPrintfUserIn)g_pPrintfUserIn(__VA_ARGS__);\
                                        } while(0)


#define GH3X2X_MP_LOG_L1_INNER(...)  GH3X2X_MP_DRV_LIB_LOG("[gh3x2x mp error]: "__VA_ARGS__)
#define GH3X2X_MP_LOG_L2_INNER(...)  GH3X2X_MP_DRV_LIB_LOG("[gh3x2x mp info]: "__VA_ARGS__)
#define GH3X2X_MP_LOG_INNER(...)  GH3X2X_MP_DRV_LIB_LOG(__VA_ARGS__)


#else
#define GH3X2X_MP_AFE_LOG(...)  do {\
                                                    char gh3x2x_log_string[GH3X2X_MP_LOG_MAX_LEN] = {0};\
                                                    snprintf(gh3x2x_log_string, GH3X2X_MP_LOG_MAX_LEN, __VA_ARGS__);\
                                                    gh3x2x_mp_log((GCHAR *)gh3x2x_log_string);\
                                    } while(0)



#define GH3X2X_MP_LOG_L1_INNER(...)  GH3X2X_MP_AFE_LOG("[gh3x2x mp error]: "__VA_ARGS__)
#define GH3X2X_MP_LOG_L2_INNER(...)  GH3X2X_MP_AFE_LOG("[gh3x2x mp info]: "__VA_ARGS__)
#define GH3X2X_MP_LOG_INNER(...)     GH3X2X_MP_AFE_LOG(__VA_ARGS__)


#endif











/* log def*/
#if (__GH3X2X_MP_LOG_LVL__) //log level, =0 not print log

	#define   GH3X2X_MP_LOG_L1(...)			GH3X2X_MP_LOG_L1_INNER(__VA_ARGS__)
	#if(__GH3X2X_MP_LOG_LVL__ > 1)
		#define GH3X2X_MP_LOG_L2(...)		GH3X2X_MP_LOG_L2_INNER(__VA_ARGS__)
	#else
		#define GH3X2X_MP_LOG_L2(...)
	#endif
	
#else //log level == 0, not print log
	#define   GH3X2X_MP_LOG_L1(...)  
	#define   GH3X2X_MP_LOG_L2(...)
#endif

#define   GH3X2X_MP_LOG(...)			GH3X2X_MP_LOG_INNER(__VA_ARGS__)							

extern GU8 GH3x2xMp_IntFlag; //use for mp
#define GH3X2X_MP_SET_INT_FLAG()			{GH3x2xMp_IntFlag = 1;}

#ifdef GOODIX_DEMO_MP_PLANFORM
#include "gh3x2x_demo_mp_goodix_platform.h"
#else
extern void gh3026_i2c_init(void);
extern void gh3026_i2c_write(uint8_t device_id, const uint8_t write_buffer[], uint16_t length);
extern void gh3026_i2c_read(uint8_t device_id, const uint8_t write_buffer[], uint16_t write_length, uint8_t read_buffer[], uint16_t read_length);
extern void gh3026_reset_pin_init(void);
extern void gh3026_reset_pin_ctrl(uint8_t pin_level);
extern void gh3026_int_pin_init(void);

#define GOODIX_MP_PLATFORM_I2C_INIT_ENTITY()        \
    gh3026_i2c_init()
#define GOODIX_MP_PLATFORM_I2C_WRITE_ENTITY(device_id, write_buffer, length)    \
    gh3026_i2c_write(device_id, write_buffer, length)
#define GOODIX_MP_PLATFORM_I2C_READ_ENTITY(device_id, write_buffer, write_length, read_buffer, read_length) \
    gh3026_i2c_read(device_id, write_buffer, write_length, read_buffer, read_length)
#define GOODIX_MP_PLATFORM_I2C_SEND_CMD_ENTITY(device_id, uchCmd)   \
    gh3026_i2c_write(device_id, &uchCmd, 1)
#define GOODIX_MP_PLATFORM_SPI_INIT_ENTITY()
#define GOODIX_MP_PLATFORM_SPI_WRITE_ENTITY(write_buffer, length)
#define GOODIX_MP_PLATFORM_SPI_READ_ENTITY(uchWriteBuffer, usWriteLength, uchReadBuffer, usReadLength)
#define GOODIX_MP_PLATFORM_SPI_SEND_CMD_ENTITY(uchCmd)
#define GOODIX_MP_PLATFORM_RESET_PIN_INIT_ENTITY()      \
    gh3026_reset_pin_init()
#define GOODIX_MP_PLATFORM_RESET_PIN_CTRL_ENTITY(pin_level) \
    gh3026_reset_pin_ctrl(pin_level)
#define GOODIX_MP_PLATFORM_INT_INIT_ENTITY()    \
    gh3026_int_pin_init()
#define GOODIX_MP_PLATFORM_INT_HANDLER_CALL_BACK_ENTITY()
#define GOODIX_MP_PLATFORM_LED_POWER_PIN_INIT_ENTITY()
#define GOODIX_MP_PLATFORM_LED_POWER_ON_ENTITY()
#define GOODIX_MP_PLATFORM_LED_POWER_OFF_ENTITY()
#define GOODIX_MP_PLATFORM_PROMPT_UP_RUBBER_ENTITY()
#define GOODIX_MP_PLATFORM_PROMPT_TEST_OK_ENTITY()
#define GOODIX_MP_PLATFORM_PROMPT_TEST_NG_ENTITY()
#define GOODIX_MP_PLATFORM_ECG_UART_SEND_ENTITY(uchSendBuffer, usLength)										1
#define GOODIX_MP_PLATFORM_ECG_UART_RECEIVE_ENTITY(uchRevBuffer, usTimeout_ms)									1
#define GOODIX_MP_PLATFORM_ECG_SIGNAL_AND_IO_STATUS_SET_ENTITY(emECG_Status, stECGSignalSetting, uchSpec)		1
#define GOODIX_MP_PLATFORM_LOG_ENTITY(log_string)
#define GOODIX_MP_PLATFORM_DELAY_US_ENTITY(usUsec)
#define GOODIX_MP_PLATFORM_DELAY_MS_ENTITY(usMsec)


#endif

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
void gh3x2x_mp_i2c_init(void);

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
GU8 gh3x2x_mp_i2c_read(GU8 device_id, const GU8 write_buffer[], GU16 write_length, GU8 read_buffer[], GU16 read_length);

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
GU8 gh3x2x_mp_i2c_send_cmd(GU8 device_id, GU8 uchCmd);

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
void gh3x2x_mp_spi_init(void);

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
GU8 gh3x2x_mp_spi_write(GU8 write_buffer[], GU16 length);

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
GU8 gh3x2x_mp_spi_read(GU8 uchWriteBuffer[], GU16 usWriteLength, GU8 uchReadBuffer[], GU16 usReadLength);

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
GU8 gh3x2x_mp_spi_send_cmd(GU8 uchCmd);

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
void gh3x2x_mp_reset_pin_init(void);

/**
 * @fn     void gh3x2x_mp_reset_pin_ctrl(GU8 pin_level)
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

void gh3x2x_mp_reset_pin_ctrl(GU8 pin_level);


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
void gh3x2x_mp_int_init(void);

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
void gh3x2x_mp_int_handler_call_back(void);

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
void gh3x2x_mp_led_power_pin_init(void);

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
void gh3x2x_mp_led_power_on(void);

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
void gh3x2x_mp_led_power_off(void);

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
GU8 gh3x2x_mp_ecg_uart_send(GU8 uchSendBuffer[], GU16 usLength);

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
GU16 gh3x2x_mp_ecg_uart_receive(GU8 uchRevBuffer[], GU16 usTimeout_ms);


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
GU8 gh3x2x_mp_ecg_signal_and_io_status_set(emECG_SIGNAL_AND_IO_STATUS_SET emECG_Status, stGh3x2xMp_ECGSignalSetting stECGSignalSetting, emECG_SET_SIGNAL_SPEC uchSpec);

/**
 * @fn     void gh3x2x_mp_prompt_message(GU8 uchMsg[], GU16 usMsglen)
 * 
 * @brief  prompt message to do something
 *
 * @attention   None
 *
 * @param[in]   GU8 uchMsg, GU16 usMsglen
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_mp_prompt_message(GU8 uchMsg[], GU16 usMsglen);

/**
 * @fn     void gh3x2x_mp_log(char *log_string)
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
void gh3x2x_mp_log(GCHAR *log_string);

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

void gh3x2x_mp_delay_us(GU16 usUsec);


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
void gh3x2x_mp_delay_ms(GU16 usMsec);



#endif /* _GH3X2X_MP_COMMON_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
