/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_agc_Ipd.c
 *
 * @brief   gh3x2x led agc process functions
 *
 * @version see gh3x2x_drv_version.h
 *
 */
#include <stdio.h>
#include "gh_agc.h"
#include "gh_drv_dump.h"
#include "gh_drv_common.h"
#include "gh_demo_config.h"

/*
 * DEFINES
 *****************************************************************************************
 */
#if __SUPPORT_SOFT_AGC_CONFIG__ 

/*
 * DEFINES
 *****************************************************************************************
 */
#define GH3X2X_SOFT_AGC_VERSION "SoftAgc_V4.3.0.0"
#define SUB_CHN_PRO_EN     1
#define MAX_PPGCFG_DRVCUR_NUM     (8)  // ppgcfg0-8 need store drv current

/*
 * LOCAL VARIABLE DEFINITIONS
 *****************************************************************************************
 */
#if __SUPPORT_SOFT_AGC_CONFIG__
//main chnl
static STGh3x2xAgcMainChnlInfo  g_pstGh3x2xAgcMainChnlInfoEntity[GH3X2X_AGC_SLOT_NUM_LIMIT];
static STGh3x2xAgcMainChnlInfo * const g_pstGh3x2xAgcMainChnlInfo = g_pstGh3x2xAgcMainChnlInfoEntity;
const GU8 g_uchNewAgcSlotNumLimit = GH3X2X_AGC_SLOT_NUM_LIMIT;
static STGh3x2xAgcMainChnKeyInfo g_pstGh3x2xAgcMainChnlKeyInfoEntity[GH3X2X_AGC_SLOT_NUM_LIMIT];
static STGh3x2xAgcMainChnKeyInfo * const g_pstGh3x2xAgcMainChnlKeyInfo = g_pstGh3x2xAgcMainChnlKeyInfoEntity;

static STGh3x2xNewAgcMainChnlIdaelAdjInfo g_pstGh3x2xAgcMainChnlIdaelAdjInfoEntity[GH3X2X_AGC_SLOT_NUM_LIMIT];
static STGh3x2xNewAgcMainChnlIdaelAdjInfo * const g_pstGh3x2xAgcMainChnlIdaelAdjInfo = g_pstGh3x2xAgcMainChnlIdaelAdjInfoEntity;

//sub chnl
#if GH3X2X_AGC_SUB_CHNL_NUM_LIMIT > 0
static STGh3x2xAgcSubChnlRxInfo    g_pstGh3x2xAgcSubChnlRxInfoEntity[GH3X2X_AGC_SUB_CHNL_NUM_LIMIT];
static STGh3x2xAgcSubChnlRxInfo  * const g_pstGh3x2xAgcSubChnlRxInfo = g_pstGh3x2xAgcSubChnlRxInfoEntity;
#else
STGh3x2xNewAgcSubChnlSlotInfo  * const g_pstGh3x2xNewAgcSubChnlSlotInfo = 0;
STGh3x2xAgcSubChnlRxInfo  * const g_pstGh3x2xAgcSubChnlRxInfo = 0;
#endif
const GU8 g_uchAgcSubChnlNumLimit = GH3X2X_AGC_SUB_CHNL_NUM_LIMIT;
#else
STGh3x2xAgcMainChnlInfo * const g_pstGh3x2xAgcMainChnlInfo = 0;
STGh3x2xNewAgcSubChnlSlotInfo * const g_pstGh3x2xNewAgcSubChnlSlotInfo = 0;
STGh3x2xAgcSubChnlRxInfo * const g_pstGh3x2xAgcSubChnlRxInfo = 0;
const GU8 g_uchNewAgcSlotNumLimit = 0;
const GU8 g_uchAgcSubChnlNumLimit = 0;
#endif

//GU8 g_puchDrvCurrentAfterSoftAgc[MAX_PPGCFG_DRVCUR_NUM] = {0}; // 8 PPGCFG numble
GU8 g_NewAgcMainChelFlag;

/*
 * STRUCT DEFINE
 *****************************************************************************************
 */
static const GU16 g_usGH3x2xTiaGainR[] = 
{
    10,
    25,
    50,
    75,
    100,
    250,
    500,
    750,
    1000,
    1250,
    1500,
    1750,
    2000,
};

// soft agc parameter
static STSoftAgcCfg g_stSoftAgcCfg =
{
    0x00,   /**< soft agc enable */
    0,        /**< soft bg cancel adjust enable */
    {0,0,0},       /**< amb slot ctrl */
    0,
    0,
    0,
    {7,6,5,4},   /**< soft gain limit */
    3000,
    1000,
    25000,
    15000,
};

/*
 * LOCAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
static void GH3X2X_StoreRxTiaGainAfterAgc(GU8 uchGain, GU8 uchSlotNo, GU8 uchAdcNo, GU8* puchGainRecord);
static void GH3X2X_StoreDrvCurrentAfterAgc(GU8 DrvCurrent, GU8 uchSlotNo, GU8 uchDrvIndex, GU16* puchDrvCurrentRecord);
static void Gh3x2xNewAgcSubChnlModuleReset(void);
static void GH3x2xAgcModuleReset(void);
//static void GH3x2xAgcRecoveryAgcRegToMainChnl(GU8 uchSlotIdx, STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo);
static GU8 GH3x2xAgcFindSubChnlSlotInfo(GU8 uchSlot);
static GU8 GH3x2xNewAgcSetNewSubChnl(GU8 uchSlotIdx, GU8 uchSubRxEn, STAgcRegConfig* pstAgcRegConfig);
static GU8 GH3x2xGetDrvCurrent(GU8 uchSlotIdx ,GU8 uchAgcEn);
static void GH3x2xAgcChnlInfoInit(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo, STAgcRegConfig* pstAgcRegConfig);
static GU32 GH32x2xMedSel(GU32 unData0, GU32 unData1, GU32 unData2);
static void GH3X2X_AgcGetThreshold(GU8 uchSpo2ChnlIndex, GU32* punTemp_H_Thd, GU32* punTemp_L_Thd);
static void GH3X2X_AgcGetExtremum(GU16 usAnalysisDiscardLen, GU16 usAnalysisCnt, GU32 unDataRaw, GU32* punMaxRawdata, GU32* punMinRawdata);
static void GH3X2X_AgcSetSatFlag(GU32 unTemp_H_Thd, GU32 unDataRaw, GU16* pusSat_CheckCnt, GU16* pusSat_cnt, GU8* puchSat_flag, GU16 usSampleRate);
static void GH3x2x_AgcAdjustGainByExtremum(GU32 unTemp_H_Thd, GU32 unTemp_L_Thd, GU8 uchGainUpLimit, GU8* uchNewGain, GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC);
static void GH3X2X_AgcSubChnlGainAdj(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo);
static GS32 GH3x2xAgcRawdata2Ipd(GU32 RawdataOffBase, GU16 usGainValue);
static void GH3x2xAgcMainChnlKeyValueCal(GU8* puchReadFifoBuffer, GU16 usFifoLen, GS32* punIpdMean, GU16 *pAdjFlag);
static void GH3x2xAgcCalDrvCurrent(GS32 usAgcMeanIpd, GU32 unTemp_Ideal_Ipd, GU8 uchCurrentDrv, GU8 uchLedCurrentMin, GU8 uchLedCurrentMax, GU32* punTempDrvCurrent);
static void GH3x2xAgcCalExtremum(GU8 uchCurrentDrv, GU32 unTempDrvCurrent,  GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC);
static GU8 GH3x2xAgcCalDrvCurrentAndGain(GS32 usAgcMeanIpd, GU8 uchSlotCnt,  GU8* puchIdealDrvCurrent, GU8* puchIdealGain);
static void GH3x2xAgcSubChnlAdjGainAndClearCnt(GU8 uchSlotIdx, GU8 uchRxEn,GU8 uchCurrentDrv,GU8 uchNewDrvCurrent,STGh3x2xAgcInfo *pSTGh3x2xAgcInfo);
static void GH3x2xAgcMainChnlPro(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo);

static void GH3X2X_StoreRxTiaGainAfterAgc(GU8 uchGain, GU8 uchSlotNo, GU8 uchAdcNo, GU8* puchGainRecord)
{
    puchGainRecord[uchSlotNo*4 + uchAdcNo] = uchGain;
}

static void GH3X2X_StoreDrvCurrentAfterAgc(GU8 DrvCurrent, GU8 uchSlotNo, GU8 uchDrvIndex, GU16* puchDrvCurrentRecord)
{
    if(!uchDrvIndex)
    {
        puchDrvCurrentRecord[uchSlotNo] = (puchDrvCurrentRecord[uchSlotNo]&0xFF00)|DrvCurrent;
    }
    else
    {
      puchDrvCurrentRecord[uchSlotNo] = (puchDrvCurrentRecord[uchSlotNo]&0x00FF)|(DrvCurrent<<8);
    }    
}

static void Gh3x2xNewAgcSubChnlModuleReset(void)
{
    #if SUB_CHN_PRO_EN
    GH3X2X_Memset((GU8*)g_pstGh3x2xAgcSubChnlRxInfo,0,(GU16)g_uchAgcSubChnlNumLimit*sizeof(STGh3x2xAgcSubChnlRxInfo));
    #endif
}

static void GH3x2xAgcModuleReset(void)
{
    GH3X2X_Memset((GU8*)g_pstGh3x2xAgcMainChnlInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xAgcMainChnlInfo));
    GH3X2X_Memset((GU8*)g_pstGh3x2xAgcMainChnlKeyInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xAgcMainChnKeyInfo)); 
    GH3X2X_Memset((GU8*)g_pstGh3x2xAgcMainChnlIdaelAdjInfo,0,(GU16)g_uchNewAgcSlotNumLimit*sizeof(STGh3x2xNewAgcMainChnlIdaelAdjInfo));                             

    Gh3x2xNewAgcSubChnlModuleReset();
}

static GU8 GH3x2xAgcFindSubChnlSlotInfo(GU8 uchSlot)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {
        STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
        if(pstAgcMainChnlInfo->uchBakRegAgcEn)
        {
            if(uchSlot == (pstAgcMainChnlInfo->uchSlotAdc >> 2))
            {
                return uchSlotCnt;
            }
        }
    }
    return 0;
}

static GU8 GH3x2xNewAgcSetNewSubChnl(GU8 uchSlotIdx, GU8 uchSubRxEn,  STAgcRegConfig* pstAgcRegConfig)
{
    GU8 uchSetSuccess = 1;
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchSubRxEn&(1 << uchRxCnt))
        {
            uchSetSuccess = 0;
            for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchAgcSubChnlNumLimit; uchChnlCnt ++)
            {
                STGh3x2xAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xAgcSubChnlRxInfo[uchChnlCnt]);
                if(0 ==  pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
                {
                    pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn = 1;
                    pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo = (uchSlotIdx << 2) + uchRxCnt;
                    //read current gain
                    pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotIdx].GainCode[uchRxCnt];                    
                    GH3X2X_INFO_LOG_PARAM_AGC("[Agc] Sub chnl current Gain = %d, Slot = %d, Rx = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain,uchSlotIdx,uchRxCnt);
                    uchSetSuccess = 1;
                    break;
                }
            }
            if(0 == uchSetSuccess)
            {
                GH3X2X_INFO_LOG_PARAM_AGC("[Agc] Set new sub chnl fail !!! Slot = %d, Rx = %d\r\n",uchSlotIdx,uchRxCnt);
                while(1);
            }
        }
    }
    return uchSetSuccess;
}

static GU8 GH3x2xGetDrvCurrent(GU8 uchSlotIdx ,GU8 uchAgcEn)
{
    GU8 uchTempDrv;
    if(2 == uchAgcEn)
    {    
        uchTempDrv = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV1, uchSlotIdx);
    }
    else
    {
        uchTempDrv = GH3x2xGetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV0, uchSlotIdx);
    }
    return uchTempDrv;
}

/**
 * @fn     GU16 GH3X2X_GetRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usRegVal)
 *
 * @brief  get register bit field from regval
 *
 * @attention   None
 *
 * @param[in]   usRegAddr      register addr
 * @param[in]   uchLsb         lsb of bit field
 * @param[in]   uchMsb         msb of bit field
 * @param[in]   usRegVal       val of register
 * @param[out]  None
 *
 * @return  register bit field data
 */
