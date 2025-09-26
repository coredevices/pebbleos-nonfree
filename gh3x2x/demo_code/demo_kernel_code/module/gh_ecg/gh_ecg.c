/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_soft_lead.c
 *
 * @brief   gh3x2x lead process functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#include <stdio.h>
#include "gh_drv_version.h"
#include "gh_drv_common.h"
#include "gh_drv.h"
#include "gh_drv_interface.h"
#include "gh_drv_control.h"
#include "gh_uprotocol.h"
#include "gh_ecg.h"
#include "gh_demo_config.h"
#include "gh_multi_sen_pro.h"

#define GH3X2X_LEAD_OFF_SUPPORT_ECG (1)
#if GH3X2X_LEAD_OFF_SUPPORT_ECG
#define GH3X2X_LEAD_OFF_VERSION_DOUBLE         0
#define GH3X2X_LEAD_OFF_VERSION_FLOAT          1
#define GH3X2X_LEAD_OFF_VERSION_FIX_POINT      2

#define GH3X2X_LEAD_OFF_VERSION         GH3X2X_LEAD_OFF_VERSION_FIX_POINT
#endif

#if __SUPPORT_PPG_DOWNSAMP__
#define __SUPPORT_ECG_PPG_DOWNSAMP__  1
#else
#define __SUPPORT_ECG_PPG_DOWNSAMP__  0
#endif

#ifdef GOODIX_DEMO_PLATFORM
#define GH3X2X_DOWNSAMPLE_OPENALL     1
#else
#define GH3X2X_DOWNSAMPLE_OPENALL     0
#endif

#if __FUNC_TYPE_ECG_ENABLE__

GU8 g_uchEcgCascadeEn = 0;
GU8 g_uchEcgCascadeWriteRegVerifyEn = 0;
///0: not init  1: initialized
extern GU8 g_uchEcgCascadeSlaveInitSuccess;

//Ecg sample rate from hardware
static GU16 g_usEcgOutputFs = 0;

/// LEADOFF_DET_DIF_THR_MV*Gain*RAWMAX/ADCRef, if GH3100 VDDTx=3.3V,ADCRef=1800,if VDDTx=1.8,ADCRef=1600;
__attribute__((used)) static GS32 g_nLeadoffDiffThrdRawData = SOFT_LEAD_OFF_DIFF_THR_DEFAULT_VAL;

/// LEADOFF_DET_VAL_THR_MV*Gain*RAWMAX/ADCRef, if GH3100 VDDTx=3.3V,ADCRef=1800,if VDDTx=1.8,ADCRef=1600;
__attribute__((used)) static GS32 g_nLeadoffPeakThrdRawData = SOFT_LEAD_OFF_PEAK_THR_DEFAULT_VAL;

/// lead off base thr
__attribute__((used)) static GS32 g_nLeadoffBaseThrdRawData = SOFT_LEAD_OFF_BASE_THR_DEFAULT_VAL;

/// lead off detect moving buffer
GS32 g_nLeadOffDetEcgBuffer2Arr[LEADOFF_MOVING_NUM];

/// buffer to head
GS32 g_nBuf2Head = 0;

/// valid counter
GU32 g_unValidCounter = 0;

/// reset counter
GU32 g_unResetCounter = 0;

/// current sum
GS32 g_nCurEcgSum = 0;

/// lead off det init status
GU32 g_unLeadOffDetInitState2 = 0;

/// ecg gain map, amplify 10x
static const GU8 g_uchEcgGainMapArr[] =
{
    10, 25, 50, 100, 200
};

/// adc ref map, amplify 10x
static const GU8 g_uchAdcRefMapArr[] =
{
    16, 18
};

/// lead state in soft lead module
static GU8 g_uchLeadStateInSoftLeadModule = LEAD_STATE_LEAD_OFF;

/// down sample lead module(ECG PPG BT)
static GU8 g_uchDownSampleModule = DOWNSAMPLE_ECG;

/// ecg or pwtt function enable 
static GU8 g_uchEcgOrPwttFunctionEn = 0;

/// ecg sampling status
GU8 g_uchEcgSampleStatus = 0;
GU8 g_uchLastEcgSampleFlag = 0;    // 0: last time ecg is  not sampling  1:  last time ecg is sampling  //G202008031001 wanpeng
GU8 g_uchIsItyp0nAPeriod = 0;   // 0: current is not itype 0nA period  //1: current is itype 0nA period  //G202010311001 wanpeng
/// R type mode enable flag
GU8 g_uchRtypeModeEn = 1;

STEcgLeadCfg g_stEcgLeadCfg;

#endif

extern void GH3X2X_CalRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen);
extern GU8 GH3X2X_CheckRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen);

#if __FUNC_TYPE_ECG_ENABLE__

STAdtCfg g_stAdtModuleCfg;

/**
 * @fn     void GH3X2X_AdtPramInit(void)
 *
 * @brief  GH3X2X_AdtPramInit
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */

void GH3X2X_AdtPramInit(void)
{
    GH3X2X_Memset(((GU8 *)(&g_stAdtModuleCfg)),0, sizeof(g_stAdtModuleCfg));
}

/**
 * @fn     void GH3X2X_AdtPramSet(GU16 usRegVal, GU8 uchRegPosi)
 *
 * @brief  GH3X2X_AdtPramSet
 *
 * @attention   None
 *
 * @param[in]   usRegVal
 * @param[in]   uchRegPosi
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_AdtPramSet(GU16 usRegVal, GU8 uchRegPosi)
{
    GU8 uchByteOffset;
    GU8 uchByteVal;
    GU8 uchByteCnt;
    for(uchByteCnt = 0; uchByteCnt < 2; uchByteCnt ++)
    {
        uchByteVal = ((usRegVal >> (8*uchByteCnt))&0x00FF);
        uchByteOffset = (uchRegPosi * 2) + uchByteCnt;
        if(uchByteOffset < sizeof(g_stAdtModuleCfg))
        {
            GH3X2X_Memcpy(((GU8 *)(&g_stAdtModuleCfg)) + uchByteOffset, (GU8*)(&uchByteVal), 1);
        }
    }
}

void GH3X2X_WriteAdtDrvConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex;
    switch (usVirtualRegAddr)
    {
    case 0x0144:
      break;
    case 0x0146:
      break;
    default:
      usValIndex = (usVirtualRegAddr - GH3X2X_ADT_CONFIG0_ADDR) / GH3X2X_REG_ADDR_SIZE;
      GH3X2X_AdtPramSet(usVirtualRegValue, usValIndex);
      break;
    }
}

/**
 * @fn     GU8 GH3X2X_ElectrodeWearRevertDebugModeIsEnabled(void)
 *
 * @brief  Check if electrode wear revert debug mode is enabled
 *
 * @attention   In this mode,not need report wear event,need report the number of wear electrode revert.
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  1: enabled, 0:disabled
 */
GU8 GH3X2X_ElectrodeWearRevertDebugModeIsEnabled(void)
{
    return g_stAdtModuleCfg.stAdtCfgByte0.uchElectrodeWearRevCntEn;
}

#if ((defined(GH3X2X_LEAD_OFF_VERSION)) && (GH3X2X_LEAD_OFF_VERSION == GH3X2X_LEAD_OFF_VERSION_DOUBLE))
#elif ((defined(GH3X2X_LEAD_OFF_VERSION)) && (GH3X2X_LEAD_OFF_VERSION == GH3X2X_LEAD_OFF_VERSION_FIX_POINT))

#define ADCREF 1800   
#define ADC_OFFSET (1<<23)
#define PI 3.14159265
#define  ABS(x) ( (x)>0?(x):(-(x)) ) 

#define NEEDRESAMPLE 1
#if GH3X2X_DOWNSAMPLE_OPENALL
#define RESAMPLE_800HZ_TO_500HZ 1
#define RESAMPLE_800HZ_TO_250HZ 1
#define RESAMPLE_320HZ_TO_250HZ 1 
#define RESAMPLE_256HZ_TO_250HZ 1 
#else
#define RESAMPLE_800HZ_TO_500HZ 1
#define RESAMPLE_800HZ_TO_250HZ 0
#define RESAMPLE_320HZ_TO_250HZ 0 
#define RESAMPLE_256HZ_TO_250HZ 0 
#endif

#if __SUPPORT_ECG_PPG_DOWNSAMP__
#define RESAMPLE_400HZ_TO_250HZ 1 
#else
#define RESAMPLE_400HZ_TO_250HZ 0 
#endif

#define AMP_NUM 5  

#if  RESAMPLE_256HZ_TO_250HZ
#define IQ_NUM 48  
#else
#define IQ_NUM 48  
#endif

const int winCoef[IQ_NUM / 2] = { 0, 9580474, 38150933, 85201535, 149892663, 231069901, 327284639, 436819923, 557721094, 687830666, 824826833, 966264894, 1109620882, 1252336609, 1391865307, 1525717084, 1651503351, 1766979451, 1870084709, 1958979209, 2032076627, 2088072537, 2125967693, 2145085852};
#define winAmpCoef  4082 
GU32 g_nLeadOffDetIqAmpThr;//unit: uv
long long g_lLeadOffDetIqAmp;
long long  g_lLeadOffDetIqAmpThrRaw;
long long  g_lLeadOffDetIqAmpFrThrRaw;

GU32 g_nLeadOffDetIqAmpDiffThr;//unit: uv
long long g_lLeadOffDetIqAmpDiff;
long long g_lLeadOffDetIqAmpDiffThrRaw;
long long g_lLeadOffDetIqAmpDiffFrThrRaw;

int COND_VAL_VALID_TIME = 1; 
GU32 g_nLeadOffDetValThr;//unit: mv
int g_LeadOffDetValThrRaw;

#if NEEDRESAMPLE

#define SHIFTBITS 30

