/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_mp_process.h
 *
 * @brief   header file of gh3x2x_demo_mp_process
 *
 * @author  Goodix Product Tools Development Team
 *
 */
 
#ifndef _GH3X2X_DEMO_MP_PROCESS_H_
#define _GH3X2X_DEMO_MP_PROCESS_H_

#include "gh3x2x_mp_config.h"
#include "gh3x2x_mp_reg_config.h"
#include "gh3x2x_mp_define.h"


/**
 * @fn     gh3x2x_demo_mp_write_reg(GU16 usReg_Addr, GU16 usReg_Value)
 *
 * @brief  GH3x2x mp write reg
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr, GU16 usReg_Value
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_write_reg(GU16 usReg_Addr, GU16 usReg_Value);

/**
 * @fn     GU16 gh3x2x_demo_mp_read_reg(GU16 usReg_Addr)
 *
 * @brief  GH3x2x mp read reg
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr
 * @param[out]  None
 *
 * @return  read reg value
 */
GU16 gh3x2x_demo_mp_read_reg(GU16 usReg_Addr);

/**
 * @fn    GU8 gh3x2x_demo_mp_write_regs(GU16 usReg_Addr, GU8 uchRegs_Write[], GU16 usLength)
 *
 * @brief  GH3x2x mp write regs
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr, GU8 uchRegs_Write[], GU16 usLength
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_write_regs(GU16 usReg_Addr, GU8 uchRegs_Write[], GU16 usLength);

/**
 * @fn    GU8 gh3x2x_demo_mp_read_regs(GU16 usReg_Addr, GU8 uchRegs_Read[], GU16 usLength)
 *
 * @brief  GH3x2x mp read regs
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr, GU16 usLength
 * @param[out]  GU8 uchRegs_Read[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_read_regs(GU16 usReg_Addr, GU8 uchRegs_Read[], GU16 usLength);

/**
 * @fn     GU8 gh3x2x_demo_mp_write_reg_mask(GU16 usRegAddr, GU16 usMask, GU16 usRegVal)
 *
 * @brief  GH3x2x mp write reg mask
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_write_reg_mask(GU16 usRegAddr, GU16 usMask, GU16 usRegVal);

/**
 * @fn     void gh3x2x_demo_mp_send_cmd(GU8 uchCmd)
 *
 * @brief  send cmd to gh3x2x chip
 *
 * @attention   None
 *
 * @param[in]   GU8 uchCmd
 * @param[out]  None
 *
 * @return  ret
 */
void gh3x2x_demo_mp_send_cmd(GU8 uchCmd);

/**
 * @fn     void gh3x2x_demo_mp_reset_operation(void)
 *
 * @brief  reset gh3x2x chip
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_demo_mp_reset_operation(void);

/**
 * @fn    GU8 gh3x2x_demo_mp_wait_for_int(GU16 usDutFlag, GU16 usTimeOut_ms)
 *
 * @brief  wait for int 
 *
 * @attention   None
 *
 * @param[in]   GU16 usDutFlag, GU16 usTimeOut
 * @param[out]  none
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_wait_for_int(GU16 usDutFlag, GU16 usTimeOut_ms);

/**
 * @fn     GU8 gh3x2x_demo_mp_set_chip_config(const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen)
 *
 * @brief  GH3x2x mp write chip reg config
 *
 * @attention   None
 *
 * @param[in]   const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_chip_config(const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen);

/**
 * @fn     GU8 gh3x2x_demo_mp_set_slot_config(stGh3x2xMp_SlotConfig stSlotConfig)
 *
 * @brief  GH3x2x mp set slot config
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_SlotConfig stSlotConfig
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_slot_config(stGh3x2xMp_SlotConfig stSlotConfig);

/**
 * @fn     GU8 gh3x2x_demo_mp_set_slot_enable(GU8 uchSlotConfigEnable)
 *
 * @brief  Calculate which slots need to be enabled according to SLOT_CFG_EN, 
 * @brief  enable the corresponding slot, and output the number of enabled slots
 *
 * @attention   None
 *
 * @param[in]   GU8 uchSlotConfigEnable
 * @param[out]  GU8 *uchSlotEnableNum
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_slot_enable(GU8 uchSlotConfigEnable, GU8 *uchSlotEnableNum);

/**
 * @fn     GU8 gh3x2x_demo_mp_set_LED_ACG_cfg(GU8 uchLED_N, GU8 uchLED_ACG_FS, stGh3x2xMp_PPGTestParam *pstTestPara)
 *
 * @brief  The automatic dimming test needs to adjust the LED driver gear to avoid the failure of the automatic dimming step by too large
 *
 * @attention   None
 *
 * @param[in]   GU8 uchSlotConfigEnable
 * @param[out]  GU8 *uchSlotEnableNum
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_LED_ACG_cfg(GU8 uchLED_N, GU8 uchLED_ACG_FS, stGh3x2xMp_PPGTestParam *pstTestPara);

/**
 * @fn     void gh3x2x_demo_mp_sampling_mode(GU8 uchMode)
 *
 * @brief  rawdata sampling start / stop
 *
 * @attention   None
 *
 * @param[in]   GU8 uchMode
 * @param[out] 
 *
 * @return  None
 */
void gh3x2x_demo_mp_sampling_mode(GU8 uchMode);