GU16 GH3X2X_GetRegBitField(GU16 usRegAddr, GU8 uchLsb, GU8 uchMsb, GU16 usRegVal)
{
    GU16 usMakData = ((((GU16)0x0001) << (uchMsb - uchLsb + 1)) - 1) << uchLsb;

    GH3X2X_VAL_GET_BIT(usRegVal, usMakData);
    usRegVal >>= uchLsb;

    return usRegVal;
}

void GH3x2xAgcRegParse(GU16 RegAddr, GU16 RegVal, GU8 SlotCfgNum, GU8 Reg_Type,
                       GU8 *EcgRxEn, GU8 *SrMultiplier,
                       GU8 *AgcSrc, GU8 *AgcEn,
                       GU8 *Gain0Code, GU8 *Gain1Code, GU8 *Gain2Code, GU8 *Gain3Code,
                       GU8 *Rx0DcCancel, GU8 *Rx1DcCancel, GU8 *Rx2DcCancel, GU8 *Rx3DcCancel,
                       GU8 *Rx0BgCancel, GU8 *Rx1BgCancel, GU8 *Rx2BgCancel, GU8 *Rx3BgCancel,
                       GU8 *LedDrv0Code, GU8 *LedDrv1Code,
                       GU8 *AdjUpLimit, GU8 *AdjDownLimit)
{
    GU16 val = 0;
    
    if (Reg_Type == SLOT_CTRL_0_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_ECGRX_EN_LSB, RG_SLOT0_ECGRX_EN_MSB, RegVal);
        *EcgRxEn = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_SR_MULTIPLIER_LSB, RG_SLOT0_SR_MULTIPLIER_MSB, RegVal);
        *SrMultiplier = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_0: EcgRxEn:%d, SrMult:%d \r\n", *EcgRxEn, *SrMultiplier);
    }
    else if (Reg_Type == SLOT_CTRL_3_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_LED_AGC_SRC_LSB, RG_SLOT0_LED_AGC_SRC_MSB, RegVal);
        *AgcSrc = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_LED_AGC_EN_LSB, RG_SLOT0_LED_AGC_EN_MSB, RegVal);
        *AgcEn = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_3: AgcSrc:%d, AgcEn:%d \r\n", *AgcSrc, *AgcEn);
    }
    else if (Reg_Type == SLOT_CTRL_4_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX0_TIA_LSB, RG_SLOT0_RX0_TIA_MSB, RegVal);
        *Gain0Code = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX1_TIA_LSB, RG_SLOT0_RX1_TIA_MSB, RegVal);
        *Gain1Code = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX2_TIA_LSB, RG_SLOT0_RX2_TIA_MSB, RegVal);
        *Gain2Code = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX3_TIA_LSB, RG_SLOT0_RX3_TIA_MSB, RegVal);
        *Gain3Code = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_4: Gain0:%d, Gain1:%d, Gain2:%d, Gain3:%d \r\n", *Gain0Code, *Gain1Code, *Gain2Code, *Gain3Code);
    }
    else if (Reg_Type == SLOT_CTRL_6_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX0_DC_CANCEL_LSB, RG_SLOT0_RX0_DC_CANCEL_MSB, RegVal);
        *Rx0DcCancel = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX1_DC_CANCEL_LSB, RG_SLOT0_RX1_DC_CANCEL_MSB, RegVal);
        *Rx1DcCancel = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_6: DcCancel0:%d, DcCancel1:%d \r\n", *Rx0DcCancel, *Rx1DcCancel);
    }
    else if (Reg_Type == SLOT_CTRL_7_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX2_DC_CANCEL_LSB, RG_SLOT0_RX2_DC_CANCEL_MSB, RegVal);
        *Rx2DcCancel = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX3_DC_CANCEL_LSB, RG_SLOT0_RX3_DC_CANCEL_MSB, RegVal);
        *Rx3DcCancel = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_7: DcCancel2:%d, DcCancel3:%d \r\n", *Rx2DcCancel, *Rx3DcCancel);
    }
    if (Reg_Type == SLOT_CTRL_8_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX0_BG_CANCEL_LSB, RG_SLOT0_RX0_BG_CANCEL_MSB, RegVal);
        *Rx0BgCancel = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX1_BG_CANCEL_LSB, RG_SLOT0_RX1_BG_CANCEL_MSB, RegVal);
        *Rx1BgCancel = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX2_BG_CANCEL_LSB, RG_SLOT0_RX2_BG_CANCEL_MSB, RegVal);
        *Rx2BgCancel = (GU8)val;        
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_RX3_BG_CANCEL_LSB, RG_SLOT0_RX3_BG_CANCEL_MSB, RegVal);
        *Rx3BgCancel = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_8: BgCancel0:%d, BgCancel1:%d, BgCancel2:%d, BgCancel3:%d \r\n", *Rx0BgCancel, *Rx1BgCancel, *Rx2BgCancel, *Rx3BgCancel);
    }
    else if (Reg_Type == SLOT_CTRL_10_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_LED_DRV0_LSB, RG_SLOT0_LED_DRV0_MSB, RegVal);
        *LedDrv0Code = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_10: LedDrv0Code:%d \r\n", *LedDrv0Code);
    }
    else if (Reg_Type == SLOT_CTRL_11_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_LED_DRV1_LSB, RG_SLOT0_LED_DRV1_MSB, RegVal);
        *LedDrv1Code = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_11: LedDrv1Code:%d \r\n", *LedDrv1Code);
    }
    else if (Reg_Type == SLOT_CTRL_12_REG)
    {
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_ADJ_UP_LIMIT_LSB, RG_SLOT0_ADJ_UP_LIMIT_MSB, RegVal);
        *AdjUpLimit = (GU8)val;
        val = GH3X2X_GetRegBitField(RegAddr, RG_SLOT0_ADJ_DOWN_LIMIT_LSB, RG_SLOT0_ADJ_DOWN_LIMIT_MSB, RegVal);
        *AdjDownLimit = (GU8)val;
        GH3X2X_INFO_LOG_PARAM_AGC("SLOT_CTRL_12: AdjUpLimit:%d, AdjDownLimit:%d \r\n", *AdjUpLimit, *AdjDownLimit);
    }

}

void GH3x2xAgcRegvalReset(STAgcRegConfig* pstAgcRegConfig)
{
    GH3X2X_INFO_LOG_PARAM_AGC("%s\r\n", __FUNCTION__);
    GH3X2X_Memset(pstAgcRegConfig, 0, sizeof(STAgcRegConfig));

    for ( GU8 i = 0; i < GH3X2X_SLOT_NUM_MAX; i++ )
    {
        pstAgcRegConfig->stAgcSlotCfgReg[i].GainCode[RX_IDX0] = GAINCODE_DEF_VAL;
        pstAgcRegConfig->stAgcSlotCfgReg[i].GainCode[RX_IDX1] = GAINCODE_DEF_VAL;
        pstAgcRegConfig->stAgcSlotCfgReg[i].GainCode[RX_IDX2] = GAINCODE_DEF_VAL;
        pstAgcRegConfig->stAgcSlotCfgReg[i].GainCode[RX_IDX3] = GAINCODE_DEF_VAL;
        pstAgcRegConfig->stAgcSlotCfgReg[i].AdjUpLimit = ADJUPLIMIT_DEF_VAL;
    }
}

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
void GH3x2xAgcRegvalGet(STAgcRegConfig* pstAgcRegConfig, GU16 RegAddr, GU16 RegVal)
{
    if (RegAddr % 2)
    {
        GH3X2X_INFO_LOG_PARAM_AGC("%s: wrong Reg address!!!\r\n", __FUNCTION__);
        return;
    }

    GH3X2X_INFO_LOG_PARAM_AGC("RegAddr: %d, RegVal: %d\r\n", RegAddr, RegVal);
    if (SYS_SAMPLE_RATE_CTRL == RegAddr)
    {
        pstAgcRegConfig->FastestSampleRate = GH3X2X_GetRegBitField(RegAddr, RG_FASTEST_SAMPLE_RATE_LSB, RG_FASTEST_SAMPLE_RATE_MSB, RegVal);
    }
    else if (SLOTCFG_REG_BEGIN_ADDR <= RegAddr && SLOTCFG_REG_END_ADDR >= RegAddr)
    {
        GU8 SlotCfgNum = (RegAddr - SLOTCFG_REG_BEGIN_ADDR) / SLOTCFG_REG_OFFSET;
        GU8 Reg_Type = (RegAddr - SLOTCFG_REG_BEGIN_ADDR) % SLOTCFG_REG_OFFSET;

        GU8 EcgRxEn = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].EcgRxEn;
        GU8 SrMultiplier = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].SrMultiplier;
        GU8 AgcSrc = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AgcSrc;
        GU8 AgcEn = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AgcEn;
        GU8 Gain0Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX0];
        GU8 Gain1Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX1];
        GU8 Gain2Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX2];
        GU8 Gain3Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX3];
        GU8 Rx0DcCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX0];
        GU8 Rx1DcCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX1];
        GU8 Rx2DcCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX2];
        GU8 Rx3DcCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX3];
        GU8 Rx0BgCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX0];
        GU8 Rx1BgCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX1];
        GU8 Rx2BgCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX2];
        GU8 Rx3BgCancel = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX3];
        GU8 LedDrv0Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].LedDrvCode[DRV_IDX0];
        GU8 LedDrv1Code = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].LedDrvCode[DRV_IDX1];
        GU8 AdjUpLimit = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AdjUpLimit;
        GU8 AdjDownLimit = pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AdjDownLimit;

        GH3x2xAgcRegParse(RegAddr, RegVal, SlotCfgNum, Reg_Type,
                 &EcgRxEn,&SrMultiplier,
                 &AgcSrc,&AgcEn,
                 &Gain0Code,&Gain1Code,&Gain2Code,&Gain3Code,
                 &Rx0DcCancel,&Rx1DcCancel,&Rx2DcCancel,&Rx3DcCancel,
                 &Rx0BgCancel,&Rx1BgCancel,&Rx2BgCancel,&Rx3BgCancel,
                 &LedDrv0Code,&LedDrv1Code,
                 &AdjUpLimit,&AdjDownLimit);
        
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].EcgRxEn = EcgRxEn;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].SrMultiplier = SrMultiplier;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AgcSrc = AgcSrc;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AgcEn = AgcEn;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX0] = Gain0Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX1] = Gain1Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX2] = Gain2Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].GainCode[RX_IDX3] = Gain3Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX0] = Rx0DcCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX1] = Rx1DcCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX2] = Rx2DcCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxDcCancel[RX_IDX3] = Rx3DcCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX0] = Rx0BgCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX1] = Rx1BgCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX2] = Rx2BgCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].RxBgCancel[RX_IDX3] = Rx3BgCancel;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].LedDrvCode[DRV_IDX0] = LedDrv0Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].LedDrvCode[DRV_IDX1] = LedDrv1Code;
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AdjUpLimit = AdjUpLimit; 
        pstAgcRegConfig->stAgcSlotCfgReg[SlotCfgNum].AdjDownLimit = AdjDownLimit;   
    }
}

