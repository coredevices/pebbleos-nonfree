/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_control_ex.c
 *
 * @brief   gh3x2x extension control functions, using function map like evk
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#include "gh_drv_version.h"
#include "gh_drv_common.h"
#include "gh_drv.h"
#include "gh_drv_interface.h"
#include "gh_drv_control.h"
#include "gh_uprotocol.h"
#include "gh_drv_dump.h"
#include "gh_drv_config.h"
#include "gh_ecg.h"
#include "gh_demo_config.h"
#include "gh_agc.h"

typedef struct 
{
    GU8 usCurrentConfigFlag;
#if 0
    GU16 usInfoConfigArrVer;  /// info: reg config array version
    GU16 usInfoConfigToolVer;   /// info: reg config create tool version
    GU16 usInfoProjectId;  /// info: project id
    GU32 unInfoCreatedTimestamp;  /// info: reg config created timestamp
#endif
    GU32 unGhFuncSupportedAtCurCfg;  //function combination supported by current config
}STGhDrvConfigManger;

STGhDrvConfigManger g_stGhDrvConfigManger;
/// function started bitmap, use for sampling control
GU32 g_unFuncStartedBitmap = 0;

void GhDrvConfigManagerInit(void)
{
    GH3X2X_Memset((void *)(&g_stGhDrvConfigManger), 0, sizeof(g_stGhDrvConfigManger));
}

GU32 GhDrvConfigManagerGetCurFunctionSupprort(void)
{
    return g_stGhDrvConfigManger.unGhFuncSupportedAtCurCfg;
}

GS32 g_nAdtWearonThrd = 0;
extern GU8 g_uchFifoPackageMode;
extern GU8 g_uchGsensorEnable;
/// Cap enable flag
extern GU8 g_uchCapEnable;

/// Temp enable flag
extern GU8 g_uchTempEnable;

GCHAR *GH3X2X_GetVirtualRegVersion(void)
{
    return (GCHAR *)GH3X2X_VIRTUAL_REG_VERSION_STRING;
}

void GH3X2X_InitSensorParameters(void)
{
    g_uchGsensorEnable = 0;
    g_uchCapEnable = 0;
    g_uchTempEnable = 0;
}

/// if support algorithm run simultaneously
//GU8 g_uchAlgoRunMode = GH3X2X_ALGORITHM_RUN_ALONE;

/// @brief get function id via virtual register
/// @param usVirRegAddr virtual register addr
/// @param usVirRegData virtual register value
/// @return fucntion ID , such as: GH3X2X_FUNCTION_ADT
GU32 GhGetFunctionIdViaVirReg(GU16 usVirRegAddr, GU16 usVirRegData)
{
    GU32 unTempFunctionId = 0;
    if(usVirRegAddr >= GH3X2X_REG_IS_VIRTUAL0X2_BIT && usVirRegAddr < 0x2880)  
    {
        if(0 == ((usVirRegAddr - GH3X2X_REG_IS_VIRTUAL0X2_BIT)%GH3X2X_CHNLMAP_OFFSET))  //addr = 0x2000,0x2022,0x2044
        {
            if(usVirRegData)
            {
                unTempFunctionId = ((GU32)1 << ((usVirRegAddr - GH3X2X_REG_IS_VIRTUAL0X2_BIT)/GH3X2X_CHNLMAP_OFFSET));
                GH3X2X_DEBUG_LOG_PARAM("[GhGetFunctionIdViaVirReg]:Got valid function, ID = 0x%x\r\n", unTempFunctionId);
            }
        }
    }
    return unTempFunctionId;
}

/**
 * @fn     GS8 GH3X2X_DecodeRegCfgArr(GU32* punRunMode, const STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen)
 *
 * @brief  Analyze reg cfg array to get the run mode.
 *
 * @attention   None
 *
 * @param[in]   pstRegConfigArr             pointer to reg config array
 * @param[in]   usRegConfigLen              reg config array length
 * @param[out]  punRunMode                  pointer to run mode contained in reg config array
 *
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return parameter error
 */
