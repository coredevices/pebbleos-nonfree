/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algo_config.h
 *
 * @brief   gh3x2x algo config
 *
 * @version ref gh3x2x_demo_algo_config.h
 *
 */
#ifndef _GH3X2X_DEMO_ALGO_CONFIG_H_
#define _GH3X2X_DEMO_ALGO_CONFIG_H_

#ifdef GOODIX_DEMO_PLATFORM
//#include "Goodix_Platform_config.h"
#endif

#define GH3X2X_ALGO_FUNCTION_GROUP_0 (GH3X2X_FUNCTION_HR|GH3X2X_FUNCTION_SPO2|GH3X2X_FUNCTION_HRV|GH3X2X_FUNCTION_SOFT_ADT_GREEN)
#define GH3X2X_ALGO_FUNCTION_GROUP_1 (GH3X2X_FUNCTION_SPO2)
#define GH3X2X_ALGO_FUNCTION_GROUP_2 (GH3X2X_FUNCTION_ECG)
#define GH3X2X_ALGO_FUNCTION_GROUP_3 (GH3X2X_FUNCTION_HRV)
#define GH3X2X_ALGO_FUNCTION_GROUP_4 (GH3X2X_NO_FUNCTION)

#define GOODIX_ALGO_BASIC                      (1)
#define GOODIX_ALGO_MEDIUM                     (2)
#define GOODIX_ALGO_PREMIUM                    (3)
#define GOODIX_ALGO_EXCLUSIVE                  (4)

#define GOODIX_ECG_ALGO_500FS_QRS_ENABLE       (1)
#define GOODIX_ECG_ALGO_500FS_QRS_DISABLE      (2)
#define GOODIX_ECG_ALGO_250FS_QRS_ENABLE       (3)
#define GOODIX_ECG_ALGO_250FS_QRS_DISABLE      (4)

#ifndef __GOODIX_HR_ALGO_VERISON__
#define __GOODIX_HR_ALGO_VERISON__             (GOODIX_ALGO_EXCLUSIVE)
#endif 
#ifndef __GOODIX_SPO2_ALGO_VERISON__
#define __GOODIX_SPO2_ALGO_VERISON__           (GOODIX_ALGO_EXCLUSIVE)
#endif 
#ifndef __GOODIX_ECG_ALGO_CONFIG__
#define __GOODIX_ECG_ALGO_CONFIG__             (GOODIX_ECG_ALGO_250FS_QRS_ENABLE)
#endif 

#if ((__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_BASIC) || (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM))
#if ((__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE))
#error "HR algo BASIC\MEDIUM version should adapt to SPO2 BASIC\MEDIUM version"
#endif
#endif

#if ((__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM) || (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE))
#if ((__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_BASIC) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM))
#error "HR algo PREMIUM\EXCLUSIVE version should adapt to SPO2 PREMIUM\EXCLUSIVE version"
#endif
#endif

/* 0: global buffer for goodix algorithm mem pool
 * 1: user dynamic buffer for goodix algorithm mem pool */
#define __USER_DYNAMIC_ALGO_MEM_EN__           (0)
#if __USER_DYNAMIC_ALGO_MEM_EN__
#define GH3X2X_ALGORITHMS_MEMORY_SIZE_USERSET  (0)
#endif

/* function algo enable config */
#define __USE_GOODIX_ADT_ALGORITHM__           (0)
#define __USE_GOODIX_HR_ALGORITHM__            (1)
#define __USE_GOODIX_HRV_ALGORITHM__           (1)
#define __USE_GOODIX_HSM_ALGORITHM__           (0)
#define __USE_GOODIX_SPO2_ALGORITHM__          (1)
#define __USE_GOODIX_ECG_ALGORITHM__           (0)
#define __USE_GOODIX_BT_ALGORITHM__            (0)
#define __USE_GOODIX_RESP_ALGORITHM__          (0)
#define __USE_GOODIX_AF_ALGORITHM__            (0)
#define __USE_GOODIX_BP_ALGORITHM__            (0)
#define __USE_GOODIX_SOFT_ADT_ALGORITHM__      (1)