static void GH3x2xAgcChnlInfoInit(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo, STAgcRegConfig* pstAgcRegConfig)
{ 
    // record TIA and DC Canel
    for (GU8 uchSlotIndex = 0; uchSlotIndex < 8; uchSlotIndex++)
    {
        for (GU8 nAdcCnt = 0; nAdcCnt < 4; nAdcCnt++)
        {
            pSTGh3x2xAgcInfo->uchTiaGainAfterSoftAgc[uchSlotIndex*4 + nAdcCnt] = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotIndex].GainCode[nAdcCnt];
            pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[uchSlotIndex*4 + nAdcCnt] = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotIndex].RxDcCancel[nAdcCnt];
        }
    }

    //Record Drv
    for (GU8 uchSlotIndex = 0; uchSlotIndex < GH3X2X_MAX_SLOT_NUM; uchSlotIndex++)
    {
        GU16 usDrv0CurRegData = 0;
        GU16 usDrv1CurRegData = 0;
        usDrv0CurRegData = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotIndex].LedDrvCode[0];
        usDrv1CurRegData = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotIndex].LedDrvCode[1];
        pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc[uchSlotIndex] =  (GU16)(usDrv0CurRegData << GH3X2X_DRV0_CURRENT_LEFT_SHIFT_BIT_NUM);
        pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc[uchSlotIndex] |= (GU16)(usDrv1CurRegData << GH3X2X_DRV1_CURRENT_LEFT_SHIFT_BIT_NUM);
    }

    if(0 == g_uchNewAgcSlotNumLimit)
    {
        return;
    }
    
    {
        GU8 uchNewAgcSlotCnt = 0;
        for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X2X_SLOT_NUM_MAX; uchSlotCnt ++)
        {
            if((g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn & (1 << uchSlotCnt)))
            {
                if(uchNewAgcSlotCnt < g_uchNewAgcSlotNumLimit)
                {
                    g_pstGh3x2xAgcMainChnlInfo[uchNewAgcSlotCnt].uchBakRegAgcEn = 1;
                    g_pstGh3x2xAgcMainChnlInfo[uchNewAgcSlotCnt].uchSlotAdc = (uchSlotCnt << 2); 
                                    
                    /******* slot function Distinguish   START******************************************************/          
                    if((g_stSoftAgcCfg.uchSpo2RedSlotCfgEn & (1 << uchSlotCnt))||(g_stSoftAgcCfg.uchSpo2IrSlotCfgEn & (1 << uchSlotCnt)))
                    {
                        g_pstGh3x2xAgcMainChnlInfo[uchNewAgcSlotCnt].uchSpo2ChnlIndex =  1;
                        GH3X2X_INFO_LOG_PARAM_AGC("[NewAgc] uchNewAgcSpo2SlotCnt = %d\r\n",uchNewAgcSlotCnt);   
                    }
                    /******* slot function Distinguish   END******************************************************/
                    GH3X2X_INFO_LOG_PARAM_AGC("[NewAgc] uchNewAgcSlotCnt = %d\r\n",uchNewAgcSlotCnt);             
                    uchNewAgcSlotCnt ++;
                }
                else
                {
                    GH3X2X_INFO_LOG_PARAM_AGC("[NewAgc] error !!! main chnl info is no enough! \r\n");    
                    while(1);
                }
            }
        }  
    }

    GU16 usFastestSampleRate =  pstAgcRegConfig->FastestSampleRate;

    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt++)
    {
        STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
        GH3X2X_INFO_LOG_PARAM_AGC("AGC_InitValue %d: AgcAdjEn = %d\r\n",uchSlotCnt, g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn);

        if(pstAgcMainChnlInfo->uchBakRegAgcEn)
        {
            GU8 uchSlotcfg;
            uchSlotcfg = pstAgcMainChnlInfo->uchSlotAdc >> 2;
            GH3X2X_INFO_LOG_PARAM_AGC("AGC_InitFlag%d: SlotCfgX=%d\r\n",uchSlotCnt, uchSlotcfg);

            {
                GU8 uchBgCancel;
                GU8 uchGainLimit;
                GU16 usSampleRate;
                GU16 usEcgPpgRxEn;

                //bak reg
                pstAgcMainChnlInfo->uchBakRegAgcEn = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].AgcEn;
                pstAgcMainChnlInfo->uchBakRegAgcSrc = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].AgcSrc;
                pstAgcMainChnlInfo->uchSlotAdc = (uchSlotcfg << 2) + pstAgcMainChnlInfo->uchBakRegAgcSrc;
                pstAgcMainChnlInfo->uchBakRegLedCurrentMax = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].AdjUpLimit;                
                pstAgcMainChnlInfo->uchBakRegLedCurrentMin = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].AdjDownLimit;    
                pstAgcMainChnlInfo->uchCurrentTiaGain =  pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].GainCode[pstAgcMainChnlInfo->uchBakRegAgcSrc];

                if((DRV_IDX1<<1) == pstAgcMainChnlInfo->uchBakRegAgcEn)
                {    
                    pstAgcMainChnlInfo->uchCurrentDrv = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].LedDrvCode[1];
                }
                else
                {
                    pstAgcMainChnlInfo->uchCurrentDrv = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].LedDrvCode[0];
                }
                uchBgCancel = pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].RxBgCancel[pstAgcMainChnlInfo->uchBakRegAgcSrc];

                if(RX_IDX0 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg32uA;
                }
                else if(RX_IDX1 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg64uA;
                }
                else if(RX_IDX2 == uchBgCancel)
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg128uA;
                }
                else
                {
                    uchGainLimit = g_stSoftAgcCfg.stSoftAgcGainLimit.uchGainLimitBg256uA;
                }
                GH3X2X_INFO_LOG_PARAM_AGC("[NewAgc] uchGainLimit = %d, uchSlotcfg = %d\r\n",uchGainLimit, uchSlotcfg);    
                pstAgcMainChnlInfo->uchTiaGainUpLimit = uchGainLimit;
                usEcgPpgRxEn =  pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].EcgRxEn;
                GH3X2X_INFO_LOG_PARAM_AGC("[NewAgc] usEcgPpgRxEn = 0x%x\r\n",usEcgPpgRxEn);        
                if(usEcgPpgRxEn&0x0001)  // ecg is enable
                {
                    usEcgPpgRxEn = ((usEcgPpgRxEn>>1) & (~0x0008));  //clear RX3 enable bit
                }
                else
                {
                    usEcgPpgRxEn = (usEcgPpgRxEn>>1);
                }
                usEcgPpgRxEn &= (~(1 << pstAgcMainChnlInfo->uchBakRegAgcSrc));   //clear main channel RX enable bit
                
                usSampleRate = GH3X_SYS_SAMPLE_RATE/(usFastestSampleRate + 1)/(pstAgcRegConfig->stAgcSlotCfgReg[uchSlotcfg].SrMultiplier + 1);
                g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt].usSampleRate = usSampleRate;  //init
                //sub channel slot
#if SUB_CHN_PRO_EN
                GH3x2xNewAgcSetNewSubChnl(uchSlotcfg, usEcgPpgRxEn, pstAgcRegConfig);
#endif
            }
        }
    }
}

#if READ_AGCINFO
void GH3X2X_RecordAgcInfo(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    GU16 usDrv0CurRegData = 0;
    GU16 usDrv0CurRegAddr = 0;
    GU16 usDrv1CurRegData = 0;
    GU16 usDrv1CurRegAddr = 0;
    GU16 uchTIAData = 0;
    GU16 uchDcCancelData0 = 0;
    GU16 uchDcCancelData1 = 0;
    GU8 uchTempTIAData = 0;
    GU16 uchTempDcCancel = 0;
    // record TIA and DC Canel
    for (GU8 uchSlotIndex = 0; uchSlotIndex < 8; uchSlotIndex++)
    {
        uchTIAData = GH3X2X_ReadReg((GU16)(GH3X2X_SLOT0_CTRL_4_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET)));
        uchDcCancelData0 = GH3X2X_ReadReg((GU16)(GH3X2X_SLOT0_CTRL_6_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET)));
        uchDcCancelData1 = GH3X2X_ReadReg((GU16)(GH3X2X_SLOT0_CTRL_7_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET)));

        for (GU8 nAdcCnt = 0; nAdcCnt < 4; nAdcCnt++)
        {
            uchTempTIAData = (GU8)(((uchTIAData) >> (GH3X2X_SLOT_TIA_GAIN_BITS_SIZE * nAdcCnt)) & GH3X2X_SLOT_TIA_GAIN_BITS_MARK);
            pSTGh3x2xAgcInfo->uchTiaGainAfterSoftAgc[uchSlotIndex*4 + nAdcCnt] = uchTempTIAData;

            if(nAdcCnt<2)
            {
                uchTempDcCancel = (GU8)(((uchDcCancelData0) >> (GH3X2X_SLOT_DC_CANCEL_BITS_SIZE * nAdcCnt)) & GH3X2X_SLOT_DC_CANCEL_BITS_MARK);
                pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[uchSlotIndex*4 + nAdcCnt] = uchTempDcCancel;
            }
            else
            {
                uchTempDcCancel = (GU8)(((uchDcCancelData1) >> (GH3X2X_SLOT_DC_CANCEL_BITS_SIZE * (nAdcCnt-2))) & GH3X2X_SLOT_DC_CANCEL_BITS_MARK);
                pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[uchSlotIndex*4 + nAdcCnt] = uchTempDcCancel;
            }
        }
    }
    GH3X2X_INFO_LOG_PARAM_AGC("[%s]slot0: adc0:%d, adc1:%d,adc2:%d,adc3:%d\r\n", __FUNCTION__,pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[0],pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[1],
                              pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[2],pSTGh3x2xAgcInfo->uchDcCancelAfterSoftAgc[3]);
    //Record Drv
    usDrv0CurRegAddr = GH3X2X_SLOT0_CTRL_10_REG_ADDR;
    usDrv1CurRegAddr = GH3X2X_SLOT0_CTRL_11_REG_ADDR;
    for (GU8 uchSlotIndexCnt = 0; uchSlotIndexCnt < GH3X2X_MAX_SLOT_NUM; uchSlotIndexCnt++)
    {
        usDrv0CurRegData = (GH3X2X_ReadReg((GU16)(usDrv0CurRegAddr + (uchSlotIndexCnt * GH3X2X_SLOT_CTRL_OFFSET))) & GH3X2X_DRV_CURRENT_BIT_MASK);
        usDrv1CurRegData = (GH3X2X_ReadReg((GU16)(usDrv1CurRegAddr + (uchSlotIndexCnt * GH3X2X_SLOT_CTRL_OFFSET))) & GH3X2X_DRV_CURRENT_BIT_MASK);
        pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc[uchSlotIndexCnt] =  (GU16)(usDrv0CurRegData << GH3X2X_DRV0_CURRENT_LEFT_SHIFT_BIT_NUM);
        pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc[uchSlotIndexCnt] |= (GU16)(usDrv1CurRegData << GH3X2X_DRV1_CURRENT_LEFT_SHIFT_BIT_NUM);
    }
}
#endif