GS8 GH3X2X_DecodeRegCfgArr(GU32* punRunMode, const STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen)
{
    GS8  chRet = GH3X2X_RET_OK;

    *punRunMode = GH3X2X_NO_FUNCTION;
    //polling every regs in reg cfg array to get functions
    if ((GH3X2X_PTR_NULL != punRunMode) && (GH3X2X_PTR_NULL != pstRegConfigArr) && (0 != usRegConfigLen))
    {
        for (GU16 usArrCnt = 0; usArrCnt < usRegConfigLen ; usArrCnt ++)
        {
            *punRunMode |= GhGetFunctionIdViaVirReg(pstRegConfigArr[usArrCnt].usRegAddr, pstRegConfigArr[usArrCnt].usRegData);
        }
    }
    else
    {
        chRet = GH3X2X_RET_PARAMETER_ERROR;
    }
    return chRet;
}

#if 0
/**
 * @fn     GU16 GH3X2X_ReadSwConfigWithVirtualReg(GU16 usVirtualRegAddr)
 *
 * @brief  Read software config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[out]  None
 *
 * @return  virtual reg val
 */
GU16 GH3X2X_ReadSwConfigWithVirtualReg(GU16 usVirtualRegAddr)
{
    GU16 usVirtualRegData = 0;
    GU16 usValIndex = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (usVirtualRegAddr < GH3X2X_VINFO_END_REG_ADDR) // config arr info
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_VINFO_CFG_VER_REG_ADDR:
            usVirtualRegData = g_stGhDrvConfigManger.usInfoConfigArrVer;
            break;
        case GH3X2X_VINFO_TOOL_VER_REG_ADDR:
            usVirtualRegData = g_stGhDrvConfigManger.usInfoConfigToolVer;
            break;
        case GH3X2X_VINFO_PROJ_ID_REG_ADDR:
            usVirtualRegData = g_stGhDrvConfigManger.usInfoProjectId;
            break;
        case GH3X2X_VINFO_TIMSTAMP_L_REG_ADDR:
            usVirtualRegData = GH3X2X_GET_LOW_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp);
            break;
        case GH3X2X_VINFO_TIMSTAMP_H_REG_ADDR:
            usVirtualRegData = GH3X2X_GET_HIGH_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp);
            break;
        default: // do nothing
            break;
        }
    } // end of if (usVirtualRegAddr < GH3X2X_VINFO_END_REG_ADDR) 
    else if (usVirtualRegAddr < GH3X2X_VCHM_END_REG_ADDR) // config channel map
    {
        if (usVirtualRegAddr < GH3X2X_VCHM_HBA_CH_END_REG_ADDR) // hba
        {
            switch (usVirtualRegAddr)
            {
            case GH3X2X_VCHM_HBA_CNT_REG_ADDR:
                usVirtualRegData = g_uchHbaChannelMapCnt;
                break;
            default:
                usValIndex = (GU16)(usVirtualRegAddr - GH3X2X_VCHM_HBA_CH0_1_REG_ADDR);
                if (usValIndex < GH3X2X_HBA_CHANNEL_MAP_MAX_CNT)
                {
                    usVirtualRegData = GH3X2X_MAKEUP_WORD(g_uchHbaChannelMapArr[usValIndex + 1], \
                                                        g_uchHbaChannelMapArr[usValIndex]);
                }
                break;
            }
        }
        else if (usVirtualRegAddr < GH3X2X_VCHM_SPO2_CH_END_REG_ADDR) // spo2
        {
            switch (usVirtualRegAddr)
            {
            case GH3X2X_VCHM_SPO2_CNT_REG_ADDR:
                usVirtualRegData = g_uchSpo2ChannelMapCnt;
                break;
            default:
                usValIndex = (GU16)(usVirtualRegAddr - GH3X2X_VCHM_SPO2_CH0_1_REG_ADDR);
                if (usValIndex < GH3X2X_SPO2_CHANNEL_MAP_MAX_CNT)
                {
                    usVirtualRegData = GH3X2X_MAKEUP_WORD(g_uchSpo2ChannelMapArr[usValIndex + 1], \
                                                        g_uchSpo2ChannelMapArr[usValIndex]);
                }
                break;
            }
        }
        else
        {
            switch (usVirtualRegAddr)
            {
            case GH3X2X_VCHM_ECG_CH0_REG_ADDR:
                usVirtualRegData = g_uchEcgChannelMap;
                break;
            case GH3X2X_VCHM_ADT_CNT_REG_ADDR:
                usVirtualRegData = g_uchAdtChannelMapCnt;
                break;
            case GH3X2X_VCHM_ADT_CH0_1_REG_ADDR:
                usVirtualRegData = GH3X2X_MAKEUP_WORD(g_uchAdtChannelMapArr[1], g_uchAdtChannelMapArr[0]);
                break;
            case GH3X2X_VCHM_ADT_CH2_3_REG_ADDR:
                usVirtualRegData = GH3X2X_MAKEUP_WORD(g_uchAdtChannelMapArr[3], g_uchAdtChannelMapArr[2]);
                break;
            default: // do nothing
                break;
            } // end of switch (usVirtualRegAddr)
        }
    } // end of else if (usVirtualRegAddr < GH3X2X_VCHM_END_REG_ADDR)
    else if (usVirtualRegAddr < GH3X2X_VDUMP_END_REG_ADDR) // dump module
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_VDUMP_DUMP_MODE_REG_ADDR:
            break;
        case GH3X2X_VDUMP_BG_LEVEL_REG_ADDR:
            break;
        default: // do nothing
            break;
        }
    }
    else if (usVirtualRegAddr < GH3X2X_VECG_END_REG_ADDR) // ecg module
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_VECG_0_REG_ADDR:
        case GH3X2X_VECG_1_REG_ADDR:
        case GH3X2X_VECG_2_REG_ADDR:
            usValIndex = (usVirtualRegAddr - GH3X2X_VECG_0_REG_ADDR) / GH3X2X_REG_ADDR_SIZE;
            break;
        default: // do nothing
            break;
        }
    } // end of else if (usVirtualRegAddr < GH3X2X_VECG_END_REG_ADDR)
    else if (usVirtualRegAddr < GH3X2X_VAGC_END_REG_ADDR) // agc config
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_VAGC_AGC_BG_SLOT_ENABLE_REG_ADDR:
            break;
        case GH3X2X_VAGC_AMB_SLOT_CTRL_REG_ADDR:
            break;
        case GH3X2X_VAGC_RES_REG_ADDR:
            break;
        default: // do nothing
            break;
        }
    } // end of else if (usVirtualRegAddr < GH3X2X_VAGC_END_REG_ADDR)
    return usVirtualRegData;
}
#endif

