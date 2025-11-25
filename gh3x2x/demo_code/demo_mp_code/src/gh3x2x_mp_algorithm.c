/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_mp_algorithm.c
 * 
 * @brief   algorithms needed for gh3x2x mp test
 * 
 * @author  Goodix Product Tools Development Team
 * 
 */

#include "gh3x2x_mp_algorithm.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

//high-pass-filter config
#define HIGH_PASS_FILTER_COEF_SIZE				7
GU8 HPF_FirstFlag = 0;
GD64 HPF_CoefA[HIGH_PASS_FILTER_COEF_SIZE + 1] = {1.00000000000000, -5.80579309703828, 14.04773386830920, -18.13186088426856, 13.16715655966765, -5.10069928436602, 0.82346285235115, 0};
GD64 HPF_CoefB[HIGH_PASS_FILTER_COEF_SIZE + 1] = {0.90744853978126, -5.44469123868758, 13.61172809671895, -18.14897079562527, 13.61172809671895, -5.44469123868758, 0.90744853978126, 0};
GD64 HPF_ArrDataIn[HIGH_PASS_FILTER_COEF_SIZE] = {0};
GD64 HPF_ArrDataOut[HIGH_PASS_FILTER_COEF_SIZE] = {0};

#define PI 3.14159265358979

//ECG signal filter define
#define LP150HZ_STAGE_FS500   4
static const GD64 LP150HZ_SOS_FS500[LP150HZ_STAGE_FS500][6] = {
	{ 1,2,1,1,0.521309265637001,0.686992220901831 },
	{ 1,2,1,1,0.404372288519084,0.308576213864916 },
	{ 1,2,1,1,0.345121039357246,0.116835143825427 },
	{ 1,2,1,1,0.319763902271563,0.034777724501379 },
};
static const GD64 LP150HZ_G_FS500[LP150HZ_STAGE_FS500 + 1] = {
	0.552075371634708,
	0.428237125596000,
	0.365489045795668,
	0.338635406693236,
	1.000000000000000
};

#define LP150HZ_STAGE_FS1000  4
static const GD64 LP150HZ_SOS_FS1000[LP150HZ_STAGE_FS1000][6] = {
	{ 1,2,1,1,-1.015320986156612,0.727367235221825 },
	{ 1,2,1,1,-0.811037098605681,0.379818727064831 },
	{ 1,2,1,1,-0.702809498205855,0.195690935532604 },
	{ 1,2,1,1,-0.655471917606866,0.115155433128685 },
};
static const GD64 LP150HZ_G_FS1000[LP150HZ_STAGE_FS1000 + 1] = {
	0.178011562266303,
	0.142195407114788,
	0.123220359331687,
	0.114920878880455,
	1.000000000000000
};

#define LP120HZ_STAGE_FS1000  4
static const GD64 LP120HZ_SOS_FS1000[LP120HZ_STAGE_FS1000][6] = {
	{ 1,2,1,1,-1.286171024111227,0.764370887483613 },
	{ 1,2,1,1,-1.056235943388000,0.448945679767090 },
	{ 1,2,1,1,-0.929107652628456,0.274550944551617 },
	{ 1,2,1,1,-0.872288338066881,0.196606143603788 },
};
static const GD64 LP120HZ_G_FS1000[LP120HZ_STAGE_FS1000 + 1] = {
	0.119549965843096,
	0.098177434094773,
	0.086360822980790,
	0.081079451384227,
	1.000000000000000
};

#define LP120HZ_STAGE_FS500   4
static const GD64 LP120HZ_SOS_FS500[LP120HZ_STAGE_FS500][6] = {
	{1,2,1,1,-0.105114653119978,0.674052928816841},
	{1,2,1,1,-0.080786840911068,0.286608894410455},
	{1,2,1,1,-0.068629935436961,0.092998369043934},
	{1,2,1,1,-0.063461628641803,0.010688064337096},
};
static const GD64 LP120HZ_G_FS500[LP120HZ_STAGE_FS500 + 1] = {
	0.392234568924216,
	0.301455513374847,
	0.256092108401743,
	0.236806608923823,
	1.000000000000000
};

#define LP120HZ_STAGE_FS250   4
static const GD64 LP120HZ_SOS_FS250[LP120HZ_STAGE_FS250][6] = {
	{ 1,2,1,1,1.936870401643021,0.952264591056672 },
	{ 1,2,1,1,1.855059020399814,0.869802975343476 },
	{ 1,2,1,1,1.796966165642348,0.811248400272168 },
	{ 1,2,1,1,1.767018651463263,0.781062864124577 },
};
static const GD64 LP120HZ_G_FS250[LP120HZ_STAGE_FS250 + 1] = {
	0.972283748174923,
	0.931215498935822,
	0.902053641478629,
	0.887020378896960,
	1.000000000000000
};

#define LP55HZ_STAGE_FS1000   4
static const GD64 LP55HZ_SOS_FS1000[LP55HZ_STAGE_FS1000][6] = {
	{ 1,2,1,1,-1.765114825012452,0.876023916371837 },
	{ 1,2,1,1,-1.583717463961252,0.683228647261576 },
	{ 1,2,1,1,-1.468233227639895,0.560488083173189 },
	{ 1,2,1,1,-1.412490873045438,0.501243217687826 },
};
static const GD64 LP55HZ_G_FS1000[LP55HZ_STAGE_FS1000 + 1] = {
	0.027727272839846,
	0.024877795825081,
	0.023063713883323,
	0.022188086160597,
	1.000000000000000,
};

#define LP55HZ_STAGE_FS500  4
static const GD64 LP55HZ_SOS_FS500[LP55HZ_STAGE_FS500][6] = {
	{ 1,2,1,1,-1.370586817292972,0.778797223984633 },
	{ 1,2,1,1,-1.138016414472014,0.476958929832650 },
	{ 1,2,1,1,-1.007207723826262,0.307190672281993 },
	{ 1,2,1,1,-0.948221252636367,0.230635893057958 },
};
static const GD64 LP55HZ_G_FS500[LP55HZ_STAGE_FS500 + 1] = {
	0.102052601672915,
	0.084735628840159,
	0.074995737113933,
	0.070603660105398,
	1.000000000000000,
};

#define LP55HZ_STAGE_FS250  4
static const GD64 LP55HZ_SOS_FS250[LP55HZ_STAGE_FS250][6] = {
	{ 1,2,1,1,-0.314494549237044,0.678366650017106 },
	{ 1,2,1,1,-0.242450322262230,0.293887401730828 },
	{ 1,2,1,1,-0.206282801447734,0.100871780645782 },
	{ 1,2,1,1,-0.190873062731622,0.018634452179060 },
};
static const GD64 LP55HZ_G_FS250[LP55HZ_STAGE_FS250 + 1] = {
	0.340968025195015,
	0.262859269867149,
	0.223647244799512,
	0.206940347361860,
	1.000000000000000,
};