static void GH3x2xAgcInfoUpdate(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    GU16 usFastestSampleRate =  GH3X2X_ReadRegBitField(SYS_SAMPLE_RATE_CTRL, RG_FASTEST_SAMPLE_RATE_LSB, RG_FASTEST_SAMPLE_RATE_MSB);
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {    
        STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
        GH3X2X_INFO_LOG_PARAM_AGC("AGC_InitValue %d: AgcAdjEn = %d\r\n",uchSlotCnt, g_stSoftAgcCfg.uchSlotxSoftAgcAdjEn);

        if(pstAgcMainChnlInfo->uchBakRegAgcEn)
        {
            GU8 uchSlotIdx = pstAgcMainChnlInfo->uchSlotAdc >> 2;
            GH3X2X_INFO_LOG_PARAM_AGC("AGC_InitFlag%d: SlotCfgX=%d \r\n",uchSlotCnt, uchSlotIdx);

            pstAgcMainChnlInfo->uchCurrentTiaGain = GH3x2xGetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx);
            pstAgcMainChnlInfo->uchCurrentDrv =  GH3x2xGetDrvCurrent(uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcEn);                            

            GU16 usSampleRate = GH3X_SYS_SAMPLE_RATE / (usFastestSampleRate + 1) / (GH3x2xGetAgcReg(GH3X2X_AGC_REG_SR_MULIPLIPER, uchSlotIdx) + 1);
            g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt].usSampleRate = usSampleRate;  //init      
            GH3x2xSetAgcReg(GH3X2X_AGC_REG_AGC_EN, uchSlotIdx, 0);   // disable hardware agc 
        }
    }
    
#if READ_AGCINFO
    GH3X2X_RecordAgcInfo(pSTGh3x2xAgcInfo);
#endif   

}


static GU32 GH32x2xMedSel(GU32 unData0, GU32 unData1, GU32 unData2)
{
    if((unData0 >= unData1)&&(unData0 <= unData2))
    {
        return unData0;
    }
    if((unData0 >= unData2)&&(unData0 <= unData1))
    {
        return unData0;
    }
    if((unData1 >= unData0)&&(unData1 <= unData2))
    {
        return unData1;
    }
    if((unData1 >= unData2)&&(unData1 <= unData0))
    {
        return unData1;
    }
    return unData2;
}

static void GH3X2X_AgcGetThreshold( GU8 uchSpo2ChnlIndex, GU32* punTemp_H_Thd, GU32* punTemp_L_Thd)
{
   if(uchSpo2ChnlIndex) // is  spo2 slot
   {
      *punTemp_H_Thd = __GH3X_AGC_SPO2_H_THD__; 
      *punTemp_L_Thd = __GH3X_AGC_SPO2_L_THD__;
   }else
   { 
      *punTemp_H_Thd = __GH3X_AGC_GENERAL_H_THD__;
      *punTemp_L_Thd = __GH3X_AGC_GENERAL_L_THD__;    
   }
}

static void GH3X2X_AgcGetExtremum( GU16 usAnalysisDiscardLen, GU16 usAnalysisCnt, GU32 unDataRaw, GU32* punMaxRawdata, GU32* punMinRawdata)
{
    if(usAnalysisDiscardLen >= usAnalysisCnt)
    {
        (*punMaxRawdata) =  0;
        (*punMinRawdata) =  16777216;
    } else{
        (*punMaxRawdata) = ((unDataRaw > (*punMaxRawdata))? unDataRaw: (*punMaxRawdata) ); // Max of  rawdata
        (*punMinRawdata) = ((unDataRaw < (*punMinRawdata))? unDataRaw: (*punMinRawdata) ); // Min of  rawdata  
    }
}

static void GH3X2X_AgcSetSatFlag( GU32 unTemp_H_Thd, GU32 unDataRaw, GU16* pusSat_CheckCnt, GU16* pusSat_cnt, GU8* puchSat_flag, GU16 usSampleRate)
{
    GU16 usSat_Check_Num_Max =   (usSampleRate<<2) + usSampleRate;   //// 5 seconds window
    (*pusSat_CheckCnt)++;
    if( unDataRaw > unTemp_H_Thd)
    {
        (*pusSat_cnt)++;
    }
    //// as soon as posible set the Saturation flag 
    if( (*pusSat_cnt) > ( usSat_Check_Num_Max>>4)) // about 6.25% of all checked data
    {
        (*puchSat_flag) = 1;  
        (*pusSat_cnt) = 0; 
    }

    if( (*pusSat_CheckCnt) >= usSat_Check_Num_Max ) 
    {
        (*pusSat_cnt) = 0; 
        (*pusSat_CheckCnt) = 0;  
    }
}

#if SUB_CHN_PRO_EN
static void GH3X2X_AgcSubChnlGainAdj(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchAgcSubChnlNumLimit; uchChnlCnt++)
    {
        GU16 usAnalysisDiscardLen;
        GU16 usAnalysisEndLen;
        GU8  uchSlotCnt;
        GU8  uchSlotIdx;
        GU8  uchAdcNo;
        GU32 unTemp_H_Thd;
        GU32 unTemp_L_Thd; 
        STGh3x2xAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xAgcSubChnlRxInfo[uchChnlCnt]);
        uchSlotIdx = pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo >> 2;
        uchAdcNo = pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo&0x03;
        uchSlotCnt = GH3x2xAgcFindSubChnlSlotInfo(uchSlotIdx);
        GU8 uchTiaGainLimit = g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt].uchTiaGainUpLimit;
        GU16 usSampleRate = g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt].usSampleRate;
        usAnalysisDiscardLen = (usSampleRate)/5;    //  1/5 Fs
        usAnalysisEndLen = usAnalysisDiscardLen + usSampleRate * GH3X2X_NEW_AGC_SUB_CHNNL_ANAYLSIS_TIME_SEC; 
        GH3X2X_AgcGetThreshold(g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt].uchSpo2ChnlIndex, &unTemp_H_Thd, &unTemp_L_Thd);

        if(pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
        { 
            GU8  uchCurrentGain = pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain;
            GU8  uchNewGain = uchCurrentGain;
            GH3X2X_INFO_LOG_PARAM_AGC("Subchnl infomaton: ChnlCnt = %d, SlotIdx = %d, AGCSlotCnt = %d, rx = %d\r\n",uchChnlCnt,uchSlotIdx, uchSlotCnt, uchAdcNo);
            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE) 
            {
                GU8 uchSlotAdcNo = (( puchReadFifoBuffer[usIndex] >> 3) & 0x1F);
                if(uchSlotAdcNo == pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo)
                {
                    GU32 unDataRaw = GH3X2X_MAKEUP_3BYTE( puchReadFifoBuffer[usIndex+1], 
                                          puchReadFifoBuffer[usIndex+2], puchReadFifoBuffer[usIndex+3]);

                    if(usAnalysisDiscardLen == pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt) //init
                    {
                        pstSTGh3x2xNewAgcSubChnlRxInfo->unLastLastRawdata = unDataRaw;
                        pstSTGh3x2xNewAgcSubChnlRxInfo->unLastRawdata = unDataRaw;
                    }

                    if( pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt < usAnalysisEndLen )  //analysising
                    {
                        GU32 unTempRawdata = GH32x2xMedSel(pstSTGh3x2xNewAgcSubChnlRxInfo->unLastLastRawdata, pstSTGh3x2xNewAgcSubChnlRxInfo->unLastRawdata, unDataRaw);  
                        pstSTGh3x2xNewAgcSubChnlRxInfo->unLastLastRawdata = pstSTGh3x2xNewAgcSubChnlRxInfo->unLastRawdata;
                        pstSTGh3x2xNewAgcSubChnlRxInfo->unLastRawdata = unDataRaw;
                        GH3X2X_AgcGetExtremum(usAnalysisDiscardLen, pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt, unTempRawdata,
                                              &(pstSTGh3x2xNewAgcSubChnlRxInfo->unMaxRawdata), &(pstSTGh3x2xNewAgcSubChnlRxInfo->unMinRawdata));
                        pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt ++;
                    }
                    //// Complete adjust as soon as possible
                    if( pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt == usAnalysisEndLen ) //anlysis finished
                    {
                         pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt = 0;
                         { 
                            GH3x2x_AgcAdjustGainByExtremum(unTemp_H_Thd, unTemp_L_Thd, uchTiaGainLimit, &uchNewGain, &(pstSTGh3x2xNewAgcSubChnlRxInfo->unMaxRawdata), &(pstSTGh3x2xNewAgcSubChnlRxInfo->unMinRawdata));  
                            GH3X2X_INFO_LOG_PARAM_AGC("SubChnlAdj_Idael: SlotAdcNo = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo,
                                                      unTemp_H_Thd, unTemp_L_Thd,uchCurrentGain, uchNewGain, pstSTGh3x2xNewAgcSubChnlRxInfo->unMaxRawdata, pstSTGh3x2xNewAgcSubChnlRxInfo->unMinRawdata);    
                            if(uchCurrentGain != uchNewGain)
                            {
                                GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + uchAdcNo, uchSlotIdx, uchNewGain);
                                GH3X2X_StoreRxTiaGainAfterAgc(uchNewGain, uchSlotIdx, uchAdcNo, pSTGh3x2xAgcInfo->uchTiaGainAfterSoftAgc);
                                pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain = uchNewGain;
                            }
                         } 
                         break;  //  Data after completing a mean calculation is discarded
                    }
                }
            }
        }
    }
}
#endif