void GH3X2X_WriteFsConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU8 uchFuncOffsetIndex = (GU8)(usVirtualRegAddr - GH3X2X_ADT_FS_ADDR) / 2;
    if(g_pstGh3x2xFrameInfo[uchFuncOffsetIndex] != 0)
    {
        g_pstGh3x2xFrameInfo[uchFuncOffsetIndex] ->pstFunctionInfo ->usSampleRate = GH3X2X_GET_LOW_WORD_FROM_DWORD(usVirtualRegValue);
    }
}

void GH3X2X_WriteChnlMapConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex = 0;
    usVirtualRegAddr = GH3X2X_GET_REG_REAL_ADRR(usVirtualRegAddr);
    switch (usVirtualRegAddr % GH3X2X_CHNLMAP_OFFSET)
    {
    case 0:
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[(GU8)(usVirtualRegAddr / GH3X2X_CHNLMAP_OFFSET)],GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue));
        break;
    default:
        usValIndex = (GU16)((usVirtualRegAddr % GH3X2X_CHNLMAP_OFFSET) - 2);
        if (usValIndex < GH3X2X_CHANNEL_MAP_MAX_CH)
        {
            GH3x2xSetFunctionChnlMap(g_pstGh3x2xFrameInfo[(GU8)(usVirtualRegAddr / GH3X2X_CHNLMAP_OFFSET)],usValIndex + 1,GH3X2X_GET_HIGH_BYTE_FROM_WORD(usVirtualRegValue));
            GH3x2xSetFunctionChnlMap(g_pstGh3x2xFrameInfo[(GU8)(usVirtualRegAddr / GH3X2X_CHNLMAP_OFFSET)],usValIndex,GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue));
        }
        break;
    }
}

