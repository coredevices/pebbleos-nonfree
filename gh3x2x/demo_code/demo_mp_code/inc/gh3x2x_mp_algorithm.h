/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_algorithm.h
 *
 * @brief   header file of gh3x2x_mp_algorithm
 *
 * @author  Goodix Product Tools Development Team
 *
 */
 
#ifndef _GH3X2X_MP_ALGORITHM_H_
#define _GH3X2X_MP_ALGORITHM_H_

#include "gh3x2x_mp_config.h"
#include "gh3x2x_mp_define.h"

//IIR struct
typedef struct
{
	GS32 stages;
	const GD64(*sos)[6];
	const GD64 *gain;
	double x[8][3];
	double y[8][2];
}IIR_State_Def;


/**
 * @fn     GU8 gh3x2x_mp_crc_cal(GU8 *puchData, GU32 unLen)
 *
 * @brief  GH3x2x mp start test
 *
 * @attention   None
 *
 * @param[in]   none
 * @param[out]  None
 *
 * @return  CRC
 */
GU8 gh3x2x_mp_crc_cal(GU8 *puchData, GU32 unLen);

/**
 * @fn     GD64 gh3x2x_mp_high_pass_filter(GD64 fData)
 *
 * @brief  GH3x2x mp high-pass filter
 *
 * @attention   None
 *
 * @param[in]   GD64 fData
 * @param[out]  None
 *
 * @return  Filtered data
 */
GD64 gh3x2x_mp_high_pass_filter(GD64 fData);

/**
 * @fn     GU8 gh3x2x_mp_ecg_lead_off_dectection(GS32 nRawdataArr[], GU16 usRawdataLen, GU8 uchECGChannel_N, GU16 *usReSampleData_Len)
 *
 * @brief  dectect ecg lead off algorithm
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usRawdataLen, GU8 uchECGChannel_N, GU16 usReSampleData_Len
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_mp_ecg_lead_off_dectection(GS32 nRawdataArr[], GU16 usRawdataLen, GU8 uchECGChannel_N, GU16 *usReSampleData_Len);

/**
 * @fn     GD64 gh3x2x_mp_signal_IIR_filter(IIR_State_Def *stIIR_State, GD64 dData)
 *
 * @brief  GH3x2x mp signal IIR filter
 *
 * @attention   None
 *
 * @param[in]   IIR_State_Def *stIIR_State, GD64 dData
 * @param[out]  None
 *
 * @return  Filtered data
 */
GD64 gh3x2x_mp_signal_IIR_filter(IIR_State_Def *stIIR_State, GD64 dData);

/**
 * @fn     void gh3x2x_mp_signal_filter(GS32 nFilterArr[], GU16 usLength, GU8 uchHpType, GU8 uchLpType, GU8 uchFsType)
 *
 * @brief  GH3x2x mp signal hp and lp filter for ecg
 *
 * @attention   None
 *
 * @param[in]   GS32 nFilterArr[], GU16 usLength, GU8 uchHpType, GU8 uchLpType, GU8 uchFsType
 				uchLpType:Low pass filter cut-off frequency
				uchHpType:High pass filter cut-off frequency
				uchFsType:Sampling rate of rawdata
 * @param[out]  GS32 nFilterArr[]
 *
 * @return  None
 */
void gh3x2x_mp_signal_filter(GS32 nFilterArr[], GU16 usLength, GU8 uchHpType, GU8 uchLpType, GU8 uchFsType);

/**
 * @fn     gh3x2x_mp_signal_fft(GS32 nFFT_InArr[], GS32 nFFT_OutArr[], GU32 unLength, GU32 step)
 *
 * @brief  fft
 *
 * @attention   None
 *
 * @param[in]   GD64 dFFT_InArr[], GS32 nFFT_OutArr[], GU32 unLength, GU32 step
 * @param[out]  GD64 dFFTArr[]
 *
 * @return  ret
 */
void gh3x2x_mp_signal_fft(GF32 nFFT_InArr[], GF32 nFFT_OutArr[], GU32 unLength, GU32 unStep);