/**
 * @fn     GU8 gh3x2x_demo_mp_get_rawdata(GU16 uchRegAddr, GU8 uchRawdataArr[], GU32 usLength)
 *
 * @brief  get rawdata
 *
 * @attention   None
 *
 * @param[in]   GU16 uchRegAddr, GU32 usLength
 * @param[out]  GU8 uchRawdataArr[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_get_rawdata(GU16 uchRegAddr, GU8 uchRawdataArr[], GU32 usLength);

/**
 * @fn     GU8 gh3x2x_demo_mp_handle_rawdata(GU8 uchMode, GS32 nHandleRawdataArr[], GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, \
										GU8 uchRawdataArr[], GU32 unRawdataArrLength, GU8 uchSampleStep)
 *
 * @brief  handle rawdata
 *
 * @attention   None
 *
 * @param[in]  GU8 uchMode, GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, GU8 uchRawdataArr[], GU32 unRawdataArrLength, GU8 uchSampleStep
 * @param[out]  GS32 nHandleRawdataArr[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_handle_rawdata(GU8 uchMode, GS32 nHandleRawdataArr[], GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, \
										GU8 uchRawdataArr[], GU32 unRawdataArrLength, GU8 uchSampleStep);

/**
 * @fn     gh3x2x_demo_mp_ppg_get_rawdata(GU8 uchMode, GU32 uchRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM], GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num)
 *
 * @brief  ppg test get rawdata
 *
 * @attention   None
 *
 * @param[in]   GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num
 * @param[out]  GU32 unRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ppg_get_rawdata(GS32 unRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM], GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num);

/**
 * @fn     void gh3x2x_demo_mp_calc_noise(GS32 nRawdataArr[], GU16 usLength, GF32 *fAvergeRawdata, GF32 *fNoise)
 *
 * @brief  cal noise
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength
 * @param[out]  GF32 *fAvergeRawdata, GF32 *fNoise
 *
 * @return  none
 */
void gh3x2x_demo_mp_calc_noise(GS32 nRawdataArr[], GU16 usLength, GF32 *fAvergeRawdata, GF32 *fNoise);

/**
 * @fn     GF32 gh3x2x_demo_mp_calc_LPCTR(GF32 fAverageRawdata, GU32 unTIA, GU32 unILed)
 *
 * @brief  cal LP CTR
 *
 * @attention   None
 *
 * @param[in]   GF32 fAverageRawdata, GU32 unTIA, GU32 unILed
 * @param[out]  
 *
 * @return  CTR
 */
GF32 gh3x2x_demo_mp_calc_LPCTR(GF32 fAverageRawdata, GU32 unTIA, GU32 unILed);

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_signal_filter(GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType)
 *
 * @brief  ECG signal filter
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType
				uchLpType:Low pass filter cut-off frequency
				uchHpType:High pass filter cut-off frequency
				uchFsType:Sampling rate of rawdata,0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
 * @param[out]  GS32 nRawdataArr[]: filter data
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_signal_filter(GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType);

/**
 * @fn     GF32 gh3x2x_demo_mp_ecg_cal_HPF_Ratio(GS32 nRawdataArr[], GU16 usLength)
 *
 * @brief  cal HPF amplitude ratio
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength
 * @param[out]  
 *
 * @return  HPF_Ratio
 */
GF32 gh3x2x_demo_mp_ecg_cal_HPF_Ratio(GS32 nRawdataArr[], GU16 usLength);

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GF32 *fFreq, \
													GU8 uchLpType, GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep)
 *
 * @brief  Cal the amplitude and frequency of the signal
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep
				nRawdataArr:Data to be processed
				usLength:Data len
 				uchLpType:Low pass filter cut-off frequency
				uchHpType:High pass filter cut-off frequency
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				sCalStart:FFT start begin point
				sCalEnd:FFT start end point
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fAmp, GF32 *fFreq
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GF32 *fFreq, GU8 uchLpType, \
											GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep);
											
/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cmmr_calculate(GS32 nRawdataArr[], GU16 usLength, GF32 *fCmmr, GU8 uchFsType, GF32 fRefFreq)
 *
 * @brief  Cal the ecg-cmmr
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchFsType, GF32 fRefFreq
				nRawdataArr:Data to be processed
				usLength:Data len
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fCmmr
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cmmr_calculate(GS32 nRawdataArr[], GU16 usLength, GF32 *fCmmr, GU8 uchFsType, GF32 fRefFreq);

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cal_signal_WaveTimeDomin_Amp(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq)
 *
 * @brief  Cal ecg-DCRange WaveTimeDomin Amp
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq
				nRawdataArr:Data to be processed
				usLength:Data len
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				fRefFreq: Reference frequency
 * @param[out]  GF32 *fAmp
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cal_signal_WaveTimeDomin_Amp(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq);

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_LPCTR_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ---- LP CTR test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_LPCTR_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_Noise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ---- noise test
 *
 * @attention   PPG Noise test requires automatic dimming. 
 * @attention   It is not possible to test all leds in parallel using multi-channel mode, only serial test
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_Noise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_BaseNoise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ----base noise test
 *
 * @attention   base noise test has nothing to do with LEDs, so only one slot is needed to test the data of 4 PDs.
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_BaseNoise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_LeakLight_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ----leak light test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_LeakLight_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_LeadOnOff_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----Lead on/off test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_LeadOnOff_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_Noise_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----Noise test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_Noise_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_HPF_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----HPF test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_HPF_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_SIGNAL_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----SIGNAL test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_SIGNAL_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_RLD_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----RLD test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_RLD_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_CMRR_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----CMRR test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_CMRR_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_LIN_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----LIN test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_LIN_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_DCRange_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----DCRange test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_DCRange_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);

#endif /* _GH3X2X_DEMO_MP_PROCESS_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
