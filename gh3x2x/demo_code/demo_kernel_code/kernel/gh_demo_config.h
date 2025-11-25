/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_demo_config.h
 *
 * @brief   gh3x2x driver lib demo configuration file
 *
 * @author  Gooidx Iot Team
 *
 */
#ifndef _GH_DEMO_CONFIG_H_
#define _GH_DEMO_CONFIG_H_
#include "gh_drv.h"
#include "stdio.h"


/* bsp config */
/**< interface config __GH3X2X_INTERFACE_I2C__ /__GH3X2X_INTERFACE_SPI__ */
#define __GH3X2X_INTERFACE__                            (__GH3X2X_INTERFACE_I2C__)
#if(__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__)
#define __GH3X2X_SPI_TYPE__                             (__GH3X2X_SPI_TYPE_SOFTWARE_CS__)
#endif
#if(__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__)
#define __GH3X2X_I2C_DEVICE_ID__                        (GH3X2X_I2C_ID_SEL_1L0L)
#endif
#define __GS_SENSITIVITY_CONFIG__                       (GSENSOR_SENSITIVITY_512_COUNTS_PER_G)  /**< gsensor sensitivity config */
#define __GS_GYRO_ENABLE__                              (0)        /*Gyro enable*/
#define __GS_NONSYNC_READ_EN__                          (0)
#if __GS_NONSYNC_READ_EN__
#define __GS_SAMPLE_RATE_HZ__                           (25)        /*gsenor sample rate(Hz)*/
#define __GS_READ_POINT_NUM__                           (6)         /* host read gsensor sample num in one time */
#define __GS_READ_POINT_NUM_JITTER__                    (3)         /* host read gsensor sample num jitter */
#endif

#define __CAP_ENABLE__                                  (0)         /*Cap enable*/
#define __TEMP_ENABLE__                                 (0)         /*Temperature sensor enable*/

#define __SUPPORT_HARD_RESET_CONFIG__                   (1)         /**< support hard reset config */
#define __INTERRUPT_PROCESS_MODE__                      (__NORMAL_INT_PROCESS_MODE__)  /**< 2:polling+int 1:use polling 0:use interrupt */
#define __PLATFORM_WITHOUT_OS__                         (0)         /**< 1:not use os  0:use os */
#define __PASS_THROUGH_MODE__                           (0)
#define __GH3X2X_MP_MODE__                              (0)         /**< 1:enable mp mode  0:unable mp mode */
#define __GH3X2X_CASCADE_EN__                           (0)         /** dual-ecg function enable **/
#if (__GH3X2X_CASCADE_EN__ && __GH3X2X_INTERFACE_I2C__)
#define __GH3X2X_I2C_DEVICE_SLAVER_ID__                 (GH3X2X_I2C_ID_SEL_1H0L)
#endif
#define __GH3X2X_ECG_RTYPE_5M__                         (0)
#define __RESET_AGCINFO_BEFORE_SAMPLE__                 (0)
#define __CURRENT_ECODE_CALIBRATE__                     (0)

/* app config */
#define __USER_DYNAMIC_DRV_BUF_EN__                     (0)        /*get rawdata buf via malloc*/
#define __DRIVER_LIB_MODE__                             (__DRV_LIB_WITH_ALGO__)
#define __DRIVER_OPEN_ALL_SOURCE__                      (0)  //0: do not open all driver source(this mode can support goodix algorithm)   1: open all driver source (this mode may can NOT support goodix algorithm) 
#define __FUNC_RUN_SIMULTANEOUSLY_SUPPORT__             (1)
#define __GH_MSG_WITH_ALGO_LAYER_EN__                   (0)        /** enable message with algorithm send/recive process **/

/* ppg function type */
#define __FUNC_TYPE_HR_ENABLE__                         (1)    /**< hb function tye */
#define __FUNC_TYPE_HRV_ENABLE__                        (1)    /**< hrv function tye */
#define __FUNC_TYPE_HSM_ENABLE__                        (1)    /**< hsm function tye */
#define __FUNC_TYPE_SPO2_ENABLE__                       (1)    /**< spo2 function tye */
#define __FUNC_TYPE_BT_ENABLE__                         (1)    /**< bt function tye */
#define __FUNC_TYPE_RESP_ENABLE__                       (1)    /**< resp function tye */
#define __FUNC_TYPE_AF_ENABLE__                         (1)    /**< af function tye */
#define __FUNC_TYPE_TEST_ENABLE__                       (1)    /**< test function tye */
#define __SUPPORT_HARD_ADT_CONFIG__                     (1)    /**< support hard adt config */
#define __FUNC_TYPE_SOFT_ADT_ENABLE__                   (1)    /**< support soft adt config */
#define __SUPPORT_SOFT_AGC_CONFIG__                     (1)    /**< support soft agc config */