#if RESAMPLE_800HZ_TO_500HZ
    const int rsFirCoef_800_500hz[] = { 58200, 102435, 156130, 215460, 274735, 326585, 362380, 372790, 348645, 281870, 166550, 0, -216285, -475850, -766815, -1071825, -1368460, -1630170, -1827760, -1931325, -1912605,
                                -1747635, -1419445, -920670, -255820, 557070, 1485515, 2482735, 3488860, 4433375, 5238695, 5824850, 6114965, 6041350, 5551740, 4615280, 3227790, 1415785, -761100, -3209770, -5805610, -8396875, -10811545, -12866385,
                                -14377925, -15174745, -15110485, -14076620, -12014295, -8924190, -4873605, 0, 5489625, 11323885, 17176845, 22682405, 27452740, 31100030, 33260510, 33619540, 31936265, 28066360, 21981325, 13782945, 3711580, -7852615,
                                -20396185, -33286830, -45795550, -57125820, -66448630, -72941905, -75832375, -74437800, -68207360, -56757870, -39903755, -17678885, 9651270, 41588185, 77410100, 116192965, 156841955, 198132190, 238757000, 277381455, 312698655, 343486005, 368658785, 387318230, 398791740, 402663190 };
#endif
#if RESAMPLE_800HZ_TO_250HZ
    const int rsFirCoef_800_250hz[] = { -74905, -95385, -116975, -138740, -159540, -178080, -192900, -202440, -205060, -199120, -183040, -155365, -114830, -60455, 8370, 91845, 189655, 300920, 424150, 557225, 697345, 841065, 984315, 1122440, 1250290, 1362325,
                                1452720, 1515555, 1544935, 1535220, 1481220, 1378385, 1223050, 1012615, 745780, 422705, 45175, -383265, -857210, -1369370, -1910565, -2469770, -3034240, -3589685, -4120510, -4610090, -5041145, -5396125, -5657660, -5809045, -5834740, -5720895, -5455890,
                                -5030825, -4440010, -3681420, -2757070, -1673325, -441140, 923805, 2401100, 3965690, 5588100, 7234765, 8868540, 10449275, 11934545, 13280495, 14442770, 15377525, 16042520, 16398240, 16409055, 16044325, 15279560, 14097430, 12488725, 10453225, 8000385,
                                5149865, 1931900, -1612575, -5431990, -9464450, -13638290, -17872830, -22079425, -26162660, -30021820, -33552510, -36648435, -39203325, -41112955, -42277200, -42602135, -42002095, -40401670, -37737600, -33960495, -29036370, -22947970, -15695790,
                                -7298825, 2204995, 12758710, 24286615, 36694830, 49872305, 63692080, 78012930, 92681285, 107533410, 122397830, 137097950, 151454790, 165289835, 178427925, 190700125, 201946505, 212018845, 220783110, 228121755, 233935695, 238146055, 240695490, 241549195 };
#endif

#if RESAMPLE_400HZ_TO_250HZ
const int rsFirCoef_250hz2[] = {-20165,29190,117750,248970,420650,623445,839855,1044105,1203060,1278285,1229350,1018175,614220,0,-823490,-1832200,-2975865,-4177020,-5332555,-6318335,-6996880,-7227970,-6881615,-5852620,-4075600,-1539030,1703020,5523035,9717440,14010255,18063685,21495860,23905235,24900525,24134595,21339975,16363405,9196555,0,-10882185,-22919440,-35404830,-47478995,-58167535,-66430495,-71221570,-71553685,-66566985,-55594615,-38221750,-14333265,15853795,51776230,92531020,136902815,183411755,230379530,276010140,318480795,356037520,387089565,410296710,424643805,429497860,};
#endif
        
#if RESAMPLE_320HZ_TO_250HZ
    const int rsFirCoef_320_250hz[] = {79095,936340,341760,-2712470,-2547785,4817105,8099190,-5170865,-17933790,0,30974265,15517780,-42765395,-45670380,44706790,92106715,-24021395,-152053275,-36768905,217784250,166596045,-277901355,-451953470,320248390,1667310365,2147483647,};
#endif
#if RESAMPLE_256HZ_TO_250HZ
    const int rsFirCoef_256_250hz[]={194000,77770,-331070,-632290,-254280,769680,1439880,622640,-1454740,-2779600,-1303600,2436730,4847325,2461115,-3753755,-7872475,-4310105,5424840,12120690,7126740,-7444180,-17902495,-11265025,9777230,25594875,17187555,-12359260,-35689230,-25526795,15096890,48893725,37213545,-17872685,-66356185,-53763065,20552645,90184745,77974330,-22995885,-124828195,-115908245,25065740,181561700,183974820,-26641130,-299102515,-347934030,27626950,749679370,1463910805,1761638120,};
#endif

STGh3x2xDownSampleCalParamInfo pstECGDownSampCalParamInfo[1];
#if __SUPPORT_ECG_PPG_DOWNSAMP__
STGh3x2xDownSampleCalParamInfo pstPPGDownSampCalParamInfo[1];
#endif

void GH3X2X_DownSampleParamInit(void)
{
    GH3X2X_Memset(((GU8 *)(&pstECGDownSampCalParamInfo)),0, sizeof(pstECGDownSampCalParamInfo));
    #if __SUPPORT_ECG_PPG_DOWNSAMP__
    GH3X2X_Memset(((GU8 *)(&pstPPGDownSampCalParamInfo)),0, sizeof(pstPPGDownSampCalParamInfo));
    #endif
}

void GH3X2X_DownSampleMarcoIni(GU16 usEcgSampleRate)
{
    switch (g_usEcgOutputFs)
    {
    case 800:
        if( usEcgSampleRate == 500)
        {
            pstECGDownSampCalParamInfo->uchDownSampleType = DOWNSAMPLE_800HZ_TO_500HZ;
        }
        else
        {
            pstECGDownSampCalParamInfo->uchDownSampleType = DOWNSAMPLE_800HZ_TO_250HZ;
        }
        break;
    case 320:
        if( usEcgSampleRate == 250)
        {
            pstECGDownSampCalParamInfo->uchDownSampleType = DOWNSAMPLE_320HZ_TO_250HZ;
        }
        break;
    case 256:
        if( usEcgSampleRate == 250)
        {
            pstECGDownSampCalParamInfo->uchDownSampleType = DOWNSAMPLE_256HZ_TO_250HZ;
        }
        break;
    default: // do nothing
        break;
    }

    // ppg downsample ini
    #if __SUPPORT_ECG_PPG_DOWNSAMP__
        pstPPGDownSampCalParamInfo->uchDownSampleType = DOWNSAMPLE_400HZ_TO_250HZ;
    #endif
}

void RsFirResetSingle(GU8 uchChipIndex, GU8 uchMode)
{
    STGh3x2xRsInfo * pstRsInfo = NULL;
    GU8 uchChnNum = 0;
    #if __SUPPORT_ECG_PPG_DOWNSAMP__
    if(DOWNSAMPLE_PPG == uchMode)
    {
        pstRsInfo = g_pstGh3x2xPPGRsInfo;
        uchChnNum = g_uchPPGDownSampleChnlNumLimit;
    }
    #endif

    if(DOWNSAMPLE_ECG == uchMode)
    {
        pstRsInfo = g_pstGh3x2xEcgRsInfo;
        uchChnNum = uchChipIndex+1;
    }

    for( GU8 i=0; i<uchChnNum;i++)
    {
        pstRsInfo[i].snBufHead = -1;
        pstRsInfo[i].snRsFirState = 0;
        pstRsInfo[i].nRsIcounter = 0;
        pstRsInfo[i].nRrsDcounter = 0;
        pstRsInfo[i].uchFlagFrPrev = 1;
        pstRsInfo[i].nRsLastRaw = 0;
        pstRsInfo[i].fDownStep = 0;
        pstRsInfo[i].uchLinearInterState = 0;
        pstRsInfo[i].uchLinearDataUpdate = 0;
    }
}

void RsFirReset(GU8 uchMode)
{
    RsFirResetSingle(0,uchMode);
    if((g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchChnlNum > 1)&&(g_uchGh3x2xEcgChnlNumLimit > 1))
    {
        RsFirResetSingle(1,uchMode);
    }
}