static GS32 GH3x2xAgcRawdata2Ipd( GU32 RawdataOffBase, GU16 usGainValue)  // usGainValue ��λKohm
{
     GS32 unIpdMean;
     if(RawdataOffBase > GH3X2X_DATA_BASE_LINE)  //// Can't open DC Cance !!!
     {
         RawdataOffBase -= GH3X2X_DATA_BASE_LINE;
     }
     else
     {
        RawdataOffBase = 1;  
     }
     //Rawdata stretch  (The principle of magnification: the mean divided by the ideal target value is the data reduction factor )
     if( RawdataOffBase < 8000000)
     {
     }
     else if( RawdataOffBase < 8100000 )
     {
         RawdataOffBase = 8000000 + ((RawdataOffBase - 8000000)<<1);
     }else
     {
         // The more saturated the data, the bigger the stretch, the key is that the data is really saturated
         RawdataOffBase = 8000000 + ((RawdataOffBase - 8000000)<<2);   
     }
     unIpdMean = (GS64)(RawdataOffBase)*900000/((GS64)usGainValue*(GH3X2X_DATA_BASE_LINE)); // Ipd unit:nA
     if(unIpdMean==0)
     {
         unIpdMean = 1;//Avoid the division is 0
     }

     return unIpdMean;
}

/****************************************************************
* Description: calculate key value of main channel  
* Input:  rawdata  
* Return: none
******************************************************************/
static void GH3x2xAgcMainChnlKeyValueCal(GU8* puchReadFifoBuffer, GU16 usFifoLen, GS32* punIpdMean, GU16 *pAdjFlag)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
    {
        STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
        GU8 uchMainChnlSlotAdcNo = pstAgcMainChnlInfo->uchSlotAdc;

        if(pstAgcMainChnlInfo->uchBakRegAgcEn)   // is  enabled  slot
        {
            GU32 unTemp_H_Thd;
            GU32 unTemp_H_Ipd;
            GU32 unTemp_L_Ipd;
            GU32 unTemp_Ideal_value;
            GU16 usAnalysisEndLen ; 
            GU16 usAnalysisDiscardLenAdjust = 0;
            GU16 usGainValue = g_usGH3x2xTiaGainR[pstAgcMainChnlInfo->uchCurrentTiaGain]; //// uint: kohm
            if(0 != pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
            {
                unTemp_H_Thd = __GH3X_AGC_SPO2_H_THD__;
                unTemp_H_Ipd = __GH3X_AGC_SPO2_H_IPD__;
                unTemp_L_Ipd = __GH3X_AGC_SPO2_L_IPD__; 
                unTemp_Ideal_value= __GH3X_AGC_SPO2_IDEAL_VALUE__;
            }else
            {
                unTemp_H_Thd = __GH3X_AGC_GENERAL_H_THD__;  
                unTemp_H_Ipd = __GH3X_AGC_GENERAL_H_IPD__;
                unTemp_L_Ipd = __GH3X_AGC_GENERAL_L_IPD__;
                unTemp_Ideal_value= __GH3X_AGC_GENERAL_IDEAL_VALUE__;
            }

            STGh3x2xAgcMainChnKeyInfo *pstAgcMainChnlkeyInfo = &g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt];
            STGh3x2xNewAgcMainChnlIdaelAdjInfo *pstAgcMainChnlIdaelAdjInfo = &g_pstGh3x2xAgcMainChnlIdaelAdjInfo[uchSlotCnt];
            GU16 usSampleRate =  pstAgcMainChnlkeyInfo->usSampleRate;
            usAnalysisEndLen = usSampleRate;//Fs

            //have agc motion at last fifo read period or first init                  
            if(pstAgcMainChnlkeyInfo->uchHaveAgcAtLastReadPrd)   
            {
                usAnalysisDiscardLenAdjust = usSampleRate/5;// drop the first 1/5 data
                //// After AGC, the previous data is drop to calcluate precise mean
                if(!pstAgcMainChnlkeyInfo->uchDropFlag)
                {  // The frame count is only cleared for the first time to prevent data num cannot to be SampleRate
                    pstAgcMainChnlkeyInfo->usAnalysisCnt = 0;
                    pstAgcMainChnlkeyInfo->uchDropFlag = 1;
                    pstAgcMainChnlkeyInfo->usSat_CheckCnt = 0;
                    pstAgcMainChnlkeyInfo->usSat_cnt = 0;
                }
            }

            //The data fluctuates when initializing the early stage  
            if(!pstAgcMainChnlIdaelAdjInfo->uchIniteFirst)  
            {
                usAnalysisDiscardLenAdjust = usSampleRate/5; // drop the first 1/5 data
            }
            GH3X2X_INFO_LOG_PARAM_AGC("uchMainChnlSlotAdcNo=%d,HaveAgcAtLast=%d\r\n",uchMainChnlSlotAdcNo,pstAgcMainChnlkeyInfo->uchHaveAgcAtLastReadPrd); 

            for (GU16 usIndex = 0; usIndex < usFifoLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE) 
            {
                GU8 uchSlotAdcNo = (( puchReadFifoBuffer[usIndex] >> 3) & 0x1F);
                if(uchSlotAdcNo == uchMainChnlSlotAdcNo)    
                {
                    GU32 unDataRaw = GH3X2X_MAKEUP_3BYTE( puchReadFifoBuffer[usIndex+1], 
                                          puchReadFifoBuffer[usIndex+2], puchReadFifoBuffer[usIndex+3]);
                    // begin Saturation judge
                    if(usAnalysisDiscardLenAdjust <= pstAgcMainChnlkeyInfo->usAnalysisCnt)  
                    {
                       GH3X2X_AgcSetSatFlag(unTemp_H_Thd, unDataRaw, &(pstAgcMainChnlkeyInfo->usSat_CheckCnt),\
                            &(pstAgcMainChnlkeyInfo->usSat_cnt),&(pstAgcMainChnlkeyInfo->Sat_flag),usSampleRate);
                       GH3X2X_INFO_LOG_PARAM_AGC("Sat_Ipd:SlotAdcNo=%d, Satflag=%d, AnaCnt=%d\r\n", uchSlotAdcNo, 
                                                 pstAgcMainChnlkeyInfo->Sat_flag, pstAgcMainChnlkeyInfo->usAnalysisCnt);
                    }

                    //// begin mean calculate      
                    if(usAnalysisDiscardLenAdjust == pstAgcMainChnlkeyInfo->usAnalysisCnt) //init
                    {
                        pstAgcMainChnlkeyInfo->unSumAdjust =  0;
                        pstAgcMainChnlkeyInfo->unLastRawdata = unDataRaw;
                        pstAgcMainChnlkeyInfo->unLast2Rawdata = unDataRaw;
                    }

                    if( pstAgcMainChnlkeyInfo->usAnalysisCnt < usAnalysisEndLen )   //analysising
                    {
                        GU32 unMedRawdata = GH32x2xMedSel(pstAgcMainChnlkeyInfo->unLast2Rawdata, pstAgcMainChnlkeyInfo->unLastRawdata, unDataRaw);
                        pstAgcMainChnlkeyInfo->unLast2Rawdata = pstAgcMainChnlkeyInfo->unLastRawdata;
                        pstAgcMainChnlkeyInfo->unLastRawdata = unDataRaw;
                        GH3X2X_AgcGetExtremum(usAnalysisDiscardLenAdjust, pstAgcMainChnlkeyInfo->usAnalysisCnt, unMedRawdata,
                                              &(pstAgcMainChnlkeyInfo->unMaxRawdata), &(pstAgcMainChnlkeyInfo->unMinRawdata));
                        pstAgcMainChnlkeyInfo->unSumAdjust +=  unMedRawdata;
                        pstAgcMainChnlkeyInfo->usAnalysisCnt++;
                        GH3X2X_INFO_LOG_PARAM_AGC("slotCnt=%d, MainChnlSlotAdc=%d, SlotAdc=%d,Raw=%d, AnaCnt=%d, SumAdjust=%d\r\n", uchSlotCnt, uchMainChnlSlotAdcNo, uchSlotAdcNo,
                                                  unDataRaw, pstAgcMainChnlkeyInfo->usAnalysisCnt, pstAgcMainChnlkeyInfo->unSumAdjust);  
                    }
                    //Anlysis finished calculate mean and judge whether AGC (Complete AGC as soon as possible)
                    if(pstAgcMainChnlkeyInfo->usAnalysisCnt == usAnalysisEndLen) 
                    {
                         // mean value to calculate gain and current
                         if(usAnalysisEndLen > usAnalysisDiscardLenAdjust)
                         {
                            pstAgcMainChnlkeyInfo->unRawdataMean = pstAgcMainChnlkeyInfo->unSumAdjust/(usAnalysisEndLen - usAnalysisDiscardLenAdjust);      
                         }else //  The normal case will not be executed, in case the division by 0 in abnormal case
                         {
                           pstAgcMainChnlkeyInfo->unRawdataMean =  unDataRaw; 
                         }
                         punIpdMean[uchSlotCnt] = GH3x2xAgcRawdata2Ipd( pstAgcMainChnlkeyInfo->unRawdataMean,  usGainValue);

                         pstAgcMainChnlkeyInfo->usAnalysisCnt = 0; 
                         pstAgcMainChnlkeyInfo->uchHaveAgcAtLastReadPrd = 0; //// cleer after mean calculate finished
                         //// end Ipd calculate

                         if(pstAgcMainChnlkeyInfo->Sat_flag)
                         {
                            *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                         }

                         ///////////////////// whether AGC  and adjust the mean value to calculate gain and current
                         if( (punIpdMean[uchSlotCnt] < unTemp_L_Ipd) || (punIpdMean[uchSlotCnt] > unTemp_H_Ipd) )
                         {
                             *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                         }

                         //////////////begin ideal value adjust 
                         if( (!((*pAdjFlag) & (1<<uchSlotCnt))) && (!(pstAgcMainChnlkeyInfo->Sat_flag)) )//if the channel not adjust and satuartion
                         {
                             //// The deviation of the rawdata and   Data fluctuation is less than a certain value
                             if( ( (pstAgcMainChnlkeyInfo->unRawdataMean * GH3X_AGC_CONVERGENCE_LEVEL < (unTemp_Ideal_value * (GH3X_AGC_CONVERGENCE_LEVEL-1)))\
                                || (pstAgcMainChnlkeyInfo->unRawdataMean * GH3X_AGC_CONVERGENCE_LEVEL > (unTemp_Ideal_value * (GH3X_AGC_CONVERGENCE_LEVEL+1))) )\
                                 &&(10*pstAgcMainChnlkeyInfo->unMaxRawdata < 12*pstAgcMainChnlkeyInfo->unMinRawdata) ) 
                             {
                                 if(!pstAgcMainChnlIdaelAdjInfo->uchIniteFirst ) //// Inite First time 
                                 {
                                     pstAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;
                                     *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                 }
                                 else
                                 {
                                     pstAgcMainChnlIdaelAdjInfo->uchWindowCnt++;
                                     if(pstAgcMainChnlIdaelAdjInfo->uchWindowCnt >= 15)
                                     {
                                        *pAdjFlag |= (1<<uchSlotCnt);  //need adjust gain and current 
                                        pstAgcMainChnlIdaelAdjInfo->uchWindowCnt  = 0; 
                                     }
                                 }
                             }
                             else
                             {
                                pstAgcMainChnlIdaelAdjInfo->uchWindowCnt  = 0;
                             }
                         }
                         else
                         {
                            pstAgcMainChnlIdaelAdjInfo->uchWindowCnt  = 0;
                         }

                         pstAgcMainChnlIdaelAdjInfo->uchIniteFirst = 1;  //// Must be set to 1 after the first second!!!
                         GH3X2X_INFO_LOG_PARAM_AGC("Mean cal: Slot_num = %d, Rawmean = %d, gain = %d, Ipd = %d\r\n",uchSlotCnt,pstAgcMainChnlkeyInfo->unRawdataMean, usGainValue,punIpdMean[uchSlotCnt]);
                         GH3X2X_INFO_LOG_PARAM_AGC("Sat flag: SatCnt = %d, SatCheck = %d, Satflag = %d\r\n", pstAgcMainChnlkeyInfo->usSat_cnt, pstAgcMainChnlkeyInfo->usSat_CheckCnt, pstAgcMainChnlkeyInfo->Sat_flag);
                         GH3X2X_INFO_LOG_PARAM_AGC("Adj flag: Slot_num = %d, Ipd = %d, Adjflag = %d\r\n", uchSlotCnt, punIpdMean[uchSlotCnt], *pAdjFlag);
                         GH3X2X_INFO_LOG_PARAM_AGC("Ideal adjust: Slot_num = %d, WindowCnt = %d\r\n", uchSlotCnt, pstAgcMainChnlIdaelAdjInfo->uchWindowCnt);
                         break; //   Data after completing a mean calculation is discarded
                         //////////////end ideal value adjust
                   }
                }
             }
        }
    }
}