#define __HARD_ADT_ALGO_CHECK_EN__                      (0)   //0: do not use algo to replace hard adt module in chip  1: use algo to replace hard adt mudule in chip
#define __ADT_ONLY_PARTICULAR_WM_CONFIG__               (0)  //0: Do not set particular watermark at openning function is ADT only  N (N>3&&N<800): set particular watermark, and this value is N at openning function is ADT only

/* ecg function type */
#define __FUNC_TYPE_ECG_ENABLE__                        (0)    /**< ecg algorithm tye */
#if __FUNC_TYPE_ECG_ENABLE__
#define __FUNC_TYPE_PWTT_ENABLE__                       (1)    /**< pwtt algorithm tye */
#define __FUNC_TYPE_BP_ENABLE__                         (1)    /**< bp algorithm tye */
#define __SUPPORT_ECG_LEAD_OFF_DET_800HZ__              (1)    
#define GH3X2X_ECG_DOWNSAMP_CHNL_NUM                    (1)    /**< ECG downsample chn number */

#if ( __FUNC_TYPE_HR_ENABLE__)
#define __SUPPORT_PPG_DOWNSAMP__                        (0)    /**< PPG downsample algorithm tye */
#define GH3X2X_PPG_DOWNSAMP_CHNL_NUM                    (0)    /**< PPG downsample chn number */
#endif
#endif

/*lead off parameters setting*/
#define LEADOFF_DET_IQ_AMP_THR_uV                       (1500)
#define LEADOFF_DET_IQ_AMP_DIFF_THR_uV                  (1000)
#define LEADOFF_DET_IQ_VAL_THR_mV                       (80)

/* config about mult-sensor event process*/
#define __GH_MULTI_SENSOR_EVENT_PRO_CONIG__             (1)        /**0: no support multi-sensor event process  1: support multi-sensor event process*/
#define __GH_MULTI_SENSOR_TIMER_PERIOD_MS__             (1000)     /** period of multi-sensor timer, unit: ms*/
#define __GH_MULTI_SENSOR_WEAR_EXAMPLE_EN__             (0)        /** this is example for using multi-senosr process to check wear on, you can set 1 to enable this example process*/

/* config about g-sensor move detection */
#define __GSENSOE_MOVE_DETECT_SUPPORT__                 (1)         /**0: no NOT support move detection 1: support move detection*/
#define __GSENSOR_MOVE_THRESHOLD__                      (65)        /**< DIFF of sqrt(x^2+y^2+z^2) */
#define __GSENSOR_MOVE_CNT_THRESHOLD__                  (1)         /**< (recomended value = 1) more than how many times of movement can be judged as effective moveing*/
#define __GSENSOR_NOT_MOVE_CNT_THRESHOLD__              (150)       /**< (recommended value = 1.5 * sample rate of g-sensor ) more than how many times of movement can be judged as effective non-moveing*/    
#define __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__          (5000)      /**range 1~5000, the time of not move > __GSENSOR_MOVE_TIME_OUT_THRESHOLD_MS__, event : GH3X2X_MULTI_SENSOR_EVENT_GS_MOVE_TIME_OUT will be generated*/

/*sofe agc config*/
#if __SUPPORT_SOFT_AGC_CONFIG__
#define GH3X2X_NEW_AGC_SLOT_NUM_LIMIT                   (8u)
#define GH3X2X_NEW_AGC_SUB_CHNL_NUM_LIMIT               (32u)
#endif

/* marco of divide zero flag enable*/
#define   GH3X2X_DIVIDE_ZERO_FLAG_UPLODE_EN             (1)

/* marco of wake up confirm enable*/
#define   GH3X2X_WAKE_UP_CONFIRM_EN                     (1)

/*sample config*/
#define __SLOT_SEQUENCE_DYNAMIC_ADJUST_EN__             (1)