void GH3X2X_ECGResampleConfig(GU16 usEcgSampleRate)
{
    RsFirReset(DOWNSAMPLE_ECG);
    GH3X2X_DownSampleMarcoIni(usEcgSampleRate);
    if(usEcgSampleRate == 500)
    {
        #if RESAMPLE_800HZ_TO_500HZ
        if(DOWNSAMPLE_800HZ_TO_500HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
        {
            pstECGDownSampCalParamInfo->rsFirCoef= (GS32*)rsFirCoef_800_500hz;
            pstECGDownSampCalParamInfo->uchRsFirOrder = 182;
            pstECGDownSampCalParamInfo->uchRsI = 5;
            pstECGDownSampCalParamInfo->uchRsD = 8;
            pstECGDownSampCalParamInfo->uchRsEnable = 1;
        }
        #endif
    }
    else if(usEcgSampleRate == 250)
    {
        #if RESAMPLE_800HZ_TO_250HZ
        if(DOWNSAMPLE_800HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
        {
            pstECGDownSampCalParamInfo->rsFirCoef= (GS32*)rsFirCoef_800_250hz;
            pstECGDownSampCalParamInfo->uchRsFirOrder = 250;
            pstECGDownSampCalParamInfo->uchRsI = 5;
            pstECGDownSampCalParamInfo->uchRsD = 16;
            pstECGDownSampCalParamInfo->uchRsEnable = 1;
        }
        #endif

        #if RESAMPLE_320HZ_TO_250HZ
        if(DOWNSAMPLE_320HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
        {
            pstECGDownSampCalParamInfo->rsFirCoef= (GS32*)rsFirCoef_320_250hz;
            pstECGDownSampCalParamInfo->uchRsFirOrder = 50;
            pstECGDownSampCalParamInfo->uchRsI = 1;
            pstECGDownSampCalParamInfo->uchRsD = 1;
            pstECGDownSampCalParamInfo->uchRsEnable = 1;
            pstECGDownSampCalParamInfo->ufStep = 128;
        }
        #endif

        #if RESAMPLE_256HZ_TO_250HZ
        if(DOWNSAMPLE_256HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
        {
            pstECGDownSampCalParamInfo->rsFirCoef= (GS32*)rsFirCoef_256_250hz;
            pstECGDownSampCalParamInfo->uchRsFirOrder = 100;
            pstECGDownSampCalParamInfo->uchRsI = 1;
            pstECGDownSampCalParamInfo->uchRsD = 1;
            pstECGDownSampCalParamInfo->uchRsEnable = 1;
            pstECGDownSampCalParamInfo->ufStep = 1024;
        }
        #endif
    }
    else
    {
        pstECGDownSampCalParamInfo->uchRsEnable = 0;
        return;
    }

    if(pstECGDownSampCalParamInfo->uchRsFirOrder % 2)
    {
        pstECGDownSampCalParamInfo->uchRsFirCoefNum = ((pstECGDownSampCalParamInfo->uchRsFirOrder+1)/2) ;
    }
    else
    {
        pstECGDownSampCalParamInfo->uchRsFirCoefNum = ((pstECGDownSampCalParamInfo->uchRsFirOrder+2)/2);
    }

    if(pstECGDownSampCalParamInfo->uchRsI)
    {
        pstECGDownSampCalParamInfo->uchRsBufSize = (pstECGDownSampCalParamInfo->uchRsFirOrder / pstECGDownSampCalParamInfo->uchRsI + 1);
    }
    else
    {
        pstECGDownSampCalParamInfo->uchRsBufSize = 1;
    }
}
#if __SUPPORT_ECG_PPG_DOWNSAMP__
void GH3X2X_PPGResampleConfig(GU16 usPPGSampleRate)
{
    RsFirReset(DOWNSAMPLE_PPG);
    GH3X2X_DownSampleMarcoIni(usPPGSampleRate);
    if(usPPGSampleRate == 250)
    {
        #if RESAMPLE_400HZ_TO_250HZ
        pstPPGDownSampCalParamInfo->rsFirCoef = (GS32*)rsFirCoef_250hz2;
        pstPPGDownSampCalParamInfo->uchRsFirOrder = 126;
        pstPPGDownSampCalParamInfo->uchRsI = 5;
        pstPPGDownSampCalParamInfo->uchRsD = 8;
        pstPPGDownSampCalParamInfo->uchRsEnable = 1;
        #else
        pstPPGDownSampCalParamInfo->uchRsEnable = 0;
        #endif
    }
    else
    {
        pstPPGDownSampCalParamInfo->uchRsEnable = 0;
        return;
    }

    if(pstPPGDownSampCalParamInfo->uchRsFirOrder % 2)
    {
        pstPPGDownSampCalParamInfo->uchRsFirCoefNum = ((pstPPGDownSampCalParamInfo->uchRsFirOrder+1)/2) ;
    }
    else
    {
        pstPPGDownSampCalParamInfo->uchRsFirCoefNum = ((pstPPGDownSampCalParamInfo->uchRsFirOrder+2)/2);
    }

    if(pstPPGDownSampCalParamInfo->uchRsI)
    {
        pstPPGDownSampCalParamInfo->uchRsBufSize = (pstPPGDownSampCalParamInfo->uchRsFirOrder / pstPPGDownSampCalParamInfo->uchRsI + 1);
    }
    else
    {
        pstPPGDownSampCalParamInfo->uchRsBufSize = 1;
    }
}
#else
void GH3X2X_PPGResampleConfig(GU16 usPPGSampleRate)
{
    return;
}
#endif

void GH3X2X_LeadOffDetectResetSingleChip(GU8 uchChipIndex)
{
    int i;
    STGh3x2xSoftLeadOffDetInfo * pstSoftLeadOffDetInfo = g_pstGh3x2xSoftLeadOffDetInfo + uchChipIndex;
    pstSoftLeadOffDetInfo -> snCounter = 0;
    pstSoftLeadOffDetInfo -> sniqCounter = 0;
    pstSoftLeadOffDetInfo -> snniVal = 0;
    pstSoftLeadOffDetInfo -> snnqVal = 0;
    pstSoftLeadOffDetInfo -> uchFastRecoryHistory = 0;
    for (i = 0; i < AMP_NUM; i++)
        pstSoftLeadOffDetInfo -> snniqAmp[i] = 0;
}

void GH3X2X_LeadOffDetectReset(void)
{
    GH3X2X_LeadOffDetectResetSingleChip(0);
    if((g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchChnlNum > 1)&&(g_uchGh3x2xEcgChnlNumLimit > 1))
    {
        GH3X2X_LeadOffDetectResetSingleChip(1);
    }
}

GS32 GH3X2X_LeadOffDetectInit(void)
{
    GH3X2X_LeadOffDetectReset();
    return 0;
}

/**
 * @fn     GU8 GH3X2X_LeadOffDetect(GU8* puchFifoBuffer, GU16 usFifoBufferLen)
 *
 * @brief  Lead off Detection, suppose the increment of  normal ECG value in LEADOFF_DET_POINT_NUM+1 samples point 
 *         not exceed   LEADOFF_DET_THR_mV
 *
 * @attention   None
 *
 * @param[in]   puchFifoBuffer           pointer to read fifo data buffer
 * @param[in]   usFifoBufferLen          read fifo buffer len
 * @param[out]  None
 *
 * @return  lead off status output 
 * @retval  #0          not detect lead off
 * @retval  bit0=1      AdcValue larger than thresh
 * @retval  bit1=1      IQ Value larger than thresh
 */

GU8 GH3X2X_LeadOffDetectSingleChip(GU8* puchFifoBuffer, GU16* pusFifoBufferLen, GU8 unChipIndex)
{
    GU16 usByteIndexTmp = 0;
    GS32 j = 0;
    GU8 uchRet = 0;
    GU32 fifoVal = 0;
    GS32 adcVal = 0;
    GU8  flagFastRecovery = 0;
    GS32 absAdcVal = 0; //ABS of adcVal
    GS64 temp = 0;

    STGh3x2xSoftLeadOffDetInfo * pstSoftLeadOffDetInfo = g_pstGh3x2xSoftLeadOffDetInfo + unChipIndex;

    for (usByteIndexTmp = 0; usByteIndexTmp < *pusFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)
    {
        if (GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[unChipIndex] & GH3X2X_SLOT_ADC_NUM_BITS))
        {
            fifoVal = GH3X2X_MAKEUP_DWORD(puchFifoBuffer[usByteIndexTmp], puchFifoBuffer[usByteIndexTmp + 1],puchFifoBuffer[usByteIndexTmp + 2], puchFifoBuffer[usByteIndexTmp + 3]);
            adcVal = (GS32)((fifoVal & 0x00FFFFFF)) - ADC_OFFSET; 
            flagFastRecovery = (fifoVal >> 26) & 0x01;
            if(flagFastRecovery)
            {
                pstSoftLeadOffDetInfo->uchFastRecoryHistory = 1;
                GH3X2X_DEBUG_LOG_PARAM("has fast recovery!!!!\r\n");
            }
            if(0 == pstSoftLeadOffDetInfo->uchFastRecoryHistory)
            {
                GH3X2X_DEBUG_LOG_PARAM("Warning: No fast recovery!!!!\r\n");
                continue;
            }
            if (pstSoftLeadOffDetInfo-> uchFlagFrPrev != flagFastRecovery)
            {
                GH3X2X_LeadOffDetectReset();
                pstSoftLeadOffDetInfo->uchFastRecoryHistory = 1;
            }
            pstSoftLeadOffDetInfo-> uchFlagFrPrev = flagFastRecovery;

            //sample counter
            if (pstSoftLeadOffDetInfo -> snCounter < 20000)
                pstSoftLeadOffDetInfo -> snCounter++;
            
            if (flagFastRecovery == 0 && pstSoftLeadOffDetInfo -> snCounter>COND_VAL_VALID_TIME)
            {
                absAdcVal = ABS(adcVal);
                if (absAdcVal > g_LeadOffDetValThrRaw)
                    uchRet |= 0x01;
            }
            if (pstSoftLeadOffDetInfo -> sniqCounter < IQ_NUM / 2)
            {
                temp = (GS64)adcVal*winCoef[pstSoftLeadOffDetInfo -> sniqCounter];
            }
            else
            {
                temp = (GS64)adcVal*winCoef[IQ_NUM-1 - pstSoftLeadOffDetInfo -> sniqCounter];
            }
            switch (pstSoftLeadOffDetInfo -> sniqCounter%4)
            {
            case 0:
                pstSoftLeadOffDetInfo -> snniVal += temp;
                break;
            case 1:
                pstSoftLeadOffDetInfo -> snnqVal += temp;
                break;
            case 2:
                pstSoftLeadOffDetInfo -> snniVal -= temp;
                break;
            case 3:
                pstSoftLeadOffDetInfo -> snnqVal -= temp;
                break;
            default:
                break;
            }
            if (pstSoftLeadOffDetInfo -> sniqCounter == (IQ_NUM - 1))
            {
                for (j = AMP_NUM-1; j >0; j--)
                {
                    pstSoftLeadOffDetInfo -> snniqAmp[j] = pstSoftLeadOffDetInfo -> snniqAmp[j - 1];
                }
                pstSoftLeadOffDetInfo -> snniVal = pstSoftLeadOffDetInfo -> snniVal >> 31;
                pstSoftLeadOffDetInfo -> snnqVal = pstSoftLeadOffDetInfo -> snnqVal >> 31;
                pstSoftLeadOffDetInfo -> snniqAmp[0] = pstSoftLeadOffDetInfo -> snniVal*(int)pstSoftLeadOffDetInfo -> snniVal + pstSoftLeadOffDetInfo -> snnqVal*(int)pstSoftLeadOffDetInfo -> snnqVal;
                pstSoftLeadOffDetInfo -> snniVal = 0;
                pstSoftLeadOffDetInfo -> snnqVal = 0;
    
                if (flagFastRecovery == 0)
                {
                    if (pstSoftLeadOffDetInfo -> snniqAmp[0] > g_lLeadOffDetIqAmpThrRaw && pstSoftLeadOffDetInfo -> snniqAmp[1] > g_lLeadOffDetIqAmpThrRaw)
                        uchRet |= 0x02;

                    if ((pstSoftLeadOffDetInfo -> snniqAmp[0] - pstSoftLeadOffDetInfo -> snniqAmp[2]) > g_lLeadOffDetIqAmpDiffThrRaw && (pstSoftLeadOffDetInfo -> snniqAmp[1] - pstSoftLeadOffDetInfo -> snniqAmp[2]) > g_lLeadOffDetIqAmpDiffThrRaw)
                    {
                        if(pstSoftLeadOffDetInfo ->snCounter > 2 * IQ_NUM)
                        {
                            uchRet |= 0x04;
                        }
                    }
                }
                else
                {
                    if (pstSoftLeadOffDetInfo -> snniqAmp[0] > g_lLeadOffDetIqAmpFrThrRaw && pstSoftLeadOffDetInfo -> snniqAmp[1] > g_lLeadOffDetIqAmpFrThrRaw)
                        uchRet |= 0x08;
                }
            }
            pstSoftLeadOffDetInfo -> sniqCounter++;
            pstSoftLeadOffDetInfo -> sniqCounter = pstSoftLeadOffDetInfo -> sniqCounter%(IQ_NUM);
        } // end of if (GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == g_uchEcgLeadChannelMap)
    } // end of for (usByteIndexTmp = 0; usByteIndexTmp < usFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)

    return uchRet;
}

#endif//end #elif((defined(GH3X2X_LEAD_OFF_VERSION)) && (GH3X2X_LEAD_OFF_VERSION == GH3X2X_LEAD_OFF_VERSION_FIX_POINT))

GU8 GH3X2X_GetSoftLeadDetMode(void)
{
    return g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchSoftLeadDetMode;
}

/**
 * @fn     void GH3X2X_SoftLeadOffEventProManalMode(void)
 *
 * @brief  soft lead off evt process @ manal mode
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SoftLeadOffEventProManalMode(void)
{
    ECG_LEAD_DET_EN_CONFIG(0);  
    ECG_LEAD_DET_EN_CONFIG(1);
    GH3X2X_CascadeEcgSlaverLeadDectDisEx();
}

void GH3X2X_ConfigLeadOffThr(GU32 uIQAmpThr, GU32 uIQAmpDiffThr,GU32 uValThr)
{
    g_nLeadOffDetIqAmpThr = uIQAmpThr;
    g_lLeadOffDetIqAmp = ((long long)g_nLeadOffDetIqAmpThr * 20 * ADC_OFFSET / ADCREF/1000*IQ_NUM*1000/winAmpCoef);
    g_lLeadOffDetIqAmpThrRaw = (g_lLeadOffDetIqAmp*g_lLeadOffDetIqAmp);
    g_lLeadOffDetIqAmpFrThrRaw = (g_lLeadOffDetIqAmp*g_lLeadOffDetIqAmp / 20 / 20); 

    g_nLeadOffDetIqAmpDiffThr = uIQAmpDiffThr;
    g_lLeadOffDetIqAmpDiff = ((long long)g_nLeadOffDetIqAmpDiffThr * 20 * ADC_OFFSET / ADCREF/1000*IQ_NUM*1000/winAmpCoef);
    g_lLeadOffDetIqAmpDiffThrRaw = (g_lLeadOffDetIqAmpDiff*g_lLeadOffDetIqAmpDiff);
    g_lLeadOffDetIqAmpDiffFrThrRaw = (g_lLeadOffDetIqAmpDiff*g_lLeadOffDetIqAmpDiff / 20 / 20); 

    g_nLeadOffDetValThr = uValThr;
    g_LeadOffDetValThrRaw = (g_nLeadOffDetValThr * (20 * ADC_OFFSET / ADCREF)); 
}

void GH3x2xSetEcgOutputFs(GU16 usEcgOutputFs)
{
    g_usEcgOutputFs = usEcgOutputFs;
}

/**
 * @fn     void GH3X2X_SoftLeadOffEventPro(void)
 *
 * @brief  soft lead off evt process
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SoftLeadOffEventPro(void)
{
    if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchDisItypeWhenEcgSample)
    {

    }
    else if (GET_LEAD_ON_AUTO_SWITCH_VAL())
    {
        GH3X2X_SlotEnableConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchSlotBit, GH3X2X_SET_SLOT_DISABLE); // disable ecg or pwtt slot
        ECG_LEAD_DET_EN_CONFIG(0); // disable lead detect
        ECG_LEAD_DET_EN_CONFIG(1);// enable lead detect
        GH3X2X_SlotEnableConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchSlotBit, GH3X2X_SET_SLOT_ENABLE); // enable ecg or pwtt slot
        GH3X2X_CascadeEcgSlaverLeadDectDisEx();
    }
}

/**
 * @fn     GU16 GH3X2X_SoftLeadOffDetPro(GU16 usCurrentIrqStatus, GU8 *puchFifoBuffer, GU16* pusFifoBufferLen)
 *
 * @brief  soft lead detect process
 *
 * @attention   None
 *
 * @param[in]   usCurrentIrqStatus     current irq status
 * @param[in]   puchFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoBufferLen        read fifo buffer len
 * @param[out]  None
 *
 * @return  irq status ouput after handle
 */
GU16 GH3X2X_SoftLeadOffDetPro(GU16 usCurrentIrqStatus, GU8 *puchFifoBuffer, GU16* pusFifoBufferLen)
{
    if ((SOFT_LEAD_OFF_DET_MODE_DISABLE == g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchSoftLeadDetMode) || (0 == g_uchEcgOrPwttFunctionEn))
    {
        //return usCurrentIrqStatus;
    }
    else
    {
        if (0 != (usCurrentIrqStatus & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT))  //lead on event
        {
            #if GH3X2X_LEAD_OFF_SUPPORT_ECG
            if(g_usEcgOutputFs == 800 || g_usEcgOutputFs == 320 || g_usEcgOutputFs == 256)
            {
                GH3X2X_LeadOffDetectInit();
            }
            #endif
        }

        if (g_uchRtypeModeEn)
        {
            return usCurrentIrqStatus;
        }

        if (SOFT_LEAD_OFF_DET_MODE_SW_AND_HW == g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchSoftLeadDetMode)
        {
        }
        else
        {
            if (0 != (usCurrentIrqStatus & GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT))  //lead off event
            {
                if (SOFT_LEAD_OFF_DET_MODE_SW_ONLY == g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchSoftLeadDetMode)  // SW only
                {
                    GH3X2X_VAL_CLEAR_BIT(usCurrentIrqStatus, (GU16)GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT); // don't need lead off evt
                }
            }
            if ((*pusFifoBufferLen) && (LEAD_STATE_LEAD_ON == g_uchLeadStateInSoftLeadModule))
            {
                GU8 uchDetResult = 0;
                if(g_usEcgOutputFs == 800 || g_usEcgOutputFs == 320 || g_usEcgOutputFs == 256)
                {
                    uchDetResult = GH3X2X_LeadOffDetect(puchFifoBuffer, pusFifoBufferLen);
                }
                if (uchDetResult)
                {
                    GH3X2X_DEBUG_LOG_PARAM("soft lead off, detect result = %d !\r\n", uchDetResult);
                    GH3X2X_VAL_SET_BIT(usCurrentIrqStatus, GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT);
                    GH3X2X_SoftLeadOffEventPro();
                }
            }
        }

        if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchWearOffLogic)  // wear off logic is ecg only
        {
            if((1 == g_uchLastEcgSampleFlag)&&(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchDisItypeWhenEcgSample))  //is ecg samping  and itype is disable
            {        
                usCurrentIrqStatus &= (~(GH3X2X_IRQ_MSK_WEAR_OFF_BIT));   //mask wear off event
            }
        }
    }

    return usCurrentIrqStatus;
}
/**
 * @fn     STSoftLeadResult GH3X2X_LeadHandle(GU16 usIrqStatus, GU8 *puchReadFifoBuffer, GU16 usFifoBufferLen)
 *
 * @brief  ecg soft lead handle
 *
 * @attention   None
 *
 * @param[in]   usIrqStatus               irq status
 * @param[in]   puchReadFifoBuffer         pointer to read fifo data buffer
 * @param[in]   usFifoBufferLen           read fifo buffer len
 * @param[out]  None
 *
 * @return  soft lead handle process, @ref STSoftLeadResult
 */
STSoftLeadResult GH3X2X_LeadHandle(GU16 usIrqStatus, GU8 *puchReadFifoBuffer, GU16* pusFifoBufferLen)
{
    STSoftLeadResult stLeadResult = {0, 0};

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT))  //got lead on event
    {
        GH3X2X_DEBUG_LOG_PARAM("handle lead on \r\n");
        GH3X2X_EcgSampleHookHandle(ECG_SAMPLE_EVENT_TYPE_LEAD, ECG_SAMPLE_EVENT_INFO_LEAD_ON);
    }

    if (0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT))  //got lead off event
    {
        GH3X2X_DEBUG_LOG_PARAM("handle lead off \r\n");
        GH3X2X_EcgSampleHookHandle(ECG_SAMPLE_EVENT_TYPE_LEAD, ECG_SAMPLE_EVENT_INFO_LEAD_OFF);
    }

    if (0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT))
    {
        g_uchLeadStateInSoftLeadModule = LEAD_STATE_LEAD_ON;
    }

    usIrqStatus = GH3X2X_SoftLeadOffDetPro(usIrqStatus, puchReadFifoBuffer, pusFifoBufferLen);

    if (0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT))
    {
        g_uchLeadStateInSoftLeadModule = LEAD_STATE_LEAD_OFF;
        if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchAutoCtrlMode)
        {
            stLeadResult.uchNeedForceReadFifo = 1;
        }
    }

    stLeadResult.usEvent = usIrqStatus;
    return stLeadResult;
}