/* algorithm log */
#define __GH3X2X_ALGO_LOG_LV_0__                (0)    /** no log **/
#define __GH3X2X_ALGO_LOG_LV_1__                (1)    /** include EXAMPLE LOG information **/
#define __GH3X2X_ALGO_LOG_LV_2__                (2)    /** include EXAMPLE LOG debug **/

#define __ALGO_LOG_CONFIG__                    (__GH3X2X_ALGO_LOG_LV_0__)

/* BT NTC Drv Convert config*/
#if __USE_GOODIX_BT_ALGORITHM__
#define __USE_GOODIX_DRV_NTC_Convert                       (1)
#endif

/* algorithm config max number */
#define __GOODIX_ALGO_CFG_LIST_MAX_NUM__       (1)

/*algorithm output value process strategy config*/
#define __GH3X2X_HR_OUTPUT_VALUE_STRATEGY_EN__ (0)

/* algoritm channel nunber config*/
#define __HR_ALGORITHM_SUPPORT_CHNL_NUM__      (4)  /* range 1~4 */
#define __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__    (1)  /* range 1-4 */

/* algo and drv interactivity*/
#define __GH_MSG_WTIH_DRV_LAYER_EN__           (0)
#define __GH_SEPARATE_ALGO_EN__                (0)

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
#define __SOFT_ADT_IR_DETECT_TIMEOUT__         (30) /** (recomended value = 30 seconds) ir detect timeout **/
#endif

#define GH_SEND_MSG_WEAR_EVENT(event)   //add your code to send message to drv layer

#define GH_ALGO_ADC_DATA_ONLY                   0
#define GH_ALGO_ADC_IPD_DATA                    1
#define GH_ALGO_DATA_TYPE                       GH_ALGO_ADC_DATA_ONLY
#define GH_ALGO_CHIP_3018                       0
#define GH_ALGO_CHIP_3X2X                       1
#define GH_ALGO_CHIP_33XX                       2
#define GH_ALGO_CHIP_3036                       3
#define GH_ALGO_CHIP_TYPE                       GH_ALGO_CHIP_3X2X
#define GH_ALGO_DATA_BIT_NUM                    24
#define PPG_CHANNEL_NUM                         4
#define GH_SPO2_PPG_CHANNEL_NUM                 __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__

#define GH_MAX(X, Y)                            (((X) > (Y)) ? (X) : (Y))
#define GH_ALGO_PPG_CHL_NUM                     GH_MAX(__HR_ALGORITHM_SUPPORT_CHNL_NUM__, __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__)
#define GH_ALGO_BYTE_BITS                       (8)
#define GH_ALGO_EN_FLAG_OFFSET                  (7)
#define GH_ALGO_EN_FLAG_MAX                     ((GH_ALGO_PPG_CHL_NUM * 3 / GH_ALGO_BYTE_BITS) \
                                                + (((GH_ALGO_PPG_CHL_NUM * 3) % GH_ALGO_BYTE_BITS) ? 1 : 0))


/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/
#if (\
    __USE_GOODIX_HR_ALGORITHM__      ||\
    __USE_GOODIX_HRV_ALGORITHM__     ||\
    __USE_GOODIX_HSM_ALGORITHM__     ||\
    __USE_GOODIX_SPO2_ALGORITHM__    ||\
    __USE_GOODIX_ECG_ALGORITHM__     ||\
    __USE_GOODIX_BT_ALGORITHM__      ||\
    __USE_GOODIX_RESP_ALGORITHM__    ||\
    __USE_GOODIX_AF_ALGORITHM__      ||\
    __USE_GOODIX_BP_ALGORITHM__      ||\
    __USE_GOODIX_SOFT_ADT_ALGORITHM__\
    )
#define __GOODIX_ALGO_CALL_MODE__           (1)
#else
#define __GOODIX_ALGO_CALL_MODE__           (0)
#endif

#endif /* _GH3X2X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
