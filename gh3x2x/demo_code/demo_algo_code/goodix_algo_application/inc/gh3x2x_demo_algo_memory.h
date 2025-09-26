/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algo_memory.h
 *
 * @brief   gh3x2x algo memory set
 *
 * @version ref gh3x2x_demo_algo_memory.h
 *
 */
#ifndef _GH3X2X_DEMO_ALGO_MEMORY_H_
#define _GH3X2X_DEMO_ALGO_MEMORY_H_

#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_config.h"

#if (__GOODIX_ALGO_CALL_MODE__)

/* algorithm run mode:simultaneously or not */
#define GH3X2X_ALGORITHM_RUN_ALONE                  (0)          /**< only excute one algorithm at the same time */
#define GH3X2X_ALGORITHM_RUN_SIMULTANEOUSLY         (1)          /**< excute more than one algorithm */

/* algorithm mem size in byte */
#if (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_BASIC)

#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_1CHNL       (2136)       /**< HB 1 chnl algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_2CHNL       (2136)       /**< HB 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_3CHNL       (2136)       /**< HB 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_4CHNL       (2136)       /**< HB 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_1CHNL   (1216)       /**< HB 1 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_2CHNL   (1216)       /**< HB 2 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_3CHNL   (1216)       /**< HB 3 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_4CHNL   (1216)       /**< HB 4 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_REDUNDANCY       (100)
#define GH3X2X_HR_ALGORITHM_ROM                     (20912)
#define GH3X2X_HR_ALGORITHM_STACK                   (540)
#define GH3X2X_HR_ALGORITHM_MIPS_MS                 (2.61)

#elif (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM)

#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_1CHNL       (3268)       /**< HB 1 chnl algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_2CHNL       (3268)       /**< HB 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_3CHNL       (3268)       /**< HB 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_4CHNL       (3268)       /**< HB 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_1CHNL   (2080)       /**< HB 1 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_2CHNL   (2080)       /**< HB 2 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_3CHNL   (2080)       /**< HB 3 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_4CHNL   (2080)       /**< HB 4 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_REDUNDANCY       (100)
#define GH3X2X_HR_ALGORITHM_ROM                     (26640)
#define GH3X2X_HR_ALGORITHM_STACK                   (532)
#define GH3X2X_HR_ALGORITHM_MIPS_MS                 (3.25)

#elif (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM)

#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_1CHNL       (8972)       /**< HB 1 chnl algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_2CHNL       (8972)       /**< HB 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_3CHNL       (8972)       /**< HB 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_4CHNL       (8972)       /**< HB 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_1CHNL   (3960)       /**< HB 1 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_2CHNL   (3960)       /**< HB 2 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_3CHNL   (3960)       /**< HB 3 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_4CHNL   (3960)       /**< HB 4 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_REDUNDANCY       (100)
#define GH3X2X_HR_ALGORITHM_ROM                     (76972)
#define GH3X2X_HR_ALGORITHM_STACK                   (612)
#define GH3X2X_HR_ALGORITHM_MIPS_MS                 (21)

#elif (__GOODIX_HR_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE)

#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_1CHNL       (19788)      /**< HB 1 chnl algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_2CHNL       (19788)      /**< HB 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_3CHNL       (19788)      /**< HB 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_PEAK_4CHNL       (19788)      /**< HB 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_1CHNL   (7688)       /**< HB 1 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_2CHNL   (7688)       /**< HB 2 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_3CHNL   (7688)       /**< HB 3 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_4CHNL   (7688)       /**< HB 4 chnl  algorithm desident memory size in byte */
#define GH3X2X_HR_ALGORITHM_MEMORY_REDUNDANCY       (100)
#define GH3X2X_HR_ALGORITHM_ROM                     (89896)
#define GH3X2X_HR_ALGORITHM_STACK                   (1676)
#define GH3X2X_HR_ALGORITHM_MIPS_MS                 (68)

#else
#error "please define __GOODIX_HR_ALGO_VERISON__ in gh3x2x_demo_algo_config.h !"
#endif

#if (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_BASIC) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_MEDIUM)

#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_1CHNL     (2748)       /**< SPO2 1 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_2CHNL     (2748)       /**< SPO2 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_3CHNL     (2748)       /**< SPO2 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_4CHNL     (2748)       /**< SPO2 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_1CHNL (1968)       /**< SPO2 1 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_2CHNL (1968)       /**< SPO2 2 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_3CHNL (1968)       /**< SPO2 3 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_4CHNL (1968)       /**< SPO2 4 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_REDUNDANCY     (100)
#define GH3X2X_SPO2_ALGORITHM_ROM                   (13128)
#define GH3X2X_SPO2_ALGORITHM_STACK                 (376)
#define GH3X2X_SPO2_ALGORITHM_MIPS_MS               (/)