GU8 GH3X2X_LeadOffDetect(GU8* puchFifoBuffer, GU16* pusFifoBufferLen)
{
    GU8 uchRet = 0;
    if(0 == g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG] || GH3X2X_CheckRawdataBuf(puchFifoBuffer, *pusFifoBufferLen))
    {
        return uchRet;
    }

    uchRet |= GH3X2X_LeadOffDetectSingleChip(puchFifoBuffer, pusFifoBufferLen, 0);

    if((g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchChnlNum > 1)&&(g_uchGh3x2xEcgChnlNumLimit > 1))
    {
        uchRet |= GH3X2X_LeadOffDetectSingleChip(puchFifoBuffer, pusFifoBufferLen, 1);
    }

    return uchRet;
}

/**
 * @fn     void GH3X2X_SlaverSoftLeadPramInit(void)
 *
 * @brief  slave soft lead off param init to default value
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SlaverSoftLeadPramInit(void)
{
    g_uchEcgOrPwttFunctionEn = 0;
    GH3X2X_Memset((GU8 *)(&g_stEcgLeadCfg), 0, sizeof(g_stEcgLeadCfg));
    g_uchEcgSampleStatus = 0;
    g_uchLastEcgSampleFlag = 0;
    g_uchRtypeModeEn = 1;
    g_uchIsItyp0nAPeriod = 0;
}

/**
 * @fn     void GH3X2X_SlaverSoftLeadPramSet(GU16 usRegVal, GU8 uchRegPosi)
 *
 * @brief  slave soft lead off param set from virtual reg
 *
 * @attention   None
 *
 * @param[in]   usRegVal          reg value
 * @param[in]   uchRegPosi        reg position
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SlaverSoftLeadPramSet(GU16 usRegVal, GU8 uchRegPosi)
{    
    GU8 uchByteOffset;
    GU8 uchByteVal;
    GU8 uchByteCnt;
    for(uchByteCnt = 0; uchByteCnt < 2; uchByteCnt ++)
    {
        uchByteVal = ((usRegVal >> (8*uchByteCnt))&0x00FF);
        uchByteOffset = (uchRegPosi * 2) + uchByteCnt;
        if(uchByteOffset < sizeof(g_stEcgLeadCfg))
        {
            GH3X2X_Memcpy(((GU8 *)(&g_stEcgLeadCfg)) + uchByteOffset, (GU8*)(&uchByteVal), 1);
            if(uchByteOffset == (GU8)((GU8*)(&(g_stEcgLeadCfg.uchLeadOffBaseVolt))- (GU8*)(&(g_stEcgLeadCfg))))
            {
                g_nLeadoffPeakThrdRawData = (GS32)((GS64)g_stEcgLeadCfg.uchLeadOffPeakVolt * (((GS64)g_uchEcgGainMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchEcgGainGrade] \
                                * GH3X2X_RAWDATA_REAL_OFFSET) \
                                / (GS64)(((GU16)g_uchAdcRefMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchAdcRefGrade]) * (GU16)SOFT_LEAD_ADC_REF_AMP_NUM)));
                g_nLeadoffDiffThrdRawData = (GS32)((GS64)g_stEcgLeadCfg.uchLeadOffDiffVolt * (((GS64)g_uchEcgGainMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchEcgGainGrade] \
                                * GH3X2X_RAWDATA_REAL_OFFSET) \
                                / (GS64)(((GU16)g_uchAdcRefMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchAdcRefGrade]) * (GU16)SOFT_LEAD_ADC_REF_AMP_NUM)));
                g_nLeadoffBaseThrdRawData = (GS32)((GS64)g_stEcgLeadCfg.uchLeadOffBaseVolt * (((GS64)g_uchEcgGainMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchEcgGainGrade] \
                                * GH3X2X_RAWDATA_REAL_OFFSET) \
                                / (GS64)(((GU16)g_uchAdcRefMapArr[g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchAdcRefGrade]) * (GU16)SOFT_LEAD_ADC_REF_AMP_NUM)));
                
            }
            g_uchEcgOrPwttFunctionEn = 1;
        }
    }
}

/**
 * @fn     void GH3X2X_EcgSampleHookHandle(GU8 uchEventType, GU8 uchEventInfo)
 *
 * @brief  ecg sample hook handle
 *
 * @attention   None
 *
 * @param[in]   uchEventType         ecg sample event type, @ref ECG_SAMPLE_EVENT_TYPE_SAMPLE etc
 * @param[in]   uchEventInfo         ecg sample event info, @ref ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP etc
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgSampleHookHandle(GU8 uchEventType, GU8 uchEventInfo)
{
    GU8 uchHandleCondition;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);

    if (0 == g_uchEcgOrPwttFunctionEn) // check flag
    {
        return;
    }

    if (ECG_SAMPLE_EVENT_TYPE_SAMPLE == uchEventType)  // if evt type equal sample config
    {
        if (ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP == uchEventInfo)
        {
            GH3X2X_VAL_CLEAR_BIT(g_uchEcgSampleStatus, (GU8)ECG_SAMPLE_EVENT_TYPE_SAMPLE); // clear sample bit
        }
        else
        {
            GH3X2X_VAL_SET_BIT(g_uchEcgSampleStatus, ECG_SAMPLE_EVENT_TYPE_SAMPLE); // set sample bit
        }
    }
    else if (ECG_SAMPLE_EVENT_TYPE_SLOT == uchEventType) // if evt type equal slot config
    {
        if ((uchEventInfo & g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchSlotBit) != 0)  // if slot enable bit equal
        {
            GH3X2X_VAL_SET_BIT(g_uchEcgSampleStatus, ECG_SAMPLE_EVENT_TYPE_SLOT);
        }
        else
        {
            GH3X2X_VAL_CLEAR_BIT(g_uchEcgSampleStatus, (GU8)ECG_SAMPLE_EVENT_TYPE_SLOT);
        }
    }
    else if (ECG_SAMPLE_EVENT_TYPE_LEAD == uchEventType)
    {
        if (ECG_SAMPLE_EVENT_INFO_LEAD_OFF == uchEventInfo)
        {
            GH3X2X_VAL_CLEAR_BIT(g_uchEcgSampleStatus, (GU8)ECG_SAMPLE_EVENT_TYPE_LEAD);
        }
        else
        {
            GH3X2X_VAL_SET_BIT(g_uchEcgSampleStatus, ECG_SAMPLE_EVENT_TYPE_LEAD);
        }
    }

    if ((0 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchAutoCtrlMode) || (2 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchAutoCtrlMode))  //disable or software
    {
        uchHandleCondition = (ECG_SAMPLE_EVENT_TYPE_SAMPLE | ECG_SAMPLE_EVENT_TYPE_SLOT);
    }
    else
    {
        uchHandleCondition = (ECG_SAMPLE_EVENT_TYPE_SAMPLE | ECG_SAMPLE_EVENT_TYPE_SLOT | ECG_SAMPLE_EVENT_TYPE_LEAD);
    }

    if (uchHandleCondition == (uchHandleCondition & g_uchEcgSampleStatus)) //condition is match
    {
        if(0 == g_uchLastEcgSampleFlag)
        {
            if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchForceRecover)
            {
                ECG_FAST_RECV_EN_CONFIG(1);
                GH3X2X_CascadeEcgSlaverFastRecTrigEx();
                GH3X2X_DEBUG_LOG_PARAM("Fast recover !!! last event is %d\r\n", uchEventType);
            }
            if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchDisItypeWhenEcgSample)
            {
                g_uchIsItyp0nAPeriod = 1;
                ECG_LEAD_DET_EN_CONFIG(0); // disable lead detect
                GH3X2X_CascadeEcgSlaverLeadDectDisEx();
            }
            g_uchRtypeModeEn = 0;
            g_uchLastEcgSampleFlag = 1;
        }
    }
    else
    {
        if(1 == g_uchLastEcgSampleFlag)  //ecg sample will be from sampling to stop sample
        {
            if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchDisItypeWhenEcgSample)
            {
                if((g_uchEcgSampleStatus&ECG_SAMPLE_EVENT_TYPE_SAMPLE))// if sample is opening
                {
                    ECG_LEAD_DET_EN_CONFIG(1);  // enable lead detect
                    GH3X2X_CascadeEcgSlaverLeadDectDisEx();
                }
                if((1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag2.uchWearOffLogic))
                {
                    GH3X2X_SetSoftEvent(GH3X2X_SOFT_EVENT_WEAR_OFF);
                }    
            }
            else
            {
                if(GET_ECG_LEAD_DET_EN_VAL())   //if detcet module is enable , reset detecet module
                {
                    ECG_LEAD_DET_EN_CONFIG(0);
                    ECG_LEAD_DET_EN_CONFIG(1);
                    GH3X2X_CascadeEcgSlaverLeadDectDisEx();
                }
            }
            g_uchIsItyp0nAPeriod = 0;
        }
        g_uchLastEcgSampleFlag = 0;
        g_uchRtypeModeEn = 1;
    }
}

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
void GH3X2X_LeadDetEnControl(GU8 uchEventInfo)
{
    if(1 == g_stEcgLeadCfg.uchEcgLeadCtrlFlag1.uchNeedHostEnableLeadDet)
    {
        if (ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP == uchEventInfo)
        {
            ECG_LEAD_DET_EN_CONFIG(0);// disable lead detect
        }
        else
        {
            ECG_LEAD_DET_EN_CONFIG(1);// enable lead detect
        }
        GH3X2X_CascadeEcgSlaverLeadDectDisEx();
    }
}

void GH3X2X_LeadDetEnInHardAdt(GU8 uchEn)
{
    if(g_stAdtModuleCfg.stAdtCfgByte0.uchElectrodeAdtEn)
    {
        if(1 == uchEn)
        {
            if(0 == g_uchIsItyp0nAPeriod)
            {
                ECG_LEAD_DET_EN_CONFIG(1);// enable lead detect
            }
        }
        else
        {
            ECG_LEAD_DET_EN_CONFIG(0);// disable lead detect
        }
    }
    GH3X2X_CascadeEcgSlaverLeadDectDisEx();
}

/**
 * @fn     void GH3X2X_EcgSampleHookHandleAfterSlotCtrl(GU16 usSlotEnableConfig, GU8 uchSlotEnable)
 *
 * @brief  ecg sample hook handle after ecg slot ctrl
 *
 * @attention   None
 *
 * @param[in]   usSlotEnableConfig         slot enable index , @ref GH3X2X_SLOT_INDEX_0 ... GH3X2X_SLOT_INDEX_ALL
 * @param[in]   uchSlotEnable              enable flag, #0 slot disable, #others slot enable
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_EcgSampleHookHandleAfterSlotCtrl(GU16 usSlotEnableConfig, GU8 uchSlotEnable)
{
    GU8 uchSlotEnableBitsTmp = (GU8)usSlotEnableConfig;
    if (uchSlotEnable == 0)
    {
        uchSlotEnableBitsTmp = ~uchSlotEnableBitsTmp; // if disable
    }
    GH3X2X_EcgSampleHookHandle(ECG_SAMPLE_EVENT_TYPE_SLOT, uchSlotEnableBitsTmp);
}

/**
 * @fn       GS8 GH3X2X_CalibrateECGGain(GU8 *puchReadFifoBuffer)
 *
 * @brief  Calibrate ECG Gain
 *
 * @attention    None
 *
 * @param[in]        puchReadFifoBuffer        pointer read data buffer
 * @param[in]        usFifoBufferLen         fifo byte length 
 * @return    error code
 * @retval    #GH3X2X_RET_OK                        return algorithm run successfully, but output hasn't refreshed
 * @retval    #GH3X2X_RET_GENERIC_ERROR            return algorithm generic error
 */
