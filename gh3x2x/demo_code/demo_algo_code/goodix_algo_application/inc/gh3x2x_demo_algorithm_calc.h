/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_algorithm_calc.h
 *
 * @brief   gh3x2x algorithm calc define & declaration
 *
 * @version ref gh3x2x_demo_algorithm_calc.h
 *
 */

#ifndef _GH3X2X_DEMO_ALGORITHM_CALC_H_
#define _GH3X2X_DEMO_ALGORITHM_CALC_H_


#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_hook.h"
#include "gh3x2x_demo_common.h"


/// algorithm calculate hook type
typedef GS8 (*pfnAlgorithmCallFunc)(const STGh3x2xFrameInfo * const pstFrameInfo);

extern STGh3x2xFrameInfo ** g_pstGh3x2xAlgoFrameInfo;

#define GOODIX_HR_PARAMS_NUM_MAX           (10)
#define GOODIX_HRV_PARAMS_NUM_MAX          (5)
#define GOODIX_SPO2_PARAMS_NUM_MAX         (26)
#define GOODIX_ECG_PARAMS_NUM_MAX          (5)
#define GOODIX_BT_PARAMS_NUM_MAX           (20)
#define GOODIX_AF_PARAMS_NUM_MAX           (2)

#define GH3X2X_RAWDATA_BASE_VALUE          (1<<23)

#define  GH3X2X_GAIN_VALUE_INVALID                      (0xFF)

/// xor val for xor num in encrypt & decrypt
#define   GH3X2X_CRYP_XOR_XOR_VAL                       (0xFF)

/// fixed val in encrypt & decrypt
#define   GH3X2X_CRYP_FIXED_VAL                         (0x55)

/// header len in encrypt & decrypt
#define   GH3X2X_CRYP_HEADER_LEN                        (5u)

/// cryp rawdata len to index
#define   GH3X2X_ENCRYPT_LEN2INDEX                      (1u)

/* encrypt & decrypt header index */
#define   GH3X2X_CRYP_HEADER_0                          (0u)     /**< cryp header index 0 */
#define   GH3X2X_CRYP_HEADER_1                          (1u)     /**< cryp header index 1 */
#define   GH3X2X_CRYP_HEADER_2                          (2u)     /**< cryp header index 2 */
#define   GH3X2X_CRYP_HEADER_3                          (3u)     /**< cryp header index 3 */
#define   GH3X2X_CRYP_HEADER_4                          (4u)     /**< cryp header index 4 */

/// encrypt data min len, min len is 4
#define   GH3X2X_ENCRYPT_CHECK_MIN_LEN(x)               ((x) > 3)

/// encrypt source add magic bytes, add after source buffer & len must >= 4, magic bytes len align to cryp header len
#define   GH3X2X_ENCRYPT_SRC_ADD_MAGIC(src, len)        do {\
                                                            (src)[(len)] = ((src)[0] ^ (src)[(len) - 1]);\
                                                            (src)[(len) + 1] = ((src)[1] ^ (src)[(len) - 3]);\
                                                            (src)[(len) + 2] = GH3X2X_CRYP_FIXED_VAL;\
                                                            (src)[(len) + 3] = ((src)[2] ^ (src)[(len) - 2]);\
                                                            (src)[(len) + 4] = ((src)[3] ^ (src)[(len) - 4]);\
                                                        } while (0)

/// encrypt source verify magic bytes, add after source buffer & len must >= 4
#define   GH3X2X_ENCRYPT_VERIFY_SRC_MAGIC(src, len)     (((src)[(len)] == ((src)[0] ^ (src)[(len) - 1])) &&\
                                                         ((src)[(len) + 1] == ((src)[1] ^ (src)[(len) - 3])) &&\
                                                         ((src)[(len) + 2] == GH3X2X_CRYP_FIXED_VAL) &&\
                                                         ((src)[(len) + 3] == ((src)[2] ^ (src)[(len) - 2])) &&\
                                                         ((src)[(len) + 4] == ((src)[3] ^ (src)[(len) - 4])))