#elif (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_PREMIUM) || (__GOODIX_SPO2_ALGO_VERISON__ == GOODIX_ALGO_EXCLUSIVE)

#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_1CHNL     (8416)       /**< SPO2 1 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_2CHNL     (8416)       /**< SPO2 2 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_3CHNL     (8416)       /**< SPO2 3 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_4CHNL     (8416)       /**< SPO2 4 chnl  algorithm peak memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_1CHNL (5440)       /**< SPO2 1 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_2CHNL (5440)       /**< SPO2 2 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_3CHNL (5440)       /**< SPO2 3 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_4CHNL (5440)       /**< SPO2 4 chnl  algorithm memory size in byte */
#define GH3X2X_SPO2_ALGORITHM_MEMORY_REDUNDANCY     (100)
#define GH3X2X_SPO2_ALGORITHM_ROM                   (53348)
#define GH3X2X_SPO2_ALGORITHM_STACK                 (1536)
#define GH3X2X_SPO2_ALGORITHM_MIPS_MS               (26)

#else
#error "please define __GOODIX_SPO2_ALGO_VERISON__ in gh3x2x_demo_algo_config.h !"
#endif

#if (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_ENABLE)

#define GH3X2X_ECG_ALGORITHM_MEMORY_PEAK            (38664)       /**< ECG algorithm peak memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT        (37280)       /**< ECG algorithm resident memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_REDUNDANCY      (100)
#define GH3X2X_ECG_ALGORITHM_ROM                    (34113)
#define GH3X2X_ECG_ALGORITHM_STACK                  (5256)
#define GH3X2X_ECG_ALGORITHM_MIPS_MS                (216)

#elif (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_500FS_QRS_DISABLE)

#define GH3X2X_ECG_ALGORITHM_MEMORY_PEAK            (32128)       /**< ECG algorithm peak memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT        (30774)       /**< ECG algorithm resident memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_REDUNDANCY      (100)
#define GH3X2X_ECG_ALGORITHM_ROM                    (34113)
#define GH3X2X_ECG_ALGORITHM_STACK                  (5256)
#define GH3X2X_ECG_ALGORITHM_MIPS_MS                (161)

#elif (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_ENABLE)

#define GH3X2X_ECG_ALGORITHM_MEMORY_PEAK            (23536)       /**< ECG algorithm peak memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT        (22192)       /**< ECG algorithm resident memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_REDUNDANCY      (100)
#define GH3X2X_ECG_ALGORITHM_ROM                    (34113)
#define GH3X2X_ECG_ALGORITHM_STACK                  (5256)
#define GH3X2X_ECG_ALGORITHM_MIPS_MS                (151)

#elif (__GOODIX_ECG_ALGO_CONFIG__ == GOODIX_ECG_ALGO_250FS_QRS_DISABLE)

#define GH3X2X_ECG_ALGORITHM_MEMORY_PEAK            (19524)       /**< ECG algorithm peak memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT        (18256)       /**< ECG algorithm resident memory size in byte */
#define GH3X2X_ECG_ALGORITHM_MEMORY_REDUNDANCY      (100)
#define GH3X2X_ECG_ALGORITHM_ROM                    (34113)
#define GH3X2X_ECG_ALGORITHM_STACK                  (5256)
#define GH3X2X_ECG_ALGORITHM_MIPS_MS                (126)

#else
#error "please define __GOODIX_ECG_ALGO_CONFIG__ in gh3x2x_demo_algo_config.h !"
#endif

#define GH3X2X_HRV_ALGORITHM_MEMORY_PEAK            (3008)        /**< HRV algorithm peak memory size in byte */
#define GH3X2X_HRV_ALGORITHM_MEMORY_RESIDENT        (2844)        /**< HRV algorithm resident memory size in byte */
#define GH3X2X_HRV_ALGORITHM_MEMORY_REDUNDANCY      (100)
#define GH3X2X_HRV_ALGORITHM_ROM                    (13636)
#define GH3X2X_HRV_ALGORITHM_STACK                  (980)
#define GH3X2X_HRV_ALGORITHM_MIPS_MS                (/)


#define GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_PEAK       (1188)        /**< SOFT ADT algorithm peak memory size in byte */
#define GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_RESIDENT   (1843)        /**< SOFT ADT algorithm resident memory size in byte */
#define GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_REDUNDANCY (100)         /**< SOFT ADT algorithm redundancy memory size in byte */
#define GH3X2X_SOFT_ADT_ALGORITHM_ROM               (18069)
#define GH3X2X_SOFT_ADT_ALGORITHM_STACK             (720)
#define GH3X2X_SOFT_ADT_ALGORITHM_MIPS_MS           (/)


