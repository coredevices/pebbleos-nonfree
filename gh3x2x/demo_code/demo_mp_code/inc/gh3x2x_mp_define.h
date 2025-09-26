/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_define.h
 *
 * @brief   define test items para, error code and so on
 *
 * @author  Goodix Product Tools Development Team
 *
 */
 
#ifndef _GH3X2X_MP_DEFINE_H_
#define _GH3X2X_MP_DEFINE_H_

#include "gh3x2x_mp_config.h"

/*******GH3x2x test fw version define************/
#define GH3X2X_MP_LIB_VERSION								("GH3x2x_MP_Lib_V1.0.0.0.0")
/***END****GH3x2x test fw version define*********/

/*******test error code define************/
#define MT_TEST_PASS															0x00

#define MT_ERROR_COMMUNICATION_TEST_ERROR										0x01
#define	MT_ERROR_EFUSE_ERROR													0x04
#define MT_ERROR_LP_NOISE_ABNORMAL												0x07
#define MT_ERROR_LPBASE_NOISE_ABNORMAL											0x08
#define MT_ERROR_LPBASE_DC_OFFSET_ABNORMAL										0x09
#define MT_ERROR_LED_LP_RAWDATA_ABNORMAL										0x10

#define MT_ERROR_LEAK_LEDLP_RAWDATA_ABNORMAL									0x30
#define MT_ERROR_LEAK_AVGER_ERR 												0x40

#define MT_ERROR_COMMUNICATE_FAIL												0x18
#define MT_ERROR_CHECK_CHIPID_ERROR												0x20

#define MT_ERROR_ECG_LEADONOFF_ERR												0x60
#define MT_ERROR_ECG_NOISE_ERR													0x61
#define MT_ERROR_ECG_HPF_ERR													0x62
#define MT_ERROR_ECG_SIGNAL_ERR													0x63
#define MT_ERROR_ECG_CMMR_ERR													0x64
#define MT_ERROR_ECG_RLD_ERR													0x65
#define MT_ERROR_ECG_LIN_ERR													0x66
#define MT_ERROR_ECG_DCRANGE_ERR												0x67
/***END****test error code define*********/


/*******test cmd define************/
#define GH3X2X_MP_CMD_RESUME            				(0xC3)  //chip resume cmd
/***END****test cmd define*********/


/*******test related information define************/
#define READ_RAWDATA_FIFO_MAX_BYTE_NUM					3200
#define RAWDATA_BUF_SIZE								MAX_SLOT_NUM*MAX_ADC_NUM*MAX_PPG_RAWDATA_SAMPLE_NUM*4


#define GH3X2X_DEMO_MP_EFUSE_MAX_LEN 		32
#define CHIP_UID_LENGTH						12

#define START_SAMPLING						0
#define STOP_SAMPLING						1

#define MAX_SLOT_NUM   						8
#define MAX_LED_NUM   						8
#define MAX_ADC_NUM   						4

#define SPEC_MODE_PPG                    	0
#define SPEC_MODE_ECG                    	1

#define SLOT_ONECH_MODE     				0        //Using single-channel mode, only one PD is collected per slot
#define SLOT_MULTICH_MODE   				1        //Using multi-channel mode, each slot can collect multiple PDs, but does not support automatic dimming
#define SLOT_CFG_INDEX   					0x1C     //Each slot register address offset

#define ADC_OFFSET        								8388608
#define RAWDATA_TO_VOL(x)   							((x)*1.8*1000000 / ADC_OFFSET)  //rawdata to voltage value, unit uV
#define ADC_MAX											(GF32)(0x800000)	// ADC full offset value
#define ADC_REFER        								1800.0f				// ADC reference voltage
#define GAIN        									20.0f				// System gain

#define LED_POWER_OFF									0
#define LED_POWER_ON									1

#define	LED0_LLPRAWDATA_INDEX							32

#define ECG_MAX_SIGNAL_NUM 								5
#define ECGBOARD_DAC_CHANNELA            				0x01
#define ECGBOARD_DAC_CHANNELB            				0x08
#define UART_CMD_GETADC  								0x13
/***END****test related information define*********/

/*******test prompt message define************/
#define PPG_PROMPT_UP_RUBBER_MSG										"Please up the Rubber!"
#define GH3X2X_MP_TEST_OK_MSG											"Gh3x2x mp test is OK!"
#define GH3X2X_MP_TEST_NG_MSG											"Gh3x2x mp test is NG!"
/***END****test prompt message define*********/


