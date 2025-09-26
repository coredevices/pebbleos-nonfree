/**
  ****************************************************************************************
  * @file    gh3x_soft_led_agc.h
  * @author  GHealth Driver Team
  * @brief   
  ****************************************************************************************
  * @attention  
  #####Copyright (c) 2022 GOODIX
   All rights reserved.
  
  ****************************************************************************************
  */
#ifndef _GH3X2X_AGC_H_
#define _GH3X2X_AGC_H_
 
#include "gh_drv.h"

#define AGC_READ_CHECK    1
#define READ_AGCINFO    0
/*sofe agc*/
#define GH3X2X_AGC_SLOT_NUM_LIMIT         (8u)
#define GH3X2X_AGC_SUB_CHNL_NUM_LIMIT     ((GH3X2X_AGC_SLOT_NUM_LIMIT)*3)
#define __GH3X_AGC_GENERAL_H_THD__  ((8388608 + 8388608*0.85f))
#define __GH3X_AGC_GENERAL_L_THD__  ((8388608 + 8388608*0.30f))
#define __GH3X_AGC_GENERAL_IDEAL_VALUE__ ((8388608 + 8388608*0.56f))
#define __GH3X_AGC_GENERAL_IDEAL_IPD__  (2000u)
#define __GH3X_AGC_GENERAL_H_IPD__  (3060u)
#define __GH3X_AGC_GENERAL_L_IPD__  (1000u)
// #define GH3X_AGC_GENERAL_RE_H_IPD  (2718u)
// #define GH3X_AGC_GENERAL_RE_L_IPD  (1314u)

#define __GH3X_AGC_SPO2_H_THD__  ((8388608 + 8388608*0.9f))
#define __GH3X_AGC_SPO2_L_THD__  ((8388608 + 8388608*0.5f))
#define __GH3X_AGC_SPO2_IDEAL_VALUE__  ((8388608 + 8388608*0.7f))
#define __GH3X_AGC_SPO2_IDEAL_IPD__  (20000u)
#define __GH3X_AGC_SPO2_H_IPD__  (25000u)
#define __GH3X_AGC_SPO2_L_IPD__  (15000u)
#define GH3X_AGC_CONVERGENCE_LEVEL  (8u)  // The values that can be set are 8, 16 and 32, and the recommended value is 16. The larger the value, the smaller the convergence range of AGC. 

#define __GH3X_AGC_WINDOW_SECONDS__  (5)
#define __GH3X_AGC_WINDOW_TIMES__    (6)

#define GH3X2X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC 1

//sub channel
//#define GH3X_AGC_SUB_CHNNL_DISCAED_TIME_SEC  2/5
#define GH3X_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC    30

/// sys sample rate, 32kHz
#define  GH3X_SYS_SAMPLE_RATE             (32000u)
/// raw data base line value
#define  GH3X2X_DATA_BASE_LINE              (8388608)   //2^23 

/// fifo rawdata size
#define   GH3X2X_FIFO_RAWDATA_SIZE          (4)

#define  GH3X2X_LED_AGC_DISABLE             (0)
#define  GH3X2X_LED_AGC_ENABLE              (1)

/// agc physical register
#define SYS_SAMPLE_RATE_CTRL            0X0004
#define RG_FASTEST_SAMPLE_RATE_LSB   0
#define RG_FASTEST_SAMPLE_RATE_MSB   7

#define SLOTCFG_REG_OFFSET      0X01C
#define SLOTCFG_REG_BEGIN_ADDR  0X010A
#define SLOTCFG_REG_END_ADDR    0X01E8

#define SLOT_CTRL_0_REG         (0X010A - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_ECGRX_EN_LSB   1
#define RG_SLOT0_ECGRX_EN_MSB   5
#define RG_SLOT0_SR_MULTIPLIER_LSB   8
#define RG_SLOT0_SR_MULTIPLIER_MSB   15

#define SLOT_CTRL_3_REG          (0X0110 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_LED_AGC_SRC_LSB   6
#define RG_SLOT0_LED_AGC_SRC_MSB   7
#define RG_SLOT0_LED_AGC_EN_LSB    8
#define RG_SLOT0_LED_AGC_EN_MSB    9

#define SLOT_CTRL_4_REG         (0X0112 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_RX0_TIA_LSB    0
#define RG_SLOT0_RX0_TIA_MSB    3
#define RG_SLOT0_RX1_TIA_LSB    4
#define RG_SLOT0_RX1_TIA_MSB    7
#define RG_SLOT0_RX2_TIA_LSB    8
#define RG_SLOT0_RX2_TIA_MSB    11
#define RG_SLOT0_RX3_TIA_LSB    12
#define RG_SLOT0_RX3_TIA_MSB    15

#define SLOT_CTRL_6_REG           (0X0116 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_RX0_DC_CANCEL_LSB   0
#define RG_SLOT0_RX0_DC_CANCEL_MSB   7
#define RG_SLOT0_RX1_DC_CANCEL_LSB   8
#define RG_SLOT0_RX1_DC_CANCEL_MSB   15