/*debug config*/
#define __SUPPORT_PROTOCOL_ANALYZE__                    (1)         /**< driver lib support protocol analyze */
#if (__SUPPORT_PROTOCOL_ANALYZE__)
#define __UPLOAD_ALGO_RESULT__                          (1)         /**< upload algorithm result or not */
#define __PROTOCOL_SERIAL_TYPE__                        (__PROTOCOL_SERIAL_USE_BLE__)  /**< protocol communicate serial port type */
#define __SUPPORT_ZIP_PROTOCOL__                        (0)
#define __FIFO_PACKAGE_SEND_ENABLE__                    (0)         /** 1: fifo package send mode enable  0: cannot open fifo package send mode */
#define __GH3X2X_PROTOCOL_SEND_TIMER_PERIOD__           (10)        /** (unit : ms ) protocal data send timer period */
#define __GH3X2X_PROTOCOL_DATA_FIFO_LEN__               (64)        /** protocal data send fifo length **/
#define __GH3X2X_PROTOCOL_EVENT_FIFO_LEN__              (16)        /** protocal event send fifo length **/
#define __GH3X2X_PROTOCOL_EVENT_WAITING_ACK_TIME__      (500)       /** (unit : ms ) protocal data waiting ack time, if time out, we will resend */
#define __GH3X2X_PROTOCOL_EVENT_RESEND_NUM__            (255)       /***** 0~255  protocal resend num (255: evenlasting resending) */
#define __GH3X2X_PROTOCOL_DATA_FUNCTION_INTERCEPT__     (GH3X2X_NO_FUNCTION) /* GH3X2X_NO_FUNCTION: none function date will be intercepted     (GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_HRV):  HR and HRV function data will be intercepted, those data will not output via protocal */
#endif
#define __SUPPORT_SAMPLE_DEBUG_MODE__                   (0)         /**< use sample debug mode */
#define __SUPPORT_ELECTRODE_WEAR_STATUS_DUMP__          (1)         /** use electrode wear status dump */
#define __GH3X2X_INFO_LOG_TYPE__                        (__GH3X2X_INFO_LOG_METHOD_0__) /**< example log config */
#define __GH3X2X_INFO_LOG_LEVEL__                       (__GH3X2X_INFO_LOG_LV_1__|__GH3X2X_INFO_LOG_LV_2__|__GH3X2X_INFO_LOG_LV_3__|__GH3X2X_INFO_LOG_LV_4__) /**< example log config */
#define   GH3X2X_INFO_LOG_MAX_LEN                       (180)
#define   GH3X2X_IODATA_LOG_MAX_LEN                     (512)
#define __SUPPORT_ENGINEERING_MODE__                    (1)         /**< enginerring mode*/
#define __GH3X2X_MEM_POOL_CHECK_EN__                    (0)         /** 1: drv lib checks algo mem pool befor every time of algo calculating   0: do not check  **/

/* operate config */
#define __SUPPORT_FUNCTION_SAMPLE_RATE_MODIFY__         (1)         /** 0: only use default sample rate in array cfg  1: modifying function sample rate is supported via API: Gh3x2xDemoFunctionSampleRateSet */

/*config list*/
#define __GH3X2X_CFG_LIST_MAX_NUM__                     (8)         /**< max num of config list(1 ~ 8) */

/* other config */
#define __SUPPORT_HOOK_FUNC_CONFIG__                    (1)         /**< support hook func config */
#define __SUPPORT_ALGO_INPUT_OUTPUT_DATA_HOOK_CONFIG__  (0)         /**< enable it ,we can get algo input and output data **/
#define __SUPPORT_ALGO_INPUT_OUTPUT_DATA_LOG__          (0)         /**< enable it ,can print algo input and output data **/

/* gh3x2x data buffer size config,this is related to gh3x2x fifo water mark config */
#define __GH3X2X_RAWDATA_BUFFER_SIZE__                  (200 * 4)     /**< rawdata buffer size in byte */
#define __GH3X2X_RAWDATA_SLAVE_BUFFER_SIZE__            (200 * 4)     /**< rawdata buffer size in byte for slave chip*/

/* gsensor data buffer size config,every g sensor data has 6 bytes(x,y,z) */
#define __GSENSOR_DATA_BUFFER_SIZE__                    (300)       /**< max num of gsensor data */

#define __CAP_DATA_BUFFER_SIZE__                        (200)       /**< max num of CAP data */
#define __TEMP_DATA_BUFFER_SIZE__                       (200)       /**< max num of TEMP data */