#if __SUPPORT_SOFT_AGC_CONFIG__
void GH3X2X_WriteAgcConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex;
    switch (usVirtualRegAddr)
    {
    case GH3X2X_AGC_BG_CANCEL_ADJUST_SLOT_EN_ADDR:
    case GH3X2X_AGC_AMB_SLOT_CTRL_ADDR:
    case GH3X2X_AGC_CONFIG_RES:
    case GH3X2X_AGC_GAIN_LIMIT_ADDR:
    case GH3X2X_AGC_TRIG_THD_H_LSB_16_ADDR:
    case GH3X2X_AGC_TRIG_THD_H_MSB_16_ADDR:
    case GH3X2X_AGC_TRIG_THD_L_LSB_16_ADDR:
    case GH3X2X_AGC_TRIG_THD_L_MSB_16_ADDR:
    case GH3X2X_AGC_RESTRAIN_THD_H_LSB_16_ADDR:
    case GH3X2X_AGC_RESTRAIN_THD_H_MSB_16_ADDR:
    case GH3X2X_AGC_RESTRAIN_THD_L_LSB_16_ADDR:
    case GH3X2X_AGC_RESTRAIN_THD_L_MSB_16_ADDR:
        usValIndex = (GU16)((usVirtualRegAddr - GH3X2X_AGC_BG_CANCEL_ADJUST_SLOT_EN_ADDR) / GH3X2X_REG_ADDR_SIZE);
    #if __SUPPORT_SOFT_AGC_CONFIG__
        GH3X2X_LedAgcPramWrite(usVirtualRegValue, usValIndex);
    #endif
        break;
    default: // do nothing
        break;
    }
}
#endif

#if 1
void GhConfigSwModuleConfigInfoParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    #if 0    //this info is not using, delet them to save code
    case GH3X2X_CFG_VER_ADDR:
        g_stGhDrvConfigManger.usInfoConfigArrVer = usVirtualRegValue;
        break;
    case GH3X2X_CFG_TOOL_VER_ADDR:
        g_stGhDrvConfigManger.usInfoConfigToolVer = usVirtualRegValue;
        break;
    case GH3X2X_PROJECT_ID_ADDR:
        g_stGhDrvConfigManger.usInfoProjectId = usVirtualRegValue;
        break;
    case GH3X2X_TIMESTAMP_L_ADDR:
        g_stGhDrvConfigManger.unInfoCreatedTimestamp = GH3X2X_MAKEUP_DWORD2(GH3X2X_GET_HIGH_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp),
                                                       usVirtualRegValue);
        break;
    case GH3X2X_TIMESTAMP_H_ADDR:
        g_stGhDrvConfigManger.unInfoCreatedTimestamp = GH3X2X_MAKEUP_DWORD2(usVirtualRegValue,
                                                       GH3X2X_GET_LOW_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp));
        break;
    #endif
    case GH3X2X_REINIT_PARAM_ADDR:
        GH3X2X_ReinitAllSwModuleParam(usVirtualRegValue);
        break;
    default: // do nothing
        break;
        }
}

void GhConfigSwModuleDumpInit(void)
{
    GH3X2X_DumpModeSet(0); 
}

void GhConfigSwModuleDumpParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case GH3X2X_PPG_DUMP_MODE_ADDR:
        GH3X2X_DumpModeSet(usVirtualRegValue);
        break;
    case GH3X2X_BG_LEVEL_SET_ADDR:
        //GH3X2X_BgLevelSet(usVirtualRegValue);
        break;
    default: // do nothing
        break;
    }
}

void GhConfigSwModuleFifoInfoParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case GH3X2X_FIFO_PACKAGE_SEND_MODE_ADDR:
        g_uchFifoPackageMode = (GU8)usVirtualRegValue;
        break;
    default: // do nothing
        break;
    }
}

void GhConfigSwModuleGsParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case GH3X2X_GSENSOR_CTRL_ADDR:
        g_uchGsensorEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        break;
    default: // do nothing
        break;
    }
}

    #if __CAP_ENABLE__
void GhConfigSwModuleCapParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
    case GH3X2X_CAP_CTRL_ADDR:
        g_uchCapEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
        break;
    default: // do nothing
        break;
    }
}
    #endif

    #if __TEMP_ENABLE__
void GhConfigSwModuleTempParmSet(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (usVirtualRegAddr)
    {
        case GH3X2X_TEMP_CTRL_ADDR:
            g_uchTempEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
        default: // do nothing
            break;
    }
}
    #endif