#define LP45HZ_STAGE_FS1000  4
static const GD64 LP45HZ_SOS_FS1000[LP45HZ_STAGE_FS1000][6] = {
	{ 1,2,1,1,-0.314494549237044,0.678366650017106 },
	{ 1,2,1,1,-0.242450322262230,0.293887401730828 },
	{ 1,2,1,1,-0.206282801447734,0.100871780645782 },
	{ 1,2,1,1,-0.190873062731622,0.018634452179060 },
};
static const GD64 LP45HZ_G_FS1000[LP45HZ_STAGE_FS1000 + 1] = {
	0.340968025195015,
	0.262859269867149,
	0.223647244799512,
	0.206940347361860,
	1.000000000000000
};

#define LP45HZ_STAGE_FS500    4
static const GD64 LP45HZ_SOS_FS500[LP45HZ_STAGE_FS500][6] = {
	{ 1,2,1,1,-1.528839220975276,0.810717346659200 },
	{ 1,2,1,1,-1.301278894960406,0.541200824533548 },
	{ 1,2,1,1,-1.168196587814481,0.383581606779027 },
	{ 1,2,1,1,-1.106929858571015,0.311018888677482 },
};
static const GD64 LP45HZ_G_FS500[LP45HZ_STAGE_FS500 + 1] = {
	0.070469531420981,
	0.059980482393286,
	0.053846254741137,
	0.051022257526617,
	1.000000000000000
};

#define LP45HZ_STAGE_FS250  4
static const GD64 LP45HZ_SOS_FS250[LP45HZ_STAGE_FS250][6] = {
	{ 1,2,1,1,-0.314494549237044,0.678366650017106 },
	{ 1,2,1,1,-0.242450322262230,0.293887401730828 },
	{ 1,2,1,1,-0.206282801447734,0.100871780645782 },
	{ 1,2,1,1,-0.190873062731622,0.018634452179060 },
};
static const GD64 LP45HZ_G_FS250[LP45HZ_STAGE_FS250 + 1] = {
	0.340968025195015,
	0.262859269867149,
	0.223647244799512,
	0.206940347361860,
	1.000000000000000
};

#define HP0D05HZ_STAGE_FS1000   2
static const GD64 HP0D05HZ_SOS_FS1000[HP0D05HZ_STAGE_FS1000][6] = {
	{ 1,-2,1,1,-1.999759483131749,0.999759581815928 },
	{ 1,-2,1,1,-1.999419579147508,0.999419677814913 },
};
static const GD64 HP0D05HZ_G_FS1000[HP0D05HZ_STAGE_FS1000 + 1] = {
	0.999879766236919,
	0.999709814240605,
	1.000000000000000
};

#define HP0D05HZ_STAGE_FS500  2
static const GD64 HP0D05HZ_SOS_FS500[HP0D05HZ_STAGE_FS500][6] = {
	{ 1,-2,1,1,-1.999518826760270,0.999519221449531 },
	{ 1,-2,1,1,-1.998839297808073,0.998839692363201 },
};
static const GD64 HP0D05HZ_G_FS500[HP0D05HZ_STAGE_FS500 + 1] = {
	0.999759512052450,
	0.999419747542819,
	1.000000000000000
};

#define HP0D05HZ_STAGE_FS250  2
static const GD64 HP0D05HZ_SOS_FS250[HP0D05HZ_STAGE_FS250][6] = {
	{ 1,-2,1,1,-1.999037095803727,0.999038674181191 },
	{ 1,-2,1,1,-1.997679153411597,0.997680730716872 },
};
static const GD64 HP0D05HZ_G_FS250[HP0D05HZ_STAGE_FS250 + 1] = {
	0.999518942496230,
	0.998839971032118,
	1.000000000000000
};

#define HP0D5HZ_STAGE_FS1000  2
static const GD64 HP0D5HZ_SOS_FS1000[HP0D5HZ_STAGE_FS1000][6] = {
	{ 1,-2,1,1,-1.997588562550215,0.997598420295188 },
	{ 1,-2,1,1,-1.994202061864258,0.994211902897450 },
};
static const GD64 HP0D5HZ_G_FS1000[HP0D5HZ_STAGE_FS1000 + 1] = {
	0.998796745711351,
	0.997103491190427,
	1.000000000000000
};

#define HP0D5HZ_STAGE_FS500   2
static const GD64 HP0D5HZ_SOS_FS500[HP0D5HZ_STAGE_FS500][6] = {
	{ 1,-2,1,1,-1.995163241283863,0.995202624875511 },
	{ 1,-2,1,1,-1.988418017374659,0.988457267818733 },
};
static const GD64 HP0D5HZ_G_FS500[HP0D5HZ_STAGE_FS500 + 1] = {
	0.997591466539843,
	0.994218821298348,
	1.000000000000000
};

#define HP0D5HZ_STAGE_FS250   2
static const GD64 HP0D5HZ_SOS_FS250[HP0D5HZ_STAGE_FS250][6] = {
	{ 1,-2,1,1,-1.990271241661729,0.990428397520602 },
	{ 1,-2,1,1,-1.976891354287120,0.977047453642916 },
};
static const GD64 HP0D5HZ_G_FS250[HP0D5HZ_STAGE_FS250 + 1] = {
	0.995174909795583,
	0.988484701982509,
	1.000000000000000
};

#define HP0D67HZ_STAGE_FS1000   2
static const GD64 HP0D67HZ_SOS_FS1000[HP0D67HZ_STAGE_FS1000][6] = {
	{ 1,-2,1,1,-1.996765507386640,0.996783200718337 },
	{ 1,-2,1,1,-1.992233931075327,0.992251584252743 },
};
static const GD64 HP0D67HZ_G_FS1000[HP0D67HZ_STAGE_FS1000 + 1] = {
	0.998387177026244,
	0.996121378832018,
	1.000000000000000
};

#define HP0D67HZ_STAGE_FS500  2
static const GD64 HP0D67HZ_SOS_FS500[HP0D67HZ_STAGE_FS500][6] = {
	{ 1,-2,1,1,-1.993506129984135,0.993576789350877 },
	{ 1,-2,1,1,-1.984492770590399,0.984563110480689 },
};
static const GD64 HP0D67HZ_G_FS500[HP0D67HZ_STAGE_FS500 + 1] = {
	0.996770729833753,
	0.992263970267772,
	1.000000000000000
};

#define HP0D67HZ_STAGE_FS250  2
static const GD64 HP0D67HZ_SOS_FS250[HP0D67HZ_STAGE_FS250][6] = {
	{ 1,-2,1,1,-1.986913427492570,0.987195155216588 },
	{ 1,-2,1,1,-1.969084562840559,0.969363762580524 },
};
static const GD64 HP0D67HZ_G_FS250[HP0D67HZ_STAGE_FS250 + 1] = {
	0.993527145677289,
	0.984612081355271,
	1.000000000000000
};