/// hba scenario mcaro max val
#define   GH3X2X_HBA_SCENARIO_CONFIG_MAX_VAL            (HBA_SCENES_SIZE)

/* rawdata package for algorithm */
#define   GH3X2X_ALGO_RAW_GX_INDEX                      (0u)    /**< rawdata gsensor x-asix index */
#define   GH3X2X_ALGO_RAW_GY_INDEX                      (1u)    /**< rawdata gsensor y-asix index */
#define   GH3X2X_ALGO_RAW_GZ_INDEX                      (2u)    /**< rawdata gsensor z-asix index */
#define   GH3X2X_ALGO_RAW_CH0_INDEX                     (3u)    /**< rawdata ch0 data index */
#define   GH3X2X_ALGO_RAW_CH1_INDEX                     (4u)    /**< rawdata ch1 data index */
#define   GH3X2X_ALGO_RAW_CH2_INDEX                     (5u)    /**< rawdata ch2 data index */
#define   GH3X2X_ALGO_RAW_CH3_INDEX                     (6u)    /**< rawdata ch3 data index */
#define   GH3X2X_ALGO_RAW_CH4_INDEX                     (7u)    /**< rawdata ch4 data index */
#define   GH3X2X_ALGO_RAW_CH5_INDEX                     (8u)    /**< rawdata ch5 data index */
#define   GH3X2X_ALGO_RAW_CH6_INDEX                     (9u)    /**< rawdata ch6 data index */
#define   GH3X2X_ALGO_RAW_CH7_INDEX                     (10u)   /**< rawdata ch7 data index */
#define   GH3X2X_ALGO_RAW_CH8_INDEX                     (11u)   /**< rawdata ch8 data index */
#define   GH3X2X_ALGO_RAW_CH9_INDEX                     (12u)   /**< rawdata ch9 data index */
#define   GH3X2X_ALGO_RAW_CH10_INDEX                    (13u)   /**< rawdata ch10 data index */
#define   GH3X2X_ALGO_RAW_CH11_INDEX                    (14u)   /**< rawdata ch11 data index */
#define   GH3X2X_ALGO_RAWDATA_LEN                       (35u)   /**< rawdata packet len for algorithm */

/// algorithm channel max cnt support
#define   GH3X2X_CHANNEL_MAP_MAX_CNT                    (32u)

/// get slot & adc num from rawdata flag byte, equal channel map formats
#define   GH3X2X_CHANNEL_MAP_GET(x)                     ((x) & GH3X2X_SLOT_ADC_NUM_BITS)

/// channel map invalid val
#define   GH3X2X_CHANNEL_MAP_INVALID_VAL                (0x07)

/// channel map is ecg channel
#define   GH3X2X_CHANNEL_MAP_ISNOT_ECG_CH(x)            (GH3X2X_CHECK_BIT_NOT_SET((x), GH3X2X_CHANNEL_MAP_ADC3))

/// algorithm output GF32 to GS32, multiply by (100)
#define   GH3X2X_ALGORITHM_GF32_2_GS32(x)               ((GS32)((x) * 100))

/// rawdata is new sampling data
#define   GH3X2X_IS_NEW_RAWDATA                         (1)

/// rawdata is new sampling data
#define   GH3X2X_IS_NOT_NEW_RAWDATA                     (0)

/// number of ECG algorithm output
#define  GH3X2X_ECG_ALGO_OUTPUT_NUM                 (sizeof(STEcgAlgoResult))

/// number of spo2 algorithm output
#define  GH3X2X_SPO2_ALGO_OUTPUT_NUM                (sizeof(STSpo2AlgoResult))

/// number of hb algorithm output
#define  GH3X2X_HB_ALGO_OUTPUT_NUM                  (sizeof(STHbAlgoResult))