const STGhVirRegSwModuleInfo g_stGhVirRegSwModuleInfo[] = 
{
    /**                   usAddrStart                 usAddrEnd                      ParmSet  *****/
    /*config info*/       {GH3X2X_TOP_INFO_ADDR,       GH3X2X_TOP_INFO_ADDR_END,      GhConfigSwModuleConfigInfoParmSet},
    /*dump*/              {GH3X2X_DUMP_CFG_ADDR,       GH3X2X_DUMP_CFG_ADDR_END,      GhConfigSwModuleDumpParmSet},
    /*fifo info*/         {GH3X2X_FIFO_CTRL_ADDR,      GH3X2X_FIFO_CTRL_ADDR_END,     GhConfigSwModuleFifoInfoParmSet},
    /*gsensor*/           {GH3X2X_G_SENSOR_CFG_ADDR,   GH3X2X_G_SENSOR_CFG_ADDR_END,  GhConfigSwModuleGsParmSet},
    #if __SUPPORT_SOFT_AGC_CONFIG__ 
    /*agc*/               {GH3X2X_SOFT_AGC_PARA_ADDR,  GH3X2X_SOFT_AGC_PARA_ADDR_END, GH3X2X_WriteAgcConfigWithVirtualReg},
    #endif
    #if __CAP_ENABLE__  
    /*cap*/               {GH3X2X_CAP_CFG_ADDR,        GH3X2X_CAP_CFG_ADDR_END,       GhConfigSwModuleCapParmSet},
    #endif
    #if __TEMP_ENABLE__ 
    /*temp*/              {GH3X2X_TEMP_CFG_ADDR,       GH3X2X_TEMP_CFG_ADDR_END,      GhConfigSwModuleTempParmSet},
    #endif    
    /*ChnlMap*/           {GH3X2X_CHNL_MAP_ADDR,       GH3X2X_CHNL_MAP_ADDR_END,      GH3X2X_WriteChnlMapConfigWithVirtualReg},
    /*Fs*/                {GH3X2X_FS_PARA_ADDR,        GH3X2X_FS_PARA_ADDR_END,       GH3X2X_WriteFsConfigWithVirtualReg},
};