GS8 GH3X2X_CalibrateECGGain(GU8 *puchReadFifoBuffer, GU16 usFifoBufferLen)
{
    GU16 usByteIndexTmp = 0;
    GS8  chGainCode = 0;
    GU32 unRawdataVal = 0;
    GS64 nRawdatalVal = 0;
    GS64 nRawdataRealVal = 0;
    GS8 chRet = GH3X2X_RET_OK;
    chGainCode = (GS8)GH3X2X_GET_LOW_BYTE_FROM_WORD(GH3X2X_ReadReg(GH3X2X_EFUSE_CTRL_EFUSE0_AUTOLOAD_2_ADDR));
    if (GH3X2X_CHECK_BIT_SET(g_unFuncStartedBitmap, GH3X2X_FUNCTION_ECG))
    {
        if(GH3X2X_CheckRawdataBuf(puchReadFifoBuffer, usFifoBufferLen))
        {
            return chRet;
        }
    
        for(usByteIndexTmp = 0; usByteIndexTmp < usFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)
        {
            if(GH3X2X_CHANNEL_MAP_GET(puchReadFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[0] & GH3X2X_SLOT_ADC_NUM_BITS))
            {
                unRawdataVal = GH3X2X_MAKEUP_DWORD(puchReadFifoBuffer[usByteIndexTmp], puchReadFifoBuffer[usByteIndexTmp + 1],
                                              puchReadFifoBuffer[usByteIndexTmp + 2], puchReadFifoBuffer[usByteIndexTmp + 3]);

                nRawdatalVal = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(unRawdataVal);

                nRawdataRealVal = ((GS64)nRawdatalVal - GH3X2X_RAWDATA_REAL_OFFSET) * 1024/((GS64)chGainCode + 1024) + GH3X2X_RAWDATA_REAL_OFFSET;

                puchReadFifoBuffer[usByteIndexTmp + 1] = GH3X2X_GET_BYTE2_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 2] = GH3X2X_GET_BYTE1_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 3] = GH3X2X_GET_BYTE0_FROM_DWORD(nRawdataRealVal);
            }
        }
        GH3X2X_CalRawdataBuf(puchReadFifoBuffer, usFifoBufferLen);
    }
    else
    {
         chRet = GH3X2X_RET_GENERIC_ERROR;
         GH3X2X_DEBUG_LOG_PARAM("ecg hasn't started error!\r\n");
    }
    return chRet;
}