/**
 * @fn     GH3x2xAgcCalDrvCurrent(GS32 usAgcMeanIpd, GU32 unTemp_Ideal_Ipd, GU8 uchCurrentDrv, GU8 uchLedCurrentMin, GU8 uchLedCurrentMax, GU32* punTempDrvCurrent )
 *
 * @brief  Calculate DrvCurrent
 *
 * @attention   
 *
 * @param[in]   usAgcMeanIpd, unTemp_Ideal_Ipd, uchCurrentDrv, uchLedCurrentMin, uchLedCurrentMax
 * @param[out]  punTempDrvCurrent
 *
 * @return
 */
static void GH3x2xAgcCalDrvCurrent(GS32 usAgcMeanIpd, GU32 unTemp_Ideal_Ipd, GU8 uchCurrentDrv, GU8 uchLedCurrentMin, GU8 uchLedCurrentMax, GU32* punTempDrvCurrent)
{

    *punTempDrvCurrent = uchCurrentDrv*(unTemp_Ideal_Ipd)/(usAgcMeanIpd);
    if (0 == *punTempDrvCurrent)
    {
        *punTempDrvCurrent = ((uchLedCurrentMin > 0) ? uchLedCurrentMin : 1);
        GH3X2X_INFO_LOG_PARAM_AGC("CurDrv:%d, Ideal_Ipd:%d, MeanIpd:%d, TempDrv:%d \r\n", uchCurrentDrv, unTemp_Ideal_Ipd, usAgcMeanIpd, *punTempDrvCurrent);
    }

    if( *punTempDrvCurrent < uchLedCurrentMin)
    {
        *punTempDrvCurrent = uchLedCurrentMin;
    }else if(*punTempDrvCurrent > uchLedCurrentMax)
    {
       *punTempDrvCurrent = uchLedCurrentMax; 
    }
}

/**
 * @fn     GU8 GH3x2xAgcCalExtremum( GU8 uchCurrentDrv, GU32 unTempDrvCurrent,  GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC )
 *
 * @brief  Calculate extremum after drv current change
 *
 * @attention   
 *
 * @param[in]    uchCurrentDrv, unTempDrvCurrent
 * @param[out]   punMaxRawdataAfterAGC, punMinRawdataAfterAGC
 *
 * @return
 */
static void GH3x2xAgcCalExtremum(GU8 uchCurrentDrv, GU32 unTempDrvCurrent,  GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC)
{
     if( (*punMinRawdataAfterAGC) == 16777216)
     {
        return;
     }

     if( unTempDrvCurrent != uchCurrentDrv)
     {
         if(uchCurrentDrv==0)
         { 
             GH3X2X_INFO_LOG_PARAM_AGC(" Warning: Default current is zero!! \r\n");  
             return;
         }
         GU32 unRawdataBase = ((  (*punMinRawdataAfterAGC) > GH3X2X_DATA_BASE_LINE )? GH3X2X_DATA_BASE_LINE: ( (*punMinRawdataAfterAGC)-1) ); 
         *punMaxRawdataAfterAGC =  (unTempDrvCurrent)*(*punMaxRawdataAfterAGC- unRawdataBase)/(uchCurrentDrv)  + unRawdataBase; 
         *punMinRawdataAfterAGC =  (unTempDrvCurrent)*(*punMinRawdataAfterAGC- unRawdataBase)/(uchCurrentDrv)  + unRawdataBase; 
     }
}

/**
 * @fn   void GH3x2x_AgcAdjustGainByExtremum( GU32 unTemp_H_Thd, GU32 unTemp_L_Thd, GU8* uchNewGain, GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC  )
 *
 * @brief    adjust gain by extremum  
 *
 * @attention   
 *
 * @param[in]    unTemp_H_Thd unTemp_L_Thd, punMaxRawdataAfterAGC, punMinRawdataAfterAGC 
 * @param[out]   GU8* uchNewGain, 
 *
 * @return   
 */
static void GH3x2x_AgcAdjustGainByExtremum( GU32 unTemp_H_Thd, GU32 unTemp_L_Thd, GU8 uchGainUpLimit, GU8* uchNewGain, GU32* punMaxRawdataAfterAGC, GU32* punMinRawdataAfterAGC)
{
    GU32 unMaxRawdataAfterAGC =  *punMaxRawdataAfterAGC; 
    GU32 unMinRawdataAfterAGC =  *punMinRawdataAfterAGC;
  
    if( unMinRawdataAfterAGC == 16777216)
    {
       return;
    }
    float  times = 1;
    if((*uchNewGain) < uchGainUpLimit)
    {
        times = (float)(g_usGH3x2xTiaGainR[(*uchNewGain)+1])/g_usGH3x2xTiaGainR[(*uchNewGain)];
    }

    GU32 unRawdataBase = ((unMinRawdataAfterAGC > GH3X2X_DATA_BASE_LINE )? GH3X2X_DATA_BASE_LINE: (unMinRawdataAfterAGC)); 
    GU32 unMaxRawdataTemp =  ((unMaxRawdataAfterAGC-unRawdataBase)*times) + unRawdataBase; 

    if(unMaxRawdataAfterAGC > unTemp_H_Thd)
    {
        while((unMaxRawdataAfterAGC > unTemp_H_Thd) && ( (*uchNewGain) > 0 ))
        {
            (*uchNewGain)--;
            times = (float)(g_usGH3x2xTiaGainR[(*uchNewGain)])/g_usGH3x2xTiaGainR[(*uchNewGain)+1];
            unMaxRawdataAfterAGC =  ((unMaxRawdataAfterAGC- unRawdataBase)*times) + unRawdataBase;
            unMinRawdataAfterAGC =  ((unMinRawdataAfterAGC- unRawdataBase)*times ) + unRawdataBase;
        }  
    }else if( unMinRawdataAfterAGC < unTemp_L_Thd )
    {   // In order to prevent nonlinearity from causing repeated adjustments, a certain margin is left in the gain increase judgment.
        while( ( 8*unMaxRawdataTemp < 7*unTemp_H_Thd) && ((*uchNewGain) < uchGainUpLimit) )
        {
            (*uchNewGain)++;
            times = (float)(g_usGH3x2xTiaGainR[(*uchNewGain)])/g_usGH3x2xTiaGainR[(*uchNewGain)-1];
            unMaxRawdataAfterAGC =   ((unMaxRawdataAfterAGC-unRawdataBase)*times) + unRawdataBase;
            unMinRawdataAfterAGC =   ((unMinRawdataAfterAGC-unRawdataBase)*times) + unRawdataBase;

            times = (float)(g_usGH3x2xTiaGainR[(*uchNewGain)+1])/g_usGH3x2xTiaGainR[(*uchNewGain)];
            unMaxRawdataTemp =  ((unMaxRawdataAfterAGC-unRawdataBase)*times) + unRawdataBase;
        }
    }

    *punMaxRawdataAfterAGC = unMaxRawdataAfterAGC;
    *punMinRawdataAfterAGC = unMinRawdataAfterAGC;
}

 /**
 * @fn     GU8 GH3x2xAgcCalDrvCurrentAndGain(GS32 RawdataOffBase, GU32 IdealRawdata, STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo, GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
 *
 * @brief  Calculate DrvCurrent and Gain
 *
 * @attention   
 *
 * @param[in]   usAgcMeanIpd, IdealRawdata, uchMainChnlInfoIndex
 * @param[out]  puchIdealDrvCurrent, puchIdealGain
 *
 * @return  0:DrvCurrent or gain will NOT change  1: DrvCurrent or gain will  change
 */