#define GH3X2X_HSM_ALGORITHM_MEMORY_PEAK            (944)         /**< HSM algorithm memory peak size in byte */
#define GH3X2X_HSM_ALGORITHM_MEMORY_RESIDENT        (944)         /**< HSM algorithm memory resident size in byte */
#define GH3X2X_HSM_ALGORITHM_MEMORY_REDUNDANCY      (100)


#define GH3X2X_BT_ALGORITHM_MEMORY_PEAK             (1600)        /**< BT algorithm memory peak size in byte */
#define GH3X2X_BT_ALGORITHM_MEMORY_RESIDENT         (1500)        /**< BT algorithm memory resident size in byte */
#define GH3X2X_BT_ALGORITHM_MEMORY_REDUNDANCY       (200)


#define GH3X2X_RESP_ALGORITHM_MEMORY_PEAK           (5435)        /**< RESP algorithm memory peak size in byte */
#define GH3X2X_RESP_ALGORITHM_MEMORY_RESIDENT       (2363)        /**< RESP algorithm memory resident size in byte */
#define GH3X2X_RESP_ALGORITHM_MEMORY_REDUNDANCY     (500)


#define GH3X2X_AF_ALGORITHM_MEMORY_PEAK             (3262)        /**< AF algorithm memory peak size in byte */
#define GH3X2X_AF_ALGORITHM_MEMORY_RESIDENT         (1102)        /**< AF algorithm memory resident size in byte */
#define GH3X2X_AF_ALGORITHM_MEMORY_REDUNDANCY       (300)


/* algorithm mem config */
#if (__GOODIX_ALGO_CALL_MODE__)

#if (__USE_GOODIX_HR_ALGORITHM__)
#if __HR_ALGORITHM_SUPPORT_CHNL_NUM__ <= 1
#define   GH3X2X_ALGORITHM_HR_MEMORY_PEAK      GH3X2X_HR_ALGORITHM_MEMORY_PEAK_1CHNL
#define   GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT  GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_1CHNL 
#elif __HR_ALGORITHM_SUPPORT_CHNL_NUM__ == 2
#define   GH3X2X_ALGORITHM_HR_MEMORY_PEAK   GH3X2X_HR_ALGORITHM_MEMORY_PEAK_2CHNL
#define   GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT  GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_2CHNL 
#elif __HR_ALGORITHM_SUPPORT_CHNL_NUM__ == 3
#define   GH3X2X_ALGORITHM_HR_MEMORY_PEAK   GH3X2X_HR_ALGORITHM_MEMORY_PEAK_3CHNL
#define   GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT  GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_3CHNL 
#elif __HR_ALGORITHM_SUPPORT_CHNL_NUM__ >= 4
#define   GH3X2X_ALGORITHM_HR_MEMORY_PEAK   GH3X2X_HR_ALGORITHM_MEMORY_PEAK_4CHNL
#define   GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT  GH3X2X_HR_ALGORITHM_MEMORY_RESIDENT_4CHNL 
#endif
#define GH3X2X_ALGORITHM_HR_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_ALGORITHM_HR_MEMORY_PEAK - GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_HR_MEMORY_REDUNDANCY                 GH3X2X_HR_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_HR_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_HR_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_HSM_ALGORITHM__)
#define   GH3X2X_ALGORITHM_HSM_MEMORY_RESIDENT   (GH3X2X_HSM_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_HSM_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_HSM_ALGORITHM_MEMORY_PEAK - GH3X2X_HSM_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_HSM_MEMORY_REDUNDANCY                GH3X2X_HSM_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_HSM_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_HSM_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_HSM_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_SPO2_ALGORITHM__)
#if __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__ <= 1
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK      GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_1CHNL
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT  GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_1CHNL 
#elif __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__ == 2
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK   GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_2CHNL
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT  GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_2CHNL 
#elif __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__ == 3
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK   GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_3CHNL
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT  GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_3CHNL 
#elif __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__ >= 4
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK   GH3X2X_SPO2_ALGORITHM_MEMORY_PEAK_4CHNL
#define   GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT  GH3X2X_SPO2_ALGORITHM_MEMORY_RESIDENT_4CHNL 
#endif
#define GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK - GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_SPO2_MEMORY_REDUNDANCY                 GH3X2X_SPO2_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_SPO2_MEMORY_REDUNDANCY                (0)

#endif