void GH3X2X_DownSampleCal( GU8 *puchFifoBuffer, GU16 usByteIndexTmp, GU16 *usRawdataBufIdx, STGh3x2xRsInfo * pstRsInfo ,STGh3x2xDownSampleCalParamInfo * pstDownSampCalParamInfo)
{
    GU32 unfifoVal = 0;
    GS32 usadcVal = 0;
    GU8  chNo = 0;
    GU8  uchflagFastRecovery = 0;
    GU32 unFirCoefIndex = 0;
    unfifoVal = GH3X2X_MAKEUP_DWORD(puchFifoBuffer[usByteIndexTmp], puchFifoBuffer[usByteIndexTmp + 1], puchFifoBuffer[usByteIndexTmp + 2], puchFifoBuffer[usByteIndexTmp + 3]);
    chNo = (unfifoVal >> 27); 
    usadcVal = (GS32)((unfifoVal & 0x00FFFFFF)) - ADC_OFFSET; 

    if(g_uchDownSampleModule == DOWNSAMPLE_ECG)
    {
        uchflagFastRecovery = (unfifoVal >> 26) & 0x01;
        if (pstRsInfo ->uchFlagFrPrev != uchflagFastRecovery)
        {
            GH3X2X_ECGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->usSampleRate);
        }
        pstRsInfo ->uchFlagFrPrev = uchflagFastRecovery;
    }

    #if __SUPPORT_ECG_PPG_DOWNSAMP__
    else if(g_uchDownSampleModule == DOWNSAMPLE_PPG)
    {
        uchflagFastRecovery = (unfifoVal >> 26) & 0x01;
        if (pstRsInfo ->uchFlagFrPrev != 1)
        {
            if(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstFunctionInfo->usSampleRate)
            {
                GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstFunctionInfo->usSampleRate);
            }
            else if(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2]->pstFunctionInfo->usSampleRate)
            {
                GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2]->pstFunctionInfo->usSampleRate);
            }
        }
        pstRsInfo ->uchFlagFrPrev = 1;
    }
    #endif

    // ECG downsample
    #if RESAMPLE_320HZ_TO_250HZ || RESAMPLE_256HZ_TO_250HZ
    if((DOWNSAMPLE_320HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType) || (DOWNSAMPLE_256HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType))
    {
        GS32 snLinerAdcVal;
        if(pstDownSampCalParamInfo->uchRsEnable)
        {
            GS32 snRsXbufIndex;
            GS64 snFirValueSum;
            GS32 snFirValue;

            //滤波
            if (pstRsInfo ->snRsFirState > 0)
            {
                pstRsInfo ->snBufHead = pstRsInfo ->snBufHead - 1;
                pstRsInfo ->snBufHead = pstRsInfo ->snBufHead < 0 ? (pstDownSampCalParamInfo->uchRsBufSize - 1) : pstRsInfo ->snBufHead;
                pstRsInfo ->snRsXbuf[pstRsInfo ->snBufHead] = usadcVal;
            }
            else
            {
                for (GU8 i = 0; i < pstDownSampCalParamInfo->uchRsFirOrder / pstDownSampCalParamInfo->uchRsI + 1; i++)
                        pstRsInfo ->snRsXbuf[i] = usadcVal;
                pstRsInfo ->snBufHead = pstDownSampCalParamInfo->uchRsBufSize - 1;
            }
                              
            snRsXbufIndex = pstRsInfo ->snBufHead;
            snFirValueSum = 0;
            snFirValue = 0;
            for (unFirCoefIndex = 0; unFirCoefIndex < pstDownSampCalParamInfo->uchRsFirCoefNum; unFirCoefIndex = unFirCoefIndex + 1) //
            {
                snFirValueSum += ((GS64)pstRsInfo ->snRsXbuf[snRsXbufIndex] * pstDownSampCalParamInfo->rsFirCoef[unFirCoefIndex]);
                snRsXbufIndex = (snRsXbufIndex + 1) % pstDownSampCalParamInfo->uchRsBufSize;
            }
            for (; unFirCoefIndex <(pstDownSampCalParamInfo->uchRsFirOrder+1); unFirCoefIndex = unFirCoefIndex + 1) //
            {
                snFirValueSum += ((GS64)pstRsInfo ->snRsXbuf[snRsXbufIndex] * pstDownSampCalParamInfo->rsFirCoef[pstDownSampCalParamInfo->uchRsFirOrder - unFirCoefIndex]);
                snRsXbufIndex = (snRsXbufIndex + 1) % pstDownSampCalParamInfo->uchRsBufSize;
            }

            snFirValue = (GS32)(snFirValueSum / 5 >> SHIFTBITS);
            snFirValue = snFirValue + ADC_OFFSET;
            if (snFirValue < 0)
                snFirValue = 0;
            else if (snFirValue>=(ADC_OFFSET * 2))
                snFirValue = (ADC_OFFSET * 2-1);

            // 线性插值降采样
            if(pstRsInfo->uchLinearInterState >0)
            {
                #if RESAMPLE_320HZ_TO_250HZ
                if(DOWNSAMPLE_320HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
                {
                    if(pstRsInfo->fDownStep <=100)
                    {
                        snLinerAdcVal = ((GU32)snFirValue*pstRsInfo->fDownStep + (100-pstRsInfo->fDownStep)*pstRsInfo->nRsLastRaw)/100;
                        pstRsInfo->nRsLastRaw = (GU32)snFirValue;
                        pstRsInfo->fDownStep = pstRsInfo->fDownStep + pstDownSampCalParamInfo->ufStep - 100;
                        pstRsInfo->uchLinearDataUpdate = 1;
                    }
                    else
                    {
                        pstRsInfo->nRsLastRaw = (GU32)snFirValue;
                        pstRsInfo->fDownStep = pstRsInfo->fDownStep - 100;
                        pstRsInfo->uchLinearDataUpdate = 0;
                    }
                }
                #endif

                #if RESAMPLE_256HZ_TO_250HZ
                if(DOWNSAMPLE_256HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)
                {
                    if(pstRsInfo->fDownStep <=1000)
                    {
                        snLinerAdcVal = (GU32)(((GS64)(snFirValue)*(GS64)(pstRsInfo->fDownStep) + (GS64)((1000-pstRsInfo->fDownStep)*(GS64)(pstRsInfo->nRsLastRaw)))/1000);
                        pstRsInfo->nRsLastRaw = (GU32)snFirValue;
                        pstRsInfo->fDownStep = pstRsInfo->fDownStep + pstDownSampCalParamInfo->ufStep - 1000;
                        pstRsInfo->uchLinearDataUpdate = 1;
                    }
                    else
                    {
                        pstRsInfo->nRsLastRaw = (GU32)snFirValue;
                        pstRsInfo->fDownStep = pstRsInfo->fDownStep - 1000;
                        pstRsInfo->uchLinearDataUpdate = 0;
                    }
                }
                #endif
            }
            else
            {
                snLinerAdcVal = (GU32)snFirValue;
                pstRsInfo->nRsLastRaw = (GU32)snFirValue;
                pstRsInfo->fDownStep = pstRsInfo->fDownStep + pstDownSampCalParamInfo->ufStep;
                pstRsInfo->uchLinearInterState =1;
                pstRsInfo->uchLinearDataUpdate = 1;
            }

            if(pstRsInfo->uchLinearDataUpdate == 1)
            {
                unfifoVal = (((GU8)chNo)<<27) + (((GU8)uchflagFastRecovery)<<26) + ((GU32)snLinerAdcVal);
                puchFifoBuffer[*usRawdataBufIdx]     = GH3X2X_GET_BYTE3_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 1] = GH3X2X_GET_BYTE2_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 2] = GH3X2X_GET_BYTE1_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 3] = GH3X2X_GET_BYTE0_FROM_DWORD(unfifoVal);

                *usRawdataBufIdx += GH3X2X_FIFO_RAWDATA_SIZE;
            }
        }
    }
    #endif

    #if  RESAMPLE_800HZ_TO_500HZ ||RESAMPLE_800HZ_TO_250HZ || (RESAMPLE_400HZ_TO_250HZ & (__SUPPORT_ECG_PPG_DOWNSAMP__))
    if((DOWNSAMPLE_800HZ_TO_500HZ == pstECGDownSampCalParamInfo->uchDownSampleType)||(DOWNSAMPLE_800HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType)||(DOWNSAMPLE_400HZ_TO_250HZ == pstECGDownSampCalParamInfo->uchDownSampleType))
    {
        if(pstDownSampCalParamInfo->uchRsEnable)
        {
            GS32 snRsXbufIndex;
            GS64 snFirValueSum;
            GS32 snFirValue;
            GS32 sndelta;
            if (pstRsInfo ->snRsFirState > 0)
            {
                pstRsInfo ->snBufHead = pstRsInfo ->snBufHead - 1;
                pstRsInfo ->snBufHead = pstRsInfo ->snBufHead < 0 ? (pstDownSampCalParamInfo->uchRsBufSize - 1) : pstRsInfo ->snBufHead;
                pstRsInfo ->snRsXbuf[pstRsInfo ->snBufHead] = usadcVal;
                pstRsInfo ->nRsIcounter += pstDownSampCalParamInfo->uchRsI;
            }
            else
            {
                for (GU8 i = 0; i < pstDownSampCalParamInfo->uchRsFirOrder / pstDownSampCalParamInfo->uchRsI + 1; i++)
                        pstRsInfo ->snRsXbuf[i] = usadcVal;
                pstRsInfo ->nRsIcounter = 0;
                pstRsInfo ->snRsFirState = 1;
                pstRsInfo ->snBufHead = pstDownSampCalParamInfo->uchRsBufSize - 1;
            }

            if (pstRsInfo ->nRrsDcounter >= pstRsInfo ->nRsIcounter && pstRsInfo ->nRrsDcounter < (pstRsInfo ->nRsIcounter + pstDownSampCalParamInfo->uchRsI))
            {
                sndelta = pstRsInfo ->nRrsDcounter - pstRsInfo ->nRsIcounter;
                snRsXbufIndex = pstRsInfo ->snBufHead;
                snFirValue = 0;
                snFirValueSum = 0;
                for (unFirCoefIndex = sndelta; unFirCoefIndex < pstDownSampCalParamInfo->uchRsFirCoefNum; unFirCoefIndex = unFirCoefIndex + pstDownSampCalParamInfo->uchRsI)
                {
                    snFirValueSum += ((GS64)pstRsInfo ->snRsXbuf[snRsXbufIndex] * pstDownSampCalParamInfo->rsFirCoef[unFirCoefIndex]);
                    snRsXbufIndex = (snRsXbufIndex + 1) % pstDownSampCalParamInfo->uchRsBufSize;
                }
                for (; unFirCoefIndex <(pstDownSampCalParamInfo->uchRsFirOrder+1); unFirCoefIndex = unFirCoefIndex + pstDownSampCalParamInfo->uchRsI)
                {
                    snFirValueSum += ((GS64)pstRsInfo ->snRsXbuf[snRsXbufIndex] * pstDownSampCalParamInfo->rsFirCoef[pstDownSampCalParamInfo->uchRsFirOrder - unFirCoefIndex]);
                    snRsXbufIndex = (snRsXbufIndex + 1) % pstDownSampCalParamInfo->uchRsBufSize;
                }
                pstRsInfo ->nRrsDcounter += pstDownSampCalParamInfo->uchRsD;

                snFirValue = (GS32)(snFirValueSum >> SHIFTBITS);
                snFirValue = snFirValue + ADC_OFFSET;
                if (snFirValue < 0)
                        snFirValue = 0;
                else if (snFirValue>=(ADC_OFFSET * 2))
                        snFirValue = (ADC_OFFSET * 2-1); 
                unfifoVal = (((GU8)chNo)<<27) + (((GU8)uchflagFastRecovery)<<26) + ((GU32)snFirValue);
                puchFifoBuffer[*usRawdataBufIdx]     = GH3X2X_GET_BYTE3_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 1] = GH3X2X_GET_BYTE2_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 2] = GH3X2X_GET_BYTE1_FROM_DWORD(unfifoVal);
                puchFifoBuffer[*usRawdataBufIdx + 3] = GH3X2X_GET_BYTE0_FROM_DWORD(unfifoVal);
                *usRawdataBufIdx += GH3X2X_FIFO_RAWDATA_SIZE;
            }
        }
    }