static GU8 GH3x2xAgcCalDrvCurrentAndGain(GS32 usAgcMeanIpd, GU8 uchSlotCnt,  GU8* puchIdealDrvCurrent, GU8* puchIdealGain)
{
    STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
    STGh3x2xAgcMainChnKeyInfo *pstMainChnlExtralInfo = &g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt];
    STGh3x2xNewAgcMainChnlIdaelAdjInfo *pstAgcMainChnlIdaelAdjInfo = &g_pstGh3x2xAgcMainChnlIdaelAdjInfo[uchSlotCnt];

    GU8 uchGainUpLimit = pstAgcMainChnlInfo->uchTiaGainUpLimit;
    GU8 uchLedCurrentMin = pstAgcMainChnlInfo->uchBakRegLedCurrentMin;
    GU8 uchLedCurrentMax = pstAgcMainChnlInfo->uchBakRegLedCurrentMax;
    GU8 uchCurrentDrv = pstAgcMainChnlInfo->uchCurrentDrv;
    GU8 uchCurrentTiaGain = pstAgcMainChnlInfo->uchCurrentTiaGain;

    GU32 unMaxRawdataAfterAGC = pstMainChnlExtralInfo->unMaxRawdata;     
    GU32 unMinRawdataAfterAGC = pstMainChnlExtralInfo->unMinRawdata;  
    GU8 uchSatFlag = pstMainChnlExtralInfo->Sat_flag; //   rawdata Saturation flag

    GU32 unTempDrvCurrent = uchCurrentDrv;
    GU8 uchNewGain = uchCurrentTiaGain;
    GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag
    GU8 uchCalFinish = 0;

    GU32 unTemp_H_Thd ;      
    GU32 unTemp_L_Thd ;      
    GU32 unTemp_Ideal_Ipd ;
    GU32 unHighIpdThd ;      
    GU32 unLowIpdThd ;

    if(pstAgcMainChnlInfo->uchSpo2ChnlIndex) // is  spo2 slot
    {
       unTemp_Ideal_Ipd = __GH3X_AGC_SPO2_IDEAL_IPD__;
       unHighIpdThd = __GH3X_AGC_SPO2_H_IPD__;
       unLowIpdThd = __GH3X_AGC_SPO2_L_IPD__;
    }else
    {                     
       unTemp_Ideal_Ipd = __GH3X_AGC_GENERAL_IDEAL_IPD__;
       unHighIpdThd = __GH3X_AGC_GENERAL_H_IPD__;
       unLowIpdThd = __GH3X_AGC_GENERAL_L_IPD__;
    }
    GH3X2X_AgcGetThreshold(pstAgcMainChnlInfo->uchSpo2ChnlIndex, &unTemp_H_Thd, &unTemp_L_Thd);

    GH3X2X_INFO_LOG_PARAM_AGC("Drv cal %d: UpLimit = %d, Mean = %d, LastMean = %d\r\n",uchSlotCnt, unTemp_H_Thd, pstMainChnlExtralInfo->unRawdataMean, pstMainChnlExtralInfo->unLastRawdataMean);            
        // reduce rawdata as soon as posible
    if((unTemp_H_Thd < pstMainChnlExtralInfo->unRawdataMean)&&(unTemp_H_Thd < pstMainChnlExtralInfo->unLastRawdataMean)) 
    {             
       uchNewGain = 0;
       unTempDrvCurrent = uchLedCurrentMin;
       uchCalFinish = 1;
       pstAgcMainChnlIdaelAdjInfo->uchIniteFirst = 0;
       pstMainChnlExtralInfo->usAnalysisCnt = 0;
    }
    pstMainChnlExtralInfo->unLastRawdataMean = pstMainChnlExtralInfo->unRawdataMean;
       
    if(0 == uchCalFinish)
    {
        //find new drv current and gain
        if(usAgcMeanIpd < unLowIpdThd)
        {
            if(0==uchSatFlag)
            {
                GH3x2xAgcCalDrvCurrent(usAgcMeanIpd, unTemp_Ideal_Ipd, uchCurrentDrv, uchLedCurrentMin, uchLedCurrentMax, &unTempDrvCurrent);
                GH3X2X_INFO_LOG_PARAM_AGC("Drv cal0: SlotCnt = %d, Ipd = %d, Ideal_Ipd = %d, CurDrv = %d, TempDrv = %d, Max = %d, Min = %d\r\n",uchSlotCnt, usAgcMeanIpd, \
                                     unTemp_Ideal_Ipd, uchCurrentDrv, unTempDrvCurrent, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   

                GH3x2xAgcCalExtremum( uchCurrentDrv, unTempDrvCurrent, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);
                GH3X2X_INFO_LOG_PARAM_AGC("Drv cal0: SlotCnt = %d ,Max = %d, Min = %d\r\n",uchSlotCnt, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   

                GH3x2x_AgcAdjustGainByExtremum( unTemp_H_Thd, unTemp_L_Thd, uchGainUpLimit, &uchNewGain, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);
                GH3X2X_INFO_LOG_PARAM_AGC("Drv cal0: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",uchSlotCnt, unTemp_H_Thd, unTemp_L_Thd, \
                                     uchCurrentTiaGain, uchNewGain, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   
            }
            else if( uchNewGain > 0 )
            {
               uchNewGain--;
            }
        }
        else if( usAgcMeanIpd <= unHighIpdThd )
        {
            GH3x2x_AgcAdjustGainByExtremum( unTemp_H_Thd, unTemp_L_Thd, uchGainUpLimit, &uchNewGain, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);
            GH3X2X_INFO_LOG_PARAM_AGC("Drv cal1: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",uchSlotCnt, unTemp_H_Thd, unTemp_L_Thd, \
                                 uchCurrentTiaGain, uchNewGain, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);  

        }else  ////    usAgcMeanIpd > unTemp_Re_H_Ipd
        {
            GH3x2xAgcCalDrvCurrent(usAgcMeanIpd, unTemp_Ideal_Ipd, uchCurrentDrv, uchLedCurrentMin, uchLedCurrentMax, &unTempDrvCurrent);
            GH3X2X_INFO_LOG_PARAM_AGC("Drv cal2: SlotCnt = %d, Ipd = %d, Ideal_Ipd = %d, CurDrv = %d, TempDrv = %d, Max = %d, Min = %d\r\n",uchSlotCnt, usAgcMeanIpd, \
                     unTemp_Ideal_Ipd, uchCurrentDrv, unTempDrvCurrent, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   

            GH3x2xAgcCalExtremum( uchCurrentDrv, unTempDrvCurrent, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);
            GH3X2X_INFO_LOG_PARAM_AGC("Drv cal2: SlotCnt = %d ,Max = %d, Min = %d\r\n",uchSlotCnt, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   

            GH3x2x_AgcAdjustGainByExtremum( unTemp_H_Thd, unTemp_L_Thd, uchGainUpLimit, &uchNewGain, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);    
            GH3X2X_INFO_LOG_PARAM_AGC("Drv cal2: SlotCnt = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",uchSlotCnt, unTemp_H_Thd, unTemp_L_Thd, \
                     uchCurrentTiaGain, uchNewGain, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);     
        }
    }

    puchIdealDrvCurrent[0] = (GU8)unTempDrvCurrent;
    puchIdealGain[0] = (GU8)uchNewGain;


#if AGC_READ_CHECK
if((puchIdealDrvCurrent[0] == uchLedCurrentMin && puchIdealGain[0] == 0)
   || (puchIdealDrvCurrent[0] == uchLedCurrentMax && puchIdealGain[0] == uchGainUpLimit))
{
    if(puchIdealDrvCurrent[0] != uchCurrentDrv ||  puchIdealGain[0] != uchCurrentTiaGain)
    {
        uchChangeFlag = 0x03;
        pstAgcMainChnlInfo->uchReadCheck = 1;
        GH3X2X_INFO_LOG_PARAM_AGC("SlotCnt:%d Agc read check enable.\r\n", uchSlotCnt);
    }
    else
    {
        if (pstAgcMainChnlInfo->uchReadCheck)
        {
            GU8  uchSlotIdx = ((pstAgcMainChnlInfo->uchSlotAdc >> 2) & 0x0f);
            GU8 ReadGain = GH3x2xGetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx);;
            GU8 ReadDrv = GH3x2xGetDrvCurrent(uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcEn);
            if(ReadGain == uchCurrentTiaGain && ReadDrv == uchCurrentDrv)
            {
                pstAgcMainChnlInfo->uchReadCheck = 0;
                uchChangeFlag = 0;
                GH3X2X_INFO_LOG_PARAM_AGC("SlotCnt:%d,ReadGain:%d,ReadDrv:%d Agc read check right.\r\n", uchSlotCnt, ReadGain, ReadDrv);
            }
            else
            {
                uchChangeFlag = 0x03;
                puchIdealDrvCurrent[0] = uchCurrentDrv;
                puchIdealGain[0] = uchCurrentTiaGain;
                GH3X2X_INFO_LOG_PARAM_AGC("%s: SlotCnt:%d,ReadGain:%d,ReadDrv:%d,CurDrv:%d AGC read check wrong!!!\r\n",
                                          __FUNCTION__, uchSlotCnt, ReadGain, ReadDrv, uchCurrentDrv);
            }
        }
    }
}
else
{
    uchChangeFlag = 0x03;
    pstAgcMainChnlInfo->uchReadCheck = 0;
}
#else
    if(puchIdealDrvCurrent[0] != uchCurrentDrv)
    {
        uchChangeFlag |= 0x01;
    }
    if(puchIdealGain[0] != uchCurrentTiaGain)
    {
        uchChangeFlag |= 0x02;
    }
#endif


    pstMainChnlExtralInfo->Sat_flag = 0;         
    return uchChangeFlag;
}

#if SUB_CHN_PRO_EN
static void GH3x2xAgcSubChnlAdjGainAndClearCnt(GU8 uchSlotIdx, GU8 uchRxEn,GU8 uchCurrentDrv,GU8 uchNewDrvCurrent,STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
    {
        if(uchRxEn&(1 << uchRxCnt))
        {
            for(GU8 uchChnlCnt = 0; uchChnlCnt < g_uchAgcSubChnlNumLimit; uchChnlCnt ++)
            {
                STGh3x2xAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xAgcSubChnlRxInfo[uchChnlCnt]);
                if(pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
                {
                    if(((uchSlotIdx << 2) + uchRxCnt) == pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo)
                    {
                        GU32 unTemp_H_Thd ;      
                        GU32 unTemp_L_Thd ; 
                        GU8 uchCurrentTiaGain = pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain;
                        GU8 uchNewGain = uchCurrentTiaGain;
                        GU32 unMaxRawdataAfterAGC = pstSTGh3x2xNewAgcSubChnlRxInfo->unMaxRawdata;     
                        GU32 unMinRawdataAfterAGC = pstSTGh3x2xNewAgcSubChnlRxInfo->unMinRawdata;  
    
                        GU8 uchSlotCnt = GH3x2xAgcFindSubChnlSlotInfo(uchSlotIdx);
                        GU8 uchGainUpLimit = g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt].uchTiaGainUpLimit;

                        pstSTGh3x2xNewAgcSubChnlRxInfo->usAnalysisCnt = 0;
                        GH3X2X_AgcGetThreshold(g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt].uchSpo2ChnlIndex, &unTemp_H_Thd, &unTemp_L_Thd);          
                        
                        GH3X2X_INFO_LOG_PARAM_AGC("SubChnlAdj: SlotCfg = %d, RxCnt = %d, Max = %d, Min = %d\r\n",uchSlotIdx,uchRxCnt,unMaxRawdataAfterAGC, unMinRawdataAfterAGC);   
                        GH3x2xAgcCalExtremum( uchCurrentDrv, uchNewDrvCurrent, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC);
                        
                        GH3x2x_AgcAdjustGainByExtremum(unTemp_H_Thd, unTemp_L_Thd, uchGainUpLimit, &uchNewGain, &unMaxRawdataAfterAGC, &unMinRawdataAfterAGC); 
                        GH3X2X_INFO_LOG_PARAM_AGC("SubChnlAdj: SlotAdcNo = %d, H_Thd = %d, L_Thd = %d, CurGain = %d, NewGain = %d, Max = %d, Min = %d\r\n",pstSTGh3x2xNewAgcSubChnlRxInfo->uchSlotAdcNo, \
                                             unTemp_H_Thd, unTemp_L_Thd,uchCurrentTiaGain, uchNewGain, unMaxRawdataAfterAGC, unMinRawdataAfterAGC);    

                        if(uchCurrentTiaGain != uchNewGain)
                        {
                            GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + uchRxCnt, uchSlotIdx, uchNewGain);
                            pstSTGh3x2xNewAgcSubChnlRxInfo->uchCurrentGain = uchNewGain;  
                            GH3X2X_StoreRxTiaGainAfterAgc(uchNewGain, uchSlotIdx, uchRxCnt, pSTGh3x2xAgcInfo->uchTiaGainAfterSoftAgc);     
                        }
                    }
                }
            }
        }
    }
}
#endif

