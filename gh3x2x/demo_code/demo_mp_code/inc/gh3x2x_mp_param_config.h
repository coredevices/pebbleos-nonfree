/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_param_config.h
 *
 * @brief   header file of gh3x2x_mp_param_config.c
 *
 * @author  Goodix Product Tools Development Team
 *
 */
#ifndef _GH3X2X_MP_PARAM_CONFIG_H_
#define _GH3X2X_MP_PARAM_CONFIG_H_

#include "gh3x2x_mp_define.h"

//define test items select/not select test
typedef enum
{
	SKIP = 0,
	TEST = 1,
	NG_CONTINUE = 2,
}emMpTestItemStatus;

//define enable/disable
typedef enum
{
	MP_DISABLE = 0,
	MP_ENABLE = 1,
}emMp_Enable;

//define LED full-scale gear
typedef enum
{
	LED_DRIVE_25MA = 0x00,
	LED_DRIVE_50MA = 0x01,
	LED_DRIVE_100MA = 0x10,
	LED_DRIVE_200MA = 0x11,
}emAutoDimming_Gear;

//define gh3x2x all test items
typedef struct
{
	//all test items
    emMpTestItemStatus COMMUNICATION_TEST;
	emMpTestItemStatus CHECK_CHIPID_TEST;
	emMpTestItemStatus RST_INT_TEST;
	emMpTestItemStatus EFUSE_TEST;
	emMpTestItemStatus PPG_TEST;
	emMpTestItemStatus ECG_TEST;

}stMpItemsConfig;

//define PPG test items
typedef struct
{
	emMpTestItemStatus PPG_LPCTR_TEST;
	emMpTestItemStatus PPG_NOISE_TEST;
	emMpTestItemStatus PPG_BASENOISE_TEST;
	emMpTestItemStatus PPG_LEAKLIGHT_TEST;
	
}stMpPPGItemsConfig;

//define ECG test items
typedef struct
{
	
	emMpTestItemStatus ECG_LEADONOFF_TEST;
	emMpTestItemStatus ECG_NOISE_TEST;
	emMpTestItemStatus ECG_HPF_TEST;
	emMpTestItemStatus ECG_SIGNAL_TEST;
	emMpTestItemStatus ECG_RLD_TEST;
	emMpTestItemStatus ECG_CMRR_TEST;
	emMpTestItemStatus ECG_LIN_TEST;
	emMpTestItemStatus ECG_DCRANGE_TEST;
	
}stMpECGItemsConfig;

//define LED automatic dimming config
typedef struct
{
	emAutoDimming_Gear LED_ACG_FS[MAX_LED_NUM];//set LED full-scale gear ,00:25mA,01:50mA,10:100mA,11:200mA,
	GU32 LED_Max_Rawdata;//LED automatic dimming threshold dimming
    GU32 LED_Min_Rawdata;
    GU32 LED_Ref_Up;
    GU32 LED_Ref_Down;

}stMpAutoDimmingConfig;


//define gh3x2x test limit
typedef struct
{
    GF32 PPG_CTR_MinLimit[MAX_SLOT_NUM*MAX_ADC_NUM];
	GF32 PPG_LeakCTR_MaxLimit[MAX_SLOT_NUM*MAX_ADC_NUM];
	GF32 PPG_LeakRatio_Throld[MAX_SLOT_NUM*MAX_ADC_NUM];
	GF32 PPG_Noise_MaxLimit;
	GF32 PPG_BaseNoise_MaxLimit;
	GF32 PPG_DCOffset_MaxLimit;
	GF32 PPG_DCOffset_MinLimit;

	
	GF32 ECG_Noise_MaxLimit;
	GF32 ECG_HPF_MinLimit;
	GF32 ECG_HPF_MaxLimit;
	GF32 ECG_MaxAMP;
	GF32 ECG_MaxFreq;
	GF32 ECG_SIGNAL_MaxLimit;
	GF32 ECG_RLD_MaxAmp;
	GF32 ECG_CMRR_MinLimit;
	GF32 ECG_LIN_MaxLimit;
	GF32 ECG_DCRange_MaxLimit;
	
}stMpItemsLimitConfig;

extern const stMpItemsConfig stGh3x2xMpTestItemsSatus;

extern const stMpPPGItemsConfig stGh3x2xMpPPGItemsSatus;

extern const stMpECGItemsConfig stGh3x2xMpECGItemsSatus;

extern const emMp_Enable PPG_LEDPD_EN[MAX_LED_NUM][MAX_ADC_NUM];

//LED automatic dimming threshold dimming
extern const stMpAutoDimmingConfig stGh3x2xPPG_AutoDimming;

extern const stMpItemsLimitConfig stGh3x2xMpLimit;

#endif /* _GH3X2X_MP_PARAM_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