#define SLOT_CTRL_7_REG           (0X0118 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_RX2_DC_CANCEL_LSB   0
#define RG_SLOT0_RX2_DC_CANCEL_MSB   7
#define RG_SLOT0_RX3_DC_CANCEL_LSB   8
#define RG_SLOT0_RX3_DC_CANCEL_MSB   15

#define SLOT_CTRL_8_REG           (0X011A - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_RX0_BG_CANCEL_LSB   0
#define RG_SLOT0_RX0_BG_CANCEL_MSB   1
#define RG_SLOT0_RX1_BG_CANCEL_LSB   2
#define RG_SLOT0_RX1_BG_CANCEL_MSB   3
#define RG_SLOT0_RX2_BG_CANCEL_LSB   4
#define RG_SLOT0_RX2_BG_CANCEL_MSB   5
#define RG_SLOT0_RX3_BG_CANCEL_LSB   6
#define RG_SLOT0_RX3_BG_CANCEL_MSB   7

#define SLOT_CTRL_10_REG           (0X011E - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_LED_DRV0_LSB   0
#define RG_SLOT0_LED_DRV0_MSB   7

#define SLOT_CTRL_11_REG            (0X0120 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_LED_DRV1_LSB   0
#define RG_SLOT0_LED_DRV1_MSB   7

#define SLOT_CTRL_12_REG            (0X0122 - SLOTCFG_REG_BEGIN_ADDR)
#define RG_SLOT0_ADJ_UP_LIMIT_LSB   0
#define RG_SLOT0_ADJ_UP_LIMIT_MSB   7
#define RG_SLOT0_ADJ_DOWN_LIMIT_LSB   8
#define RG_SLOT0_ADJ_DOWN_LIMIT_MSB   15

#define GAINCODE_DEF_VAL  4  
#define ADJUPLIMIT_DEF_VAL    0XFF

#pragma pack(push, 1) 
typedef struct
{
    GU8 uchAddrOffset;
    GU8 uchMsb;
    GU8 uchLsb;
} STGh3x2xAgcReg;

/**
 * @brief AMB config
 */
typedef struct
{
    GU8 uchAmbSampleEn  : 2;                /**< AMB sampling enable */
    GU8 uchAmbSlot      : 3;                /**< slot of amb sampling */
    GU8 uchRes          : 3;                /**< reserved */
} STAmbSlotCtrl;
 
 /**
 * @brief gain limit
 */
typedef struct
{
    GU8 uchGainLimitBg32uA  : 4;          /**< gain limit bg current */
    GU8 uchGainLimitBg64uA  : 4;          
    GU8 uchGainLimitBg128uA : 4;        
    GU8 uchGainLimitBg256uA : 4;
} STSoftAgcGainLimit;

 /**
  * @brief soft agc parameter
  */
typedef struct
{
    GU8 uchSlotxSoftAgcAdjEn;              /**< soft agc enable */
    GU8 uchSlotxSoftBgAdjEn;               /**< soft bg cancel adjust enable */
    STAmbSlotCtrl stAmbSlotCtrl;           /**< amb slot ctrl */
    GU8 uchRes0;                           /**< reserved */
    GU8 uchSpo2RedSlotCfgEn;
    GU8 uchSpo2IrSlotCfgEn;
    STSoftAgcGainLimit stSoftAgcGainLimit; /**< soft gain limit */
    GU32 usNewAgcGeneH_Ipd;                   /**< trig threshold(high) of soft agc */
    GU32 usNewAgcGeneL_Ipd;                   /**< trig threshold(low) of soft agc */
    GU32 usNewAgcSpo2H_Ipd;               /**< restrain threshold(high) of soft agc */
    GU32 usNewAgcSpo2L_Ipd;               /**< restrain threshold(low) of soft agc */
} STSoftAgcCfg;


typedef struct
{
    GU8  uchCurrentDrv;
    GU8  uchBakRegLedCurrentMin;
    GU8  uchBakRegLedCurrentMax;
    GU8  uchSlotAdc;
    GU8  uchSpo2ChnlIndex;      //0: is not spo2 channel   N(N > 0):  is spo2 channl  
    GU8  uchBakRegAgcSrc;
    GU8  uchTiaGainUpLimit;
    GU8  uchBakRegAgcEn;
    GU8  uchCurrentTiaGain;
    GU8  uchReadCheck;
} STGh3x2xAgcMainChnlInfo;