/// number of hrv algorithm output
#define  GH3X2X_HRV_ALGO_OUTPUT_NUM                 (sizeof(STHrvAlgoResult))

/// memory for algorithm is not inited
#define ALGO_MEM_NO_INIT                            (0) 

/// memory for algorithm is already inited
#define ALGO_MEM_INIT_OK                            (1)

// algo chnl map num
#define ALGO_CHNL_NUM                               (32)

// algo green chnl pos
#define ALGO_GREEN_CHNL_POS                         (0)

// algo ir chnl pos
#define ALGO_IR_CHNL_POS                            (8)

// algo red chnl pos
#define ALGO_RED_CHNL_POS                           (16)

/* ecg algorithm */

// ECG 采样率
#ifndef ECG_FS
#define ECG_FS 500
#endif

#define ECG_OUT_FACTOR                  (100)
#define ECG_SNR_FACTOR                  (10000000)

#define ECG_FRAME_UNUSED                (5)

#define  GH3X2X_REG_IS_VIRTUAL0X3_BIT               (0x3000)
#define GH3X2X_VREG_FUNCTION_OFFSET                 (0X0300)
#define GH3X2X_ADT_DRV_CFG_ADDR                     (0x3000)
#define GH3X2X_ADT_ALG_CFG_ADDR                     (0x30C0)
#define GH3X2X_REG_ADDR_SIZE                        (0x0002)    /**< reg addr size */
#define GH3X2X_VREG_ALGO_CHNL_MAP_OFFSET            (0X0200)
#define GH3X2X_HR_SCENARIO_CFG_L_ADDR               (0x33C0)
#define GH3X2X_HR_ALGO_CHNL_NUM_ADDR                (0x35C0)
#define GH3X2X_SPO2_ALGO_CHNL_NUM_ADDR              (0x44C0)
#define GH3X2X_SPO2_CH3_CORRECTION_FACTOR4_H_ADDR   (0x430E)
#define GH3X2X_SPO2_CH0_CORRECTION_FACTOR0_L_ADDR   (0x42C0)

#define GH3X2X_HSM_FLAG_BUFF_LEN (120)

typedef enum
{
    ADT_ALGO_RESULT_DEFULT = 0,
    ADT_ALGO_RESULT_WAER_ON,   /**< detect wear on */
    ADT_ALGO_RESULT_WAER_OFF,
} EMAdtAlgoStatus;

/**
 * @brief gh3x2x algo function id
 */
typedef enum 
{ 
    GH3X2X_ALGO_INIT_FUNCTION = 0,
    GH3X2X_ALGO_CALCULATE_FUNCTION,
//    GH3X2X_ALGO_FINAL_CALCULATE_FUNCTION,
    GH3X2X_ALGO_DEINIT_FUNCTION,
    GH3X2X_ALGO_FUNCTION_NUM_MAX,
} EMAlgoFunctionID;

// algo chnl map struct
typedef struct
{
    GU8 uchFlag;
    GU8 uchNum;
    GU8 uchEcgAlgoChnlMap;
    GU8 uchAlgoChnlMap[ALGO_CHNL_NUM];
} STAlgoChnlMap;

typedef struct
{
    GS32 nHbaScenario;
    GS32 nHbaCalcOutputTime;
    GS32 nFrequency;
}STAlgoHrParas;

/* hb algorithm */
#if (__USE_GOODIX_HR_ALGORITHM__)
extern GS32 g_unHrParamsArr[GOODIX_HR_PARAMS_NUM_MAX];
/// hbd rawdata channel map
extern STAlgoChnlMap g_stHbaAlgoChnlMap;
#endif

/* hrv algorithm */
#if (__USE_GOODIX_HRV_ALGORITHM__)
extern GS32 g_nHrvParamsArr[GOODIX_HRV_PARAMS_NUM_MAX];
#endif