/* function channel nunber config*/
#define   GH3X2X_ADT_CHNL_NUM                           (8)        /**< ADT function channel num */
#define   GH3X2X_HR_CHNL_NUM                            (32)        /**< HR function channel num */
#define   GH3X2X_HRV_CHNL_NUM                           (16)        /**< HRV function channel num */
#define   GH3X2X_HSM_CHNL_NUM                           (16)        /**< HSM function channel num */
#define   GH3X2X_BP_CHNL_NUM                            (16)        /**< BP function channel num */
#define   GH3X2X_PWA_CHNL_NUM                           (16)        /**< PWA function channel num */
#define   GH3X2X_SPO2_CHNL_NUM                          (32)        /**< SPO2 function channel num */
#define   GH3X2X_ECG_CHNL_NUM                           (8)        /**< ECG function channel num */
#define   GH3X2X_PWTT_CHNL_NUM                          (32)        /**< PWTT function channel num */
#define   GH3X2X_SOFT_ADT_CHNL_NUM                      (16)        /**< soft adt function channel num */
#define   GH3X2X_BT_CHNL_NUM                            (16)        /**< BT function channel num */
#define   GH3X2X_RESP_CHNL_NUM                          (16)        /**< RESP function channel num */
#define   GH3X2X_AF_CHNL_NUM                            (16)        /**< AF function channel num */
#define   GH3X2X_TEST_CHNL_NUM                          (16)        /**< TEST1 function channel num */

/*fifo monitor*/
#define GH3X2X_FIFO_MONITOR_EN                          (1)
#define GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT          (0)

#define GH3X2X_SEND_MSG_ALGO_CFG_INIT(g_pstGh3x2xFrameInfo, g_uchGh3x2xRegCfgArrIndex)         //add your code to send message to algorithm layer
#define GH3X2X_SEND_MSG_ALGO_START(unFuncMode)  //add your code to send message to algorithm layer
#define GH3X2X_SEND_MSG_ALGO_STOP(unFuncMode)   //add your code to send message to algorithm layer
#define GH3X2X_SEND_MSG_ALGO_CAL(unFunctionID)              //add your code to send message to algorithm layer
#define GH3X2X_PRO_MSG_SOFT_WEAR_EVEVT()        // if got soft wear event, GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF/GH3X2X_SOFT_EVENT_WEAR_ON)

/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/
/* log def*/
#if (__GH3X2X_INFO_LOG_LEVEL__ > __GH3X2X_INFO_LOG_LV_0__)
extern void hrm_print_fmt(const char * fmt, ...);
#if (__GH3X2X_INFO_LOG_LEVEL__ & __GH3X2X_INFO_LOG_LV_1__)
#define   GH3X2X_INFO_LOG(fmt, ...)         do{hrm_print_fmt(fmt, ##__VA_ARGS__);}while(0)
#else
#define   GH3X2X_INFO_LOG(...)
#endif

#if (__GH3X2X_INFO_LOG_LEVEL__ & __GH3X2X_INFO_LOG_LV_2__)
#define   GH3X2X_DEBUG_LOG(fmt, ...)        do{hrm_print_fmt(fmt, ##__VA_ARGS__);}while(0)
#else
#define   GH3X2X_DEBUG_LOG(...)	
#endif

#if (__GH3X2X_INFO_LOG_LEVEL__ & __GH3X2X_INFO_LOG_LV_3__)
#define   GH3X2X_WARNING_LOG(fmt, ...)      do{hrm_print_fmt(fmt, ##__VA_ARGS__);}while(0)
#else
#define   GH3X2X_WARNING_LOG(...)	
#endif

#if (__GH3X2X_INFO_LOG_LEVEL__ & __GH3X2X_INFO_LOG_LV_4__)
#define   GH3X2X_ERROR_LOG(fmt, ...)        do{hrm_print_fmt(fmt, ##__VA_ARGS__);}while(0)
#else
#define   GH3X2X_ERROR_LOG(...)	
#endif

#else
#define   GH3X2X_INFO_LOG(...)
#define   GH3X2X_ERROR_LOG(...)
#define   GH3X2X_DEBUG_LOG(...)
#define   GH3X2X_WARNING_LOG(...)
#endif

#if (__SUPPORT_ALGO_INPUT_OUTPUT_DATA_LOG__)
#define   GH3X2X_IODATA_LOG(...)            do{\
                                            if(g_pSnprintfUserIn)\
                                            {\
                                                char gh3x2x_GH3X2X_INFO_LOG[GH3X2X_IODATA_LOG_MAX_LEN] = {0};\
                                                g_pSnprintfUserIn(gh3x2x_GH3X2X_INFO_LOG, GH3X2X_IODATA_LOG_MAX_LEN,__VA_ARGS__);\
                                                GH3X2X_PlatformLog((GCHAR *)gh3x2x_GH3X2X_INFO_LOG);\
                                            }\
                                            if(g_pPrintfUserIn)g_pPrintfUserIn(__VA_ARGS__);\
                                        } while(0)