void GH3X2X_WriteSwConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    for(GU8 ubModuleCnt = 0; ubModuleCnt < sizeof(g_stGhVirRegSwModuleInfo)/sizeof(STGhVirRegSwModuleInfo); ubModuleCnt ++)
    {
        if((usVirtualRegAddr >= g_stGhVirRegSwModuleInfo[ubModuleCnt].usAddrStart)&&(usVirtualRegAddr < g_stGhVirRegSwModuleInfo[ubModuleCnt].usAddrEnd))
        {
            if(g_stGhVirRegSwModuleInfo[ubModuleCnt].ParmSet)
            {
                g_stGhVirRegSwModuleInfo[ubModuleCnt].ParmSet(usVirtualRegAddr, usVirtualRegValue);
            }
        }
    }
}
#else
/**
 * @fn     GS8 GH3X2X_WriteSwConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write software param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_WriteSwConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    if (usVirtualRegAddr < GH3X2X_SLOT_LEDDRV_ADDR) // config arr info         0x1F
    {
        switch (usVirtualRegAddr)
        {
    #if 0
        case GH3X2X_CFG_VER_ADDR:
            g_stGhDrvConfigManger.usInfoConfigArrVer = usVirtualRegValue;
            break;
        case GH3X2X_CFG_TOOL_VER_ADDR:
            g_stGhDrvConfigManger.usInfoConfigToolVer = usVirtualRegValue;
            break;
        case GH3X2X_PROJECT_ID_ADDR:
            g_stGhDrvConfigManger.usInfoProjectId = usVirtualRegValue;
            break;
        case GH3X2X_TIMESTAMP_L_ADDR:
            g_stGhDrvConfigManger.unInfoCreatedTimestamp = GH3X2X_MAKEUP_DWORD2(GH3X2X_GET_HIGH_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp),
                                                                usVirtualRegValue);
            break;
        case GH3X2X_TIMESTAMP_H_ADDR:
            g_stGhDrvConfigManger.unInfoCreatedTimestamp = GH3X2X_MAKEUP_DWORD2(usVirtualRegValue,
                                                            GH3X2X_GET_LOW_WORD_FROM_DWORD(g_stGhDrvConfigManger.unInfoCreatedTimestamp));
            break;
    #endif
        case GH3X2X_REINIT_PARAM_ADDR:
            GH3X2X_ReinitAllSwModuleParam(usVirtualRegValue);
            break;
        default: // do nothing
            break;
        }
    } // end of if (usVirtualRegAddr < GH3X2X_VINFO_END_REG_ADDR) 
    else if (usVirtualRegAddr < GH3X2X_DUMP_CFG_ADDR) // slot drv map module
    {
        //TO BE DONE
    }
    else if (usVirtualRegAddr < GH3X2X_FIFO_CTRL_ADDR) // dump module
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_PPG_DUMP_MODE_ADDR:
            GH3X2X_DumpModeSet(usVirtualRegValue);
            break;
        case GH3X2X_BG_LEVEL_SET_ADDR:
            //GH3X2X_BgLevelSet(usVirtualRegValue);
            break;
        default: // do nothing
            break;
        }
    } // end of else if (usVirtualRegAddr < GH3X2X_VDUMP_END_REG_ADDR)
    else if (usVirtualRegAddr < GH3X2X_G_SENSOR_CFG_ADDR) // fifo info
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_FIFO_PACKAGE_SEND_MODE_ADDR:
            g_uchFifoPackageMode = usVirtualRegValue;
            break;
        default: // do nothing
            break;
        }
    }
    else if (usVirtualRegAddr < GH3X2X_SOFT_AGC_PARA_ADDR)
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_GSENSOR_CTRL_ADDR:
            g_uchGsensorEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
        default: // do nothing
            break;
        }
    } // end of else if (usVirtualRegAddr < GH3X2X_VGS_END_REG_ADDR)
    else if (usVirtualRegAddr < GH3X2X_CAP_CFG_ADDR) // agc config
    {
    #if __SUPPORT_SOFT_AGC_CONFIG__
        GH3X2X_WriteAgcConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
    #endif

    } // end of else if (usVirtualRegAddr < GH3X2X_VAGC_END_REG_ADDR)
    else if (usVirtualRegAddr < GH3X2X_TEMP_CFG_ADDR)
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_CAP_CTRL_ADDR:
            g_uchCapEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
        default: // do nothing
            break;
        }
    } 
    else if (usVirtualRegAddr < GH3X2X_CHNL_MAP_ADDR)
    {
        switch (usVirtualRegAddr)
        {
        case GH3X2X_TEMP_CTRL_ADDR:
            g_uchTempEnable = GH3X2X_GET_LOW_BYTE_FROM_WORD(usVirtualRegValue);
            break;
        default: // do nothing
            break;
        }
    } 
    else if (usVirtualRegAddr < GH3X2X_FS_PARA_ADDR)
    {
        GH3X2X_WriteChnlMapConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
    }
    else if (usVirtualRegAddr < GH3X2X_REG_IS_VIRTUAL0X3_BIT)
    {
        GH3X2X_WriteFsConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
    }
}
#endif

#if __FUNC_TYPE_ECG_ENABLE__
void GH3X2X_WriteEcgDrvConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GU16 usValIndex;
    switch (usVirtualRegAddr)
    {
    case GH3X2X_ECG_SETTING0_ADDR:
    case GH3X2X_ECG_SETTING1_ADDR:
    case GH3X2X_ECG_SETTING2_ADDR:
        usValIndex = (usVirtualRegAddr - GH3X2X_ECG_SETTING0_ADDR) / GH3X2X_REG_ADDR_SIZE;
        GH3X2X_SlaverSoftLeadPramSet(usVirtualRegValue, usValIndex);   
        break;
    case GH3X2X_ECG_SETTING3_ADDR:
        GH3x2xSetEcgOutputFs(usVirtualRegValue);
        break;
    default: // do nothing
        UNUSED_VAR(usVirtualRegAddr);
        UNUSED_VAR(usVirtualRegValue);
        break;
    }
}
#endif

void GH3X2X_WriteDrvConfigWithVirtualReg(GU32 unFunctionID, GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    switch (unFunctionID)
    {
    case GH3X2X_FUNC_OFFSET_ADT:
#if __FUNC_TYPE_ECG_ENABLE__   //only for ADT by using ecg-electronic
        GH3X2X_WriteAdtDrvConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
#endif
        break;
    case GH3X2X_FUNC_OFFSET_ECG:
#if __FUNC_TYPE_ECG_ENABLE__
        GH3X2X_WriteEcgDrvConfigWithVirtualReg(usVirtualRegAddr, usVirtualRegValue);
#endif
        break;
    default:
        break;
    }
}

/**
 * @fn     GS8 GH3X2X_WriteAlgorithmConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write algorithm param config with virtual reg
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_WriteFunctionConfigWithVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GU32 unFunctionID = (GU32)((usVirtualRegAddr - GH3X2X_REG_IS_VIRTUAL0X3_BIT) / GH3X2X_VREG_FUNCTION_OFFSET);
    if (usVirtualRegAddr >= GH3X2X_ADT_DRV_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * unFunctionID &&\
        usVirtualRegAddr < GH3X2X_ADT_ALG_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * unFunctionID)
    {
        GH3X2X_WriteDrvConfigWithVirtualReg(unFunctionID, usVirtualRegAddr, usVirtualRegValue);
    }
    else if (usVirtualRegAddr >= GH3X2X_ADT_ALG_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * unFunctionID &&\
             usVirtualRegAddr < GH3X2X_ADT_DRV_CFG_ADDR + GH3X2X_VREG_FUNCTION_OFFSET * (unFunctionID + 1))
    {
        /* call hook */
        HOOK_FUNC_CALL(g_pGh3x2xWriteAlgConfigWithVirtualRegHookFunc, (usVirtualRegAddr, usVirtualRegValue) );
    }
}