#endif
}
#endif

void GH3X2X_EcgPPGRsHandleSingleChip(GU8 *puchFifoBuffer, GU16 *pusFifoBufferLen, GU8 uchChipIndex)
{
    GU16 usByteIndexTmp = 0;
    GU16 usRawdataBufIdx = 0;
    GU8  usCheckDataType = 0; // 1:ECG 2:PPG 0:other

    for (usByteIndexTmp = 0; usByteIndexTmp < *pusFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)
    { 
        if(GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[uchChipIndex] & GH3X2X_SLOT_ADC_NUM_BITS))
        {
            STGh3x2xRsInfo * pstRsInfo_ECG = g_pstGh3x2xEcgRsInfo + uchChipIndex;
            g_uchDownSampleModule = DOWNSAMPLE_ECG;
            GH3X2X_DownSampleCal( puchFifoBuffer, usByteIndexTmp, &usRawdataBufIdx, pstRsInfo_ECG,pstECGDownSampCalParamInfo);
            usCheckDataType =1;
        }

        #if __SUPPORT_ECG_PPG_DOWNSAMP__
        if(g_uchPPGDownSampleChnlNumLimit && pstPPGDownSampCalParamInfo->uchRsEnable==1)
        {
            for(GU8 uchPPGSlotNumTmp = 0; uchPPGSlotNumTmp < g_uchPPGDownSampleChnlNumLimit; uchPPGSlotNumTmp++)
            {
                if(GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pchChnlMap[uchPPGSlotNumTmp] & GH3X2X_SLOT_ADC_NUM_BITS)
//                             || GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2]->pchChnlMap[uchPPGSlotNumTmp] & GH3X2X_SLOT_ADC_NUM_BITS)
//                                || GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_GREEN]->pchChnlMap[uchPPGSlotNumTmp] & GH3X2X_SLOT_ADC_NUM_BITS)
//                                || GH3X2X_CHANNEL_MAP_GET(puchFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_IR]->pchChnlMap[uchPPGSlotNumTmp] & GH3X2X_SLOT_ADC_NUM_BITS)
                )
                {
                    STGh3x2xRsInfo * pstRsInfo_PPG = g_pstGh3x2xPPGRsInfo + uchPPGSlotNumTmp;
                    g_uchDownSampleModule = DOWNSAMPLE_PPG;
                    GH3X2X_DownSampleCal(puchFifoBuffer, usByteIndexTmp, &usRawdataBufIdx, pstRsInfo_PPG,pstPPGDownSampCalParamInfo);
                    usCheckDataType =2;
                }
            }
        }
        #endif
        if(!usCheckDataType)
        {
            if(pstECGDownSampCalParamInfo->uchRsEnable)
            {
                //this rawdata is not ECG data
                puchFifoBuffer[usRawdataBufIdx]     = puchFifoBuffer[usByteIndexTmp];
                puchFifoBuffer[usRawdataBufIdx + 1] = puchFifoBuffer[usByteIndexTmp + 1];
                puchFifoBuffer[usRawdataBufIdx + 2] = puchFifoBuffer[usByteIndexTmp + 2];
                puchFifoBuffer[usRawdataBufIdx + 3] = puchFifoBuffer[usByteIndexTmp + 3];
                usRawdataBufIdx += GH3X2X_FIFO_RAWDATA_SIZE;
            }
        }
        usCheckDataType = 0;
    } // end of for (usByteIndexTmp = 0; usByteIndexTmp < usFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)
    *pusFifoBufferLen = usRawdataBufIdx;
}

GU8 GH3X2X_EcgRsHandle(GU8* puchFifoBuffer, GU16* pusFifoBufferLen)
{
    if(pstECGDownSampCalParamInfo->uchRsEnable == 1)
    {
        GH3X2X_EcgPPGRsHandleSingleChip(puchFifoBuffer, pusFifoBufferLen, 0);
        GH3X2X_CalRawdataBuf(puchFifoBuffer, *pusFifoBufferLen);
        return 1;
    }
    else
    {
        return 0;
    }
}

#endif

//---------------------cascade-----------------------------

#if !__GH3X2X_CASCADE_EN__
GU8 GH3X2X_CascadeGetEcgEnFlag(void){return 0;}
void GH3X2X_CascadeEcgSlaverLeadDectDisEx(void){}
void GH3X2X_CascadeEcgSlaverFastRecTrigEx(void){}
#else
GU8 guchChip1EcgLeadFlag = 0;   //0:lead off 1:lead on
GU8 guchChip2EcgLeadFlag = 0;   //0:lead off 1:lead on

