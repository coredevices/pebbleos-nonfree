/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_mp.c
 * 
 * @brief   gh3x2x mp test example
 * 
 * @author  Goodix Product Tools Development Team
 * 
 */
 
/* includes */
#include <stdio.h>
#include <string.h>
#include "gh3x2x_demo_mp.h"
#include "gh3x2x_mp_reg_config.h"
#include "gh3x2x_mp_common.h"
#include "gh3x2x_mp_algorithm.h"
#include "gh3x2x_demo_mp_process.h"
#include "gh3x2x_mp_param_config.h"

extern GU8 GH3x2xMp_IntFlag;

/**
 * @fn     void Gh3x2xDemoMp_Init()
 *
 * @brief  GH3x2x mp init
 *
 * @attention   None
 *
 * @param[in]   none
 * @param[out]  None
 *
 * @return  none
 */
void Gh3x2xDemoMp_Init(void)
{

#if( __GH3X2X_MP_USE_DRV_INTERFACE__ )
	//use drv lib interface
	
#else
#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_MP_INTERFACE_I2C__ )
	gh3x2x_mp_i2c_init();
#else
	gh3x2x_mp_spi_init();
#endif //__GH3X2X_MP_INTERFACE__
	
	gh3x2x_mp_reset_pin_init();
	gh3x2x_mp_int_init();
#endif //__GH3X2X_MP_USE_DRV_INTERFACE__
	
	gh3x2x_mp_led_power_pin_init();
	gh3x2x_mp_led_power_off(); //need do it
	
}

/**
 * @fn     GU8 Gh3x2xDemoMp_StartTest()
 *
 * @brief  GH3x2x mp start test
 *
 * @attention   None
 *
 * @param[in]   none
 * @param[out]  None
 *
 * @return  0:test ok, other:error code
 */

GU8 Gh3x2xDemoMp_StartTest()
{
	GU8 gh3x2x_mp_test_code = 0; //Save the first NG error code
	GU8 test_code_tmp = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp start! \r\n");
	//print version
	GH3X2X_MP_LOG_L2("Gh3x2x mp lib version: %s \r\n", GH3X2X_MP_LIB_VERSION);

	stGh3x2xMp_TestParam stTestParam;
	stGh3x2xMp_TestResult stTestResult;
	memset(&stTestParam, 0, sizeof(stGh3x2xMp_TestParam));
	memset(&stTestResult, 0, sizeof(stGh3x2xMp_TestResult));
	
	//set test param
	Gh3x2xDemoMp_SetTestParam(&stTestParam);
	
	//reset
	gh3x2x_demo_mp_reset_operation();
	
	do
	{
		// Communication Test
		if(stGh3x2xMpTestItemsSatus.COMMUNICATION_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_CommunicationTest(NULL, NULL);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp && (NG_CONTINUE != stGh3x2xMpTestItemsSatus.COMMUNICATION_TEST))
				break;
		}

		//Check Chip ID Test
		//need use drv interface to check, at present Only need to test this item when the driver is AFE
#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		//No need to do at the moment
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		if(stGh3x2xMpTestItemsSatus.CHECK_CHIPID_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_CheckChipIDTest(NULL, NULL);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp)
				break;
		}
	#endif
#endif		

		//RstInt Test
		if(stGh3x2xMpTestItemsSatus.RST_INT_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_RstIntTest(NULL, NULL);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp && (NG_CONTINUE != stGh3x2xMpTestItemsSatus.RST_INT_TEST))
				break;
		}
		
		//Efuse Test
		if(stGh3x2xMpTestItemsSatus.EFUSE_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_EfuseTest(NULL, &stTestResult.stEfuse_TestResult);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp && (NG_CONTINUE != stGh3x2xMpTestItemsSatus.EFUSE_TEST))
				break;
		}
		
		//PPG Test 
		if(stGh3x2xMpTestItemsSatus.PPG_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_PPGTest(&stTestParam.stPPG_TestParam, &stTestResult.stPPG_TestResult);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp && (NG_CONTINUE != stGh3x2xMpTestItemsSatus.PPG_TEST))
				break;
		}
		
		//ECGTest
		if(stGh3x2xMpTestItemsSatus.ECG_TEST)
		{
			test_code_tmp = Gh3x2xDemoMp_ECGTest(&stTestParam.stECG_TestParam, &stTestResult.stECG_TestResult);
			if(!gh3x2x_mp_test_code) //Save the first NG error code
				gh3x2x_mp_test_code = test_code_tmp;
			if(test_code_tmp && (NG_CONTINUE != stGh3x2xMpTestItemsSatus.ECG_TEST))
				break;
		}
		
	}while(0);
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp end! \r\n");
	
	if(!gh3x2x_mp_test_code)
	{
		//test ok,OK light comes on
		GU8 prompt_msg[] = GH3X2X_MP_TEST_OK_MSG;
		gh3x2x_mp_prompt_message(prompt_msg, sizeof(prompt_msg) / sizeof(GU8));
		GH3X2X_MP_LOG_L2("\r\n ************************************************************ \r\n");
		GH3X2X_MP_LOG_L2("\r\n Gh3x2x mp test is OK! \r\n");
		GH3X2X_MP_LOG_L2("\r\n ************************************************************ \r\n");
	}
	else
	{
		//test NG,NG light comes on
		GU8 prompt_msg[] = GH3X2X_MP_TEST_NG_MSG;
		gh3x2x_mp_prompt_message(prompt_msg, sizeof(prompt_msg) / sizeof(GU8));
		GH3X2X_MP_LOG_L2("\r\n ************************************************************ \r\n");
		GH3X2X_MP_LOG_L2("\r\n Gh3x2x mp test is NG, error code is: %d!\r\n", gh3x2x_mp_test_code);
		GH3X2X_MP_LOG_L2("\r\n ************************************************************ \r\n");
	}
	
	if(__GH3X2X_MP_PRINT_TEST_RESULT__)
		Gh3x2xDemoMp_PrintTestResult(&stTestParam, &stTestResult);
	
	return gh3x2x_mp_test_code;
}