/* spo2 algorithm */
#if (__USE_GOODIX_SPO2_ALGORITHM__)
extern GS32 g_nSpo2ParamssArr[GOODIX_SPO2_PARAMS_NUM_MAX];
/// spo2 rawdata channel map
extern STAlgoChnlMap g_stSpo2AlgoChnlMap;
#endif

/* ecg algorithm */
#if (__USE_GOODIX_ECG_ALGORITHM__)
extern GS32 g_nEcgParamsArr[GOODIX_ECG_PARAMS_NUM_MAX];
/// ecg frame data cnt
extern GU32 g_unEcgFrameCnt;
extern GU8 g_uchClassificationFlag;
extern GU8 g_unEcgTimeSeconds;
#endif

/* bt algorithm */
#if (__USE_GOODIX_BT_ALGORITHM__)
extern GS32 g_nBtParamsArr[GOODIX_BT_PARAMS_NUM_MAX];
#endif

/* af algorithm */
#if (__USE_GOODIX_AF_ALGORITHM__)
extern GS32 g_nAfParamsArr[GOODIX_AF_PARAMS_NUM_MAX];
#endif

extern GU8 g_uchAlgoGsensorEnable;
extern GU8 g_uchAlgoCapEnable;
extern GU8 g_uchAlgoTempEnable;

extern GU8 GH3X2X_GetAlgoMemStatus(void);

/**
 * @fn     GS8 GH3X2X_AlgoMemInit(void)
 *
 * @brief  algorithm memory init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_RESOURCE_ERROR          return algorithm mem init error
 */
GS8 GH3X2X_AlgoMemInit(void);

/**
 * @fn     void GH3X2X_AlgoMemDeinit(void)
 *
 * @brief  algorithm memory deinit
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AlgoMemDeinit(void);

/**
 * @fn     void GH3X2X_SetNewDataFlag(EMChannelMapId emFuncId, GU8 uchFlag)
 *
 * @brief  Set new sampling data flag
 *
 * @attention   None
 *
 * @param[in]   emFuncId    function id
 * @param[in]   uchFlag      new data or not
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SetNewDataFlag(GU8 emFuncId, GU8 uchFlag);
extern void Gh3x2xOutputValueStrategyInit(GU32 unFunctionID);
extern void GH3x2xSleepFlagSet(GU8 uchSleepFlag);
extern GU8 GH3x2xSleepFlagGet(void);
extern GS32 GH3x2x_Round(GF32 fValue);
extern GU8 GH3x2x_BitCount(GU32 n);
extern void GH3X2X_LoadGoodixAlgoRegConfigArr(const STGh3x2xInitConfig *pstGoodixAlgoCfgListArr);

extern GU8* GH3x2xAdtVersion(void);
extern GS8 GH3x2xAdtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAdtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAdtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xHrAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHrAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHrAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);
extern void GH3X2X_HbaAlgoChnlMapDefultSet(void);
extern void Gh3x2xHrOutputValueStrategyPro(STGh3x2xAlgoResult * pstAlgoResult, GU32 lubFrameId);

extern GS8 GH3x2xHrvAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHrvAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHrvAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xSpo2AlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xSpo2AlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xSpo2AlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);
extern void GH3X2X_Spo2AlgoChnlMapDefultSet(void);

extern GS8 GH3x2xAfAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAfAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAfAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xAfAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAfAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xAfAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xRespAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xRespAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xRespAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xBtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xBtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xBtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xHsmAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHsmAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xHsmAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xSoftAdtAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xSoftAdtAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xSoftAdtAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);

extern GS8 GH3x2xEcgAlgoInit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xEcgAlgoDeinit(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GS8 GH3x2xEcgAlgoExe(const STGh3x2xFrameInfo * const pstFrameInfo);
extern GU8 GH3x2xEcgAlgoClassification(void);

#endif /* _GH3X2X_DRV_ALGORITHM_CALC_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