#define HP3HZ_STAGE_FS1000  2
static const GD64 HP3HZ_SOS_FS1000[HP3HZ_STAGE_FS1000][6] = {
	{ 1, - 2,	1,	1, - 1.98532458709362,	0.985677337945189 },
	{ 1, - 2,	1,	1, - 1.96541950338359,	0.965768717516138 }

};
static const GD64 HP3HZ_G_FS1000[HP3HZ_STAGE_FS1000 + 1] = {
	0.992750481259703,
	0.982797055224932,
	1
};

#define HP3HZ_STAGE_FS500   2
static const GD64 HP3HZ_SOS_FS500[HP3HZ_STAGE_FS500][6] = {
	{1, - 2,	1,	1, - 1.97016248697805,	0.971563336669560 },
	{1, - 2,	1,	1, - 1.93132781565559,	0.932701052631051 }

};
static const GD64 HP3HZ_G_FS500[HP3HZ_STAGE_FS500 + 1] = {
	0.985431455911901,
	0.966007217071661,
	1
};

#define HP3HZ_STAGE_FS250   2
static const GD64 HP3HZ_SOS_FS250[HP3HZ_STAGE_FS250][6] = {
	{1, - 2,	1,	1, - 1.93843971601956,	0.943962707962528 },
	{1, - 2,	1,	1, - 1.86455782563483,	0.869870313695752 }

};
static const GD64 HP3HZ_G_FS250[HP3HZ_STAGE_FS250 + 1] = {
	0.970600605995522,
	0.933607034832644,
	1
};

#define HP5HZ_STAGE_FS1000  2
static const GD64 HP5HZ_SOS_FS1000[HP5HZ_STAGE_FS1000][6] = {
	{1, - 2,	1,	1, - 1.97526963485187,	0.976244792359440 },
	{1, - 2,	1,	1, - 1.94263823054011,	0.943597278470367 }

};
static const GD64 HP5HZ_G_FS1000[HP5HZ_STAGE_FS1000 + 1] = {
	0.987878606802828,
	0.971558877252620,
	1
};

#define HP5HZ_STAGE_FS500   2
static const GD64 HP5HZ_SOS_FS500[HP5HZ_STAGE_FS500][6] = {
	{1, - 2,	1,	1, - 1.97526963485187,	0.976244792359440 },
	{1, - 2,	1,	1, - 1.94263823054011,	0.943597278470367 }

};
static const GD64 HP5HZ_G_FS500[HP5HZ_STAGE_FS500 + 1] = {
	0.987878606802828,
	0.971558877252620,
	1
};