/**
 * @fn     void Gh3x2xDemoMp_SetTestParam_PPG(stGh3x2xMp_TestParam *stTestParam)
 *
 * @brief  GH3x2x MP set PPG test items param
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *stPPG_TestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam_PPG(stGh3x2xMp_PPGTestParam *stPPG_TestParam)
{
	GU8 i = 0;
	GU8 j = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp start set ppg test param! \r\n");
	
	//CTR test slot config
	memset(&stPPG_TestParam->stCTRSlotConfig, 0, sizeof(stGh3x2xMp_SlotConfig));
	stPPG_TestParam->stCTRSlotConfig.Slot_Mutiple = 9;
	stPPG_TestParam->stCTRSlotConfig.TIA_RF = 0;
	stPPG_TestParam->stCTRSlotConfig.TIA_CF = 0;
	stPPG_TestParam->stCTRSlotConfig.ADC_N = 5;
	stPPG_TestParam->stCTRSlotConfig.ADC_IntTimes = stPPG_TestParam->stCTRSlotConfig.ADC_N;
	stPPG_TestParam->stCTRSlotConfig.BG_Level = 2;
	stPPG_TestParam->stCTRSlotConfig.LED_ACG = 0;
	stPPG_TestParam->stCTRSlotConfig.LED_Drv = 51;
	stPPG_TestParam->stCTRSlotConfig.Slot_TMR = 985;
	stPPG_TestParam->stCTRSlotConfig.Slot_Mode = SLOT_MULTICH_MODE;
	
	//Noise test slot config
	memset(&stPPG_TestParam->stNoiseSlotConfig, 0, sizeof(stGh3x2xMp_SlotConfig));
	stPPG_TestParam->stNoiseSlotConfig.Slot_Mutiple = 9;
	stPPG_TestParam->stNoiseSlotConfig.TIA_RF = 4;
	stPPG_TestParam->stNoiseSlotConfig.TIA_CF = 0;
	stPPG_TestParam->stNoiseSlotConfig.ADC_N = 5;
	stPPG_TestParam->stNoiseSlotConfig.ADC_IntTimes = stPPG_TestParam->stNoiseSlotConfig.ADC_N;
	stPPG_TestParam->stNoiseSlotConfig.BG_Level = 1;
	stPPG_TestParam->stNoiseSlotConfig.LED_ACG = 3;
	stPPG_TestParam->stNoiseSlotConfig.LED_Drv = 0;
	stPPG_TestParam->stNoiseSlotConfig.Slot_TMR = 985;
	stPPG_TestParam->stNoiseSlotConfig.Slot_Mode = SLOT_ONECH_MODE; //Single channel mode is required when using automatic dimming
	
	//BaseNoise test slot config
	memset(&stPPG_TestParam->stBaseNoiseSlotConfig, 0, sizeof(stGh3x2xMp_SlotConfig));
	stPPG_TestParam->stBaseNoiseSlotConfig.Slot_Mutiple = 9;
	stPPG_TestParam->stBaseNoiseSlotConfig.TIA_RF = 4;
	stPPG_TestParam->stBaseNoiseSlotConfig.TIA_CF = 0;
	stPPG_TestParam->stBaseNoiseSlotConfig.ADC_N = 6;
	stPPG_TestParam->stBaseNoiseSlotConfig.ADC_IntTimes = stPPG_TestParam->stBaseNoiseSlotConfig.ADC_N;
	stPPG_TestParam->stBaseNoiseSlotConfig.BG_Level = 0;
	stPPG_TestParam->stBaseNoiseSlotConfig.LED_ACG = 0;
	stPPG_TestParam->stBaseNoiseSlotConfig.LED_Drv = 0;
	stPPG_TestParam->stBaseNoiseSlotConfig.Slot_TMR = 985;
	stPPG_TestParam->stBaseNoiseSlotConfig.Slot_Mode = SLOT_MULTICH_MODE;
	
	//LeakLight test slot config
	memset(&stPPG_TestParam->stLeakLightSlotConfig, 0, sizeof(stGh3x2xMp_SlotConfig));
	stPPG_TestParam->stLeakLightSlotConfig.Slot_Mutiple = 9;
	stPPG_TestParam->stLeakLightSlotConfig.TIA_RF = 4;
	stPPG_TestParam->stLeakLightSlotConfig.TIA_CF = 0;
	stPPG_TestParam->stLeakLightSlotConfig.ADC_N = 5;
	stPPG_TestParam->stLeakLightSlotConfig.ADC_IntTimes = stPPG_TestParam->stLeakLightSlotConfig.ADC_N;
	stPPG_TestParam->stLeakLightSlotConfig.BG_Level = 2;
	stPPG_TestParam->stLeakLightSlotConfig.LED_ACG = 0;
	stPPG_TestParam->stLeakLightSlotConfig.LED_Drv = 51;
	stPPG_TestParam->stLeakLightSlotConfig.Slot_TMR = 985;
	stPPG_TestParam->stLeakLightSlotConfig.Slot_Mode = SLOT_MULTICH_MODE;
	
	//get rawdata num
	stPPG_TestParam->PPG_CTRRawdata_Num = 100; //not more than MAX_PPG_RAWDATA_SAMPLE_NUM
	stPPG_TestParam->PPG_NoiseRawdata_Num = 100; //not more than MAX_PPG_RAWDATA_SAMPLE_NUM
	stPPG_TestParam->PPG_BaseNoiseRawdata_Num = 100; //not more than MAX_PPG_RAWDATA_SAMPLE_NUM
	stPPG_TestParam->PPG_LeakLightRawdata_Num = 20; //not more than MAX_PPG_RAWDATA_SAMPLE_NUM
	stPPG_TestParam->PPG_CTRRawdata_SkipNum = 0;
	stPPG_TestParam->PPG_NoiseRawdata_SkipNum = 100;
	stPPG_TestParam->PPG_BaseNoiseRawdata_SkipNum = 0;
	stPPG_TestParam->PPG_LeakLightRawdata_SkipNum = 0;
	
	//LEDPDConfig
	memset(stPPG_TestParam->stLEDPDConfig.PD_Channel, 0, sizeof(stPPG_TestParam->stLEDPDConfig.PD_Channel));
    memset(stPPG_TestParam->stLEDPDConfig.PD_Num, 0, sizeof(stPPG_TestParam->stLEDPDConfig.PD_Channel));
	stPPG_TestParam->stLEDPDConfig.PD_Channel_AllSlot = 0;
	stPPG_TestParam->stLEDPDConfig.PD_Num_AllSlot = 0;
	stPPG_TestParam->stLEDPDConfig.LED_PD_Num = 0;
	stPPG_TestParam->stLEDPDConfig.LED_Channel = 0;
	stPPG_TestParam->stLEDPDConfig.LED_Num = 0;
	for (i = 0; i < MAX_LED_NUM; i++)
    {
        for (j = 0; j < MAX_ADC_NUM; j++)
        {
            if (PPG_LEDPD_EN[i][j])
            {
                stPPG_TestParam->stLEDPDConfig.PD_Channel[i] |= 1 << j;
                stPPG_TestParam->stLEDPDConfig.PD_Num[i]++;
                stPPG_TestParam->stLEDPDConfig.LED_Channel |= 1 << i;
                stPPG_TestParam->stLEDPDConfig.LED_PD_Num++;
            }
        }
        stPPG_TestParam->stLEDPDConfig.PD_Channel_AllSlot |= stPPG_TestParam->stLEDPDConfig.PD_Channel[i];
    }
    for (i = 0; i < MAX_LED_NUM; i++)
    {
        if (stPPG_TestParam->stLEDPDConfig.PD_Channel_AllSlot & (1 << i))
			stPPG_TestParam->stLEDPDConfig.PD_Num_AllSlot++;
        if (stPPG_TestParam->stLEDPDConfig.LED_Channel & (1 << i))
			stPPG_TestParam->stLEDPDConfig.LED_Num++;
    }

}

/**
 * @fn     void Gh3x2xDemoMp_SetTestParam_ECG(stGh3x2xMp_ECGTestParam *stECG_TestParam)
 *
 * @brief  GH3x2x MP set ECG test items param
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *stECG_TestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam_ECG(stGh3x2xMp_ECGTestParam *stECG_TestParam)
{
	GH3X2X_MP_LOG_L2("Gh3x2x mp start set ecg test param! \r\n");
	
	//ecg signal num
	stECG_TestParam->HPF_ECGSignalNum = 2;
	stECG_TestParam->SIGNAL_ECGSignalNum = 3;
	stECG_TestParam->CMRR_ECGSignalNum = 1;
	stECG_TestParam->LIN_ECGSignalNum = 2;
	stECG_TestParam->DCRange_ECGSignalNum = 3;
	
	//LeadOnOff param set
	stECG_TestParam->stECGSignal_LeadOnOff.Rawdata_Num = 1000;
	stECG_TestParam->stECGSignal_LeadOnOff.Rawdata_SkipNum = 100;
	
	//Noise param set
	stECG_TestParam->stECGSignal_Noise.Rawdata_Num = 1000; //need >= 600
	stECG_TestParam->stECGSignal_Noise.Rawdata_SkipNum = 500;
	
	//HPF0 signal param set
	stECG_TestParam->stECGSignal_HPF[0].Rawdata_Num = 200; //num need >= 200
	stECG_TestParam->stECGSignal_HPF[0].Rawdata_SkipNum = 0;
	stECG_TestParam->stECGSignal_HPF[0].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_HPF[0].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_HPF[0].ECG_SignalType = NONE_MODE;
	stECG_TestParam->stECGSignal_HPF[0].ECG_Freq = 0;
	stECG_TestParam->stECGSignal_HPF[0].ECG_Vol = 0;
	stECG_TestParam->stECGSignal_HPF[0].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_HPF[0].ECG_DCOffset = 810;
	//HPF1 signal param set
	stECG_TestParam->stECGSignal_HPF[1].Rawdata_Num = 200; //num need >= 200
	stECG_TestParam->stECGSignal_HPF[1].Rawdata_SkipNum = 0;
	stECG_TestParam->stECGSignal_HPF[1].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_HPF[1].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_HPF[1].ECG_SignalType = NONE_MODE;
	stECG_TestParam->stECGSignal_HPF[1].ECG_Freq = 0;
	stECG_TestParam->stECGSignal_HPF[1].ECG_Vol = 0;
	stECG_TestParam->stECGSignal_HPF[1].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_HPF[1].ECG_DCOffset = 860;
	
	//SIGNAL TEST signal0 param set
	stECG_TestParam->stECGSignal_SIGNAL[0].Rawdata_Num = 2048;
	stECG_TestParam->stECGSignal_SIGNAL[0].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_SIGNAL[0].Sampling_Step = 4; //must > 0
	stECG_TestParam->stECGSignal_SIGNAL[0].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_SIGNAL[0].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_SIGNAL[0].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_SIGNAL[0].ECG_Freq = 5;
	stECG_TestParam->stECGSignal_SIGNAL[0].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_SIGNAL[0].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_SIGNAL[0].ECG_DCOffset = 815;
	//SIGNAL TEST signal1 param set
	stECG_TestParam->stECGSignal_SIGNAL[1].Rawdata_Num = 14336;
	stECG_TestParam->stECGSignal_SIGNAL[1].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_SIGNAL[1].Sampling_Step = 4; //must > 0
	stECG_TestParam->stECGSignal_SIGNAL[1].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_SIGNAL[1].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_SIGNAL[1].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_SIGNAL[1].ECG_Freq = 0.67;
	stECG_TestParam->stECGSignal_SIGNAL[1].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_SIGNAL[1].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_SIGNAL[1].ECG_DCOffset = 815;
	//SIGNAL TEST signal2 param set
	stECG_TestParam->stECGSignal_SIGNAL[2].Rawdata_Num = 512;
	stECG_TestParam->stECGSignal_SIGNAL[2].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_SIGNAL[2].Sampling_Step = 1; //must > 0
	stECG_TestParam->stECGSignal_SIGNAL[2].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_SIGNAL[2].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_SIGNAL[2].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_SIGNAL[2].ECG_Freq = 40;
	stECG_TestParam->stECGSignal_SIGNAL[2].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_SIGNAL[2].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_SIGNAL[2].ECG_DCOffset = 815;
	
	//RLD param set
	stECG_TestParam->stECGSignal_RLD.Rawdata_Num = 100;
	stECG_TestParam->stECGSignal_RLD.Rawdata_SkipNum = 0;
	
	//CMRR0 signal param set
	stECG_TestParam->stECGSignal_CMRR[0].Rawdata_Num = 712; //need >= 200
	stECG_TestParam->stECGSignal_CMRR[0].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_CMRR[0].bECG_NegativeSignal = 1;
	stECG_TestParam->stECGSignal_CMRR[0].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_CMRR[0].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_CMRR[0].ECG_Freq = 50;
	stECG_TestParam->stECGSignal_CMRR[0].ECG_Vol = 1000;
	stECG_TestParam->stECGSignal_CMRR[0].ECG_ACOffset = 410;
	stECG_TestParam->stECGSignal_CMRR[0].ECG_DCOffset = 10;
	
	//LIN0 signal param set
	stECG_TestParam->stECGSignal_LIN[0].Rawdata_Num = 512;
	stECG_TestParam->stECGSignal_LIN[0].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_LIN[0].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_LIN[0].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_LIN[0].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_LIN[0].ECG_Freq = 5;
	stECG_TestParam->stECGSignal_LIN[0].ECG_Vol = 285;
	stECG_TestParam->stECGSignal_LIN[0].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_LIN[0].ECG_DCOffset = 835;
	//LIN1 signal param set
	stECG_TestParam->stECGSignal_LIN[1].Rawdata_Num = 512;
	stECG_TestParam->stECGSignal_LIN[1].Rawdata_SkipNum = 300;
	stECG_TestParam->stECGSignal_LIN[1].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_LIN[1].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_LIN[1].ECG_SignalType = AC_MODE;
	stECG_TestParam->stECGSignal_LIN[1].ECG_Freq = 5;
	stECG_TestParam->stECGSignal_LIN[1].ECG_Vol = 285;
	stECG_TestParam->stECGSignal_LIN[1].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_LIN[1].ECG_DCOffset = 835;
	
	//DCRange0 signal param set
	stECG_TestParam->stECGSignal_DCRange[0].Rawdata_Num = 500;
	stECG_TestParam->stECGSignal_DCRange[0].Rawdata_SkipNum = 500;
	stECG_TestParam->stECGSignal_DCRange[0].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_DCRange[0].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_DCRange[0].ECG_SignalType = ANG_MODE;
	stECG_TestParam->stECGSignal_DCRange[0].ECG_Freq = 10.4;
	stECG_TestParam->stECGSignal_DCRange[0].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_DCRange[0].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_DCRange[0].ECG_DCOffset = 815;
	//DCRange1 signal param set
	stECG_TestParam->stECGSignal_DCRange[1].Rawdata_Num = 500;
	stECG_TestParam->stECGSignal_DCRange[1].Rawdata_SkipNum = 500;
	stECG_TestParam->stECGSignal_DCRange[1].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_DCRange[1].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_DCRange[1].ECG_SignalType = ANG_MODE;
	stECG_TestParam->stECGSignal_DCRange[1].ECG_Freq = 10.4;
	stECG_TestParam->stECGSignal_DCRange[1].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_DCRange[1].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_DCRange[1].ECG_DCOffset = 315;
	//DCRange2 signal param set
	stECG_TestParam->stECGSignal_DCRange[2].Rawdata_Num = 500;
	stECG_TestParam->stECGSignal_DCRange[2].Rawdata_SkipNum = 500;
	stECG_TestParam->stECGSignal_DCRange[2].bECG_NegativeSignal = 0;
	stECG_TestParam->stECGSignal_DCRange[2].bECG_HighImpedance = 0;
	stECG_TestParam->stECGSignal_DCRange[2].ECG_SignalType = ANG_MODE;
	stECG_TestParam->stECGSignal_DCRange[2].ECG_Freq = 10.4;
	stECG_TestParam->stECGSignal_DCRange[2].ECG_Vol = 57;
	stECG_TestParam->stECGSignal_DCRange[2].ECG_ACOffset = 57;
	stECG_TestParam->stECGSignal_DCRange[2].ECG_DCOffset = 1315;
	

}

/**
 * @fn     GU8 Gh3x2xDemoMp_SetTestParam(stGh3x2xMp_TestParam *stTestParam)
 *
 * @brief  GH3x2x MP set all test items param.
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_TestParam *stTestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam(stGh3x2xMp_TestParam *stTestParam)
{
	
	Gh3x2xDemoMp_SetTestParam_PPG(&stTestParam->stPPG_TestParam);
	Gh3x2xDemoMp_SetTestParam_ECG(&stTestParam->stECG_TestParam);
	
}

/**
 * @fn     void Gh3x2xDemoMp_PrintTestResult(stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult)
 *
 * @brief  GH3x2x mp print all test result
 *
 * @attention   It must be called after the test ends
 *
 * @param[in]   stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult
 * @param[out]  None
 *
 * @return  none
 */