/**
 * @fn     void gh3x2x_mp_signal_quick_fft(GF32 fFFTInArr_real[], GF32 fFFTInArr_image[], GU32 unLength, GU32 unPower_2, \
										GF32 fFFTOutArr_real[], GF32 fFFTOutArr_image[], GU8 uchFFTMode, GU32 uchCalModulus_Angle)
 *
 * @brief  more quick do fft process
 *
 * @attention   None
 *
 * @param[in]   GF32 fFFTInArr_real[], GF32 fFFTInArr_image[], GU32 unLength, GU32 unPower_2, GU8 uchFFTMode, GU32 uchCalModulus_Angle
				uchFFTMode: 0, Fourier transform; 1, inverse Fourier transform
				uchCalModulus_Angle: 0, do not calculate Fourier transform or inverse transform mode and amplitude Angle;1, calculate the modulus and amplitude Angle
				unLength: the number of points entered, which is an even number, is generally 32,64,128...1024, etc.
				unPower_2: satisfy n=2^unPower_2(unPower_2>0)
				fFFTInArr_real: 0,store the real part of the sampled data at N points; 1,store N real parts of the Fourier transform
				fFFTInArr_image: 0,store the imaginary part of the sampled data at N points; 1,store N imaginary parts of the Fourier transform

 * @param[out]  GF32 fFFTInArr_real[], GF32 fFFTInArr_image[], 
				fFFTOutArr_real: uchFFTMode=0, return the real part of the Fourier transform
								uchFFTMode=1, return the real part of the inverse Fourier transform
				fFFTOutArr_image:uchFFTMode=0, return the imaginary part of the Fourier transform
								uchFFTMode=1, return the imaginary part of the inverse Fourier transform
				fFFTInArr_real: uchCalModulus_Angle = 1, uchFFTMode = 0, return the modulus of Fourier transform
								uchCalModulus_Angle = 1, uchFFTMode = 1, return the inverse Fourier transform modulus
				fFFTInArr_image:uchCalModulus_Angle = 1, uchFFTMode = 0, return the amplitude Angle of the Fourier transform
								uchCalModulus_Angle = 1, uchFFTMode = 1, return the amplitude Angle of the inverse Fourier transform
 *
 * @return  none
 */
void gh3x2x_mp_signal_quick_fft(GF32 fFFTInArr_real[], GF32 fFFTInArr_image[], GU32 unLength, GU32 unPower_2, \
										GF32 fFFTOutArr_real[], GF32 fFFTOutArr_image[], GU8 uchFFTMode, GU32 uchCalModulus_Angle);

/**
 * @fn     GU8 gh3x2x_mp_signal_fft_process(GS32 nFFTArr[] GU32 unLength)
 *
 * @brief  Perform FFT transformation on the data and then calculate the power spectrum
 *
 * @attention   None
 *
 * @param[in]   GS32 nFFTArr[], GU32 unLength
 * @param[out]  GS32 nFFTArr[]
 *
 * @return  ret
 */
GU8 gh3x2x_mp_signal_fft_process(GF32 nFFTArr[], GU32 unLength);

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_fft_cal_amp_freq(GF32 fRawdataArr[], GU16 usLength,  GU32 unFFTLength, GU32 unSampleRate, GF32 *fAmp, GF32 *fFreq, \
														GF32 fRefFreq, GU8 uchWintype)
 *
 * @brief  cal amp and freq after fft
 *
 * @attention   None
 *
 * @param[in]   GF32 fRawdataArr[], GU16 usLength, GU32 unFFTLength, GU32 unSampleRate, GU8 uchWintype, GF32 fRefFreq
				unSampleRate: Sampling frequency
				winType：Window type, 0: rectangular window; 1: flattop window, used to accurately estimate the amplitude; 2: hanning window
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fAmp, GF32 *fFreq
				fAmp: peak-to-peak value
				fFreq:freq, unit Hz
 *				
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_fft_cal_amp_freq(GF32 fRawdataArr[], GU16 usLength,  GU32 unFFTLength, GU32 unSampleRate, GF32 *fAmp, GF32 *fFreq, \
														GF32 fRefFreq, GU8 uchWintype);

#endif /* _GH3X2X_MP_ALGORITHM_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