/*******test param define************/
//define gh3x2x mp slot config struct
typedef struct
{
	GU8 Slot_Cfg_N; 						//Configure the corresponding slot cfg number
	GU8 Slot_Mutiple;  						//Current slot period multiple
	GU8 ADC_N;								//ADC number used
	GU8 ADC_IntTimes;						//ADC integration times
	GU8 PD_N;								//PD number, can be set to multi-channel, such as PD0|PD1
	GU8 LED_N;								//LED number, greater than 8 means not connect any LED
	GU8 LED_ACG;							//Automatic dimming enable configuration
	GU8 LED_Drv;							//LED current configuration, this configuration is invalid after turning on automatic dimming
	GU8 TIA_RF; 							//TIA gain resistor configuration
	GU8 TIA_CF; 							//TIA capacitor configuration
	GU8 BG_Level;							//bg suppression level, 0:only led, 1:1st order amb cancel, 2:2nd order amb cancel, 3:2x 2nd order amb cancel, 4: ECG mode
	GU16 Slot_TMR;							//Slot running time, unit 1us
	GU8 Slot_Mode;							//SLOT_MULTICH_MODE or SLOT_ONECH_MODE
}stGh3x2xMp_SlotConfig;

//LED&PD channel enable config struct
typedef struct{
    GU8 PD_Channel[MAX_LED_NUM];     //PD location to be tested for each slot
    GU8 PD_Num[MAX_LED_NUM];         //The number of PDs to be tested for each slot, the maximum is MAX_ADC_NUM
    GU8 PD_Channel_AllSlot;          //PD positions to be tested for all slots
    GU8 PD_Num_AllSlot;              //The number of PDs to be tested for all slots, the maximum is MAX_ADC_NUM
    GU8 LED_PD_Num;                  //The number of LED*PD to be tested, the maximum is MAX_ADC_NUM*MAX_LED_NUM
    GU8 LED_Channel;                 //LED location to be tested
    GU8 LED_Num;                     //The number of LEDs to be tested, the maximum is MAX_LED_NUM
}stGh3x2xMp_LEDPDConfig;


//define gh3x2x mp PPG test param
typedef struct
{
	GU32 PPG_CTRRawdata_Num;  						//need get nums, max is MAX_PPG_RAWDATA_SAMPLE_NUM
	GU32 PPG_CTRRawdata_SkipNum; 					//need skip num
	GU32 PPG_NoiseRawdata_Num;
	GU32 PPG_NoiseRawdata_SkipNum;
	GU32 PPG_BaseNoiseRawdata_Num;
	GU32 PPG_BaseNoiseRawdata_SkipNum;
	GU32 PPG_LeakLightRawdata_Num;
	GU32 PPG_LeakLightRawdata_SkipNum;
	
	//test items slot config
	stGh3x2xMp_SlotConfig stCTRSlotConfig;  
	stGh3x2xMp_SlotConfig stNoiseSlotConfig;
	stGh3x2xMp_SlotConfig stBaseNoiseSlotConfig;
	stGh3x2xMp_SlotConfig stLeakLightSlotConfig;
	
	stGh3x2xMp_LEDPDConfig stLEDPDConfig; //LED&PD channel enable config
	
}stGh3x2xMp_PPGTestParam;


//Set ECG IO status according to ECG test item
typedef enum
{
	ECG_SIGNAL_AND_IO_STATUS_DEFAULT = 0x00,
	ECG_SIGNAL_AND_IO_STATUS_LEAD_ON = 0x01,
	ECG_SIGNAL_AND_IO_STATUS_LEAD_OFF = 0x02,
	ECG_SIGNAL_AND_IO_STATUS_NOISE = 0x03,
	ECG_SIGNAL_AND_IO_STATUS_HPF = 0x04,
	ECG_SIGNAL_AND_IO_STATUS_SIGNAL = 0x05,
	ECG_SIGNAL_AND_IO_STATUS_RLD = 0x06,
	ECG_SIGNAL_AND_IO_STATUS_CMMR = 0x07,
	ECG_SIGNAL_AND_IO_STATUS_LIN = 0x08,
	ECG_SIGNAL_AND_IO_STATUS_DC_RANGE = 0x09,
	
}emECG_SIGNAL_AND_IO_STATUS_SET;

//define Special treatment for ECG items set signal and io status 
typedef enum
{
	NONE_SPEC = 0,
	ECG_HPF_SPEC = 1,
	
}emECG_SET_SIGNAL_SPEC;

//Set ECG signal type
typedef enum
{
	NONE_MODE = 0,
	DC_MODE,
	AC_MODE,
	ANG_MODE,
	
}emECG_Signal_Type;

//define gh3x2x mp ECG signal struct param
typedef struct
{
	GU16 Rawdata_Num;
	GU16 Rawdata_SkipNum;
	GU8 Sampling_Step; //rawdata sampling step length, must need > 0
	
	GBOOL bECG_NegativeSignal;//Positive or negative signals
	GBOOL bECG_HighImpedance;//whether High impedance
	GF32  ECG_Freq;//signal freq
	GF32  ECG_Vol; //Signal amplitude
	GF32  ECG_ACOffset;//AC signal offset
	GF32  ECG_DCOffset;//DC signal offset
	emECG_Signal_Type   ECG_SignalType;//Signal type, sine wave, triangle wave
}stGh3x2xMp_ECGSignalSetting;

