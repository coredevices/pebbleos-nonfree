/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_mp_param_config.c
 * 
 * @brief   gh3x2x mp test param config
 * 
 * @author  Goodix Product Tools Development Team
 * 
 */

#include "gh3x2x_mp_param_config.h"

//test items select SKIP or TEST or NG_CONTINUE
const stMpItemsConfig stGh3x2xMpTestItemsSatus = 
{
	//SKIP:item skip, not test; TEST:item need test;  NG_CONTINUE: test item NG, continue to test
	.COMMUNICATION_TEST = 		TEST,
	.CHECK_CHIPID_TEST = 		TEST,  //need use drv interface to check, at present Only need to test this item when the driver is AFE
	.RST_INT_TEST = 			TEST,
	.EFUSE_TEST = 				TEST,
	.PPG_TEST = 				TEST,
	.ECG_TEST = 				TEST,
};

//PPG test items select SKIP or TEST or NG_CONTINUE
const stMpPPGItemsConfig stGh3x2xMpPPGItemsSatus = 
{
	.PPG_LPCTR_TEST = 			TEST,
	.PPG_NOISE_TEST = 			TEST,
	.PPG_BASENOISE_TEST = 		TEST,
	.PPG_LEAKLIGHT_TEST = 		SKIP,
	
};

//ECG test items select SKIP or TEST or NG_CONTINUE
const stMpECGItemsConfig stGh3x2xMpECGItemsSatus = 
{
	
	.ECG_LEADONOFF_TEST = 		TEST,
	.ECG_NOISE_TEST = 			TEST,
	.ECG_HPF_TEST = 			TEST,
	.ECG_SIGNAL_TEST = 			TEST,
	.ECG_RLD_TEST = 			TEST,
	.ECG_CMRR_TEST = 			TEST,
	.ECG_LIN_TEST = 			TEST,
	.ECG_DCRANGE_TEST = 		TEST,
};

//LED and PD enable/disable config
const emMp_Enable PPG_LEDPD_EN[MAX_LED_NUM][MAX_ADC_NUM] = 
{
	//every led can enable/disable 4 pd
	//PD0,   PD1,    PD2,    PD3
	{MP_ENABLE, MP_ENABLE, MP_ENABLE, MP_ENABLE}, 		//led 0
	{MP_ENABLE, MP_ENABLE, MP_ENABLE, MP_ENABLE}, 		//led 1
	{MP_ENABLE, MP_ENABLE, MP_ENABLE, MP_ENABLE}, 		//led 2
	{MP_DISABLE, MP_DISABLE, MP_DISABLE, MP_DISABLE}, 	//led 3
	{MP_DISABLE, MP_DISABLE, MP_DISABLE, MP_DISABLE}, 	//led 4
	{MP_DISABLE, MP_DISABLE, MP_DISABLE, MP_DISABLE}, 	//led 5
	{MP_DISABLE, MP_DISABLE, MP_DISABLE, MP_DISABLE}, 	//led 6
	{MP_DISABLE, MP_DISABLE, MP_DISABLE, MP_DISABLE} 	//led 7
};

//PPG test auto Dimming Config
const stMpAutoDimmingConfig stGh3x2xPPG_AutoDimming = 
{
	.LED_ACG_FS = {
						LED_DRIVE_50MA,   //led 0
						LED_DRIVE_25MA,   //led 1
						LED_DRIVE_25MA,   //led 2
						LED_DRIVE_25MA,   //led 3
						LED_DRIVE_25MA,   //led 4
						LED_DRIVE_25MA,   //led 5
						LED_DRIVE_25MA,   //led 6
						LED_DRIVE_25MA,	  //led 7
					},
	.LED_Max_Rawdata = 48787,
	.LED_Min_Rawdata = 45874,
	.LED_Ref_Up = 47816,
	.LED_Ref_Down = 46845,
};

//test limit config
const stMpItemsLimitConfig stGh3x2xMpLimit = 
{
	//PPG test limit
	.PPG_CTR_MinLimit = { //PPG CTR test limit
						  //PD0,  PD1,  PD2,  PD3
							11.0, 11.0, 11.0, 11.0,		//led 0
							24.0, 24.0, 24.0, 24.0,		//led 1
							24.0, 24.0, 24.0, 24.0,		//led 2
							24.0, 24.0, 24.0, 24.0,		//led 3
							24.0, 24.0, 24.0, 24.0,		//led 4
							24.0, 24.0, 24.0, 24.0,		//led 5
							24.0, 24.0, 24.0, 24.0,		//led 6
							24.0, 24.0, 24.0, 24.0,		//led 7
						},
	
	.PPG_LeakCTR_MaxLimit = { //PPG leak light test limit
							  //PD0, PD1, PD2, PD3
								1.0, 1.0, 1.0, 1.0,		//led 0
								0.8, 0.8, 0.8, 0.8,		//led 1
								0.8, 0.8, 0.8, 0.8,		//led 2
								0.8, 0.8, 0.8, 0.8,		//led 3
								0.8, 0.8, 0.8, 0.8,		//led 4
								0.8, 0.8, 0.8, 0.8,		//led 5
								0.8, 0.8, 0.8, 0.8,		//led 6
								0.8, 0.8, 0.8, 0.8,		//led 7
							},
	
	.PPG_LeakRatio_Throld = {  //PPG leak light test limit
								//PD0, PD1,   PD2,   PD3
								0.037, 0.037, 0.037, 0.037,		//led 0
								0.015, 0.015, 0.015, 0.015,		//led 1
								0.015, 0.015, 0.015, 0.015,		//led 2
								0.015, 0.015, 0.015, 0.015,		//led 3
								0.015, 0.015, 0.015, 0.015,		//led 4
								0.015, 0.015, 0.015, 0.015,		//led 5
								0.015, 0.015, 0.015, 0.015,		//led 6
								0.015, 0.015, 0.015, 0.015,		//led 7
							},
	
	.PPG_Noise_MaxLimit = 280,     //PPG Noise test limit
	.PPG_BaseNoise_MaxLimit = 97,  //PPG BaseNoise noise test limit
	.PPG_DCOffset_MaxLimit = 1.60, //PPG BaseNoise test limit
	.PPG_DCOffset_MinLimit = -7.7, //PPG BaseNoise test limit

							
	//ECG test limit
	.ECG_Noise_MaxLimit = 35,	   //ECG Noise test limit
	.ECG_HPF_MaxLimit = 0.87,	   //ECG HPF test limit
	.ECG_HPF_MinLimit = 0.67,	   //ECG HPF test limit
	.ECG_MaxAMP = 0.05,	   		   //ECG SIGNAL test limit
	.ECG_MaxFreq = 0.012,	       //ECG SIGNAL test limit
	.ECG_SIGNAL_MaxLimit = 0.25,   //ECG SIGNAL test limit
	.ECG_RLD_MaxAmp = 900.0,	   //ECG RLD test limit
	.ECG_CMRR_MinLimit = 85.0,	   //ECG CMRR test limit
	.ECG_LIN_MaxLimit = 0.0470,	   //ECG LIN test limit
	.ECG_DCRange_MaxLimit = 0.060, //ECG DCRange test limit
					
};

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