/**
 * @fn     GS8 GH3X2X_WriteVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
 *
 * @brief  Write virtual reg val, for software param config
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[in]   usVirtualRegValue       virtual reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_WriteVirtualReg(GU16 usVirtualRegAddr, GU16 usVirtualRegValue)
{
    if (usVirtualRegAddr < GH3X2X_REG_IS_VIRTUAL0X3_BIT)
    {
        if (usVirtualRegAddr > GH3X2X_TOP_INFO_ADDR && 0 == g_stGhDrvConfigManger.usCurrentConfigFlag)
        {
            GH3X2X_DEBUG_LOG_PARAM("[%s]:Config version error!!!\r\n", __FUNCTION__);
        }
        else
        {
            GH3X2X_SW_CONFIG_WRITE(usVirtualRegAddr, usVirtualRegValue);
        }
    }
    else if (usVirtualRegAddr < GH3X2X_FINISH_FLAG_ADDR)
    {
        GH3X2X_FUNCTION_CONFIG_WRITE(usVirtualRegAddr, usVirtualRegValue);
    }
    
    switch (usVirtualRegAddr)
    {
        case GH3X2X_END_FLAG_ADDR: // virtual reg write finish
            if (g_stGhDrvConfigManger.usCurrentConfigFlag == 1)
            {
                g_stGhDrvConfigManger.usCurrentConfigFlag = 0;
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("[%s]:Config version error!!!\r\n", __FUNCTION__);
            }
            GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WriteVirtualReg]: unGhFuncSupportedAtCurCfg = 0x%x.\r\n", g_stGhDrvConfigManger.unGhFuncSupportedAtCurCfg);
            HOOK_FUNC_CALL(g_pGh3x2xConfigSetStopHookFunc, ());
            break;
        case GH3X2X_TOP_INFO_ADDR:
            GhDrvConfigManagerInit();
            HOOK_FUNC_CALL(g_pGh3x2xConfigSetStartHookFunc, ());
            g_stGhDrvConfigManger.usCurrentConfigFlag = 1;
            GH3X2X_InitSensorParameters();
            GH3X2X_SetConfigFuncMode(0);
            break;
        default:
            break;
    }
    g_stGhDrvConfigManger.unGhFuncSupportedAtCurCfg |= GhGetFunctionIdViaVirReg(usVirtualRegAddr, usVirtualRegValue);
}

/**
 * @fn     GU16 GH3X2X_ReadVirtualReg(GU16 usVirtualRegAddr)
 *
 * @brief  Read virtual reg val, for software param config
 *
 * @attention   Virtual reg addr has del control bits, so reg addr is [0:11] valid.
 *
 * @param[in]   usVirtualRegAddr        virtual reg addr
 * @param[out]  None
 *
 * @return  virtual reg val
 */
GU16 GH3X2X_ReadVirtualReg(GU16 usVirtualRegAddr)
{
    UNUSED_VAR(usVirtualRegAddr);
    GU16 usVirtualRegData = 0;
#if GH3X2X_SUPPORT_READ_BACK_VIRTUAL_REG
#endif
    return usVirtualRegData;
}

void GH3X2X_CheckChipModel(void)
{
    GU16 usRegVal = 0;
    usRegVal = GH3X2X_ReadReg(GH3X2X_EFUSE_CTRL_EFUSE1_AUTOLOAD_0_ADDR);
    //if chip is 3026,disable rx2 and rx3
    if(((usRegVal >> 5)&0x07) == 1)
    {
        for(GU8 uchSlotNo = 0; uchSlotNo < 8; uchSlotNo++)
        {
            GH3X2X_WriteRegBitField((GU16)(GH3X2X_SLOT0_CTRL_0_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo),4,5, 0);
        }
    }
}

