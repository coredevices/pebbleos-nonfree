/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_ecg.h
 *
 * @brief   gh3x2x lead process functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#ifndef _GH3X2X_DRV_ECG_H_
#define _GH3X2X_DRV_ECG_H_

#include "gh_drv_config.h"
#include "gh_drv.h"

/// difference threshold, unit: mV
#define  LEADOFF_DET_DIF_THR_MV             (15)

/// Val threshold, unit:mV
#define  LEADOFF_DET_VAL_THR_MV             (70)

/// detect sample point, if input volt v(n)-v(n- LEADOFF_DET_POINT_NUM) >  LEADOFF_DET_THR_mV,  assert lead off
#define  LEADOFF_DET_POINT_NUM              (3)

/// base offset detect, unit:mV
#define LEADOFF_BASE_THR1_MV                (5)

/// lead off base hold samp
#define LEADOFF_BASE_HOLD_SAMP              (400)

/// lead off base rst samp
#define LEADOFF_BASE_RST_SAMP               (100)

/// lead off moving num
#define LEADOFF_MOVING_NUM                  (20)

/* lead state */
#define LEAD_STATE_LEAD_ON                  (1)     /**< lead state lead on */
#define LEAD_STATE_LEAD_OFF                 (0)     /**< lead state lead off */

/* soft lead type */
#define SOFT_LEAD_NULL_TYPE                 (0)     /**< soft lead null type */
#define SOFT_LEAD_RTYPE                     (1)     /**< soft lead R type */
#define SOFT_LEAD_ITYPE                     (2)     /**< soft lead I type */

/* soft lead off detect mode */
#define SOFT_LEAD_OFF_DET_MODE_DISABLE      (0)     /**< soft lead off detect mode disable */
#define SOFT_LEAD_OFF_DET_MODE_SW_AND_HW    (1)     /**< soft lead off detect mode sw and hw */
#define SOFT_LEAD_OFF_DET_MODE_SW_ONLY      (2)     /**< soft lead off detect mode sw only */

/// get ecg lead channel map from slot num
#define  ECG_LEAD_GET_CHANNEL_MAP(n)        (GH3X2X_CHANNEL_MAP(GH3X2X_VAL_LEFT_SHIFT((n), 5), \
                                                                GH3X2X_CHANNEL_MAP_ADC3))

#define GET_ECG_LEAD_DET_EN_VAL()              (GH3X2X_ReadRegBitField(GH3X2X_ADT_LEADON_CR_REG_ADDR, \
                                                    GH3X2X_ECG_LEAD_DET_EN_LSB, GH3X2X_ECG_LEAD_DET_EN_MSB))
/// ecg lead detect enable config
#define  ECG_LEAD_DET_EN_CONFIG(en)         do {\
                                                GH3X2X_WriteRegBitField(GH3X2X_ADT_LEADON_CR_REG_ADDR, \
                                                    GH3X2X_ECG_LEAD_DET_EN_LSB, GH3X2X_ECG_LEAD_DET_EN_MSB, (en)); \
                                            } while (0)

/// ecg fast recover enable config
#define  ECG_FAST_RECV_EN_CONFIG(en)        do {\
                                                GH3X2X_WriteRegBitField(GH3X2X_ECG_CTRL_REG_ADDR, \
                                                    GH3X2X_ECG_FAST_RECV_EN_LSB, GH3X2X_ECG_FAST_RECV_EN_MSB, (en)); \
                                            } while (0)

/// lead on auto switch bit field val
#define  GET_LEAD_ON_AUTO_SWITCH_VAL()      (GH3X2X_ReadRegBitField(GH3X2X_ECG_CTRL_REG_ADDR, \
                                                    GH3X2X_LEAD_ON_AUTO_SWITCH_LSB, GH3X2X_LEAD_ON_AUTO_SWITCH_MSB))

/// lead off auto switch bit field val
#define  GET_LEAD_OFF_AUTO_SWITCH_VAL()     (GH3X2X_ReadRegBitField(GH3X2X_ECG_CTRL_REG_ADDR, \
                                                    GH3X2X_LEAD_OFF_AUTO_SWITCH_LSB, GH3X2X_LEAD_OFF_AUTO_SWITCH_MSB))

/// soft lead adc ref amplify num
#define SOFT_LEAD_ADC_REF_AMP_NUM               (1000)

/// lead off diff thr default val
#define SOFT_LEAD_OFF_DIFF_THR_DEFAULT_VAL      ((GS32)(15 * (20 * GH3X2X_RAWDATA_REAL_OFFSET / 1600)))