GS8 GH3X2X_CascadeCalibrateECGGain(GU8 *puchReadFifoBuffer, GU16 usFifoBufferLen)
{
    GU16 usByteIndexTmp = 0;
    GS8  chGainCodeMaster = 0;
    GS8  chGainCodeSlaver = 0;
    GU32 unRawdataVal = 0;
    GS64 nRawdatalVal = 0;
    GS64 nRawdataRealVal = 0;
    GS8 chRet = GH3X2X_RET_OK;

    chGainCodeMaster = (GS8)GH3X2X_GET_LOW_BYTE_FROM_WORD(GH3X2X_ReadReg(GH3X2X_EFUSE_CTRL_EFUSE0_AUTOLOAD_2_ADDR));
    GH3X2X_CascadeOperationSlaverChip();
    chGainCodeSlaver = (GS8)GH3X2X_GET_LOW_BYTE_FROM_WORD(GH3X2X_ReadReg(GH3X2X_EFUSE_CTRL_EFUSE0_AUTOLOAD_2_ADDR));
    GH3X2X_CascadeOperationMasterChip();
    
    //if (GH3X2X_CHECK_BIT_SET(GH3X2X_GET_FUNC_START_BITMAP, GH3X2X_STARTED_BITMAP_ECG))
    if (GH3X2X_CHECK_BIT_SET(g_unFuncStartedBitmap, GH3X2X_FUNCTION_ECG))
    {
        if(GH3X2X_CheckRawdataBuf(puchReadFifoBuffer, usFifoBufferLen))
        {
            return chRet;
        }

        for(usByteIndexTmp = 0; usByteIndexTmp < usFifoBufferLen; usByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE)
        {
            //if(GH3X2X_CHANNEL_MAP_GET(puchReadFifoBuffer[usByteIndexTmp]) == g_uchEcgChannelMap)
            if(GH3X2X_CHANNEL_MAP_GET(puchReadFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[0]& GH3X2X_SLOT_ADC_NUM_BITS))
            
            {
                unRawdataVal = GH3X2X_MAKEUP_DWORD(puchReadFifoBuffer[usByteIndexTmp], puchReadFifoBuffer[usByteIndexTmp + 1],
                                              puchReadFifoBuffer[usByteIndexTmp + 2], puchReadFifoBuffer[usByteIndexTmp + 3]);

                nRawdatalVal = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(unRawdataVal);

                nRawdataRealVal = ((GS64)nRawdatalVal - GH3X2X_RAWDATA_REAL_OFFSET) * 1024/((GS64)chGainCodeMaster + 1024) + GH3X2X_RAWDATA_REAL_OFFSET;

                puchReadFifoBuffer[usByteIndexTmp + 1] = GH3X2X_GET_BYTE2_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 2] = GH3X2X_GET_BYTE1_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 3] = GH3X2X_GET_BYTE0_FROM_DWORD(nRawdataRealVal);
                
            }
            else  if(GH3X2X_CHANNEL_MAP_GET(puchReadFifoBuffer[usByteIndexTmp]) == (g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[1]& GH3X2X_SLOT_ADC_NUM_BITS))
            {
                unRawdataVal = GH3X2X_MAKEUP_DWORD(puchReadFifoBuffer[usByteIndexTmp], puchReadFifoBuffer[usByteIndexTmp + 1],
                                              puchReadFifoBuffer[usByteIndexTmp + 2], puchReadFifoBuffer[usByteIndexTmp + 3]);

                nRawdatalVal = GH3X2X_RAWDATA_CLEAR_ALL_FLAG(unRawdataVal);

                nRawdataRealVal = ((GS64)nRawdatalVal - GH3X2X_RAWDATA_REAL_OFFSET) * 1024/((GS64)chGainCodeSlaver + 1024) + GH3X2X_RAWDATA_REAL_OFFSET;

                puchReadFifoBuffer[usByteIndexTmp + 1] = GH3X2X_GET_BYTE2_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 2] = GH3X2X_GET_BYTE1_FROM_DWORD(nRawdataRealVal);
                puchReadFifoBuffer[usByteIndexTmp + 3] = GH3X2X_GET_BYTE0_FROM_DWORD(nRawdataRealVal);
            }
        }
        GH3X2X_CalRawdataBuf(puchReadFifoBuffer, usFifoBufferLen);
    }
    else
    {
         chRet = GH3X2X_RET_GENERIC_ERROR;
         GH3X2X_DEBUG_LOG_PARAM("ecg hasn't started error!\r\n");
    }
    return chRet;
}

void GH3X2X_CascadeEcgSlaverLeadDectDis(void)
{
    GH3X2X_DEBUG_LOG_PARAM("Chip222 Lead Dec Stop\r\n");
    GH3X2X_CascadeOperationSlaverChip();
    ECG_LEAD_DET_EN_CONFIG(0);
    GH3X2X_CascadeOperationMasterChip();
}

void GH3X2X_CascadeEcgSlaverLeadDectEn(void)
{
    //ECG_LEAD_DET_EN_CONFIG(1);
    GH3X2X_DEBUG_LOG_PARAM("Chip222 Lead Dec Start\r\n");
    GH3X2X_CascadeOperationSlaverChip();
    ECG_LEAD_DET_EN_CONFIG(1);
    GH3X2X_CascadeOperationMasterChip();
}

void GH3X2X_CascadeSetChip1LeadSta(GU8 uchLeadOnStatus)
{
    guchChip1EcgLeadFlag = uchLeadOnStatus;
}

GU8 GH3X2X_CascadeGetEcgMasterLeadStatus(GU8 *puchEvent)
{
    GU8 uchLastLeadFlag = guchChip1EcgLeadFlag;
    guchChip1EcgLeadFlag =  GH3X2X_ReadReg(0x50c) & 0x01;
    if(uchLastLeadFlag != guchChip1EcgLeadFlag)
    {
        if(guchChip1EcgLeadFlag)
        {
            (*puchEvent) |= GH3X2X_EVENT_EX_BIT_CHIP0_LEAD_ON;
        }
        else
        {
            (*puchEvent) |= GH3X2X_EVENT_EX_BIT_CHIP0_LEAD_OFF;
        }
    }
    return guchChip1EcgLeadFlag;
}

void GH3X2X_CascadeSetChip2LeadSta(GU8 uchLeadOnStatus)
{
    guchChip2EcgLeadFlag = uchLeadOnStatus;
}

GU8 GH3X2X_CascadeGetEcgSlaverLeadStatus(GU8 *puchEvent)
{
     GU8 uchLastLeadFlag = guchChip2EcgLeadFlag;
     GH3X2X_CascadeOperationSlaverChip();
     guchChip2EcgLeadFlag =  GH3X2X_ReadReg(0x50c) & 0x01;
     if(uchLastLeadFlag != guchChip2EcgLeadFlag)
     {
       if(guchChip2EcgLeadFlag)
       {
           (*puchEvent) |= GH3X2X_EVENT_EX_BIT_CHIP1_LEAD_ON;
       }
       else
       {
           (*puchEvent) |= GH3X2X_EVENT_EX_BIT_CHIP1_LEAD_OFF;
       }
     }
    GH3X2X_CascadeOperationMasterChip();

    return guchChip2EcgLeadFlag;
}

GU16 GH3X2X_CascadeLeadOnEventPreDeal(GU16 usIrqStatus,GU16 usIrqStatusChip2,GU8 *puchEventEx)
{
    GH3X2X_DEBUG_LOG_PARAM("GH3X2X_CascadeLeadOnEventPreDeal!!!!\r\n");
    GU8 uchMasterLeadStatus;  //0: lead off   1: lead on
    GU8 uchSlaveLeadStatus;  //0: lead off   1: lead on

    if (0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT))  //chip1 got lead on event
    {
        GH3X2X_CascadeEcgSlaverLeadDectEn();
        GH3X2X_DEBUG_LOG_PARAM("chip1 lead on!!!!\r\n");
    }

    if (0 != (usIrqStatusChip2 & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT))  //chip2 got lead on event
    {
        GH3X2X_DEBUG_LOG_PARAM("chip2 lead on!!!!\r\n");
    }

    uchMasterLeadStatus = GH3X2X_CascadeGetEcgMasterLeadStatus(puchEventEx);
    uchSlaveLeadStatus = GH3X2X_CascadeGetEcgSlaverLeadStatus(puchEventEx);
    
    if ((0 != (usIrqStatus & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT)) || (0 != (usIrqStatusChip2 & GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT)))
    {
        if ( uchMasterLeadStatus & uchSlaveLeadStatus)
        {
            usIrqStatus |= GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT;
            GH3X2X_DEBUG_LOG_PARAM("2ecg Lead On\r\n");
        }
        else
        {
            usIrqStatus &= ~GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT;
        }
    }
    //GH3X2X_CascadeLeadEventReport();

    return usIrqStatus;
}

void GH3X2X_CascadeLeadOffResultDeal(GU8 uchLeadoffRes)
{
    if(uchLeadoffRes == 0)
    {
        return;
    }

    if( 0 != (uchLeadoffRes & 0x0f))  //chip1 lead off
    {
        GH3X2X_DEBUG_LOG_PARAM("chip1 Leadoff!!!!\r\n");
    }

    if( 0 != (uchLeadoffRes & 0xf0)) //chip2 lead off
    {
        GH3X2X_DEBUG_LOG_PARAM("Chip2 Leadoff!!!!\r\n");
    }

    if (0 != uchLeadoffRes) //As long as there is one chip lead off; restart
    {
        GH3X2X_CascadeEcgSlaverLeadDectDis();
        GH3X2X_DEBUG_LOG_PARAM("Leadoff uchLeadoffRes:0x%02x!!!!\r\n", uchLeadoffRes);
    }
    //report lead status
    //GH3X2X_CascadeLeadEventReport();
}

void GH3X2X_CascadeSetEcgEnFlag(GU8 uchCascadeEnFlag)
{
    g_uchEcgCascadeEn = uchCascadeEnFlag;
    GH3X2X_LeadOffDetectInit();
}

// 1: init success  0: init fail
void GH3X2X_CascadeSetEcgSlaveInitSuccess(GU8 uchCascadeInitSuccess)
{
    g_uchEcgCascadeSlaveInitSuccess = uchCascadeInitSuccess;
}


GU8 GH3X2X_CascadeGetEcgSlaveInitSuccess(void)
{
    return g_uchEcgCascadeSlaveInitSuccess;
}

GU8 GH3X2X_CascadeGetEcgEnFlag(void)
{
    return g_uchEcgCascadeEn;
}

void GH3X2X_CascadeEcgSlaverLeadDectDisEx(void)
{
    if (GH3X2X_CascadeGetEcgEnFlag())
    {
        GH3X2X_CascadeEcgSlaverLeadDectDis();
    }
}

void GH3X2X_CascadeEcgSlaverFastRecTrigEx(void)
{
    if (GH3X2X_CascadeGetEcgEnFlag())
    {
        GH3X2X_CascadeOperationSlaverChip();
        ECG_FAST_RECV_EN_CONFIG(1);
        GH3X2X_CascadeOperationSlaverChip();
    }
}

#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