/**
 * @fn     GS8 GH3X2X_LoadNewRegConfigArr(const STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen)
 *
 * @brief  Load new gh3x2x reg config array
 *
 * @attention   If reg val don't need verify, should set reg addr bit 12;
 *              If reg is virtual reg, should set reg addr bit 13;
 *              e.g.      need config reg 0x0000: 0x0611
 *                        {0x0000, 0x0611} //verify write by read reg
 *                        {0x1000, 0x0611} //don't need verify write val
 *                        {0x2000, 0x0611} //set virtual reg
 *
 * @param[in]   pstRegConfigArr       pointer to the reg struct array
 * @param[in]   usRegConfigLen        reg struct array length
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_COMM_ERROR       gh3x2x communicate error
 */
GS8 GH3X2X_LoadNewRegConfigArr(const STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen , GU8 uchChipIndex, STAgcRegConfig* pstAgcRegConfig)
{
    GS8 chRet = GH3X2X_RET_OK;
    GU16 usIndex = 0;
    GU16 usReadRegVal = 0;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
 
    if ((pstRegConfigArr != GH3X2X_PTR_NULL) && (usRegConfigLen != 0))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        for (usIndex = 0; usIndex < usRegConfigLen; usIndex++) // write GH3X2X reg.
        {
            switch (GH3X2X_GET_BIT_IS_SET(pstRegConfigArr[usIndex].usRegAddr))
            {
            case 0:
                GH3X2X_WriteReg(pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
                usReadRegVal = GH3X2X_ReadReg(pstRegConfigArr[usIndex].usRegAddr);
                if (usReadRegVal != pstRegConfigArr[usIndex].usRegData)
                {
                    GH3X2X_DEBUG_LOG_PARAM("[%s]:reg verify error! addr:0x%.4x,w_val:0x%.4x,r_val:0x%.4x\r\n", __FUNCTION__,
                                            GH3X2X_GET_REG_REAL_ADRR(pstRegConfigArr[usIndex].usRegAddr),
                                            pstRegConfigArr[usIndex].usRegData, usReadRegVal);
                    chRet = GH3X2X_RET_COMM_ERROR;
                }
#if __SUPPORT_SOFT_AGC_CONFIG__                
                GH3x2xAgcRegvalGet(pstAgcRegConfig, pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
#endif                
                break;
            default:
                if(0 == uchChipIndex)
                {
                    /* write & verify virtual reg, if verify error, only log error, shouldn't return error */
                    GH3X2X_WriteVirtualReg(pstRegConfigArr[usIndex].usRegAddr, pstRegConfigArr[usIndex].usRegData);
                }
#if 0
                usReadRegVal = GH3X2X_ReadVirtualReg(pstRegConfigArr[usIndex].usRegAddr);
                if (usReadRegVal != pstRegConfigArr[usIndex].usRegData)
                {
                    GH3X2X_DEBUG_LOG_PARAM("[%s]:vreg reg verify error! addr:0x%.4x,w_val:0x%.4x,r_val:0x%.4x\r\n", __FUNCTION__,
                                            pstRegConfigArr[usIndex].usRegAddr,
                                            pstRegConfigArr[usIndex].usRegData, usReadRegVal);
                }
#endif
                break;
            }
//// all register send
            if(GH3X2X_END_FLAG_ADDR == pstRegConfigArr[usIndex].usRegAddr)
            {
#if __SUPPORT_SOFT_AGC_CONFIG__
               GH3X2X_LedAgcInit(g_pstGh3x2xAgcInfo, pstAgcRegConfig);
#endif  
            }
            
        } // end of for (usIndex = 0; usIndex < usRegConfigLen; usIndex++)
        GH3X2X_WAIT_CHIP_DSLP();
    } // end of if ((pstRegConfigArr != GH3X2X_PTR_NULL) && (usRegConfigLen != 0))
    GH3X2X_CheckChipModel();
     
    return chRet;
}

void GH3X2X_SetCurrentConfigFlag(GU8 value)
{
    g_stGhDrvConfigManger.usCurrentConfigFlag = value;
}