/// lead off peak thr default val
#define SOFT_LEAD_OFF_PEAK_THR_DEFAULT_VAL      ((GS32)(70 * (20 * GH3X2X_RAWDATA_REAL_OFFSET / 1600)))

/// lead off base thr default val
#define SOFT_LEAD_OFF_BASE_THR_DEFAULT_VAL      ((GS32)(5 * (20 * GH3X2X_RAWDATA_REAL_OFFSET / 1600)))

/// soft lead buffer head index init val
#define SOFT_LEAD_BUFFER_HEAD_INIT_INDEX        (-1)

/* soft lead off detect function ret bit val */
#define SOFT_LEAD_OFF_DETECT_RET_BIT0           (0x01)      /**< bit0=1 absAdcValue larger than thresh */
#define SOFT_LEAD_OFF_DETECT_RET_BIT1           (0x02)      /**< bit1=1 diff larger than thresh */
#define SOFT_LEAD_OFF_DETECT_RET_BIT2           (0x04)      /**< bit2=1 adc value continuously larger than thresh */

/* soft lead cfg param reg posi */
#define SOFT_LEAD_CFG_PARAM_REG_POSI_0          (0u)        /**< soft lead cfg param reg posi 0 */
#define SOFT_LEAD_CFG_PARAM_REG_POSI_1          (1u)        /**< soft lead cfg param reg posi 1 */
#define SOFT_LEAD_CFG_PARAM_REG_POSI_2          (2u)        /**< soft lead cfg param reg posi 2 */

/* soft lead cfg of bit filed */
#define SOFT_LEAD_CFG_ECG_FORCE_RECV_LSB        (0)         /**< cfg index for g_uchEcgForceRecover */
#define SOFT_LEAD_CFG_ECG_FORCE_RECV_MASK       (0x01)      /**< cfg mask for g_uchEcgForceRecover */
#define SOFT_LEAD_CFG_EX_EN_VIA_HOST_LSB        (1)         /**< cfg index for g_uchExtraLeadDetectEnableViaHost */
#define SOFT_LEAD_CFG_EX_EN_VIA_HOST_MASK       (0x01)      /**< cfg mask for g_uchExtraLeadDetectEnableViaHost */
#define SOFT_LEAD_CFG_OFF_DET_MODE_LSB          (2)         /**< cfg index for g_uchSoftLeadOffDetMode */
#define SOFT_LEAD_CFG_OFF_DET_MODE_MASK         (0x03)      /**< cfg mask for g_uchSoftLeadOffDetMode */
#define SOFT_LEAD_CFG_ECG_GAIN_GRADE_LSB        (4)         /**< cfg index for g_uchEcgGainGrade */
#define SOFT_LEAD_CFG_ECG_GAIN_GRADE_MASK       (0x07)      /**< cfg mask for g_uchEcgGainGrade */
#define SOFT_LEAD_CFG_ADC_REF_GRADE_LSB         (7)         /**< cfg index for g_uchAdcRefGrade */
#define SOFT_LEAD_CFG_ADC_REF_GRADE_MASK        (0x01)      /**< cfg mask for g_uchAdcRefGrade */
#define SOFT_LEAD_CFG_ECG_AUTO_CTRL_LSB         (0)         /**< cfg index for g_uchEcgAutoCtrlMode */
#define SOFT_LEAD_CFG_ECG_AUTO_CTRL_MASK        (0x03)      /**< cfg mask for g_uchEcgAutoCtrlMode */

//G202008231001 wanpeng START
/****************************** ecg lead cfg ****************************************/

/**
 * @brief Downsample Function
*/
typedef enum
{
	DOWNSAMPLE_ECG = 0,
	DOWNSAMPLE_PPG,
}EmDownSampleFunction;

/**
 * @brief Downsample type
*/
typedef enum
{
	DOWNSAMPLE_DEFAULT = 0,
	DOWNSAMPLE_800HZ_TO_500HZ = 1,
	DOWNSAMPLE_800HZ_TO_250HZ = 2,
	DOWNSAMPLE_400HZ_TO_250HZ = 3,
	DOWNSAMPLE_320HZ_TO_250HZ = 4,
	DOWNSAMPLE_256HZ_TO_250HZ = 5,
}EmDownSampleType;