//define gh3x2x mp ECG test param
typedef struct
{
	//test item signal num setting, only setting HPF,SIGNAL,CMRR,LIN and DCRange
	GU8 HPF_ECGSignalNum; //must 2 signals in a group
	GU8 SIGNAL_ECGSignalNum;
	GU8 CMRR_ECGSignalNum;
	GU8 LIN_ECGSignalNum; //must 2 signals in a group
	GU8 DCRange_ECGSignalNum; //must 3 signals in a group
	
	//setting HPF,SIGNAL,CMRR,LIN and DCRange signal config
	stGh3x2xMp_ECGSignalSetting stECGSignal_HPF[ECG_MAX_SIGNAL_NUM]; //rawdata num need >= 200
	stGh3x2xMp_ECGSignalSetting stECGSignal_SIGNAL[ECG_MAX_SIGNAL_NUM];
	stGh3x2xMp_ECGSignalSetting stECGSignal_CMRR[ECG_MAX_SIGNAL_NUM]; //rawdata num need >= 200
	stGh3x2xMp_ECGSignalSetting stECGSignal_LIN[ECG_MAX_SIGNAL_NUM];
	stGh3x2xMp_ECGSignalSetting stECGSignal_DCRange[ECG_MAX_SIGNAL_NUM];
	
	//setting LeadOnOff, Noise and RLD signal config, only need set Rawdata_Num and Rawdata_SkipNum
	stGh3x2xMp_ECGSignalSetting stECGSignal_LeadOnOff;
	stGh3x2xMp_ECGSignalSetting stECGSignal_Noise; //rawdata num need >= 600
	stGh3x2xMp_ECGSignalSetting stECGSignal_RLD;

}stGh3x2xMp_ECGTestParam;

//define gh3x2x mp all test param
typedef struct
{
	//PPG and ECG test param set
	stGh3x2xMp_PPGTestParam stPPG_TestParam;
	stGh3x2xMp_ECGTestParam stECG_TestParam;

}stGh3x2xMp_TestParam;
/***END****test param define*********/


/*******test result define************/
//define gh3x2x mp Efuse test result
typedef struct
{
	GU8	uchChipUid[CHIP_UID_LENGTH];

}stGh3x2xMp_EfuseTestResult;

//define gh3x2x mp rawdata handle data result
typedef struct
{
	GF32 Noise[MAX_LED_NUM*MAX_SLOT_NUM];
	GF32 AvergeRawdata[MAX_LED_NUM*MAX_SLOT_NUM];

}stGh3x2xMp_RawdataHandleResult;

//define gh3x2x mp PPG test result
typedef struct
{
	GF32 LED_Noise[MAX_LED_NUM*MAX_ADC_NUM];
	GF32 LED_BaseNoise[MAX_LED_NUM*MAX_ADC_NUM];
	GF32 LED_LPCTR[MAX_LED_NUM*MAX_ADC_NUM];
	GF32 LED_DCOffset[MAX_LED_NUM*MAX_ADC_NUM];
	GF32 LED_LLPCTR[MAX_LED_NUM*MAX_ADC_NUM];
	GF32 LED_LeakRatio[MAX_LED_NUM*MAX_ADC_NUM];
	
	stGh3x2xMp_RawdataHandleResult stCTRRawHandleResult;
	stGh3x2xMp_RawdataHandleResult stNoiseRawHandleResult;
	stGh3x2xMp_RawdataHandleResult stBaseNoiseRawHandleResult;
	stGh3x2xMp_RawdataHandleResult stLeakLightRawHandleResult;

}stGh3x2xMp_PPGTestResult;

//define gh3x2x mp ECG test result
typedef struct
{
	GF32 ECG_Noise;
	GF32 ECG_HPFRatio[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_Amp[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_Freq[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_RLD;
	GF32 ECG_CMRR[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_LIN_Ratio[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_DCRange_Ratio[ECG_MAX_SIGNAL_NUM];
	GF32 ECG_DCRange_Amp[ECG_MAX_SIGNAL_NUM];
	
}stGh3x2xMp_ECGTestResult;

//define gh3x2x mp all test result
typedef struct
{
	stGh3x2xMp_EfuseTestResult stEfuse_TestResult;
	stGh3x2xMp_PPGTestResult stPPG_TestResult;
	stGh3x2xMp_ECGTestResult stECG_TestResult;
}stGh3x2xMp_TestResult;
/***END****test result define************/

#endif /* _GH3X2X_MP_DEFINE_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