static void GH3x2xAgcMainChnlPro(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    GS32 usAgcMeanIpd[GH3X2X_AGC_SLOT_NUM_LIMIT] = {0};
    GU16 usAdjFlag = 0;     // Local variables used as judgments must be initialized to 0
    GU8 uchNewDrvCurrent;
    GU8 uchNewGain;
    GU8 uchChangeFlag = 0;  //bit0: drv current flag  bit1: gain flag

    if(0 == g_uchNewAgcSlotNumLimit)
    {
        return;
    }

     GH3x2xAgcMainChnlKeyValueCal( puchReadFifoBuffer,usFifoLen,usAgcMeanIpd,&usAdjFlag); // caculate Ipdmean and whther AGC
     GH3X2X_INFO_LOG_PARAM_AGC("AGC_Flag: AdjFlag = %d\r\n",usAdjFlag);
     for(GU8 uchSlotCnt = 0; uchSlotCnt < g_uchNewAgcSlotNumLimit; uchSlotCnt ++)
     {
         if( usAdjFlag & (1<<uchSlotCnt)) // need to AGC
         {
             STGh3x2xAgcMainChnlInfo *pstAgcMainChnlInfo = &g_pstGh3x2xAgcMainChnlInfo[uchSlotCnt];
             uchChangeFlag = GH3x2xAgcCalDrvCurrentAndGain(usAgcMeanIpd[uchSlotCnt], uchSlotCnt, &uchNewDrvCurrent,&uchNewGain);
             GU8  uchSlotIdx = ((pstAgcMainChnlInfo->uchSlotAdc >> 2) & 0x0f);
             if(uchChangeFlag&0x02)  //write tia gain to chip
             {
                 GH3X2X_INFO_LOG_PARAM_AGC("[SoftAGC Change Gain] Slotcfg:%d, NewGain:%d, CurGain:%d\r\n", uchSlotIdx, uchNewGain, pstAgcMainChnlInfo->uchCurrentTiaGain);
                 GH3x2xSetAgcReg(GH3X2X_AGC_REG_TIA_GAIN_RX0 + pstAgcMainChnlInfo->uchBakRegAgcSrc, uchSlotIdx, uchNewGain);
                 pstAgcMainChnlInfo->uchCurrentTiaGain = uchNewGain;
                 GH3X2X_StoreRxTiaGainAfterAgc(uchNewGain, uchSlotIdx, pstAgcMainChnlInfo->uchBakRegAgcSrc, pSTGh3x2xAgcInfo->uchTiaGainAfterSoftAgc);
             }
             if(uchChangeFlag&0x01)  //write drv current to chip
             {
                 //change drv0/1
                 GH3X2X_INFO_LOG_PARAM_AGC("[AGC Change Drv] Slotcfg:%d, NewDrv:%d, CurDrv:%d, AgcEn:%d\r\n", uchSlotIdx, uchNewDrvCurrent,
                                           pstAgcMainChnlInfo->uchCurrentDrv, pstAgcMainChnlInfo->uchBakRegAgcEn);
                 GU8 uchCurrentDrv =  pstAgcMainChnlInfo->uchCurrentDrv;
                 pstAgcMainChnlInfo->uchCurrentDrv = uchNewDrvCurrent;

                 if((pstAgcMainChnlInfo->uchBakRegAgcEn)&0x01)
                 {
                     GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV0, uchSlotIdx, uchNewDrvCurrent);
                     GH3X2X_StoreDrvCurrentAfterAgc(uchNewDrvCurrent, uchSlotIdx,0,pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc);
                 }
                 if((pstAgcMainChnlInfo->uchBakRegAgcEn)&0x02)
                 {
                    GH3x2xSetAgcReg(GH3X2X_AGC_REG_LED_CURRENT_DRV1, uchSlotIdx, uchNewDrvCurrent);
                    GH3X2X_StoreDrvCurrentAfterAgc(uchNewDrvCurrent, uchSlotIdx,1,pSTGh3x2xAgcInfo->uchDrvCurrentAfterSoftAgc);
                 }
             
                 #if SUB_CHN_PRO_EN
                 GH3x2xAgcSubChnlAdjGainAndClearCnt(uchSlotIdx, 0x0F,uchCurrentDrv,uchNewDrvCurrent,pSTGh3x2xAgcInfo); //clear all rx analysis cnt and adjast gain 
                 #endif
              }
              if(uchChangeFlag)
              {
                  g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt].uchHaveAgcAtLastReadPrd = 1; // current or  gain change at last time
                  g_pstGh3x2xAgcMainChnlKeyInfo[uchSlotCnt].uchDropFlag = 0;
                  g_NewAgcMainChelFlag |= (1<<uchSlotIdx); // ADjFlag write to CSV file
              }
         }
     }
}


void GH3x2xAgcMeanInfoReset(GU8 uchSlotEnChange)
{
     for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt++)
     {
        if( uchSlotEnChange & (1<<uchSlotCnt)) // SlotEn Change
        {
            for(GU8 uchAgcSlotCnt = 0; uchAgcSlotCnt < GH3X2X_NEW_AGC_SLOT_NUM_LIMIT; uchAgcSlotCnt ++) //// uchAgcSlotCnt
            {
                GU8  uchSlotIdx = ((g_pstGh3x2xAgcMainChnlInfo[uchAgcSlotCnt].uchSlotAdc >> 2) & 0x0f);
                GU8  uchEnable = g_pstGh3x2xAgcMainChnlInfo[uchAgcSlotCnt].uchBakRegAgcEn;
                if (uchEnable)
                {
                   if(uchSlotIdx == uchSlotCnt)
                   {
                       g_pstGh3x2xAgcMainChnlKeyInfo[uchAgcSlotCnt].usAnalysisCnt = 0;
#if GH3X2X_AGC_SUB_CHNL_NUM_LIMIT > 0
                       //clear all rx analysis cnt in subchel slot
                       for(GU8 uchRxCnt = 0; uchRxCnt < 4; uchRxCnt ++)
                       {
                           if(0x0F & (1 << uchRxCnt))
                           {
                               for(GU8 uchChnlCnt = 0; uchChnlCnt < GH3X2X_AGC_SUB_CHNL_NUM_LIMIT; uchChnlCnt ++)
                               {
                                   STGh3x2xAgcSubChnlRxInfo *pstSTGh3x2xNewAgcSubChnlRxInfo = &(g_pstGh3x2xAgcSubChnlRxInfoEntity[uchChnlCnt]);
                                   if(pstSTGh3x2xNewAgcSubChnlRxInfo->uchEn)
                                   {
                                       if(((uchSlotIdx << 2) + uchRxCnt) == pstSTGh3x2xNewAgcSubChnlRxInfo ->uchSlotAdcNo)
                                       {
                                           pstSTGh3x2xNewAgcSubChnlRxInfo ->usAnalysisCnt = 0;
                                       }
                                   }
                               }
                           }
                       }
#endif
                   }
                }
            }
        }
     }
}

/**
 * @fn     void GH3X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen)
 * 
 * @brief  led agc process function
 *
 * @attention   None
 *
 * @param[in]   puchReadFifoBuffer pointer to read fifo data buffer
 * @param[in]   usFifoLen read fifo data buffer length 
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LedAgcProcess(GU8* puchReadFifoBuffer, GU16 usFifoLen, STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    if(usFifoLen==0)
    {
        return;
    }

#if SUB_CHN_PRO_EN
    GH3X2X_AgcSubChnlGainAdj(puchReadFifoBuffer, usFifoLen, pSTGh3x2xAgcInfo);
#endif

    GH3x2xAgcMainChnlPro(puchReadFifoBuffer, usFifoLen, pSTGh3x2xAgcInfo); 
}

/**
 * @fn  void GH3X_LedAgcReset(void)
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
 void GH3X2X_LedAgcReset(void)
{
    GH3X2X_Memset(&g_stSoftAgcCfg, 0, sizeof(STSoftAgcCfg));
    GH3x2xAgcModuleReset();
}

/**
 * @fn     void GH3X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
 *
 * @brief  write AGC parameter
 *
 * @attention   None
 *
 * @param[in]   usRegVal         reg value
 * @param[in]   uchRegPosi         reg posi
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LedAgcPramWrite(GU16 usRegVal, GU16 usRegPosi)
{
    GU16 usByteOffset;
    GU8 uchByteVal;
    GU8 uchByteCnt;
    for(uchByteCnt = 0; uchByteCnt < 2; uchByteCnt ++)
    {
        uchByteVal = ((usRegVal >> (8*uchByteCnt))&0x00FF);
        usByteOffset = (usRegPosi * 2) + uchByteCnt;
        if(usByteOffset < sizeof(g_stSoftAgcCfg))
        {
            GH3X2X_Memcpy(((GU8 *)(&g_stSoftAgcCfg)) + usByteOffset, (GU8*)(&uchByteVal), 1);
        }
    }
}

/**
 * @fn void GH3X_LedAgcInit()
 * 
 * @brief  init led agc
 *
 * @attention   just use to show function support
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
 void GH3X2X_LedAgcInit(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo, STAgcRegConfig* pstAgcRegConfig)
{
    GH3x2xAgcChnlInfoInit(pSTGh3x2xAgcInfo, pstAgcRegConfig);
}


/**
 * @fn void GH3X2X_AgcParaUpdate()
 * 
 * @brief  Update Agc Parameter
 *
 * @attention
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
 void GH3X2X_AgcParaUpdate(STGh3x2xAgcInfo *pSTGh3x2xAgcInfo)
{
    GH3x2xAgcInfoUpdate(pSTGh3x2xAgcInfo);
}


char* GH3X2X_GetSoftAgcVersion(void)
{
    return GH3X2X_SOFT_AGC_VERSION;
}

#endif
/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