#else
#define   GH3X2X_IODATA_LOG(...)
#endif

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#define __GH3X2X_MEM_POOL_CHECK_EN__ (0)
#endif

#ifndef __FUNC_TYPE_SOFT_ADT_ENABLE__
#define __FUNC_TYPE_SOFT_ADT_ENABLE__  (0)
#endif

#if __FUNC_TYPE_ECG_ENABLE__ == 0
#define __FUNC_TYPE_PWTT_ENABLE__           (0)    /**< pwtt algorithm tye */
#define __FUNC_TYPE_BP_ENABLE__             (0)    /**< bp algorithm tye */
#define __SUPPORT_ECG_LEAD_OFF_DET_800HZ__  (0)
#endif

#if(__GH3X2X_INTERFACE__ != __GH3X2X_INTERFACE_SPI__)
#define __GH3X2X_SPI_TYPE__                 (__GH3X2X_SPI_TYPE_SOFTWARE_CS__)              
#endif
#if(__GH3X2X_INTERFACE__ != __GH3X2X_INTERFACE_I2C__)
#define __GH3X2X_I2C_DEVICE_ID__            (GH3X2X_I2C_ID_SEL_1L0L)
#endif

#ifdef GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#endif
#define GH3X2X_MAIN_FUNC_CHNL_NUM 1
#if __SUPPORT_HARD_ADT_CONFIG__
#if GH3X2X_ADT_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_ADT_CHNL_NUM
#endif
#endif
#if __FUNC_TYPE_HR_ENABLE__
#if GH3X2X_HR_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_HR_CHNL_NUM
#endif
#endif
#if __FUNC_TYPE_HRV_ENABLE__
#if GH3X2X_HRV_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_HRV_CHNL_NUM
#endif
#endif
#if __FUNC_TYPE_SPO2_ENABLE__
#if GH3X2X_SPO2_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_SPO2_CHNL_NUM
#endif
#endif
#if __FUNC_TYPE_ECG_ENABLE__
#if GH3X2X_ECG_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_ECG_CHNL_NUM
#endif
#endif
#if __FUNC_TYPE_TEST_ENABLE__
#if GH3X2X_TEST_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_MAIN_FUNC_CHNL_NUM
#define GH3X2X_MAIN_FUNC_CHNL_NUM GH3X2X_TEST_CHNL_NUM
#endif
#endif

#ifdef GH3X2X_FUNC_CHNL_NUM_MAX
#undef GH3X2X_FUNC_CHNL_NUM_MAX
#endif
#define GH3X2X_FUNC_CHNL_NUM_MAX GH3X2X_MAIN_FUNC_CHNL_NUM
#if __FUNC_TYPE_SOFT_ADT_ENABLE__
#if GH3X2X_SOFT_ADT_CHNL_NUM > GH3X2X_MAIN_FUNC_CHNL_NUM
#undef GH3X2X_FUNC_CHNL_NUM_MAX
#define GH3X2X_FUNC_CHNL_NUM_MAX GH3X2X_SOFT_ADT_CHNL_NUM
#endif
#endif

#define __GS_EXTRA_BUF_LEN__    (0)
#if __GS_NONSYNC_READ_EN__
#undef __GS_EXTRA_BUF_LEN__
#define __GS_EXTRA_BUF_LEN__    (__GS_READ_POINT_NUM__ + __GS_READ_POINT_NUM_JITTER__)
#endif

#ifndef __GH3X2X_PROTOCOL_DATA_FUNCTION_INTERCEPT__
#define __GH3X2X_PROTOCOL_DATA_FUNCTION_INTERCEPT__   0
#endif

#ifndef __FIFO_PACKAGE_SEND_ENABLE__
#define __FIFO_PACKAGE_SEND_ENABLE__   0
#endif

#if __ADT_ONLY_PARTICULAR_WM_CONFIG__
#if __ADT_ONLY_PARTICULAR_WM_CONFIG__ < 4
#error __ADT_ONLY_PARTICULAR_WM_CONFIG__ must be zero or bigger than 3 !!!
#endif
#endif

#ifdef GOODIX_DEMO_PLATFORM
#include "Goodix_Platform_config.h"
#endif

#if (__SUPPORT_ALGO_INPUT_OUTPUT_DATA_LOG__)
#include "stm32f4xx_hal.h"
#include <time.h>
#endif

#endif /* _GH_DEMO_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