#define HP5HZ_STAGE_FS250 2
static const GD64 HP5HZ_SOS_FS250[HP5HZ_STAGE_FS250][6] = {
	{ 1, - 2,	1,	1, - 1.89341560102250,	0.908464412949295 },
	{ 1, - 2,	1,	1, - 1.77831348813944,	0.792447471832947 }
};
static const GD64 HP5HZ_G_FS250[HP5HZ_STAGE_FS250 + 1] = {
	0.950470003492949,
	0.892690239993096,
	1
}; 
//END define ecg signal filter


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
GU8 gh3x2x_mp_crc_cal(GU8 *puchData, GU32 unLen)
{
	int i;
	GU8 crc = 0x00;

	while (unLen--)
	{
		crc ^= *puchData++;
		for (i = 8; i>0; --i)
		{
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ 0x31;
			}
			else
			{
				crc = (crc << 1);
			}
			crc = crc & 0xff;
		}
	}
	return(crc);
}

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
GD64 gh3x2x_mp_high_pass_filter(GD64 fData)
{
	GU32 i = 0;
	
	if(HPF_FirstFlag)
	{
		memset(HPF_ArrDataIn, 0, HIGH_PASS_FILTER_COEF_SIZE*sizeof(GD64));
		memset(HPF_ArrDataOut, 0, HIGH_PASS_FILTER_COEF_SIZE*sizeof(GD64));
		
		GD64 skip = fData - HPF_ArrDataIn[HIGH_PASS_FILTER_COEF_SIZE - 1];
		for(i = 0; i < HIGH_PASS_FILTER_COEF_SIZE; i++)
		{
			HPF_ArrDataIn[i] += skip;
		}
		HPF_FirstFlag = 0;
	}

	GD64 sum = 0;
	for (i = 0; i < HIGH_PASS_FILTER_COEF_SIZE; i++) 
	{
		sum += (HPF_CoefB[i + 1] * HPF_ArrDataIn[HIGH_PASS_FILTER_COEF_SIZE - i - 1]);
	}
	GD64 dataBuffer_1 = HPF_CoefB[0] * fData + sum;

	sum = 0;
	for (i = 0; i < HIGH_PASS_FILTER_COEF_SIZE; i++) 
	{
		sum += (HPF_CoefA[i + 1] * HPF_ArrDataOut[HIGH_PASS_FILTER_COEF_SIZE - i - 1]);
	}
	GD64 dataBuffer_2 = sum;

	GD64 dataOut = (dataBuffer_1 - dataBuffer_2) / HPF_CoefA[0];

	for(i = 0; i < HIGH_PASS_FILTER_COEF_SIZE - 1; i++)
	{
		HPF_ArrDataIn[i] = HPF_ArrDataIn[i + 1];
	}
	HPF_ArrDataIn[i] = fData;

	for(i = 0; i < HIGH_PASS_FILTER_COEF_SIZE - 1; i++)
	{
		HPF_ArrDataOut[i] = HPF_ArrDataOut[i + 1];
	}
	HPF_ArrDataOut[i] = dataOut;

	return dataOut;
}

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
GU8 gh3x2x_mp_ecg_lead_off_dectection(GS32 nRawdataArr[], GU16 usRawdataLen, GU8 uchECGChannel_N, GU16 *usReSampleData_Len)
{
	
	//ECG LeadOff detection algorithm define
#define ADCREF 1800   //ref
#define IS_BIG_ENDIAN 1  //data read from GH3100 FIFO is big endian?
//#define PI 3.14159265

#define  ABS(x) ( (x)>0?(x):(-(x)) ) //Abs value

#define AMP_NUM 									5   //IQ amplitude	
#define IQ_NUM 										48  //IQ cumulative points
#define winAmpCoef  								4082 //
#define LEADOFF_DET_IQ_AMP_THR_uV  					1500 //IQ lead off threshold (Vpp/2), unit uV
#define LEADOFF_DET_IQ_AMP 							((long long)LEADOFF_DET_IQ_AMP_THR_uV * 20 * ADC_OFFSET / ADCREF/1000*IQ_NUM*1000/winAmpCoef)
#define LEADOFF_DET_IQ_AMP_DIFF_THR_uV  			1000 //IQ lead off jump threshold (Vpp/2), unit uV
#define LEADOFF_DET_IQ_AMP_DIFF 					((long long)LEADOFF_DET_IQ_AMP_DIFF_THR_uV * 20 * ADC_OFFSET / ADCREF/1000*IQ_NUM*1000/winAmpCoef)
#define LEADOFF_DET_VAL_THR_mV  					80 //Amplitude detection threshold, unit mV, more than 80mV is considered leadoff,
#define RESAMPLE_800HZ_TO_500HZ 					1 	//re-sampling switch	


#if RESAMPLE_800HZ_TO_500HZ

#define RS_FIR_ORDER 182  //Resample FIR filter order
#define RS_I 5 //Insertion factor
#define RS_D 8 //Decimation factor

#if RS_FIR_ORDER%2 //Odd order, even coefficients

#define RS_FIR_COEF_NUM ((RS_FIR_ORDER+1)/2) //The number of coefficients that need to be stored
#else//Even order, odd coefficients
#define RS_FIR_COEF_NUM ((RS_FIR_ORDER+2)/2)

#endif
//Resampling filter coefficients
#define SHIFTBITS 30
	const GS32 rsFirCoef[RS_FIR_COEF_NUM] = { 58200, 102435, 156130, 215460, 274735, 326585, 362380, 372790, 348645, 281870, 166550, 0, -216285, -475850, \
											-766815, -1071825, -1368460, -1630170, -1827760, -1931325, -1912605, -1747635, -1419445, -920670, -255820, \
											557070, 1485515, 2482735, 3488860, 4433375, 5238695, 5824850, 6114965, 6041350, 5551740, 4615280, 3227790, \
											1415785, -761100, -3209770, -5805610, -8396875, -10811545, -12866385, -14377925, -15174745, -15110485, -14076620, \
											-12014295, -8924190, -4873605, 0, 5489625, 11323885, 17176845, 22682405, 27452740, 31100030, 33260510, 33619540, \
											31936265, 28066360, 21981325, 13782945, 3711580, -7852615, -20396185, -33286830, -45795550, -57125820, -66448630, \
											-72941905, -75832375, -74437800, -68207360, -56757870, -39903755, -17678885, 9651270, 41588185, 77410100, 116192965, \
											156841955, 198132190, 238757000, 277381455, 312698655, 343486005, 368658785, 387318230, 398791740, 402663190 };
#define RS_BUF_SIZE (RS_FIR_ORDER / RS_I + 1)
	GS32 rsXbuf[RS_BUF_SIZE];//Filtered data buffer
	GS32 bufHead = -1; //Reverse circular buffer head position
	GS32 rsFirState = 0; //Resampling filter state
	GU32 rsIcounter = 0; //Insert data sequence number count，4khz sample rate
	GU32 rsDcounter = 0; //Extracted data sequence number count，4khz sample rate

#endif 
	
	//IQ detection variables
	GU32 guiCounter = 0;  //Global counter
	GU32 iqCounter = 0; //iq integral counter
	GS64 iVal = 0; //I Accumulate
	GS64 qVal = 0; //Q Accumulate
	GS64 iqAmp[AMP_NUM] = {0}; //IQ amplitude value buffer
	const GS32 winCoef[IQ_NUM / 2] = { 0, 9580474, 38150933, 85201535, 149892663, 231069901, 327284639, 436819923, 557721094, 687830666, 824826833, 966264894, \
									   1109620882, 1252336609, 1391865307, 1525717084, 1651503351, 1766979451, 1870084709, 1958979209, 2032076627, 2088072537, \
									   2125967693, 2145085852 };

	GU8 gscFlagFrPrev = 0; // Fast recovery marks the cache, after fast recovery, reset the IQ detection and resampling module
	GS64  LEADOFF_DET_IQ_AMP_THR_RAW = (LEADOFF_DET_IQ_AMP*LEADOFF_DET_IQ_AMP); //Threshold during normal sampling
	GS64  LEADOFF_DET_IQ_AMP_FR_THR_RAW = (LEADOFF_DET_IQ_AMP*LEADOFF_DET_IQ_AMP / 20 / 20); //Threshold during Fast recovery, at this time the gain is only 1

	GS64 LEADOFF_DET_IQ_AMP_DIFF_THR_RAW = (LEADOFF_DET_IQ_AMP_DIFF*LEADOFF_DET_IQ_AMP_DIFF);//Threshold during normal sampling
	GS64 LEADOFF_DET_IQ_AMP_DIFF_FR_THR_RAW = (LEADOFF_DET_IQ_AMP_DIFF*LEADOFF_DET_IQ_AMP_DIFF / 20 / 20); //Threshold during Fast recovery, at this time the gain is only 1

	// Amplitude detection variable
	GS32 COND_VAL_VALID_TIME = 1; //Effective time of amplitude detection
	GS32  LEADOFF_DET_VAL_THR_RAW = (LEADOFF_DET_VAL_THR_mV * (20 * ADC_OFFSET / ADCREF)); //


	//start
	GU32 i = 0;
	GU32 j = 0;
	
	//reset 
	guiCounter = 0;
    iqCounter = 0;
    iVal = 0;
    qVal = 0;
    for (i = 0; i < AMP_NUM; i++)
        iqAmp[i] = 0;
		
#if RESAMPLE_800HZ_TO_500HZ
    bufHead = -1;
    rsFirState = 0;
    rsIcounter = 0;
    rsDcounter = 0;
#endif

	//detect
	GU32 fifoVal;
    GS32 adcVal;
    GU8 flagFastRecovery;
    GU8 chNo;
    GS32 absAdcVal; //ABS of adcVal
    GS64 temp;
    GS32 rstValue = 0;
	
#if RESAMPLE_800HZ_TO_500HZ
    GU16 rsFifoDataNum = 0;
    GS32 k;
    GS64 y;
    GS32 delta;
#endif

	for(i = 0; i < usRawdataLen; i++)
	{
		fifoVal = (GU32)nRawdataArr[i];
	#if IS_BIG_ENDIAN
		//big endian to little endian
		fifoVal = ((fifoVal & 0xFF) << 24) | ((fifoVal & 0x0000FF00) << 8) | ((fifoVal & 0x00FF0000) >> 8) | ((fifoVal & 0xFF000000) >> 24);
	#endif
		chNo = (fifoVal >> 27); //Channel number
        if(chNo != uchECGChannel_N)
        {
            continue;
        }
		adcVal = (int)(fifoVal & 0x00FFFFFF) - ADC_OFFSET; //adc Sample value extraction
        flagFastRecovery = (fifoVal >> 26) & 0x01;
		
		//Reset IQ test after fast recovery is over
        if(gscFlagFrPrev != flagFastRecovery)
        {
            //reset 
			guiCounter = 0;
			iqCounter = 0;
			iVal = 0;
			qVal = 0;
			for (i = 0; i < AMP_NUM; i++)
				iqAmp[i] = 0;
		
		#if RESAMPLE_800HZ_TO_500HZ
			bufHead = -1;
			rsFirState = 0;
			rsIcounter = 0;
			rsDcounter = 0;
		#endif
        }
        gscFlagFrPrev = flagFastRecovery;
		
		//sample counter
        if(guiCounter < 20000)
            guiCounter++;

        //Absolute value judgment of amplitude
        if(flagFastRecovery == 0 && guiCounter>COND_VAL_VALID_TIME)
        {
            absAdcVal = ABS(adcVal);
            if(absAdcVal > LEADOFF_DET_VAL_THR_RAW)
                rstValue |= 0x01;
        }
		
		//Add window
        if(iqCounter < IQ_NUM / 2) //Front half window
        {
            temp = (long long)adcVal*winCoef[iqCounter];
        }
        else //Back half window
        {
            temp = (long long)adcVal*winCoef[IQ_NUM - 1 - iqCounter];
        }
		
		//IQ integral
        switch (iqCounter % 4)
        {
        case 0:
            iVal += temp;
            break;
        case 1:
            qVal += temp;
            break;
        case 2:
            iVal -= temp;
            break;
        case 3:
            qVal -= temp;
            break;
        default:
            break;
        }
		
		if(iqCounter == (IQ_NUM - 1))
        {
            for (j = AMP_NUM - 1; j >0; j--)
            {
                iqAmp[j] = iqAmp[j - 1];
            }
            iVal = iVal >> 31;
            qVal = qVal >> 31;
            iqAmp[0] = iVal*(int)iVal + qVal*(int)qVal;
            iVal = 0;
            qVal = 0;

            //judge leadoff
            if(flagFastRecovery == 0) // During normal ECG
            {
                //IQ Absolute amplitude
                if(iqAmp[0] > LEADOFF_DET_IQ_AMP_THR_RAW && iqAmp[1] > LEADOFF_DET_IQ_AMP_THR_RAW)
                    rstValue |= 0x02;
                //IQ Jump amplitude
                if((iqAmp[0] - iqAmp[2]) > LEADOFF_DET_IQ_AMP_DIFF_THR_RAW && (iqAmp[1] - iqAmp[2]) > LEADOFF_DET_IQ_AMP_DIFF_THR_RAW)
                    rstValue |= 0x04;

            }
            else//during Fast Recovery
            {
                //IQ Absolute amplitude
                if(iqAmp[0] > LEADOFF_DET_IQ_AMP_FR_THR_RAW && iqAmp[1] > LEADOFF_DET_IQ_AMP_FR_THR_RAW)
                    rstValue |= 0x02;
                //IQ Jump amplitude
                if((iqAmp[0] - iqAmp[1]) > LEADOFF_DET_IQ_AMP_DIFF_FR_THR_RAW)
                    rstValue |= 0x04;
            }
		}
		
		//IQ counter
        iqCounter++;
        iqCounter = iqCounter % (IQ_NUM);
		
		 //re samping
	#if RESAMPLE_800HZ_TO_500HZ
        //Data cache
        if(rsFirState > 0)
        {
            bufHead = bufHead - 1;
            bufHead = bufHead < 0 ? (RS_BUF_SIZE - 1) : bufHead;
            rsXbuf[bufHead] = adcVal;
            rsIcounter += RS_I;
        }
        else //The first point fills the data array
        {
            for (j = 0; j < RS_FIR_ORDER / RS_I + 1; j++)
                rsXbuf[j] = adcVal;
            rsIcounter = 0;
            rsFirState = 1;
            bufHead = RS_BUF_SIZE - 1;
        }

        //Extract
        if(rsDcounter >= rsIcounter && rsDcounter < (rsIcounter + RS_I)) //Meet the extraction conditions
        {

            delta = rsDcounter - rsIcounter;
            k = bufHead;
            y = 0;
            for(j = delta; j < RS_FIR_COEF_NUM; j = j + RS_I) //Multiply and add the first half of the filter
            {
                y += ((long long)rsXbuf[k] * rsFirCoef[j]);
                k = (k + 1) % RS_BUF_SIZE;
            }
            for(j = j; j <(RS_FIR_ORDER + 1); j = j + RS_I) //Multiply and add the second half
            {
                y += ((long long)rsXbuf[k] * rsFirCoef[RS_FIR_ORDER - j]);
                k = (k + 1) % RS_BUF_SIZE;
            }
            //y *= RS_I; 
            rsDcounter += RS_D;

            y = y >> SHIFTBITS;
            k = (int)y + ADC_OFFSET; //k is used as a temporary variable
            if(k < 0)
                k = 0;
            else if(k >= (ADC_OFFSET * 2))
                k = (ADC_OFFSET * 2 - 1);
            fifoVal = (((unsigned int)chNo) << 27) + (((unsigned int)flagFastRecovery) << 26) + ((unsigned int)k); //Assemble output data
		#if IS_BIG_ENDIAN
            fifoVal = ((fifoVal & 0xFF) << 24) | ((fifoVal & 0x0000FF00) << 8) | ((fifoVal & 0x00FF0000) >> 8) | ((fifoVal & 0xFF000000) >> 24); //endian reverse
		#endif
            nRawdataArr[rsFifoDataNum] = fifoVal;
            rsFifoDataNum++; //Add 1 to the extracted data count
        }//Complete extraction

	#endif //RESAMPLE_800HZ_TO_500HZ
		
	
	}
	

#if RESAMPLE_800HZ_TO_500HZ	
    *usReSampleData_Len = rsFifoDataNum;
#else
    *usReSampleData_Len = usRawdataLen;
#endif 


	return rstValue;

}

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
GD64 gh3x2x_mp_signal_IIR_filter(IIR_State_Def *stIIR_State, GD64 dData)
{
	GD64 x = dData;
	GD64 y = 0;
	
	for (int i = 0; i < stIIR_State->stages; i++)
	{
		stIIR_State->x[i][2] = stIIR_State->x[i][1];
		stIIR_State->x[i][1] = stIIR_State->x[i][0];
		stIIR_State->x[i][0] = x;

		y = (stIIR_State->x[i][0] * stIIR_State->sos[i][0] + stIIR_State->x[i][1] * stIIR_State->sos[i][1] +
			stIIR_State->x[i][2] * stIIR_State->sos[i][2]) * stIIR_State->gain[i] - stIIR_State->y[i][0] * stIIR_State->sos[i][4] -
			stIIR_State->y[i][1] * stIIR_State->sos[i][5];
		stIIR_State->y[i][1] = stIIR_State->y[i][0];
		stIIR_State->y[i][0] = y;

		x = y;	// Output to the next node
	}

	return y;
}

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
void gh3x2x_mp_signal_filter(GS32 nFilterArr[], GU16 usLength, GU8 uchHpType, GU8 uchLpType, GU8 uchFsType)
{
	GU16 i = 0;
	IIR_State_Def stState;
	
	
	if(uchLpType != 0)
	{
		//init lp filter
		memset(&stState, 0, sizeof(IIR_State_Def));
		if(uchLpType == 1)	// LP150Hz
		{
			if(uchFsType == 0)	//150Hz cut-off frequency cannot be selected for 250Hz sampling, here is forced to choose 120Hz
			{
				stState.gain = LP120HZ_G_FS250;
				stState.sos = LP120HZ_SOS_FS250;
				stState.stages = LP120HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = LP150HZ_G_FS500;
				stState.sos = LP150HZ_SOS_FS500;
				stState.stages = LP150HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = LP150HZ_G_FS1000;
				stState.sos = LP150HZ_SOS_FS1000;
				stState.stages = LP150HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = LP150HZ_G_FS500;
				stState.sos = LP150HZ_SOS_FS500;
				stState.stages = LP150HZ_STAGE_FS500;
			}
		}
		else if(uchLpType == 2)	// LP120Hz
		{
			if(uchFsType == 0)
			{
				stState.gain = LP120HZ_G_FS250;
				stState.sos = LP120HZ_SOS_FS250;
				stState.stages = LP120HZ_STAGE_FS250;
				
			}
			else if(uchFsType == 1)
			{
				stState.gain = LP120HZ_G_FS500;
				stState.sos = LP120HZ_SOS_FS500;
				stState.stages = LP120HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = LP120HZ_G_FS1000;
				stState.sos = LP120HZ_SOS_FS1000;
				stState.stages = LP120HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = LP120HZ_G_FS500;
				stState.sos = LP120HZ_SOS_FS500;
				stState.stages = LP120HZ_STAGE_FS500;
			}
		}
		else if(uchLpType == 3)
		{
			if (uchFsType == 0)
			{
				stState.gain = LP55HZ_G_FS250;
				stState.sos = LP55HZ_SOS_FS250;
				stState.stages = LP55HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = LP55HZ_G_FS500;
				stState.sos = LP55HZ_SOS_FS500;
				stState.stages = LP55HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = LP55HZ_G_FS1000;
				stState.sos = LP55HZ_SOS_FS1000;
				stState.stages = LP55HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = LP55HZ_G_FS500;
				stState.sos = LP55HZ_SOS_FS500;
				stState.stages = LP55HZ_STAGE_FS500;
			}
		}
		else if(uchLpType == 4)	// LP45Hz
		{
			if(uchFsType == 0)
			{
				stState.gain = LP45HZ_G_FS250;
				stState.sos = LP45HZ_SOS_FS250;
				stState.stages = LP45HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = LP45HZ_G_FS500;
				stState.sos = LP45HZ_SOS_FS500;
				stState.stages = LP45HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = LP45HZ_G_FS1000;
				stState.sos = LP45HZ_SOS_FS1000;
				stState.stages = LP45HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = LP45HZ_G_FS500;
				stState.sos = LP45HZ_SOS_FS500;
				stState.stages = LP45HZ_STAGE_FS500;
			}
		}
		else
		{
			if(uchFsType == 0)
			{
				stState.gain = LP120HZ_G_FS500;
				stState.sos = LP120HZ_SOS_FS500;
				stState.stages = LP120HZ_STAGE_FS500;
			}
			else if(uchFsType == 1)
			{
				stState.gain = LP150HZ_G_FS500;
				stState.sos = LP150HZ_SOS_FS500;
				stState.stages = LP150HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = LP150HZ_G_FS1000;
				stState.sos = LP150HZ_SOS_FS1000;
				stState.stages = LP150HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = LP150HZ_G_FS500;
				stState.sos = LP150HZ_SOS_FS500;
				stState.stages = LP150HZ_STAGE_FS500;
			}
		}
		
		
		for(i = 0; i < usLength; i++)
		{
			nFilterArr[i] = gh3x2x_mp_signal_IIR_filter(&stState, (GD64)nFilterArr[i]);
		}
	
	}
	
	if(uchHpType != 0)
	{
		//init hp filter
		memset(&stState, 0, sizeof(IIR_State_Def));
		if(uchHpType == 1) //0.05hz
		{
			if(uchFsType == 0)
			{
				stState.gain = HP0D05HZ_G_FS250;
				stState.sos = HP0D05HZ_SOS_FS250;
				stState.stages = HP0D05HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP0D05HZ_G_FS500;
				stState.sos = HP0D05HZ_SOS_FS500;
				stState.stages = HP0D05HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP0D05HZ_G_FS1000;
				stState.sos = HP0D05HZ_SOS_FS1000;
				stState.stages = HP0D05HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP0D05HZ_G_FS500;
				stState.sos = HP0D05HZ_SOS_FS500;
				stState.stages = HP0D05HZ_STAGE_FS500;
			}
		}
		else if(uchHpType == 2)//0.5hz
		{
			if(uchFsType == 0)
			{
				stState.gain = HP0D5HZ_G_FS250;
				stState.sos = HP0D5HZ_SOS_FS250;
				stState.stages = HP0D5HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP0D5HZ_G_FS500;
				stState.sos = HP0D5HZ_SOS_FS500;
				stState.stages = HP0D5HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP0D5HZ_G_FS1000;
				stState.sos = HP0D5HZ_SOS_FS1000;
				stState.stages = HP0D5HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP0D5HZ_G_FS500;
				stState.sos = HP0D5HZ_SOS_FS500;
				stState.stages = HP0D5HZ_STAGE_FS500;
			}
		}
		else if(uchHpType == 3)//0.67hz
		{
			if(uchFsType == 0)
			{
				stState.gain = HP0D67HZ_G_FS250;
				stState.sos = HP0D67HZ_SOS_FS250;
				stState.stages = HP0D67HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP0D67HZ_G_FS500;
				stState.sos = HP0D67HZ_SOS_FS500;
				stState.stages = HP0D67HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP0D67HZ_G_FS1000;
				stState.sos = HP0D67HZ_SOS_FS1000;
				stState.stages = HP0D67HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP0D67HZ_G_FS500;
				stState.sos = HP0D67HZ_SOS_FS500;
				stState.stages = HP0D67HZ_STAGE_FS500;
			}
		}
		else if(uchHpType == 4)//3hz
		{
			if(uchFsType == 0)
			{
				stState.gain = HP3HZ_G_FS250;
				stState.sos = HP3HZ_SOS_FS250;
				stState.stages = HP3HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP3HZ_G_FS500;
				stState.sos = HP3HZ_SOS_FS500;
				stState.stages = HP3HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP3HZ_G_FS1000;
				stState.sos = HP3HZ_SOS_FS1000;
				stState.stages = HP3HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP3HZ_G_FS500;
				stState.sos = HP3HZ_SOS_FS500;
				stState.stages = HP3HZ_STAGE_FS500;
			}
		}
		else if(uchHpType == 5)//5hz
		{
			if(uchFsType == 0)
			{
				stState.gain = HP5HZ_G_FS250;
				stState.sos = HP5HZ_SOS_FS250;
				stState.stages = HP5HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP5HZ_G_FS500;
				stState.sos = HP5HZ_SOS_FS500;
				stState.stages = HP5HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP5HZ_G_FS1000;
				stState.sos = HP5HZ_SOS_FS1000;
				stState.stages = HP5HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP5HZ_G_FS500;
				stState.sos = HP5HZ_SOS_FS500;
				stState.stages = HP5HZ_STAGE_FS500;
			}
		}
		else
		{
			if(uchFsType == 0)
			{
				stState.gain = HP0D05HZ_G_FS250;
				stState.sos = HP0D05HZ_SOS_FS250;
				stState.stages = HP0D05HZ_STAGE_FS250;
			}
			else if(uchFsType == 1)
			{
				stState.gain = HP0D05HZ_G_FS500;
				stState.sos = HP0D05HZ_SOS_FS500;
				stState.stages = HP0D05HZ_STAGE_FS500;
			}
			else if(uchFsType == 2)
			{
				stState.gain = HP0D05HZ_G_FS1000;
				stState.sos = HP0D05HZ_SOS_FS1000;
				stState.stages = HP0D05HZ_STAGE_FS1000;
			}
			else
			{
				stState.gain = HP0D05HZ_G_FS500;
				stState.sos = HP0D05HZ_SOS_FS500;
				stState.stages = HP0D05HZ_STAGE_FS500;
			}
		}
	
		for(i = 0; i < usLength; i++)
		{
			nFilterArr[i] = gh3x2x_mp_signal_IIR_filter(&stState, (GD64)nFilterArr[i]);
		}
	
	}

}