#if (__USE_GOODIX_ECG_ALGORITHM__)
#define   GH3X2X_ALGORITHM_ECG_MEMORY_RESIDENT   (GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_ECG_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_ECG_ALGORITHM_MEMORY_PEAK - GH3X2X_ECG_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_ECG_MEMORY_REDUNDANCY                GH3X2X_ECG_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_ECG_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_ECG_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_ECG_MEMORY_REDUNDANCY                (0)
#endif

#if (__USE_GOODIX_HRV_ALGORITHM__)
#define   GH3X2X_ALGORITHM_HRV_MEMORY_RESIDENT   (GH3X2X_HRV_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_HRV_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_HRV_ALGORITHM_MEMORY_PEAK - GH3X2X_HRV_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_HRV_MEMORY_REDUNDANCY                GH3X2X_HRV_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_HRV_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_HRV_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_HRV_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_BT_ALGORITHM__)
#define   GH3X2X_ALGORITHM_BT_MEMORY_RESIDENT   (GH3X2X_BT_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_BT_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_BT_ALGORITHM_MEMORY_PEAK - GH3X2X_BT_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_BT_MEMORY_REDUNDANCY                GH3X2X_BT_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_BT_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_BT_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_BT_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_RESP_ALGORITHM__)
#define   GH3X2X_ALGORITHM_RESP_MEMORY_RESIDENT   (GH3X2X_RESP_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_RESP_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_RESP_ALGORITHM_MEMORY_PEAK - GH3X2X_RESP_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_RESP_MEMORY_REDUNDANCY                GH3X2X_RESP_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_RESP_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_RESP_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_RESP_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_AF_ALGORITHM__)
#define   GH3X2X_ALGORITHM_AF_MEMORY_RESIDENT   (GH3X2X_AF_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_AF_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_AF_ALGORITHM_MEMORY_PEAK - GH3X2X_AF_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_AF_MEMORY_REDUNDANCY                GH3X2X_AF_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_AF_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_AF_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_AF_MEMORY_REDUNDANCY               (0)
#endif

#if (__USE_GOODIX_SOFT_ADT_ALGORITHM__)
#define   GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_RESIDENT   (GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_PEAK_WITHOUT_RESIDENT    (GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_PEAK - GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_RESIDENT)
#define GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_REDUNDANCY                GH3X2X_SOFT_ADT_ALGORITHM_MEMORY_REDUNDANCY
#else
#define GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_RESIDENT   (0)
#define GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_PEAK_WITHOUT_RESIDENT    (0)
#define GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_REDUNDANCY                (0)
#endif

#endif

#define MAX2(a0, a1)                                   (((a0) > (a1)) ? (a0) : (a1))
#define ADD2(a0, a1)                                   ((a0) + (a1))
#define MAX4(a0,a1,a2,a3)                              (MAX2(MAX2(a0, a1),MAX2(a2, a3)))
#define MAX5(a0,a1,a2,a3,a4)                           (MAX2(MAX4(a0,a1,a2,a3),a4))
#define MAX9(a0, a1, a2, a3, a4, a5 ,a6, a7, a8)   (MAX2(MAX5(a0,a1,a2,a3,a4),MAX4(a5,a6,a7,a8))))

#define GH3X2X_FUNCTION_SOFT_ADT  (GH3X2X_FUNCTION_SOFT_ADT_GREEN|GH3X2X_FUNCTION_SOFT_ADT_IR)

#define   GH3X2X_ALGORITHMS_MEMORY_RESIDENT_(x) (\
    ((0 != (x & GH3X2X_FUNCTION_HR)) * __USE_GOODIX_HR_ALGORITHM__ * GH3X2X_ALGORITHM_HR_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_SPO2)) * __USE_GOODIX_SPO2_ALGORITHM__ * GH3X2X_ALGORITHM_SPO2_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_ECG)) * __USE_GOODIX_ECG_ALGORITHM__ * GH3X2X_ALGORITHM_ECG_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_HRV)) * __USE_GOODIX_HRV_ALGORITHM__ * GH3X2X_ALGORITHM_HRV_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_HSM)) * __USE_GOODIX_HSM_ALGORITHM__ * GH3X2X_ALGORITHM_HSM_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_BT)) * __USE_GOODIX_BT_ALGORITHM__ * GH3X2X_ALGORITHM_BT_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_RESP)) * __USE_GOODIX_RESP_ALGORITHM__ * GH3X2X_ALGORITHM_RESP_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_AF)) * __USE_GOODIX_AF_ALGORITHM__ * GH3X2X_ALGORITHM_AF_MEMORY_RESIDENT) + \
    ((0 != (x & GH3X2X_FUNCTION_SOFT_ADT)) * __USE_GOODIX_SOFT_ADT_ALGORITHM__ * GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_RESIDENT))