typedef struct
{
    GS64 unSumAdjust;     //  cacluate drv current and gain
    GU32 unLastRawdata;
    GU32 unLast2Rawdata;
    GU32 unMaxRawdata;
    GU32 unMinRawdata;
    GU32 unRawdataMean;
    GU32 unLastRawdataMean;
    GU16 usSat_cnt;        /* sat num of each channel*/
    GU16 usSat_CheckCnt;   /* num of _Check data*/
    GU16 usAnalysisCnt;   // numble of nalysis data
    GU16 usSampleRate;
    GU8 Sat_flag;           /* sat flag*/
    GU8 uchHaveAgcAtLastReadPrd;    //0: no agc action at last fifo read period  1: have agc action at last fifo read preiod
    GU8 uchDropFlag;
} STGh3x2xAgcMainChnKeyInfo;

typedef struct
{
    GU8  uchIniteFirst;   // inite first time   0:yes   1:not
    GU8  uchWindowCnt;    // 
} STGh3x2xNewAgcMainChnlIdaelAdjInfo;

typedef struct
{
    GU32 unLastRawdata;
    GU32 unLastLastRawdata;
    GU32 unMaxRawdata;
    GU32 unMinRawdata;
    GU16 usAnalysisCnt;
    GU16 usSat_cnt;          /* sat num of each channel*/
    GU16 usSat_CheckCnt;     /* num of _Check data*/
    GU8  uchSlotAdcNo;
    GU8  Sat_flag;           /* sat flag*/
    GU8  uchWindowCnt;
    GU8  uchEn;
    GU8  uchCurrentGain;	
}STGh3x2xAgcSubChnlRxInfo;

#pragma pack(pop)

/**
 * @fn  void GH3X2X_LedAgcReset(void)
 *
 * @brief   reset agc parameter
 *
 * @attention    None
 *
 * @param[in]    None
 * @param[out]   None
 *
 * @return  None
 */
void GH3X2X_LedAgcReset(void);


/**
 * @fn     GH3x2xAgcRegvalReset(STAgcRegConfig* pstAgcRegConfig)
 *
 * @brief  reset Agc Regester Configure  struct variable 
 *
 * @attention   None
 *
 * @param[in]   pstAgcRegConfig  point of Agc Regester Configure  struct variable
 * @param[out]  None
 *
 * @return None
 */
void GH3x2xAgcRegvalReset(STAgcRegConfig* pstAgcRegConfig);

/**
 * @fn     void GH3x2xAgcRegvalGet(STAgcRegConfig* pstAgcRegConfig, GU16 RegAddr, GU16 RegVal)
 *
 * @brief  set the value of Agc Regester Configure  struct variable 
 *
 * @attention   None
 *
 * @param[in]   pstAgcRegConfig  point of Agc Regester Configure  struct variable
 * @param[in]   RegAddr      register addr
 * @param[in]   RegVal       val of register
 * @param[out]  None
 *
 * @return none 
 */
void GH3x2xAgcRegvalGet(STAgcRegConfig* pstAgcRegConfig, GU16 RegAddr, GU16 RegVal);

/**
 * @fn     void GH3X2X_LedAgcPramWrite(GU16 usRegAddr, GU16 usRegData)
 *
 * @brief    write AGC parameter
 *
 * @attention     None
 *
 * @param[in]     usRegAddr           reg addr
 * @param[in]     usRegData           reg data
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LedAgcPramWrite(GU16 usRegAddr, GU16 usRegData);


/**
 * @fn     void GH3x2xAgcRegvalGet(STAgcRegConfig* pstAgcRegConfig, GU16 RegAddr, GU16 RegVal)
 *
 * @brief  set the value of Agc Regester Configure  struct variable 
 *
 * @attention   None
 *
 * @param[in]   pstAgcRegConfig  point of Agc Regester Configure  struct variable
 * @param[in]   RegAddr      register addr
 * @param[in]   RegVal       val of register
 * @param[out]  None
 *
 * @return none 
 */
void GH3x2xAgcRegvalGet(STAgcRegConfig* pstAgcRegConfig, GU16 RegAddr, GU16 RegVal);

/**
 * @fn   GH3X2X_LedAgcInit(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo, STAgcRegConfig* pstAgcRegConfig)
 * 
 * @brief  init led agc
 *
 * @attention   just use to show function support
 *
 * @param[in]   pSTGh3x2xAgcInfo    pointer to agc info
 * @param[in]   pSTGh3x2xAgcInfo    point of Agc Regester Configure  struct variable
 * @param[out]  None
 *
 * @return  None
 */
 void GH3X2X_LedAgcInit(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo, STAgcRegConfig* pstAgcRegConfig);


/**
 * @fn void GH3X2X_AgcParaUpdate()
 * 
 * @brief  Update Agc Parameter
 *
 * @attention
 *
 * @param[in]   pSTGh3x2xAgcInfo    pointer to agc info
 * @param[out]  None
 *
 * @return  None
 */
 void GH3X2X_AgcParaUpdate(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo);

/**
 * @fn    void GH3X2X_LedAgcProcessIpd(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
 * 
 * @brief  led agc process function
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoLen           read fifo data buffer length
 * @param[in]   pSTGh3x2xAgcInfo           pointer to agc info
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo);

#endif  // _GH3X2X_AGC_H_

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