/**
 * @fn     void gh3x2x_mp_signal_fft(GF32 fFFT_InArr[], GF32 fFFT_OutArr[], GU32 unLength, GU32 unStep)
 *
 * @brief  fft
 *
 * @attention   None
 *
 * @param[in]   GF32 fFFT_InArr[], GS32 nFFT_OutArr[], GU32 unLength, GU32 step
 * @param[out]  GF32 fFFT_InArr[]
 *
 * @return  ret
 */
void gh3x2x_mp_signal_fft(GF32 fFFT_InArr[], GF32 fFFT_OutArr[], GU32 unLength, GU32 unStep)
{
	GU32 i;
	GF32 real = 0.0;
	GF32 image = 0.0;
	GF32 a, b, c, d, theta;

	if (unStep < unLength)
	{
		gh3x2x_mp_signal_fft(fFFT_OutArr, fFFT_InArr, unLength, unStep * 2);
		gh3x2x_mp_signal_fft(fFFT_OutArr + 2*unStep, fFFT_InArr + 2*unStep, unLength, unStep * 2);
		
		for (i = 0; i < unLength; i += 2 * unStep)
		{
			theta = -PI * i / unLength;
			a = cos(theta);
			b = sin(theta);
			c = fFFT_OutArr[2*(i + unStep)];
			d = fFFT_OutArr[2*(i + unStep) + 1];
			real = a * c - b * d;
			image = b * c + a * d;
			fFFT_InArr[i] = fFFT_OutArr[2*i] + real;
			fFFT_InArr[i+1] = fFFT_OutArr[2*i + 1] + image;
			fFFT_InArr[i + unLength] = fFFT_OutArr[2*i] - real;
			fFFT_InArr[i + unLength + 1] = fFFT_OutArr[2*i + 1] - image;
		}
	}

}

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
										GF32 fFFTOutArr_real[], GF32 fFFTOutArr_image[], GU8 uchFFTMode, GU32 uchCalModulus_Angle)
{
	GS32 it,m,is,i,j,nv,l0;
	GF32 p,q,s,vr,vi,poddr,poddi;
	
	for (it=0; it<=unLength-1; it++)
	{
		m = it;
		is = 0;
		for(i=0; i<=unPower_2-1; i++)
		{
			j = m/2;
			is = 2*is+(m-2*j);
			m = j;
		}
		fFFTOutArr_real[it] = fFFTInArr_real[is];
		fFFTOutArr_image[it] = fFFTInArr_image[is];
	}
	
	fFFTInArr_real[0] = 1.0;
	fFFTInArr_image[0] = 0.0;
	p = 6.283185306/(1.0*unLength);
	fFFTInArr_real[1] = cos(p);
	fFFTInArr_image[1] = -sin(p);
	
	if (uchFFTMode!=0)
	fFFTInArr_image[1]=-fFFTInArr_image[1];
	
	for (i=2; i<=unLength-1; i++)
	{
		p = fFFTInArr_real[i-1]*fFFTInArr_real[1];
		q = fFFTInArr_image[i-1]*fFFTInArr_image[1];
		s = (fFFTInArr_real[i-1]+fFFTInArr_image[i-1])*(fFFTInArr_real[1]+fFFTInArr_image[1]);
		fFFTInArr_real[i] = p-q;
		fFFTInArr_image[i] = s-p-q;
	}
	
	for (it=0; it<=unLength-2; it=it+2)
	{
		vr = fFFTOutArr_real[it];
		vi = fFFTOutArr_image[it];
		fFFTOutArr_real[it] = vr+fFFTOutArr_real[it+1];
		fFFTOutArr_image[it] = vi+fFFTOutArr_image[it+1];
		fFFTOutArr_real[it+1] = vr-fFFTOutArr_real[it+1];
		fFFTOutArr_image[it+1] = vi-fFFTOutArr_image[it+1];
	}
	m = unLength/2;
	nv = 2;
	
	for (l0=unPower_2-2; l0>=0; l0--)
	{
		m = m/2;
		nv = 2*nv;
		for(it=0; it<=(m-1)*nv; it=it+nv)
		for (j=0; j<=(nv/2)-1; j++)
		{
			p = fFFTInArr_real[m*j]*fFFTOutArr_real[it+j+nv/2];
			q = fFFTInArr_image[m*j]*fFFTOutArr_image[it+j+nv/2];
			s = fFFTInArr_real[m*j]+fFFTInArr_image[m*j];
			s = s*(fFFTOutArr_real[it+j+nv/2]+fFFTOutArr_image[it+j+nv/2]);
			poddr = p-q;
			poddi = s-p-q;
			fFFTOutArr_real[it+j+nv/2] = fFFTOutArr_real[it+j]-poddr;
			fFFTOutArr_image[it+j+nv/2] = fFFTOutArr_image[it+j]-poddi;
			fFFTOutArr_real[it+j] = fFFTOutArr_real[it+j]+poddr;
			fFFTOutArr_image[it+j] = fFFTOutArr_image[it+j]+poddi;
		}
	}
	
	if(uchFFTMode!=0)
	for(i=0; i<=unLength-1; i++)
	{
		fFFTOutArr_real[i] = fFFTOutArr_real[i]/(1.0*unLength);
		fFFTOutArr_image[i] = fFFTOutArr_image[i]/(1.0*unLength);
	}
	
	if(uchCalModulus_Angle!=0)
	for(i=0; i<=unLength-1; i++)
	{
		fFFTInArr_real[i] = sqrt(fFFTOutArr_real[i]*fFFTOutArr_real[i]+fFFTOutArr_image[i]*fFFTOutArr_image[i]);
		if(fabs(fFFTOutArr_real[i])<0.000001*fabs(fFFTOutArr_image[i]))
		{
			if ((fFFTOutArr_image[i]*fFFTOutArr_real[i])>0)
			fFFTInArr_image[i] = 90.0;
			else
			fFFTInArr_image[i] = -90.0;
		}
		else
			fFFTInArr_image[i] = atan(fFFTOutArr_image[i]/fFFTOutArr_real[i])*360.0/6.283185306;
	}
	return;
}

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
GF32 *fFFT_HandleArr = NULL;
extern GU8 GH3x2xMp_RawdataBuf[RAWDATA_BUF_SIZE];
GU8 gh3x2x_mp_signal_fft_process(GF32 fFFTArr[], GU32 unLength)
{
	GU8 ret = 1;
	GS32 i = 0;
	//memset(fFFT_HandleArr, 0, 2*MAX_FFT_LENGTH*sizeof(GF32));
	fFFT_HandleArr = (GF32 *)(GH3x2xMp_RawdataBuf) + 2*MAX_FFT_LENGTH + 1; //Reuse GH3x2xMp_RawdataBuf memory
	
	//find the smallest 2^n from unLength
	GU32 fft_len = 0;
	i = 0;
	while(fft_len < MAX_FFT_LENGTH)
	{
		if(fft_len >= unLength)
			break;
		fft_len = pow(2, i);
		i++;
	}
	unLength = 	fft_len;

//	for(i = 2*unLength - 2; i >= 0; i -= 2)
//	{
//		fFFTArr[i] = fFFTArr[i / 2];
//		fFFTArr[i+1] = 0;
//	}
//	
//	for(i = 0; i < 2*unLength; i++)
//	{
//		fFFT_HandleArr[i] = fFFTArr[i];
//	}
	

//	gh3x2x_mp_signal_fft(fFFTArr, fFFT_HandleArr, unLength, 1);

//	for(i = 0; i < unLength; i++)
//		fFFTArr[i] = sqrt(fFFTArr[2*i] * fFFTArr[2*i] + fFFTArr[2*i + 1] * fFFTArr[2*i + 1]);
	
	gh3x2x_mp_signal_quick_fft(fFFTArr, fFFTArr+MAX_FFT_LENGTH, unLength, i-1, fFFT_HandleArr, fFFT_HandleArr+MAX_FFT_LENGTH, 0, 1);
	
	fFFT_HandleArr = NULL;
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_fft_cal_amp_freq(GF32 fRawdataArr[], GU16 usLength, GU32 unFFTLength, GU32 unSampleRate, GF32 *fAmp, GF32 *fFreq, \
														GF32 fRefFreq, GU8 uchWintype)
 *
 * @brief  cal amp and freq after fft
 *
 * @attention   None
 *
 * @param[in]   GF32 fRawdataArr[], GU16 usLength,  GU32 unFFTLength, GU32 unSampleRate, GU8 uchWintype, GF32 fRefFreq
				unSampleRate: Sampling frequency
				winType：Window type, 0: rectangular window; 1: flattop window, used to accurately estimate the amplitude; 2: hanning window
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fAmp, GF32 *fFreq
				fAmp: peak-to-peak value
				fFreq:freq, unit Hz
 *				
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_fft_cal_amp_freq(GF32 fRawdataArr[], GU16 usLength, GU32 unFFTLength, GU32 unSampleRate, GF32 *fAmp, GF32 *fFreq, \
														GF32 fRefFreq, GU8 uchWintype)
{
	GU8 ret = 1;
	GU32 i = 0;
	
	GD64 sum = 0;
	GD64 mean = 0;
	
	if(unFFTLength > MAX_FFT_LENGTH)
		unFFTLength = MAX_FFT_LENGTH;
	
	if(usLength > unFFTLength)
		usLength = unFFTLength;
	
	
	for(i = 0; i < usLength; i++)
		sum += fRawdataArr[i];
	mean = sum / usLength;
	
	// Subtract the mean value of the signal and add a window
	GD64 N = usLength - 1;
	GD64 win = 0;
	GD64 a0 = 0.21557895;
	GD64 a1 = 0.41663158;
	GD64 a2 = 0.277263158;
	GD64 a3 = 0.083578947;
	GD64 a4 = 0.006947368;
	
	if(uchWintype == 1)
	{
		for (i = 0; i < usLength; i++)
		{
			win = a0 - a1*cos(2 * PI*i / N) + a2*cos(4 * PI*i / N) - a3*cos(6 * PI*i / N) + a4*cos(8 * PI*i / N); //flattopwin
			fRawdataArr[i] = (fRawdataArr[i] - mean)*win;
		}
	}
	else if(uchWintype == 2)
	{
		for(i = 0; i < usLength; i++)
		{
			win = 0.5*(1-cos(2*PI*i/N)); //hanning
			fRawdataArr[i] = (fRawdataArr[i] - mean)*win;
		}

	}
	else
	{
		for(i = 0; i < usLength; i++)
		{
			fRawdataArr[i] = (fRawdataArr[i] - mean);
		}

	}

	
	gh3x2x_mp_signal_fft_process(fRawdataArr, unFFTLength);
	
	//find peak 
	GS32 start = 0;
	GS32 end = 0;
	
	if(fRefFreq > 0) //If the frequency is specified, look for it near the frequency
	{
		if((fRefFreq - 5) > (GF32)0.1)
			start = (GS32)((fRefFreq - 5)* unFFTLength / unSampleRate);
		else
			start = (GS32)(0.1*unFFTLength / unSampleRate);

		if((fRefFreq + 5) * 2 < unSampleRate)
			end = (GS32)((fRefFreq + 5) * unFFTLength / unSampleRate);
		else
			end = (GS32)(0.5*unFFTLength);
	}
	else //If the frequency is not specified, it will be found in the full frequency range.
	{
		start = (GS32)(0.1*unFFTLength / unSampleRate);
		end = (GS32)(0.5*unFFTLength);
	}
	
	GS32 peakPos = start;
	GD64 peak = fRawdataArr[start];
	for(i = start + 1; i <= end; i++)
	{
		if (peak < fRawdataArr[i])
		{
			peakPos = i;
			peak = fRawdataArr[i];
		}
	}
	
	//cal freq
	*fFreq =  (GF32)peakPos* unSampleRate / unFFTLength;
	
	//calculate signal amplitude through frequency domain
	if (uchWintype == 1)
		peak = peak / unFFTLength * 9.2792;	// Cal the amp, 9.2792 is amp correction coefficient of flattopwin
	else
		peak = peak / unFFTLength * 2;     //2 is rectangular window amplitude correction factor
	*fAmp = peak * 2;		// To calculate the peak-to-peak value must be multiplied by 2
	
	return ret;
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