#define   GH3X2X_ALGORITHMS_MEMORY_PEAK_WITHOUT_RESIDENT_(x) (MAX9(\
    ADD2(((0 != (x & GH3X2X_FUNCTION_HR))* __USE_GOODIX_HR_ALGORITHM__ * GH3X2X_ALGORITHM_HR_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_HR))* __USE_GOODIX_HR_ALGORITHM__ * GH3X2X_ALGORITHM_HR_MEMORY_REDUNDANCY)),\
    ADD2(((0 != (x & GH3X2X_FUNCTION_SPO2))* __USE_GOODIX_SPO2_ALGORITHM__* GH3X2X_ALGORITHM_SPO2_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_SPO2))* __USE_GOODIX_SPO2_ALGORITHM__* GH3X2X_ALGORITHM_SPO2_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_ECG))* __USE_GOODIX_ECG_ALGORITHM__* GH3X2X_ALGORITHM_ECG_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_ECG))* __USE_GOODIX_ECG_ALGORITHM__* GH3X2X_ALGORITHM_ECG_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_HRV))* __USE_GOODIX_HRV_ALGORITHM__* GH3X2X_ALGORITHM_HRV_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_HRV))* __USE_GOODIX_HRV_ALGORITHM__* GH3X2X_ALGORITHM_HRV_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_HSM))* __USE_GOODIX_HSM_ALGORITHM__* GH3X2X_ALGORITHM_HSM_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_HSM))* __USE_GOODIX_HSM_ALGORITHM__* GH3X2X_ALGORITHM_HSM_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_BT))* __USE_GOODIX_BT_ALGORITHM__* GH3X2X_ALGORITHM_BT_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_BT))* __USE_GOODIX_BT_ALGORITHM__* GH3X2X_ALGORITHM_BT_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_RESP))* __USE_GOODIX_RESP_ALGORITHM__* GH3X2X_ALGORITHM_RESP_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_RESP))* __USE_GOODIX_RESP_ALGORITHM__* GH3X2X_ALGORITHM_RESP_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_AF))* __USE_GOODIX_AF_ALGORITHM__* GH3X2X_ALGORITHM_AF_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_AF))* __USE_GOODIX_AF_ALGORITHM__* GH3X2X_ALGORITHM_AF_MEMORY_REDUNDANCY)), \
    ADD2(((0 != (x & GH3X2X_FUNCTION_SOFT_ADT))* __USE_GOODIX_SOFT_ADT_ALGORITHM__ * GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_PEAK_WITHOUT_RESIDENT),\
    ((0 != (x & GH3X2X_FUNCTION_SOFT_ADT))* __USE_GOODIX_SOFT_ADT_ALGORITHM__ * GH3X2X_ALGORITHM_SOFT_ADT_MEMORY_REDUNDANCY)))

#define GH3X2X_ALGORITHMS_MEMORY_(x)  (GH3X2X_ALGORITHMS_MEMORY_RESIDENT_(x) + GH3X2X_ALGORITHMS_MEMORY_PEAK_WITHOUT_RESIDENT_(x))

#define __GH3X2X_ALGORITHMS_MEMORY  (\
                                    MAX5(\
                                    GH3X2X_ALGORITHMS_MEMORY_(GH3X2X_ALGO_FUNCTION_GROUP_0),\
                                    GH3X2X_ALGORITHMS_MEMORY_(GH3X2X_ALGO_FUNCTION_GROUP_1),\
                                    GH3X2X_ALGORITHMS_MEMORY_(GH3X2X_ALGO_FUNCTION_GROUP_2),\
                                    GH3X2X_ALGORITHMS_MEMORY_(GH3X2X_ALGO_FUNCTION_GROUP_3),\
                                    GH3X2X_ALGORITHMS_MEMORY_(GH3X2X_ALGO_FUNCTION_GROUP_4)\
                                    )\
                                )
//final size
#define GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL  (((GU32)((__GH3X2X_ALGORITHMS_MEMORY + 3) / 4)) * 4)

/**
 * @fn     GS8 GH3X2X_AlgoMemConfig( GU32 unMemSize)
 *
 * @brief  Config algorithm memory.
 *
 * @attention   Mem can only be static,such as global array
 *
 * @param[in]  unMemSize       memory size(unit:byte)
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_RESOURCE_ERROR          return algorithm mem config error
 */
GS8 GH3X2X_AlgoMemConfig( GU32 unMemSize);
#endif

#endif /* _GH3X2X_DEMO_ALGO_CALL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