typedef struct 
{
    GU8 uchElectrodeAdtEn          : 1;  //0:disable electrode adt  1:enable electrode adt
    GU8 uchElectrodeWearRevCntEn   : 1;  //elctrode wear state reverse counter dis/en    0: disable  1: enable
    GU8 uchSoftWearOffDetEn        : 1;  //software wear off detction enable   0: disable  1: enable
    GU8 uchElectrodeSoftWearColor  : 1;  //software wear led color             0:green     1:ir
    GU8 uchHardAdtCheckByAlgo      : 1;  //0: do not use algo to judge adt   1: use algo to judge adt
    GU8 uchRes                     : 3;
}STAdtCfgByte0;

typedef struct t_adt_cfg
{
    GU8 uchAdtSampleCtrlMode;     // 0: wear state do not affect sample  1:  wear off only open adt slot     2: wear on/off need switch cfg    
    STAdtCfgByte0 stAdtCfgByte0;   
    GU8 ubResByte2;
    GU8 ubResByte3;
}STAdtCfg;

typedef struct
{
    GU8 uchForceRecover           :1;  /* 0: do not force recover when ecg starting sample   1: do force recover when ecg starting sample */
    GU8 uchNeedHostEnableLeadDet  :1;  /* 0: host no need extra enable lead detecting  1:  host need extra enable lead detecting */
    GU8 uchSoftLeadDetMode        :2;  /* 0: disable  lead off detect  1: SW&HW 2: SW only    */
    GU8 uchEcgGainGrade           :3;  /* 0: 1x  1: 2.5x  2: 5x  3: 10x  4: 20x */
    GU8 uchAdcRefGrade            :1;  /* 0: 1.6 V  1: 1.8 V */
} STEcgLeadCtrlFlag1;


typedef struct
{
    GU8 uchAutoCtrlMode           :2;  /* 0: disable   1: hardware 2: software */
    GU8 uchDisItypeWhenEcgSample  :1;  /* 0: do not disable Itype  when ecg sample stating  1:  disable Itype  when ecg sample stating */
    GU8 uchWearOffLogic           :1;  /* 0:do not rest with ecg elctronde only  1: rest with ecg elctronde only    */
    GU8 Res                       :4;  
} STEcgLeadCtrlFlag2;

typedef struct
{
    STEcgLeadCtrlFlag1 uchEcgLeadCtrlFlag1;             
    GU8 uchLeadOffPeakVolt;              
    GU8 uchLeadOffDiffVolt; 
    GU8 uchLeadOffBaseVolt; 
    STEcgLeadCtrlFlag2 uchEcgLeadCtrlFlag2;                            
    GU8 uchRes1;             
} STEcgLeadCfg;

typedef struct
{
    GU8   uchRsFirOrder;
    GU8   uchRsI;
    GU8   uchRsD;
    GU8   uchRsFirCoefNum;
    GU8   uchRsBufSize;
    GU8   uchRsEnable;
    GU32  ufStep;
    GS32* rsFirCoef;
    GU8   uchDownSampleType;
} STGh3x2xDownSampleCalParamInfo;

/**
 * @fn     void GH3X2X_EcgForceRecoverAfterSlotCtrl(GU16 usSlotEnableConfig, GU8 uchSlotEnable)
 *
 * @brief  ecg sample  hook handle after ecg slot ctrl
 *
 * @attention   None
 *
 * @param[in]   usSlotEnableConfig         slot enable index , @ref GH3X2X_SLOT_INDEX_0 ... GH3X2X_SLOT_INDEX_ALL
 * @param[in]   uchSlotEnable              enable flag, #0 slot disable, #others slot enable
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgSampleHookHandleAfterSlotCtrl(GU16 usSlotEnableConfig, GU8 uchSlotEnable);

/**
 * @fn     void GH3X2X_EcgLeadSlotConfig(GU8 uchEcgSlotBit)
 *
 * @brief  ecg lead slot config
 *
 * @attention   None
 *
 * @param[in]   uchEcgSlotBit                      ecg slot bit, @ref GH3X2X_SLOT_INDEX_0...
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgLeadSlotConfig(GU8 uchEcgSlotBit);

/**
 * @fn     GH3X2X_LeadDetEnControl(GU8 uchEventInfo)
 *
 * @brief  lead detect enable control
 *
 * @attention   None
 *
 * @param[in]   uchEventInfo         control event, only ecg start/stop evt 
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LeadDetEnControl(GU8 uchEventInfo);

void GH3x2xSetEcgOutputFs(GU16 usEcgOutputFs);
void GH3X2X_WriteAdtDrvConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

#endif /* _GH3X2X_DRV_SOFT_LEAD_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