void Gh3x2xDemoMp_PrintTestResult(stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult)
{
	GU32 i = 0;
	//GCHAR Gh3x2xDemoMp_TestResultLog[GH3X2X_MP_LOG_MAX_LEN] = {0};
	GH3X2X_MP_LOG("\r\n *************************Start print Gh3x2x mp test all result info!*********************************** \r\n");
	
	//print efuse test result
	if(stGh3x2xMpTestItemsSatus.EFUSE_TEST)
	{
		
		GH3X2X_MP_LOG("\r\n *************************Start print chip uid*********************************** \r\n");
		GH3X2X_MP_LOG("\r\n Gh3x2x mp efuse test result, chip uid is: ");
		for(i = 0; i < CHIP_UID_LENGTH; i++)
		{
			GH3X2X_MP_LOG("%02X \r\n", stTestResult->stEfuse_TestResult.uchChipUid[i]);
		}
		GH3X2X_MP_LOG("\r\n");
		GH3X2X_MP_LOG("\r\n *************************End print chip uid*********************************** \r\n");
	}
	
	//print ppg tset result
	if(stGh3x2xMpTestItemsSatus.PPG_TEST)
	{
		GH3X2X_MP_LOG("\r\n *************************Start print ppg test result*********************************** \r\n");
		if(stGh3x2xMpPPGItemsSatus.PPG_LPCTR_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("LPCTR[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_LPCTR[i]);
			}
		}
		
		if(stGh3x2xMpPPGItemsSatus.PPG_NOISE_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("Noise[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_Noise[i]);
			}
		}
		
		if(stGh3x2xMpPPGItemsSatus.PPG_BASENOISE_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("BaseNoise[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_BaseNoise[i]);
			}
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("DC_Offset[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_DCOffset[i]);
			}
		}
		
		if(stGh3x2xMpPPGItemsSatus.PPG_LEAKLIGHT_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("LeakLight_CTR[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_LLPCTR[i]);
			}
			GH3X2X_MP_LOG("\r\n");
			for(i = 0; i < MAX_LED_NUM*MAX_ADC_NUM; i++)
			{
				GH3X2X_MP_LOG("LeakLight_Ratio[%d]: %.5f, \r\n", i, stTestResult->stPPG_TestResult.LED_LeakRatio[i]);
			}
		}
		GH3X2X_MP_LOG("\r\n *************************End print ppg test result*********************************** \r\n");
	}
	
	
	
	//print ppg tset result
	if(stGh3x2xMpTestItemsSatus.ECG_TEST)
	{
		GH3X2X_MP_LOG("\r\n *************************Start print ecg test result*********************************** \r\n");
		if(stGh3x2xMpECGItemsSatus.ECG_NOISE_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			GH3X2X_MP_LOG("ECG_Noise: %.5f, \r\n", stTestResult->stECG_TestResult.ECG_Noise);
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_HPF_TEST)
		{
			if(stTestParam->stECG_TestParam.HPF_ECGSignalNum < ECG_MAX_SIGNAL_NUM)
			{
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.HPF_ECGSignalNum / 2; i++) //2 signals in a group, ratio is Calculate the latter one 
				{
					GH3X2X_MP_LOG("ECG_HPFRatio[%d]: %.5f, \r\n", i, stTestResult->stECG_TestResult.ECG_HPFRatio[i]);
				}		
			}
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_SIGNAL_TEST)
		{
			if(stTestParam->stECG_TestParam.SIGNAL_ECGSignalNum < ECG_MAX_SIGNAL_NUM)
			{
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.SIGNAL_ECGSignalNum; i++)
				{
					GH3X2X_MP_LOG("ECG_SIGNAL_Amp[%d]: %.5fmV, \r\n", i, stTestResult->stECG_TestResult.ECG_Amp[i]);
				}
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.SIGNAL_ECGSignalNum; i++) 
				{
					GH3X2X_MP_LOG("ECG_SIGNAL_Freq[%d]: %.5fHz, \r\n", i, stTestResult->stECG_TestResult.ECG_Freq[i]);
				}
			}		
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_RLD_TEST)
		{
			GH3X2X_MP_LOG("\r\n");
			GH3X2X_MP_LOG("ECG_RLD_Amp: %.5fmV, \r\n", stTestResult->stECG_TestResult.ECG_RLD);
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_CMRR_TEST)
		{
			if(stTestParam->stECG_TestParam.CMRR_ECGSignalNum < ECG_MAX_SIGNAL_NUM)
			{
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.CMRR_ECGSignalNum; i++) 
				{
					GH3X2X_MP_LOG("ECG_CMRR[%d]: %.5f, \r\n", i, stTestResult->stECG_TestResult.ECG_CMRR[i]);
				}
			}		
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_LIN_TEST)
		{
			if(stTestParam->stECG_TestParam.LIN_ECGSignalNum < ECG_MAX_SIGNAL_NUM)
			{
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.LIN_ECGSignalNum / 2; i++) //2 signals in a group to cal one ratio
				{
					GH3X2X_MP_LOG("ECG_LIN_Ratio[%d]: %.5f, \r\n", i, stTestResult->stECG_TestResult.ECG_LIN_Ratio[i]);
				}
			}
		}
		
		if(stGh3x2xMpECGItemsSatus.ECG_DCRANGE_TEST)
		{
			if(stTestParam->stECG_TestParam.DCRange_ECGSignalNum < ECG_MAX_SIGNAL_NUM)
			{
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.DCRange_ECGSignalNum; i++) //3 signals in a group to cal two ratio
				{
					GH3X2X_MP_LOG("ECG_DCRange_Ratio[%d]: %.5f, \r\n", i, stTestResult->stECG_TestResult.ECG_DCRange_Ratio[i]);
				}
				GH3X2X_MP_LOG("\r\n");
				for(i = 0; i < stTestParam->stECG_TestParam.DCRange_ECGSignalNum; i++) //3 signals in a group to cal two ratio
				{
					GH3X2X_MP_LOG("ECG_DCRange_Amp[%d]: %.5f, \r\n", i, stTestResult->stECG_TestResult.ECG_DCRange_Amp[i]);
				}
			}
		}
		GH3X2X_MP_LOG("\r\n *************************End print ecg test result*********************************** \r\n");
	
	}
	
	
	
	GH3X2X_MP_LOG("\r\n *************************End print Gh3x2x mp test all result info!*********************************** \r\n");
}

/**
 * @fn     GU8 Gh3x2xDemoMp_CommunicationTest(GU8 *puchPara, GU8 *puchTestResult)
 *
 * @brief  GH3x2x MP Communication Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_CommunicationTest(GU8 *puchPara, GU8 *puchTestResult)
{
    GU8 test_code = 0;
	GU8 i = 0;
	GU8 retry_time = 3;
    GH3X2X_MP_LOG_L2("Gh3x2x mp Communication test start! \r\n");
	
	for(i = 0; i < retry_time; i++)
	{
		//tset before operation, chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		//communication test
		for(GU32 i = 0; i < sizeof(stGh3x2xMpRegConfigDefault) / sizeof(stGh3x2xMpRegConfigDefault[0]); i++)
		{
			if(stGh3x2xMpRegConfigDefault[i].usRegAddr && (stGh3x2xMpRegConfigDefault[i].usRegAddr < 0x780))
			{
				gh3x2x_demo_mp_write_reg(stGh3x2xMpRegConfigDefault[i].usRegAddr, stGh3x2xMpRegConfigDefault[i].usRegValue);
				gh3x2x_mp_delay_ms(2);
	
				if(stGh3x2xMpRegConfigDefault[i].usRegValue != gh3x2x_demo_mp_read_reg(stGh3x2xMpRegConfigDefault[i].usRegAddr))
				{
					test_code = MT_ERROR_COMMUNICATION_TEST_ERROR;
					GH3X2X_MP_LOG_L1("Gh3x2x mp Communication test fail, error code is: %d!\r\n", test_code);
					break;
				}
			}
		}
		if(!test_code)
		{
			GH3X2X_MP_LOG_L2("Gh3x2x mp Communication test is pass! \r\n");
			break;
		}
			
	}
	if(i == retry_time)
	{
		test_code = MT_ERROR_COMMUNICATION_TEST_ERROR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp Communication test fail, error code is: %d!\r\n", test_code);
	}

	GH3X2X_MP_LOG_L2("Gh3x2x mp Communication test end! \r\n");
	
	return test_code;
}

/**
 * @fn     GU8 Gh3x2xDemoMp_CheckChipIDTest(GU8 *puchPara, GU8 *puchTestResult)
 *
 * @brief  GH3x2x MP Check Chip ID Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_CheckChipIDTest(GU8 *puchPara, GU8 *puchTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 retry_time = 3;
	GH3X2X_MP_LOG_L2("Gh3x2x mp Check ChipID test start! \r\n");
	
	for(i = 0; i < retry_time; i++)
	{
		//need use drv interface to check, at present Only need to test this item when the driver is AFE
	#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			//No need to do at the moment
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		if(GH3X2X_CHIP_ID == GH3X2X_AFE_GetChipId())
		{
			GH3X2X_MP_LOG_L2("Gh3x2x mp Check ChipID test is pass! \r\n");
			break;
		}
		else
			continue;
		#endif
	#endif
		
	}
	if(i == retry_time)
	{
		test_code = MT_ERROR_CHECK_CHIPID_ERROR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp Check ChipID test fail--Chip ID error, error code is: %d!\r\n", test_code);
	}
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp Check ChipID test end! \r\n");
	
	return test_code;
	
}

/**
 * @fn     GU8 Gh3x2xDemoMp_RstIntTest(GU8 *puchPara, GU8 *puchTestResult)
 *
 * @brief  GH3x2x MP Communication Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_RstIntTest(GU8 *puchPara, GU8 *puchTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 retry_time = 3;
	GU16 time_out = 200;
	GH3X2X_MP_LOG_L2("Gh3x2x mp RST&INT test start! \r\n");
	
	for(i = 0; i < retry_time; i++)
	{
		//tset before operation, chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		//rst gh3x2x,set low and set high later
		gh3x2x_demo_mp_reset_operation();
	
		//wait int
		if(gh3x2x_demo_mp_wait_for_int(0x4000, time_out))
		{
			GH3X2X_MP_LOG_L2("Gh3x2x mp RST&INT test is pass! \r\n");
			break;
		}
	}
	if(i == retry_time)
	{
		test_code = MT_ERROR_COMMUNICATION_TEST_ERROR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp RST&INT test fail, error code is: %d!\r\n", test_code);
	}
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp RST&INT test end! \r\n");
	
	return test_code;
	
}

/**
 * @fn     GU8 Gh3x2xDemoMp_EfuseTest(GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP efuse Test, check FT/CP and crc, and get chipUid
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_EfuseTest(GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 retry_time = 3;
	GU8 i = 0;
	GU8 j = 0;
	GU16 usTimeOut_ms = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp eFuse test start! \r\n");
	
	memset(pstTestResult->uchChipUid, 0, 12);
	
	//read efuse, check FT/CP and get chip uid
	GU8 eFuse_data[GH3X2X_DEMO_MP_EFUSE_MAX_LEN] = {0};
	GU16 eFuse_block[3] = {0x0030, 0x0034, 0x0038};
	GU8 eFuseReadPass = 1;
	for(i = 0; i < retry_time; i++)
	{
		eFuseReadPass = 1;
		//tset before operation, chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		gh3x2x_demo_mp_write_reg(0x700, 0x0020);
		GU8 tmp_data[24] = {0};
		for(j = 0; j < 3; j++)
		{
			gh3x2x_demo_mp_write_reg(0x700, eFuse_block[j]);
			usTimeOut_ms = 200;
			while(usTimeOut_ms--)
			{
				if((gh3x2x_demo_mp_read_reg(0x718) & 0x0001))
					break;
				gh3x2x_mp_delay_ms(1);
			}
			if(0 == usTimeOut_ms)
			{
				eFuseReadPass = 0;
				break;
			}
			memset(tmp_data, 0, 24);
			gh3x2x_demo_mp_read_regs(0x710, tmp_data, 8);
			for(GU16 k = 0; k < 4; k++)
			{
				eFuse_data[8*j + 2*k] = tmp_data[2*k + 1];
				eFuse_data[8*j + 2*k + 1] = tmp_data[2*k];
			}
		}
		if(!eFuseReadPass)
			continue;
		
		//check FT/CP
		GU8 FT_Result = eFuse_data[23] & (0x0C >> 2);
		GU8 CP_Result = eFuse_data[23] & 0x03;
		if((0x01 == FT_Result) && (0x01 == CP_Result))
		{
			GU8 FT_CRC = eFuse_data[7];
			GU8 CP_CRC = eFuse_data[11];		
			GU8 FT_CRC_Check[24] = {0};
			GU8 CP_CRC_Check[24] = {0};
			GH3X2X_Memcpy(&FT_CRC_Check[1], eFuse_data, 7);
			GH3X2X_Memcpy(&CP_CRC_Check[1], &eFuse_data[8], 3);
			FT_CRC_Check[0] = gh3x2x_mp_crc_cal(&FT_CRC_Check[1], 7); //cal FT crc
			CP_CRC_Check[0] = gh3x2x_mp_crc_cal(&CP_CRC_Check[1], 3); //cal CP crc
			
			if((FT_CRC != FT_CRC_Check[0]) || (CP_CRC != CP_CRC_Check[0]))
			{
				GH3X2X_MP_LOG_L1("Gh3x2x mp efuse test fail, FT or CP check crc error\r\n ");
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp eFuse test is pass! \r\n");
				break;
			}
		}
		else
			GH3X2X_MP_LOG_L1("Gh3x2x mp efuse test fail, FT or CP check result error\r\n ");
	}
	if(i == retry_time)
	{
		test_code = MT_ERROR_EFUSE_ERROR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp efuse test fail, error code is: %d!\r\n", test_code);
	}
	
	//get chip uid
	for(GU8 j = 0; j < 12; j++)
	{
		if(GH3X2X_DEMO_MP_EFUSE_MAX_LEN > 12)
			pstTestResult->uchChipUid[j] = eFuse_data[j];
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp eFuse test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 Gh3x2xDemoMp_PPGTest(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP PPG Test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_PPGTest(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
{
	GU8 test_code = 0; //Save the first NG error code
	GU8 test_code_tmp = 0;
	GU8 retry_times = 3;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test start! \r\n");
	
	//init pstTestResult
	for(i = 0; i < (MAX_LED_NUM*MAX_ADC_NUM); i++)
	{
		pstTestResult->LED_BaseNoise[i] = 0.0;
		pstTestResult->LED_Noise[i] = 0.0;
		pstTestResult->LED_LPCTR[i] = 0.0;
		pstTestResult->LED_DCOffset[i] = 0.0;
		pstTestResult->LED_LLPCTR[i] = 0.0;
		pstTestResult->LED_LeakRatio[i] = 0.0;
	}
	memset(&pstTestResult->stCTRRawHandleResult, 0, sizeof(stGh3x2xMp_RawdataHandleResult));
	memset(&pstTestResult->stNoiseRawHandleResult, 0, sizeof(stGh3x2xMp_RawdataHandleResult));
	memset(&pstTestResult->stBaseNoiseRawHandleResult, 0, sizeof(stGh3x2xMp_RawdataHandleResult));
	memset(&pstTestResult->stLeakLightRawHandleResult, 0, sizeof(stGh3x2xMp_RawdataHandleResult));
	
	//power on led,
	gh3x2x_mp_led_power_on();
	
	do
	{
		//LP_CTR test
		if(stGh3x2xMpPPGItemsSatus.PPG_LPCTR_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_PPG_LPCTR_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG-CTR test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpPPGItemsSatus.PPG_LPCTR_TEST))
				break;
		}
		//Noise test
		if(stGh3x2xMpPPGItemsSatus.PPG_NOISE_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_PPG_Noise_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG-Noise test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpPPGItemsSatus.PPG_NOISE_TEST))
				break;
		}
		//Base_Noise test
		if(stGh3x2xMpPPGItemsSatus.PPG_BASENOISE_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_PPG_BaseNoise_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG-BaseNoise test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpPPGItemsSatus.PPG_BASENOISE_TEST))
				break;
		}
		//LeakLight test
		if(stGh3x2xMpPPGItemsSatus.PPG_LEAKLIGHT_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_PPG_LeakLight_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG-LeakLight test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpPPGItemsSatus.PPG_LEAKLIGHT_TEST))
				break;
		}
		
	}while(0);
	
	//power off led
	gh3x2x_mp_led_power_off();
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 Gh3x2xDemoMp_ECGTest(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP ECG Test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_ECGTest(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0; //Save the first NG error code
	GU8 test_code_tmp = 0;
	GU8 retry_times = 3;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test start! \r\n");
	
	//init pstTestResult
	pstTestResult->ECG_Noise = 0;
	pstTestResult->ECG_RLD = 0;
	for(i = 0; i < ECG_MAX_SIGNAL_NUM; i++)
	{
		pstTestResult->ECG_HPFRatio[i] = 0;
		pstTestResult->ECG_Amp[i] = 0;
		pstTestResult->ECG_CMRR[i] = 0;
		pstTestResult->ECG_DCRange_Amp[i] = 0;
		pstTestResult->ECG_DCRange_Ratio[i] = 0;
		pstTestResult->ECG_Freq[i] = 0;
		pstTestResult->ECG_LIN_Ratio[i] = 0;
	}
	
	//rst gh3x2x,set low and set high later
	gh3x2x_demo_mp_reset_operation();
	
	do
	{
		//Lead on/off test
		if(stGh3x2xMpECGItemsSatus.ECG_LEADONOFF_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_LeadOnOff_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-LeadOnOff test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_LEADONOFF_TEST))
				break;
		}
		//Noise test
		if(stGh3x2xMpECGItemsSatus.ECG_NOISE_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_Noise_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-Noise test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_NOISE_TEST))
				break;
		}
		//HPF test
		if(stGh3x2xMpECGItemsSatus.ECG_HPF_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_HPF_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-HPF test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_HPF_TEST))
				break;
		}
		//SIGNAL test
		if(stGh3x2xMpECGItemsSatus.ECG_SIGNAL_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_SIGNAL_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-SIGNAL test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_SIGNAL_TEST))
				break;
		}
		//RLD test
		if(stGh3x2xMpECGItemsSatus.ECG_RLD_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_RLD_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-RLD test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_RLD_TEST))
				break;
		}
		//CMRR test
		if(stGh3x2xMpECGItemsSatus.ECG_CMRR_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_CMRR_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-CMRR test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_CMRR_TEST))
				break;
		}
		//LIN test
		if(stGh3x2xMpECGItemsSatus.ECG_LIN_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_LIN_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-LIN test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_LIN_TEST))
				break;
		}
		//DC_Range test
		if(stGh3x2xMpECGItemsSatus.ECG_DCRANGE_TEST)
		{
			for(i = 0; i < retry_times; i++)
			{
				test_code_tmp = gh3x2x_demo_mp_ECG_DCRange_Test(pstTestPara, pstTestResult);
				if(!test_code_tmp)
					break;
				else
					GH3X2X_MP_LOG_L2("Gh3x2x mp ECG-DCRange test retry:%d !\r\n", i+1);
			}
			if(!test_code) //Save the first NG error code
				test_code = test_code_tmp;
			if((i == retry_times) && (NG_CONTINUE != stGh3x2xMpECGItemsSatus.ECG_DCRANGE_TEST))
				break;
		}

		
	}while(0);
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test end! \r\n");
	return test_code;
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
