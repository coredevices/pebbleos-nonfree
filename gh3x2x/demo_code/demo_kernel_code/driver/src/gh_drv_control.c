/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_control.c
 *
 * @brief   gh3x2x control functions
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
#include "gh_ecg.h"
#include "gh_drv_dump.h"
#include "gh_drv_config.h"
#include "gh_zip.h"
#include "gh_demo.h"
#include "gh_demo_config.h"
#include "gh_demo_inner.h"
#include "gh_demo_version.h"
#include "gh_drv.h"
#include "gh_multi_sen_pro.h"
#include "gh_agc.h"

GU8 g_uchFifoPackageID = 0xFF;
GU8 g_uchFifoPackageMode = 0;
//GU8 *gpuchSendFifoPackage = GH3X2X_PTR_NULL;
GU32 g_unSendFifoPackageCnt = 0;

//function set in config
GU32 g_unConfigFuncMode = 0;

GU8 g_uchAlgoEnableFlag = 1;

GU8 g_uchSingleChipModeEnableFlag = 0; //1��spi/iic only operate one chip

GU8 g_uchOddEvenChangeFlag = 1;

/// soft event
GU32 g_unSoftEvent = 0;  //G202008231001 wanpeng

/// G sensor enable flag
GU8 g_uchGsensorEnable = 0;

/// Cap enable flag
GU8 g_uchCapEnable = 0;

/// Temp enable flag
GU8 g_uchTempEnable = 0;

GU8 g_uchResetFromProtocolFlag = 0;

/// gh3x2x status
GU8 g_uchGh3x2xStatus = GH3X2X_STATUS_NO_INIT;

/// current fifo water line
GU16 g_usCurrentFiFoWaterLine = 0;

/// init hook func ptr
pfnNormalHookFunc g_pGh3x2xInitHookFunc = GH3X2X_PTR_NULL;

/// start hook func ptr
pfnNormalHookFunc g_pGh3x2xStartHookFunc = GH3X2X_PTR_NULL;

/// stop hook func ptr
pfnNormalHookFunc g_pGh3x2xStopHookFunc = GH3X2X_PTR_NULL;

/// get rawdata hook func ptr
pfnGetRawdataHookFunc g_pGh3x2xGetRawdataHookFunc = GH3X2X_PTR_NULL;

/// get struct hook func ptr
pfnGetIoStructInfo g_pGh3x2xGetIoStructInfoHookFunc = GH3X2X_PTR_NULL;

/// set algo config hook func ptr
pfnWriteAlgConfigWithVirtualReg g_pGh3x2xWriteAlgConfigWithVirtualRegHookFunc = GH3X2X_PTR_NULL;

/// config start set hook func ptr
pfnNormalHookFunc g_pGh3x2xConfigSetStartHookFunc = GH3X2X_PTR_NULL;

/// config stop set hook func ptr
pfnNormalHookFunc g_pGh3x2xConfigSetStopHookFunc = GH3X2X_PTR_NULL;

/// start hook func ptr
pfnNormalHookFunc g_pGh3x2xResetByProtocolHookFunc = GH3X2X_PTR_NULL;

/// read int pin status func ptr
pfnReadPinStatus g_pGh3x2xReadIntPinStatusFunc = GH3X2X_PTR_NULL;

/// read reset pin status func ptr
pfnReadPinStatus g_pGh3x2xReadResetPinStatusFunc = GH3X2X_PTR_NULL;

/// read spcs pin status func ptr
pfnReadPinStatus g_pGh3x2xReadSpcsPinStatusFunc = GH3X2X_PTR_NULL;

/// read spdo pin status func ptr
pfnReadPinStatus g_pGh3x2xReadSpdoPinStatusFunc = GH3X2X_PTR_NULL;

/// set reset pin level spi
pfnSetPinLevel g_pGh3x2xResetPinLevelControlFunc = GH3X2X_PTR_NULL;

#if     GH3X2X_PMU_FIFO_POWER_CTRL_ENABLED

/// fifo power ctrl fifo control reg cache var
GU16 g_pusPmuFifoModuleCtrlVal[2] = {0x0000, 0x0000};

/// for fifo power ctrl cache status
GU8 g_puchPmuFifoModuleCtrlCacheStatus[2] = {0,0};

/// max rawdata num read from fifo every time
GU16 g_usMaxNumReadFromFifo = GH3X2X_FIFO_DATA_BYTES_MAX_LEN;

#endif

GU8 g_uchActiveChipResetFlag = 0;   //1: user have done chip reset(soft reset and hardwear reset) actively   gh3x2x_init will clear it 
GU8 g_uchChipResetRecoveringFlag = 0;  //0: is not in chip reset recovering flag  1: is in chip reset recovering

extern GU32 g_unFuncStartedBitmap;
extern GU8 g_uchGhx2xChipSleepFlag;
extern GU8 g_uchChipDivZeroFlag;
/**
 * @fn     void GH3X2X_RegisterHookFunc(void (*pInitHookFunc)(void),
 *                            void (*pStartHookFunc)(void),
 *                           void (*pStopHookFunc)(void),
 *                            void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen))
 *
 * @brief  Register hook function callback
 *
 * @attention   None
 *
 * @param[in]   pInitHookFunc           pointer to init hook function 
 * @param[in]   pStartHookFunc          pointer to start hook function 
 * @param[in]   pStopHookFunc           pointer to stop hook function 
 * @param[in]   pGetRawdataHookFunc     pointer to get rawdata hook function
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_RegisterHookFunc(void (*pInitHookFunc)(void),
                             void (*pStartHookFunc)(void),
                             void (*pStopHookFunc)(void),
                             void (*pGetRawdataHookFunc)(GU8 *puchReadDataBuffer, GU16 usReadDataLen),
                             void (*pGetIoStructInfoHookFunc)(const STGh3x2xFrameInfo * const pstFrameInfo),
                             void (*pResetByProtocolHookFunc)(void),
                             void (*pConfigSetStartHookFunc)(void),
                             void (*pConfigSetStopHookFunc)(void),
                             void (*pWriteAlgConfigWithVirtualReg)(GU16 usVirtualRegAddr, GU16 usVirtualRegValue))
{
    g_pGh3x2xInitHookFunc = pInitHookFunc;
    g_pGh3x2xStartHookFunc = pStartHookFunc;
    g_pGh3x2xStopHookFunc = pStopHookFunc;
    g_pGh3x2xGetRawdataHookFunc = pGetRawdataHookFunc;
    g_pGh3x2xGetIoStructInfoHookFunc = pGetIoStructInfoHookFunc;
    g_pGh3x2xResetByProtocolHookFunc = pResetByProtocolHookFunc;
    g_pGh3x2xConfigSetStartHookFunc = pConfigSetStartHookFunc;
    g_pGh3x2xConfigSetStopHookFunc = pConfigSetStopHookFunc;
    g_pGh3x2xWriteAlgConfigWithVirtualRegHookFunc = pWriteAlgConfigWithVirtualReg;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
}

/**
 * @fn     GS8 GH3X2X_ExitLowPowerMode(void)
 *
 * @brief  Exit lowpower mode, in this mode, can read&write reg with gh3x2x
 *
 * @attention   Power consumption bigger than lowpower mode, detaile ref gh3x2x datasheet
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    exit low power mode error
 */
GS8 GH3X2X_ExitLowPowerMode(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_CHIP_WAKEUP(chRet);
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_EnterLowPowerMode(void)
 *
 * @brief  Enter lowpower mode, in this mode, can't read&write reg with gh3x2x.
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    enter low power mode error
 */
GS8 GH3X2X_EnterLowPowerMode(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_CHIP_SLEEP(chRet);
    g_uchGhx2xChipSleepFlag |= (1 << g_uchGh3x2xChipOperateIndexRealtime);  //set sleep flag
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_SoftReset(void)
 *
 * @brief  Gh3x2x softreset via i2c/spi, can read&write reg with gh3x2x after reset 
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    exit low power mode error
 */
GS8 GH3X2X_SoftReset(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_CHIP_RESET(chRet);
    if(0 == GH3X2X_GetSingleChipModeEnableFlag())
    {
        GH3X2X_SoftResetSlaveChip();
    }
    g_uchActiveChipResetFlag = 1;
    if (g_uchResetFromProtocolFlag == 1)
    {
        g_uchResetFromProtocolFlag = 0;
        HOOK_FUNC_CALL(g_pGh3x2xResetByProtocolHookFunc, () );  /* call hook */
    }
    return chRet;
}

#if !__GH3X2X_CASCADE_EN__
void GH3X2X_SoftResetSlaveChip(void){}
#else
void GH3X2X_SoftResetSlaveChip(void)
{
    GH3X2X_CascadeOperationSlaverChip();
    GH3X2X_SEND_RESET_CMD();
    GH3X2X_CascadeOperationMasterChip();
}
#endif

#if defined(GH3X2X_LOG_DEBUG) && (GH3X2X_LOG_DEBUG > 0) // debug level > 0
/**
 * @fn     void GH3X2X_LogConfigArr(STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen)
 *
 * @brief  log gh3x2x reg config array
 *
 * @attention   Only use for dbg version;
 *
 * @param[in]   pstRegConfigArr       pointer to the reg struct array
 * @param[in]   usRegConfigLen        reg struct array length
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_LogConfigArr(STGh3x2xReg *pstRegConfigArr, GU16 usRegConfigLen)
{
    GU16 usIndex = 0;
    GH3X2X_DEBUG_LOG_PARAM("Log reg config arr:\r\n");
    if ((pstRegConfigArr != GH3X2X_PTR_NULL) && (usRegConfigLen != 0))
    {
        for (usIndex = 0; usIndex < usRegConfigLen; usIndex++)
        {
            GH3X2X_DEBUG_LOG_PARAM("addr:0x%.4x, val:0x%.4x\r\n", pstRegConfigArr[usIndex].usRegAddr,
                                                                pstRegConfigArr[usIndex].usRegData);
        }
    }
}
#endif

/**
 * @fn     GS8 GH3X2X_DumpRegs(STGh3x2xReg *pstDumpRegsArr, GU16 usDumpRegsStartAddr, GU16 usDumpRegsLen)
 *
 * @brief  Dump gh3x2x regs
 *
 * @attention   usDumpRegsStartAddr only allow even address, if param set odd address val that val & 0xFFFE in api;
 *              If address greater than reg max addres 0xFFFE, it will return GH3X2X_RET_GENERIC_ERROR.
 *
 * @param[out]  pstDumpRegsArr           pointer to the dump reg struct output
 * @param[in]   usDumpRegsStartAddr     dump reg address
 * @param[in]   usDumpRegsLen           dump reg length
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    dump gh3x2x address out of bounds
 */
GS8 GH3X2X_DumpRegs(STGh3x2xReg *pstDumpRegsArr, GU16 usDumpRegsStartAddr, GU16 usDumpRegsLen)
{
    GS8 chRet = GH3X2X_RET_OK;
    GU16 usIndex = 0;
    GU16 usDumpRegsAddr = usDumpRegsStartAddr & GH3X2X_REG_ADDR_EVEN_FIXED; // just allow even addr val

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((pstDumpRegsArr != GH3X2X_PTR_NULL) && (usDumpRegsLen != 0))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        for (usIndex = 0; usIndex < usDumpRegsLen; usIndex++) // read GH3X2X reg.
        {
            pstDumpRegsArr[usIndex].usRegAddr = usDumpRegsAddr;
            pstDumpRegsArr[usIndex].usRegData = GH3X2X_ReadReg(usDumpRegsAddr);
            if (usDumpRegsAddr < GH3X2X_REG_ADDR_MAX) // last reg addr : 0xfffe
            {
                usDumpRegsAddr += GH3X2X_REG_ADDR_SIZE;
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("Dump reg address out of bounds!\r\n");
                chRet = GH3X2X_RET_GENERIC_ERROR;
                break;
            }
        }
        GH3X2X_WAIT_CHIP_DSLP();
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_CommunicateConfirm(void)
 *
 * @brief  Communication operation interface confirm
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_COMM_ERROR       gh3x2x communicate error
 */
GS8 GH3X2X_CommunicateConfirm(void)
{
    GU8 uchIndex = GH3X2X_COMMUNICATE_CONFIRM_MAX_CNT;
    GS8 chRet = GH3X2X_RET_COMM_ERROR;
    GU16 uchReadData;
    GU16 uchWriteData;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    do
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        if (GH3X2X_ReadReg(GH3X2X_CHIP_READY_CODE_REG_ADDR) == GH3X2X_CHIP_READY_REG_VAL)
        {
            uchReadData = GH3X2X_ReadReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR);
            GH3X2X_DEBUG_LOG_PARAM("slot_time0 reg = %d\r\n", (int)uchReadData);
            uchWriteData = ~uchReadData;
            GH3X2X_WriteReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR, uchWriteData);
            uchReadData = GH3X2X_ReadReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR);
            GH3X2X_DEBUG_LOG_PARAM("slot_time0 reg = %d\r\n", (int)uchReadData);
            if (uchWriteData == uchReadData)
            {
                chRet = GH3X2X_RET_OK;
                uchWriteData = ~uchReadData;
                GH3X2X_WriteReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR, uchWriteData);
                break;
            }
        }
        else
        {
            GH3X2X_DEBUG_LOG_PARAM("ready code reg is error !!! %s\r\n", __FUNCTION__);
        }
        uchIndex--;
    } while (uchIndex > 0);
    GH3X2X_WAIT_CHIP_DSLP();

    return chRet;
}

extern GU8 g_uchEcgCascadeEn;
extern GU8 g_uchEcgCascadeWriteRegVerifyEn;
///0: not init  1: initialized
GU8 g_uchEcgCascadeSlaveInitSuccess;

void Gh3x2x_CHECK_0x0088_Ex(void)
{
    GH3X2X_CHECK_0x0088();
}

#if !__GH3X2X_CASCADE_EN__
void GH3X2X_LoadSlaveCfgForCascadeMode(void)
{}
#else
void GH3X2X_LoadSlaveCfgForCascadeMode(void)
{
    GH3X2X_CascadeOperationSlaverChip();

    /* clear int status. */
    GH3X2X_WAIT_CHIP_WAKEUP();
    GH3X2X_WriteReg(GH3X2X_INT_STR_REG_ADDR, GH3X2X_INT_STR_MSK_ALL_BIT);
    GH3X2X_WAIT_CHIP_DSLP();
    if(GH3X2X_CascadeGetEcgEnFlag())
    {
        GH3X2X_DEBUG_LOG_PARAM("Slaver is enable!\r\n");
        //g_uchEcgCascadeEn = 1;
        g_uchEcgCascadeSlaveInitSuccess = 0;
        //Write the others chip's reg
        
        // GU16 lusRegLen = sizeof(stGh3x2xEcgCascadeSlaveRealReglist) / sizeof(STGh3x2xReg);
        GU16 lusRegLen = g_usEcgCascadeSlaveRealRegLen;
        //GH3X2X_WAIT_CHIP_WAKEUP();   
    #if 0
        GU16 usReadRegVal = 0;
        for (GU16 lusIndex = 0; lusIndex < lusRegLen; lusIndex++)
        {
            GH3X2X_WriteReg(stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegAddr,
                                     stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegData);

            /* need verify val. */
            if (g_uchEcgCascadeWriteRegVerifyEn)
            {
                usReadRegVal = GH3X2X_ReadReg(stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegAddr);
                if (usReadRegVal != stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegData)
                {
                    GH3X2X_DEBUG_LOG_PARAM("reg verify error! addr:0x%.4x,w_val:0x%.4x,r_val:0x%.4x\r\n",
                                            stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegAddr,
                                            stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegData, usReadRegVal);
                    //chRet = GH3X2X_RET_COMM_ERROR;
                    g_uchEcgCascadeSlaveInitSuccess = 0;
                    break;
                }
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("reg don't verify! addr:0x%.4x,w_val:0x%.4x\r\n",
                                            stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegAddr,
                                            stGh3x2xEcgCascadeSlaveRealReglist[lusIndex].usRegData);
            }
            if ((lusRegLen -1)  == lusIndex)
            {
                g_uchEcgCascadeSlaveInitSuccess = 1;
            }
        }
    #else
        if (GH3X2X_RET_OK == GH3X2X_LoadNewRegConfigArr(stGh3x2xEcgCascadeSlaveRealReglist, lusRegLen, 1))
        {
            g_uchEcgCascadeSlaveInitSuccess = 1;
        }
        else
        {
            g_uchEcgCascadeSlaveInitSuccess = 0;
        }
    #endif
       //GH3X2X_WAIT_CHIP_DSLP();
      
       //modify ecg chnl map and chnl num
       g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->uchChnlNum = 2;
       if(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->uchFuntionChnlLimit > 1)
       {
            g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pchChnlMap[1] = 0x08;
       }
   }
    GH3X2X_CascadeOperationMasterChip();
}
#endif

void GH3X2X_SensorPramInit(void)
{
    g_uchGsensorEnable = 0;
    g_uchCapEnable = 0;
    g_uchTempEnable = 0;
}

void GH3X2X_DrvControlInit(void)
{
    g_uchAlgoEnableFlag = 1;
    g_unSoftEvent = 0;
    g_unConfigFuncMode = 0;
    g_unFuncStartedBitmap = 0;
    g_uchGh3x2xStatus = GH3X2X_STATUS_NO_INIT;
    g_uchSingleChipModeEnableFlag = 0; 
    g_uchActiveChipResetFlag = 0;
    g_uchGhx2xChipSleepFlag = 0x03;
    g_uchChipDivZeroFlag = 0;
}

void GH3X2X_FifoControlInit(void)
{
    g_uchFifoPackageID = 0xFF; 
    g_uchFifoPackageMode = 0;
    g_unSendFifoPackageCnt = 0;
    g_usCurrentFiFoWaterLine = 0;
}

void GH3X2X_ProtocolFlagInit(void)
{
    g_uchResetFromProtocolFlag = 0;
}

void GH3X2XDrvConfigInit()
{
    GH3X2X_ProtocolFlagInit();
    GH3X2X_FifoControlInit();
#if __FUNC_TYPE_ECG_ENABLE__
    GH3X2X_AdtPramInit();
    GH3X2X_DownSampleParamInit();
#endif
    GH3X2X_SensorPramInit();
    GH3X2X_DrvControlInit();
}
 
/**
 * @fn     GS8 GH3X2X_Init(const STGh3x2xInitConfig *pstGh3x2xInitConfigParam)
 *
 * @brief  Init gh3x2x with configure parameters
 *
 * @attention   None
 *
 * @param[in]   pstGh3x2xInitConfigParam      pointer to gh3x2x init config param
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_COMM_ERROR       gh3x2x communicate error
 */
GS8 GH3X2X_Init(const STGh3x2xInitConfig *pstGh3x2xInitConfigParam)
{
    GH3X2XDrvConfigInit();
    GH3X2X_DEBUG_LOG_PARAM("%s : Lib Version : %s\r\n", __FUNCTION__, GH3X2X_GetDriverLibVersion());
    if (GH3X2X_CommunicateConfirm() != GH3X2X_RET_OK) /* confirm GH3X2X communicate. */
    {
        GH3X2X_DEBUG_LOG_PARAM("communicate confirm error!\r\n");
        return GH3X2X_RET_COMM_ERROR;
    }

    /* clear int status. */
    GH3X2X_WAIT_CHIP_WAKEUP();
    GH3X2X_WriteReg(GH3X2X_INT_STR_REG_ADDR, GH3X2X_INT_STR_MSK_ALL_BIT);
    GH3X2X_WAIT_CHIP_DSLP();

    /* init record pmu fifo value. */
    g_pusPmuFifoModuleCtrlVal[g_uchGh3x2xChipOperateIndexRealtime] = GH3X2X_ReadReg(GH3X2X_PMU_CTRL4_REG_ADDR);

    if (pstGh3x2xInitConfigParam != GH3X2X_PTR_NULL)
    {
        STAgcRegConfig stAgcRegConfig;  
#if __SUPPORT_SOFT_AGC_CONFIG__ 
        GH3x2xAgcRegvalReset(&stAgcRegConfig);
#endif
        /* load config */
        if (GH3X2X_LoadNewRegConfigArr(pstGh3x2xInitConfigParam->pstRegConfigArr,
            pstGh3x2xInitConfigParam->usConfigArrLen, 0, &stAgcRegConfig) != GH3X2X_RET_OK)
        {
            return GH3X2X_RET_COMM_ERROR;
        }
        /* load slave chip config for cascade mode */
        if(0 == GH3X2X_GetSingleChipModeEnableFlag())
        {
            GH3X2X_LoadSlaveCfgForCascadeMode();
        }
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("gh init param is null!\r\n");
    }

    GH3X2X_WriteReg(0x0700,0x0020);
    GH3X2X_WriteReg(0x0700,0x0030);
    while(!(GH3X2X_ReadReg(0x0718)&0x0001));
    GS8 err_code = (GS8)(GH3X2X_ReadReg(0x0712) & 0xFF);
    GH3X2X_SetDrvEcode(err_code);
    GH3X2X_SetConfigDrvEcode(pstGh3x2xInitConfigParam);

    /* @ fix chip error code below here.
     * e.g. otp clk err
     */

    /* @end of fix chip error. */

    /* call hook */
    GH3X2X_WAIT_CHIP_WAKEUP();
    HOOK_FUNC_CALL(g_pGh3x2xInitHookFunc, ());
    GH3X2X_WAIT_CHIP_DSLP();

    /* set chip status inited. */
    GH3X2X_SET_CHIP_INIED();
    g_uchGh3x2xStatus = GH3X2X_STATUS_INITED;
    return GH3X2X_RET_OK;
}

/**
 * @fn     GS8 GH3X2X_StartSampling(void)
 *
 * @brief  Start gh3x2x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_NO_INITED_ERROR     gh3x2x has not inited
 * @retval  #GH3X2X_RET_GENERIC_ERROR       gh3x2x has started, need restart should call GH3X2X_StopSampling then start
 */
GS8 GH3X2X_StartSampling(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (g_uchGh3x2xStatus != GH3X2X_STATUS_STARTED)
    {
        g_uchFifoPackageID = 0xFF;
        if (GH3X2X_IS_CHIP_INIED() && (g_uchGh3x2xStatus == GH3X2X_STATUS_INITED))
        {
            GU16 usRegVal = 0;
            GH3X2X_WAIT_CHIP_WAKEUP();
            g_usCurrentFiFoWaterLine = GH3X2X_ReadReg(GH3X2X_FIFO_WATERLINE_REG_ADDR);  //record water line
            usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
            GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_SET_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
#if GH3X2X_ALGORITHM_ECG_SUPPORT && __FUNC_TYPE_ECG_ENABLE__
            GH3X2X_LeadDetEnControl(ECG_SAMPLE_EVENT_INFO_SAMPLE_START);
#endif

//            GH3X2X_RecordAgcInfo(); //record agc info
#if __SUPPORT_SOFT_AGC_CONFIG__
            GH3X2X_AgcParaUpdate(g_pstGh3x2xAgcInfo);
#endif

#if GH3X2X_DUMP_MODE_EN
            GH3X2X_DumpInit();
#endif
            g_uchGh3x2xStatus = GH3X2X_STATUS_STARTED;
            HOOK_FUNC_CALL(g_pGh3x2xStartHookFunc, () );  /* call hook */
            GH3X2X_WAIT_CHIP_DSLP(); //  GH3X2X_SEND_DSLEEP_CMD(); // enter sleep mode in application code
            GH3X2X_StartSamplingSlaveChip();
            chRet = GH3X2X_RET_OK;
        }
        else
        {
            g_uchGh3x2xStatus = GH3X2X_STATUS_NO_INIT;
            chRet = GH3X2X_RET_NO_INITED_ERROR;
            GH3X2X_DEBUG_LOG_PARAM("gh start error that don't inited!\r\n");
        }
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("gh started,restart call stop->start!\r\n");
    }

    return chRet;
}

#if !__GH3X2X_CASCADE_EN__
void GH3X2X_StartSamplingSlaveChip(void){}
void GH3X2X_StopSamplingSlaveChip(void){}
#else
void GH3X2X_StartSamplingSlaveChip(void)
{
    GU16 usRegVal = 0;
    if(GH3X2X_CascadeGetEcgEnFlag())
    {
        GH3X2X_CascadeOperationSlaverChip();
        GH3X2X_WAIT_CHIP_WAKEUP();
        if (g_usCurrentFiFoWaterLine != GH3X2X_ReadReg(GH3X2X_FIFO_WATERLINE_REG_ADDR))  //record water line
        {
            GH3X2X_WriteReg(GH3X2X_FIFO_WATERLINE_REG_ADDR, g_usCurrentFiFoWaterLine);
            GH3X2X_DEBUG_LOG_PARAM("gh chip2 and chip1 fifowater linie not equal!\r\n");
        }         
        usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
        GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_SET_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
//      #if GH3X2X_ALGORITHM_ECG_SUPPORT
//      GH3X2X_LeadDetEnControl(ECG_SAMPLE_EVENT_INFO_SAMPLE_START);
//      #endif
        GH3X2X_CascadeOperationMasterChip();
        GH3X2X_DEBUG_LOG_PARAM("gh chip2 ecg start!\r\n");
    }
}

void GH3X2X_StopSamplingSlaveChip(void)
{
    GU16 usRegVal = 0;
    if (GH3X2X_CascadeGetEcgEnFlag())
    {
        GH3X2X_CascadeOperationSlaverChip();
        GH3X2X_WAIT_CHIP_WAKEUP();
        //g_uchGh3x2xStatus = GH3X2X_STATUS_INITED;
        usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
        GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_CLEAR_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
//      #if GH3X2X_ALGORITHM_ECG_SUPPORT
//      GH3X2X_LeadDetEnControl(ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP);
//      #endif
        //HOOK_FUNC_CALL(g_pGh3x2xStopHookFunc, () );  /* call hook */
        GH3X2X_WAIT_CHIP_DSLP();            
        GH3X2X_CascadeOperationMasterChip();
    }
}
#endif
/**
 * @fn     GS8 GH3X2X_StopSampling(void)
 *
 * @brief  Stop gh3x2x sampling
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 * 
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_NO_INITED_ERROR     gh3x2x has not inited
 */
GS8 GH3X2X_StopSampling(void)
{
    GS8 chRet = GH3X2X_RET_NO_INITED_ERROR;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((g_uchGh3x2xStatus == GH3X2X_STATUS_INITED) || (g_uchGh3x2xStatus == GH3X2X_STATUS_STARTED))
    {
        GU16 usRegVal = 0;
        GH3X2X_WAIT_CHIP_WAKEUP();
        g_uchGh3x2xStatus = GH3X2X_STATUS_INITED;
        usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
        GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_CLEAR_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
#if GH3X2X_ALGORITHM_ECG_SUPPORT && __FUNC_TYPE_ECG_ENABLE__
        GH3X2X_LeadDetEnControl(ECG_SAMPLE_EVENT_INFO_SAMPLE_STOP);
#endif
        HOOK_FUNC_CALL(g_pGh3x2xStopHookFunc, () );  /* call hook */
        GH3X2X_WAIT_CHIP_DSLP();
        GH3X2X_StopSamplingSlaveChip();
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("gh stop error that don't inited!\r\n");
    }

    return chRet;
}

/**
 * @fn     GU16 GH3X2X_GetIrqStatus(void)
 *
 * @brief  Get irq status reg val
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  irq status val, ref irq status mask
 */
GU16 GH3X2X_GetIrqStatus(void)
{
    GU16 usIrqRegVal = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_WAIT_CHIP_WAKEUP();
    usIrqRegVal = GH3X2X_ReadReg(GH3X2X_INT_STR_REG_ADDR);
    GH3X2X_WriteReg(GH3X2X_INT_STR_REG_ADDR, usIrqRegVal);
    GH3X2X_WAIT_CHIP_DSLP();
    return (GU16)(usIrqRegVal & (GU16)GH3X2X_INT_STR_MSK_ALL_BIT);
}

/**
 * @fn     void GH3X2X_UnpackRawdataPackage(STGh3x2xSlotRawdata *pstSlotRawdataArr,
 *                                    GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen)
 *
 * @brief  Unpack to 8 slot rawdata from read fifo data buffer
 *
 * @attention   This function should use in get rawdata hook;
 *              param pstSlotRawdataArr size must equal 8, define like stSlotRawdataArr[8]. If not define 8,
 *              that mcu will occur array out of bounds error, careful!!!
 *
 * @param[out]  pstSlotRawdataArr       pointer to 8 slot rawdata struct output
 * @param[in]   puchReadRawdataBuffer   pointer to read fifo buffer
 * @param[in]   usReadRawdataLen        read fifo rawdata data len (bytes)
 *
 * @return  None
 */
void GH3X2X_UnpackRawdataPackage(STGh3x2xSlotRawdata *pstSlotRawdataArr,
                                    GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen)
{
    GU16 usIndex = 0;
    GU8  uchSlotNum = 0;
    GU8  uchAdcNum = 0;
    GU8  uchLastSlotNum = 0;
    GU8  uchLastAdcNum = 0;
    GU16 usRawdataIndex = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    for (usIndex = 0; usIndex < GH3X2X_SLOT_NUM_MAX; usIndex ++)
    {
        pstSlotRawdataArr[usIndex].usRawdataCnt = 0; // clear cnt
    }
    for (usIndex = 0; usIndex < usReadRawdataLen; usIndex += GH3X2X_FIFO_RAWDATA_SIZE)
    {
        uchSlotNum = GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(puchReadRawdataBuffer[usIndex]);
        uchAdcNum = GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(puchReadRawdataBuffer[usIndex]);
        if (usIndex == 0) // fixed started
        {
            uchLastSlotNum = uchSlotNum;
            uchLastAdcNum = uchAdcNum;
        }
        if ((uchLastSlotNum != uchSlotNum)
            || ((uchLastSlotNum == uchSlotNum) && (uchAdcNum <= uchLastAdcNum) && (usIndex != 0)))
        {
            pstSlotRawdataArr[uchLastSlotNum].usRawdataCnt++;
        }
        usRawdataIndex = pstSlotRawdataArr[uchSlotNum].usRawdataCnt;
        if (pstSlotRawdataArr[uchSlotNum].punRawdata != GH3X2X_PTR_NULL)
        {
            pstSlotRawdataArr[uchSlotNum].punRawdata[usRawdataIndex][uchAdcNum] = \
                                                        GH3X2X_RAWDATA_CLEAR_SLOT_ADC_NUM(\
                                                            GH3X2X_MAKEUP_DWORD(puchReadRawdataBuffer[usIndex],
                                                                                puchReadRawdataBuffer[usIndex + 1],
                                                                                puchReadRawdataBuffer[usIndex + 2],
                                                                                puchReadRawdataBuffer[usIndex + 3]));
        }
        uchLastSlotNum = uchSlotNum;
        uchLastAdcNum = uchAdcNum;
        if ((usIndex + GH3X2X_FIFO_RAWDATA_SIZE) == usReadRawdataLen)
        {
            pstSlotRawdataArr[uchSlotNum].usRawdataCnt++;
        }
    } // for (usIndex = 0; usIndex < usReadRawdataLen; usIndex += 4)
}

/**
 * @fn     GS16 GH3X2X_GetRawdata(STGh3x2xRawdata *pstGh3x2xRawdata, GU16* usFifoLength)
 *
 * @brief  Get rawdata from fifo
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  pstGh3x2xRawdata     pointer to rawdata struct output
 * @param[out]  usFifoLength         pointer to read fifo bytes
 *
 * @return  error code
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 * @retval  #GH3X2X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 */
GS16 GH3X2X_GetRawdata(STGh3x2xRawdata *pstGh3x2xRawdata, GU16* usFifoLength)
{
    GU16 usIndex = 0;
    GS16 sRet = GH3X2X_RET_PARAMETER_ERROR;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (pstGh3x2xRawdata != GH3X2X_PTR_NULL)
    {
        if (pstGh3x2xRawdata->puchReadBuffer != GH3X2X_PTR_NULL)
        {
            GH3X2X_WAIT_CHIP_WAKEUP();
            *usFifoLength = GH3X2X_FIFO_CNT_CALC(GH3X2X_ReadReg(GH3X2X_INT_FIFO_UR_REG_ADDR));

            if (*usFifoLength > g_usMaxNumReadFromFifo)
            {
                *usFifoLength = g_usMaxNumReadFromFifo;
                sRet = GH3X2X_RET_READ_FIFO_CONTINUE;
            }

            if ((*usFifoLength > 0) && (*usFifoLength <= GH3X2X_FIFO_DATA_BYTES_MAX_LEN))
            {
                GH3X2X_ReadFifo(pstGh3x2xRawdata->puchReadBuffer, *usFifoLength);
            }
            else
            {
                if (*usFifoLength > GH3X2X_FIFO_DATA_BYTES_MAX_LEN)
                {
                    *usFifoLength = 0;
                    GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo len greater than max, pelease check i2c/spi!\r\n");
                }
                else
                {
                    GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo equl 0!\r\n");
                }
            }
            
            /* call hook */
            HOOK_FUNC_CALL(g_pGh3x2xGetRawdataHookFunc, (pstGh3x2xRawdata->puchReadBuffer, *usFifoLength) );
            GH3X2X_WAIT_CHIP_DSLP();
            GH3X2X_UnpackRawdataPackage(pstGh3x2xRawdata->stSlotRawdataArr,
                                        pstGh3x2xRawdata->puchReadBuffer, *usFifoLength);
        }// if (pstGh3x2xRawdata->puchReadBuffer != GH3X2X_PTR_NULL)
        else // fixed clear cnt if readbuffer ptr is null
        {
            GH3X2X_DEBUG_LOG_PARAM("get rawdata error that readbuffer is null!\r\n");
            for (usIndex = 0; usIndex < GH3X2X_SLOT_NUM_MAX; usIndex ++)
            {
                pstGh3x2xRawdata->stSlotRawdataArr[usIndex].usRawdataCnt = 0; // clear cnt
            }
        }
    }// if (pstGh3x2xRawdata != GH3X2X_PTR_NULL)
    
    return sRet;
}

#if 0
void GH3X2X_SetFifoPackageMode(GU8 uchMode, GU8 *puchFifoBuffer)
{
    g_uchFifoPackageMode = uchMode;
    gpuchSendFifoPackage = puchFifoBuffer;
}
#endif

#if (0 == __FIFO_PACKAGE_SEND_ENABLE__)
GU8 GH3X2X_GetFifoPackageMode(void)
{
    return 0;
}
void GH3X2X_SendRawdataFifoPackage(GU8 *puchGh3x2xReadFifoData, GU16 usFifoReadByteNum)
{
    (void) puchGh3x2xReadFifoData;
    (void) usFifoReadByteNum;
}
#else
GU8 GH3X2X_GetFifoPackageMode(void)

{
    return g_uchFifoPackageMode;
}

void GH3X2X_SendRawdataFifoPackage(GU8 *puchGh3x2xReadFifoData, GU16 usFifoReadByteNum)
{
    if (usFifoReadByteNum == 0)
    {
        return;
    }
    g_uchFifoPackageID ++;
    GU8 unRawdataFifoBuffer[220] = {0};
    GU8 unSendRawdataFifoBuffer[235] = {0};
    GU32 uRawdataBufCnt = 0;
    GU8 uchIdChangeFlag = 0;
    GU16 usPackageNum = usFifoReadByteNum / 200;
    GU16 usLastRawdataFifoBufNum = usFifoReadByteNum % 200;
    if (usLastRawdataFifoBufNum == 0)
    {
        usLastRawdataFifoBufNum = 200;
        usPackageNum--;
    }
    for (uRawdataBufCnt = 0 ; uRawdataBufCnt < usPackageNum ; uRawdataBufCnt ++)
    {
        unRawdataFifoBuffer[0] = g_uchFifoPackageID;
        unRawdataFifoBuffer[1] = 200;
        unRawdataFifoBuffer[2] = uchIdChangeFlag;
        uchIdChangeFlag ++;
        GH3X2X_Memcpy(&unRawdataFifoBuffer[5], &puchGh3x2xReadFifoData[uRawdataBufCnt * 200], (GU32)200);
        GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_RAWDATA_FIFO_UPDATE, unSendRawdataFifoBuffer,
                                                unRawdataFifoBuffer, 200 + 5);
        Gh3x2xDemoSendProtocolData(unSendRawdataFifoBuffer, usRespondLen);
    }
    unRawdataFifoBuffer[0] = g_uchFifoPackageID;
    unRawdataFifoBuffer[1] = (GU8)(usLastRawdataFifoBufNum);
    unRawdataFifoBuffer[2] = 0x10 | uchIdChangeFlag;
    GH3X2X_Memcpy(&unRawdataFifoBuffer[5], &puchGh3x2xReadFifoData[usPackageNum * 200], (GU32)usLastRawdataFifoBufNum);
    GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_RAWDATA_FIFO_UPDATE, unSendRawdataFifoBuffer,
                                            unRawdataFifoBuffer, usLastRawdataFifoBufNum + 5);
    Gh3x2xDemoSendProtocolData(unSendRawdataFifoBuffer, usRespondLen);
}
#endif

#if 0
void GH3X2X_PackRawdataFifoPackage(GU8 *puchGh3x2xReadFifoData, GU16 usFifoReadByteNum)
{
    if (usFifoReadByteNum == 0)
    {
        return;
    }

    if (usFifoReadByteNum + g_unSendFifoPackageCnt >= GH3X2X_RAWDATA_BUFFER_SIZE)
    {
        GU32 usFifoReadExternByteNum = GH3X2X_RAWDATA_BUFFER_SIZE - g_unSendFifoPackageCnt;
        GH3X2X_Memcpy(&gpuchSendFifoPackage[g_unSendFifoPackageCnt], puchGh3x2xReadFifoData, usFifoReadExternByteNum);
        g_unSendFifoPackageCnt += usFifoReadExternByteNum;

        GH3X2X_SendRawdataFifoPackage(gpuchSendFifoPackage, g_unSendFifoPackageCnt);

        g_unSendFifoPackageCnt = 0;
        
        GH3X2X_Memcpy(&gpuchSendFifoPackage[g_unSendFifoPackageCnt],\
            &puchGh3x2xReadFifoData[usFifoReadByteNum - (GH3X2X_RAWDATA_BUFFER_SIZE - g_unSendFifoPackageCnt)],\
            usFifoReadByteNum + g_unSendFifoPackageCnt - GH3X2X_RAWDATA_BUFFER_SIZE);
        g_unSendFifoPackageCnt += usFifoReadByteNum + g_unSendFifoPackageCnt - GH3X2X_RAWDATA_BUFFER_SIZE;
    }
    else
    {
        GH3X2X_Memcpy(&gpuchSendFifoPackage[g_unSendFifoPackageCnt], puchGh3x2xReadFifoData, usFifoReadByteNum);
        g_unSendFifoPackageCnt += usFifoReadByteNum;
    }
}
#endif

/**
 * @fn     GU16 GH3X2X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  min val, if len = 0, return 0
 */
GU16 GH3X2X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMinVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] < usMinVal)
        {
            usMinVal = pusBuffer[uchIndex];
        }
    }
    return usMinVal; 
}

/**
 * @fn     GU16 GH3X2X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find max val
 *
 * @attention   len must > 0
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  None
 *
 * @return  max val, if len = 0, return 0
 */
GU16 GH3X2X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMaxVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] > usMaxVal)
        {
            usMaxVal = pusBuffer[uchIndex];
        }
    }
    return usMaxVal; 
}

/**
 * @fn     void GH3X2X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen)
 *
 * @brief  Find min val & max val
 *
 * @attention   len must > 0, ptr not null
 * 
 * @param[in]   pusBuffer        pointer to buffer
 * @param[in]   uchLen           buffer length
 * @param[out]  pusMaxVal        pointer to max val
 * @param[out]  pusMinVal        pointer to min val
 *
 * @return  None
 */
void GH3X2X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen)
{
    GU16 usMaxVal = pusBuffer[0];
    GU16 usMinVal = pusBuffer[0];
    GU8 uchIndex = 1;

    for (uchIndex = 1; uchIndex < uchLen; uchIndex++)
    {
        if (pusBuffer[uchIndex] > usMaxVal)
        {
            usMaxVal = pusBuffer[uchIndex];
        }
        if (pusBuffer[uchIndex] < usMinVal)
        {
            usMinVal = pusBuffer[uchIndex];
        }
    }
    SET_VAL_VIA_PTR(pusMaxVal, usMaxVal);
    SET_VAL_VIA_PTR(pusMinVal, usMinVal);
}

/**
 * @fn     GS8 GH3X2X_UnpackRawdataWithChannelMap(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata, 
 *               GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr)
 *
 * @brief  Unpack to channel rawdata from read fifo data buffer;
 *         if last channel rawdata incomplete, should change fifo watermark
 *
 * @attention   This function should use in get rawdata hook
 * 
 * @param[in]   puchReadRawdataBuffer       pointer to read data buffer
 * @param[in]   usReadRawdataLen            read data length
 * @param[in]   uchChannelMapCnt            channel map array cnt, max:32
 * @param[in]   puchChannelMapArr           pointer to channel map array
 * @param[out]  pstGh3x2xChannelRawdata     pointer to channel rawdata struct
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 */
GS8 GH3X2X_UnpackRawdataWithChannelMap(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata, 
                GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr)
{
    GU8  uchChCntIndex = 0;
    GU16 usRawdataIndex = 0;
    GU16 usRawdataCnt = 0;
    GU16 usRawdataCntMin = 0;
    GU16 usRawdataCntMax = 0;
    GU16 usRawdataIndexArr[GH3X2X_CHANNEL_MAP_MAX_CH] = {0}; // use for rawdata cnt & rawdata bytes index
    GU16 usRawdataByteIndexTmp = 0;
    GU16 usRawdataBaseIndex = 0;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);

    /* check pram */
    if ((pstGh3x2xChannelRawdata->punIncompleteChRawdataArr == GH3X2X_PTR_NULL)
        || (pstGh3x2xChannelRawdata->punChRawdataArr == GH3X2X_PTR_NULL) || (puchReadRawdataBuffer == GH3X2X_PTR_NULL)
        || (pstGh3x2xChannelRawdata == GH3X2X_PTR_NULL) || (puchChannelMapArr == GH3X2X_PTR_NULL))
    {
        GH3X2X_DEBUG_LOG_PARAM("pointer param set null error!\r\n");
        return GH3X2X_RET_PARAMETER_ERROR;
    }
    for (uchChCntIndex = 0; uchChCntIndex < uchChannelMapCnt; uchChCntIndex++)
    {
        if (GH3X2X_CLEAR_BIT(puchChannelMapArr[uchChCntIndex], GH3X2X_SLOT_ADC_NUM_BITS) != 0)
        {
            GH3X2X_DEBUG_LOG_PARAM("channel map error! set @ref GH3X2X_CHANNEL_MAP!\r\n");
            return GH3X2X_RET_PARAMETER_ERROR;
        }
    }
    if (uchChannelMapCnt > GH3X2X_CHANNEL_MAP_MAX_CH)
    {
        GH3X2X_DEBUG_LOG_PARAM("channel cnt greater than max!\r\n");
        return GH3X2X_RET_PARAMETER_ERROR;
    }

    /* calc rawdata len */
    for (uchChCntIndex = 0; uchChCntIndex < uchChannelMapCnt; uchChCntIndex++)
    {
        if (GH3X2X_CHECK_LEFT_BIT_SET(pstGh3x2xChannelRawdata->unIncompleteChMark, uchChCntIndex))
        {
            usRawdataIndexArr[uchChCntIndex]++;
        }
        for (usRawdataIndex = 0; usRawdataIndex < usReadRawdataLen; usRawdataIndex += GH3X2X_FIFO_RAWDATA_SIZE)
        {
            if (puchChannelMapArr[uchChCntIndex] == GH3X2X_CHANNEL_MAP_GET(puchReadRawdataBuffer[usRawdataIndex]))
            {
                usRawdataIndexArr[uchChCntIndex]++; // if rawdata channel map equal config(included slot&adc)
            }
        }
    }
    GH3X2X_FindGu16MaxMinVal(&usRawdataCntMax, &usRawdataCntMin, usRawdataIndexArr, uchChannelMapCnt); // calc min&max
    if (usRawdataCntMax == usRawdataCntMin)
    {
        usRawdataCnt = usRawdataCntMax; // whatever last data complete or incomplete
    }
    else
    {
        if ((usRawdataCntMax - usRawdataCntMin) > 1)
        {
            GH3X2X_DEBUG_LOG_PARAM("rawdata doesn't correspond to channel map error!\r\n");
            usRawdataCnt = 0;
            pstGh3x2xChannelRawdata->unIncompleteChMark = 0;
            GH3X2X_Memset(pstGh3x2xChannelRawdata->punIncompleteChRawdataArr, 0, uchChannelMapCnt * sizeof(GU32));
        }
        else // cnt tolerable deviation is 1
        {
            usRawdataCnt = GH3X2X_FindGu16MinVal(usRawdataIndexArr, uchChannelMapCnt);
        }
    } // end of if (usRawdataCntMax == usRawdataCntMin)
    GH3X2X_Memset(usRawdataIndexArr, 0, uchChannelMapCnt * sizeof(GU16)); // clear index array, before next use

    if (usRawdataCnt != 0)
    {
        for (usRawdataIndex = 0; usRawdataIndex < usRawdataCnt; usRawdataIndex++)
        {
            usRawdataBaseIndex = (GU16)(usRawdataIndex * uchChannelMapCnt);
            for (uchChCntIndex = 0; uchChCntIndex < uchChannelMapCnt; uchChCntIndex++) // search each channel
            {
                if (GH3X2X_CHECK_LEFT_BIT_SET(pstGh3x2xChannelRawdata->unIncompleteChMark, uchChCntIndex))
                {
                    pstGh3x2xChannelRawdata->punChRawdataArr[usRawdataBaseIndex + uchChCntIndex] = \
                                                    pstGh3x2xChannelRawdata->punIncompleteChRawdataArr[uchChCntIndex];
                    pstGh3x2xChannelRawdata->punIncompleteChRawdataArr[uchChCntIndex] = 0;
                    GH3X2X_VAL_CLEAR_LEFT_BIT(pstGh3x2xChannelRawdata->unIncompleteChMark, uchChCntIndex);
                }
                else
                {
                    usRawdataByteIndexTmp = usRawdataIndexArr[uchChCntIndex];
                    while (usRawdataByteIndexTmp < usReadRawdataLen) // search each
                    {
                        if (GH3X2X_CHANNEL_MAP_GET(puchReadRawdataBuffer[usRawdataByteIndexTmp]) == \
                                                                    puchChannelMapArr[uchChCntIndex]) // if map equal
                        {
                            pstGh3x2xChannelRawdata->punChRawdataArr[usRawdataBaseIndex + uchChCntIndex] = \
                                                GH3X2X_RAWDATA_CLEAR_SLOT_ADC_NUM(
                                                    GH3X2X_MAKEUP_DWORD(puchReadRawdataBuffer[usRawdataByteIndexTmp],
                                                                    puchReadRawdataBuffer[usRawdataByteIndexTmp + 1],
                                                                    puchReadRawdataBuffer[usRawdataByteIndexTmp + 2],
                                                                    puchReadRawdataBuffer[usRawdataByteIndexTmp + 3]));
                            usRawdataByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE;
                            break;
                        }
                        usRawdataByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE;
                    }
                    usRawdataIndexArr[uchChCntIndex] = usRawdataByteIndexTmp; // write back byte index
                }
            } // end of for (uchChCntIndex = 0; uchChCntIndex < uchChannelMapCnt; uchChCntIndex++)
        } // end of for (usRawdataIndex = 0; usRawdataIndex < usRawdataCnt; usRawdataIndex++)
        if (usRawdataCntMax != usRawdataCnt)
        {
            for (uchChCntIndex = 0; uchChCntIndex < uchChannelMapCnt; uchChCntIndex++) // search each channel
            {
                usRawdataByteIndexTmp = usRawdataIndexArr[uchChCntIndex];
                while (usRawdataByteIndexTmp < usReadRawdataLen) // search each
                {
                    if (GH3X2X_CHANNEL_MAP_GET(puchReadRawdataBuffer[usRawdataByteIndexTmp]) == \
                                                                puchChannelMapArr[uchChCntIndex]) // if map equal
                    {
                        pstGh3x2xChannelRawdata->punIncompleteChRawdataArr[uchChCntIndex] = \
                                            GH3X2X_RAWDATA_CLEAR_SLOT_ADC_NUM(
                                                GH3X2X_MAKEUP_DWORD(puchReadRawdataBuffer[usRawdataByteIndexTmp],
                                                                puchReadRawdataBuffer[usRawdataByteIndexTmp + 1],
                                                                puchReadRawdataBuffer[usRawdataByteIndexTmp + 2],
                                                                puchReadRawdataBuffer[usRawdataByteIndexTmp + 3]));
                        pstGh3x2xChannelRawdata->unIncompleteChMark |= GH3X2X_GET_LEFT_SHIFT_VAL(uchChCntIndex);
                        break;
                    }
                    usRawdataByteIndexTmp += GH3X2X_FIFO_RAWDATA_SIZE;
                }
                if (GH3X2X_CHECK_LEFT_BIT_NOT_SET(pstGh3x2xChannelRawdata->unIncompleteChMark, uchChCntIndex))
                {
                    pstGh3x2xChannelRawdata->punIncompleteChRawdataArr[uchChCntIndex] = 0;
                }
            }
        } // end of if (usRawdataCntMax != usRawdataCnt)
    } // end of if (usRawdataCnt != 0)
    pstGh3x2xChannelRawdata->usChRawdataCnt = usRawdataCnt;
    return GH3X2X_RET_OK;
}

/**
 * @fn     GS16 GH3X2X_GetRawdataWithChannelMap(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata, 
 *                                       GU8 *puchReadRawdataBuffer, GU16* usFifoLength, GU8 uchChannelMapCnt, 
 *                                       GU8 *puchChannelMapArr)
 *
 * @brief  Get rawdata from fifo with channel map
 *
 * @attention   None
 *
 * @param[in]   puchReadRawdataBuffer       pointer to read data buffer
 * @param[in]   uchChannelMapCnt            channel map array cnt, max:32
 * @param[in]   puchChannelMapArr           pointer to channel map array
 * @param[out]  pstGh3x2xChannelRawdata     pointer to channel rawdata struct output
 * @param[out]  usFifoLength                pointer to read fifo data length in bytes
 *
 * @return  GH3X2X_RET_OK                       return read fifo successful
 * @retval  #GH3X2X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 */
GS16 GH3X2X_GetRawdataWithChannelMap(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata, 
                                        GU8 *puchReadRawdataBuffer, GU16* usFifoLength, GU8 uchChannelMapCnt, 
                                        GU8 *puchChannelMapArr)
{
    GS16 sRet = GH3X2X_RET_OK;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);

    if ((puchReadRawdataBuffer != GH3X2X_PTR_NULL) && (usFifoLength != GH3X2X_PTR_NULL))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        *usFifoLength = GH3X2X_FIFO_CNT_CALC(GH3X2X_ReadReg(GH3X2X_INT_FIFO_UR_REG_ADDR));
        if (*usFifoLength > g_usMaxNumReadFromFifo)
        {
            *usFifoLength = g_usMaxNumReadFromFifo;
            sRet = GH3X2X_RET_READ_FIFO_CONTINUE;
        }
        if ((*usFifoLength > 0) && (*usFifoLength <= GH3X2X_FIFO_DATA_BYTES_MAX_LEN))
        {
            GH3X2X_ReadFifo(puchReadRawdataBuffer, *usFifoLength);
        }
        else
        {
            if (*usFifoLength > GH3X2X_FIFO_DATA_BYTES_MAX_LEN)
            {
                *usFifoLength = 0;
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo len greater than max, pelease check i2c/spi!\r\n");
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo equl 0!\r\n");
            }
        }

        /* call hook */
        HOOK_FUNC_CALL(g_pGh3x2xGetRawdataHookFunc, (puchReadRawdataBuffer, *usFifoLength));
        GH3X2X_WAIT_CHIP_DSLP();
        sRet = GH3X2X_UnpackRawdataWithChannelMap(pstGh3x2xChannelRawdata, puchReadRawdataBuffer, *usFifoLength, 
                                                    uchChannelMapCnt, puchChannelMapArr);
    }
    else // fixed clear cnt if readbuffer ptr is null
    {
        GH3X2X_DEBUG_LOG_PARAM("get rawdata error that readbuffer is null!\r\n");
        sRet = GH3X2X_RET_PARAMETER_ERROR;
    }

    return sRet;
}

/**
 * @fn     GS8 GH3X2X_ChannelMapRawdataClear(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata)
 *
 * @brief  clear channel map rawdata struct
 *
 * @attention   This function must use before get rawdata by onne kind channel map
 *              e.g. channel map start/stop need clear
 *
 * @param[in]   pstGh3x2xChannelRawdata     pointer to channel rawdata struct
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 */
GS8 GH3X2X_ChannelMapRawdataClear(STGh3x2xChannelRawdata *pstGh3x2xChannelRawdata)
{
    GS8 chRet = GH3X2X_RET_PARAMETER_ERROR;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (pstGh3x2xChannelRawdata != GH3X2X_PTR_NULL)
    {
        pstGh3x2xChannelRawdata->unIncompleteChMark = 0;
        pstGh3x2xChannelRawdata->usChRawdataCnt = 0;
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("channel rawdata param is null!\r\n");
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_SlotEnableConfig(GU16 usSlotEnableConfig, EMSlotEnableConfigType emSlotEnableConfigType)
 *
 * @brief  Slot enable config
 *
 * @attention  Set slot enable or disable, if just need enable slot by enable config param
 *
 * @param[in]  usSlotEnableConfig         slot enable index , @ref GH3X2X_SLOT_INDEX_0 ... GH3X2X_SLOT_INDEX_ALL
 * @param[in]  emSlotEnableConfigType     slot config type, @ref EMSlotEnableConfigType
 * @param[out]  None
 * 
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR     return param error
 */
GS8 GH3X2X_SlotEnableConfig(GU16 usSlotEnableConfig, EMSlotEnableConfigType emSlotEnableConfigType)
{
    GS8 chRet = GH3X2X_RET_OK;
    GU16 usSlotEnableCfgRegVal = 0;
    
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (GH3X2X_CHECK_BIT_SET(GH3X2X_SLOT_INDEX_ALL, usSlotEnableConfig))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        switch (emSlotEnableConfigType)
        {
        case GH3X2X_SET_SLOT_ENABLE:
            usSlotEnableCfgRegVal = GH3X2X_ReadReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR);
            GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
                            GH3X2X_SET_BIT(usSlotEnableCfgRegVal, usSlotEnableConfig) & GH3X2X_SLOT_INDEX_ALL);
            break;
        case GH3X2X_SET_SLOT_DISABLE:
            usSlotEnableCfgRegVal = GH3X2X_ReadReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR);
            GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
                            GH3X2X_CLEAR_BIT(usSlotEnableCfgRegVal, usSlotEnableConfig) & GH3X2X_SLOT_INDEX_ALL);
            break;
        default: // include GH3X2X_SET_SLOT_DIRECT_ENABLE mode
            GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR, usSlotEnableConfig & GH3X2X_SLOT_INDEX_ALL);
            break;
        }
        GH3X2X_WAIT_CHIP_DSLP();
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("slot enable config error!\r\n");
        chRet = GH3X2X_RET_PARAMETER_ERROR;
    }
    return chRet;
}

/**
 * @fn     void GH3X2X_SlotEnRegSet(GU8 uchSetValue)
 *
 * @brief  Slot enable reg set
 *
 * @attention  None
 *
 * @param[in]  set value
 * @param[out]  None
 * 
 * @return  None
 */
void GH3X2X_SlotEnRegSet(GU8 uchSetValue)
{    
    GH3X2X_DEBUG_LOG_PARAM("[SlotEnRegSet] set slot en: 0x%x\r\n",uchSetValue);
    GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
                (GU16)uchSetValue);
}

void GH3X2X_EcgCascadeSlotEnRegSet(GU8 uchSetValue)
{
    //chip1 disable ecg_slot
    GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
            (GU16)0);
    GU16 usRegVal = 0;
    //chip1 clear fifo
    usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
    GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_CLEAR_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
    GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, usRegVal);
    

    GH3X2X_CascadeOperationSlaverChip();
    //chip2 clear fifo, chip2 reset lead status
    usRegVal = GH3X2X_ReadReg(GH3X2X_CARDIFF_CTRL_REG_ADDR);
    GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, GH3X2X_CLEAR_BIT(usRegVal, GH3X2X_CARDIFF_CTRL_START_BIT));
    GH3X2X_WriteReg(GH3X2X_CARDIFF_CTRL_REG_ADDR, usRegVal);
    
    GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
    (GU16)uchSetValue);
    GH3X2X_CascadeOperationMasterChip();
    
    //chip1 enable ecg_slot
    GH3X2X_WriteReg(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR,
    (GU16)uchSetValue);
}

/**
 * @fn     GS8 GH3X2X_FifoWatermarkThrConfig(GU16 usFifoWatermarkThr)
 *
 * @brief  Fifo water mark threshold config
 *
 * @attention   Watermark threshold val must in (2, 800]. if val <= 2, will set 3, if val > 800, set 800;
 *              Be careful that fifo_use_cnt greater than val, gh3x2x willn't generate fifo_watermark interrupt after!
 *
 * @param[in]   usFifoWatermarkThr         watermark threshold val
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 */
GS8 GH3X2X_FifoWatermarkThrConfig(GU16 usFifoWatermarkThr)
{
    GU16 usFifoWatermarkThrVal = usFifoWatermarkThr;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (usFifoWatermarkThrVal < GH3X2X_FIFO_WATERMARK_THR_MIN)
    {
        GH3X2X_DEBUG_LOG_PARAM("fifo wateramrk thr param less than min!\r\n");
        usFifoWatermarkThrVal = GH3X2X_FIFO_WATERMARK_THR_MIN;
    }
    else if (usFifoWatermarkThrVal > GH3X2X_FIFO_WATERMARK_THR_MAX)
    {
        GH3X2X_DEBUG_LOG_PARAM("fifo wateramrk thr param greater than max!\r\n");
        usFifoWatermarkThrVal = GH3X2X_FIFO_WATERMARK_THR_MAX;
    }
    GH3X2X_WAIT_CHIP_WAKEUP();
    GH3X2X_WriteReg(GH3X2X_FIFO_WATERLINE_REG_ADDR, usFifoWatermarkThrVal);
    GH3X2X_WAIT_CHIP_DSLP();
    g_usCurrentFiFoWaterLine = usFifoWatermarkThrVal;
    return GH3X2X_RET_OK;
}

/**
 * @fn     GU16 GH3X2X_GetFifoWatermarkThr(void)
 *
 * @brief  Get fifo water mark threshold
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  fifo water mark threshold
 */
GU16 GH3X2X_GetFifoWatermarkThr(void)
{
    GU16 usFifoWatermarkThrVal = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_WAIT_CHIP_WAKEUP();
    usFifoWatermarkThrVal = GH3X2X_ReadReg(GH3X2X_FIFO_WATERLINE_REG_ADDR);
    GH3X2X_WAIT_CHIP_DSLP();
    return usFifoWatermarkThrVal;
}

/**
 * @fn     GS8 GH3X2X_SlotLedCurrentConfig(GU8 uchSlotIndex, GU8 uchDrvIndex, GU8 uchCurrentVal)
 *
 * @brief  Slot led current config
 *
 * @attention   None
 *
 * @param[in]   uchSlotIndex        slot index [0-7]
 * @param[in]   uchDrvIndex         driver index @ref GH3X2X_LED_DRV_INDEX_0 ... GH3X2X_LED_DRV_INDEX_1
 * @param[in]   uchCurrentVal       current val
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR     return param error
 */
GS8 GH3X2X_SlotLedCurrentConfig(GU8 uchSlotIndex, GU8 uchDrvIndex, GU8 uchCurrentVal)
{
    GU16 usCurrentRegVal = 0;
    GU16 usSlotDrvRegAddr = 0;
    GS8 chRet = GH3X2X_RET_PARAMETER_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((uchSlotIndex < GH3X2X_SLOT_NUM_MAX) && (uchDrvIndex < GH3X2X_SLOT_LED_DRV_NUM_MAX))
    {
        usSlotDrvRegAddr = GH3X2X_SLOT0_CTRL_10_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET)\
                             + (uchDrvIndex * GH3X2X_REG_ADDR_SIZE);
        GH3X2X_WAIT_CHIP_WAKEUP();
        usCurrentRegVal = GH3X2X_ReadReg(usSlotDrvRegAddr);
        GH3X2X_VAL_CLEAR_BIT(usCurrentRegVal, (GU16)GH3X2X_SLOT_LED_CURRENT_CLEAR_MASK);
        GH3X2X_VAL_SET_BIT(usCurrentRegVal, (GU16)uchCurrentVal);
        GH3X2X_WriteReg(usSlotDrvRegAddr, usCurrentRegVal);
        GH3X2X_WAIT_CHIP_DSLP();
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        if (uchSlotIndex >= GH3X2X_SLOT_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("slot index param greater than max!\r\n");
        }
        else // uchDrvIndex >= GH3X2X_SLOT_LED_DRV_NUM_MAX
        {
            GH3X2X_DEBUG_LOG_PARAM("drv index param greater than max!\r\n");
        }
    }
    return chRet;
}

/**
 * @fn     GU8 GH3X2X_GetSlotLedCurrent(GU8 uchSlotIndex, GU8 uchDrvIndex)
 *
 * @brief  Get slot led current config
 *
 * @attention   None
 *
 * @param[in]   uchSlotIndex        slot index [0-7]
 * @param[in]   uchDrvIndex         driver index @ref GH3X2X_LED_DRV_INDEX_0 ... GH3X2X_LED_DRV_INDEX_1
 * @param[out]  None
 *
 * @return  current val. if param error, return val always equal 0
 */
GU8 GH3X2X_GetSlotLedCurrent(GU8 uchSlotIndex, GU8 uchDrvIndex)
{
    GU16 usCurrentRegVal = 0;
    GU16 usSlotDrvRegAddr = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((uchSlotIndex < GH3X2X_SLOT_NUM_MAX) && (uchDrvIndex < GH3X2X_SLOT_LED_DRV_NUM_MAX))
    {
        usSlotDrvRegAddr = GH3X2X_SLOT0_CTRL_10_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET)\
                             + (uchDrvIndex * GH3X2X_REG_ADDR_SIZE);
        GH3X2X_WAIT_CHIP_WAKEUP();
        usCurrentRegVal = GH3X2X_ReadReg(usSlotDrvRegAddr);
        GH3X2X_WAIT_CHIP_DSLP();
    }
    else
    {
        if (uchSlotIndex >= GH3X2X_SLOT_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("slot index param greater than max!\r\n");
        }
        else // uchDrvIndex >= GH3X2X_SLOT_LED_DRV_NUM_MAX
        {
            GH3X2X_DEBUG_LOG_PARAM("drv index param greater than max!\r\n");
        }
    }
    return GH3X2X_GET_LOW_BYTE_FROM_WORD(usCurrentRegVal);
}

/**
 * @fn     GS8 GH3X2X_SlotLedTiaGainConfig(GU8 uchSlotIndex, GU8 uchAdcIndex, GU8 uchGainVal)
 *
 * @brief  Slot gain config
 *
 * @attention   None
 *
 * @param[in]   uchSlotIndex        slot index [0-7]
 * @param[in]   uchAdcIndex         adc index [0-3]
 * @param[in]   uchGainVal          tia gain val [0-12]
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR     return param error
 */
GS8 GH3X2X_SlotLedTiaGainConfig(GU8 uchSlotIndex, GU8 uchAdcIndex, GU8 uchGainVal)
{
    GU16 usGainRegVal = 0;
    GU16 usSlotTiaGainRegAddr = 0;
    GS8 chRet = GH3X2X_RET_PARAMETER_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((uchSlotIndex < GH3X2X_SLOT_NUM_MAX) && (uchAdcIndex < GH3X2X_SLOT_TIA_GAIN_NUM_MAX)
        && (uchGainVal < GH3X2X_SLOT_TIA_GAIN_VAL_MAX))
    {
        usSlotTiaGainRegAddr = GH3X2X_SLOT0_CTRL_4_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET);
        GH3X2X_WAIT_CHIP_WAKEUP();
        usGainRegVal = GH3X2X_ReadReg(usSlotTiaGainRegAddr);
        GH3X2X_VAL_CLEAR_BIT(usGainRegVal,
                                (GH3X2X_SLOT_TIA_GAIN_BITS_MARK << (GH3X2X_SLOT_TIA_GAIN_BITS_SIZE * uchAdcIndex)));
        GH3X2X_VAL_SET_BIT(usGainRegVal, (((GU16)uchGainVal) << (GH3X2X_SLOT_TIA_GAIN_BITS_SIZE * uchAdcIndex)));
        GH3X2X_WriteReg(usSlotTiaGainRegAddr, usGainRegVal);
        GH3X2X_WAIT_CHIP_DSLP();
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        if (uchSlotIndex >= GH3X2X_SLOT_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("slot index param greater than max!\r\n");
        }
        else if (uchAdcIndex >= GH3X2X_SLOT_TIA_GAIN_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("adc index param greater than max!\r\n");
        }
        else
        {
            GH3X2X_DEBUG_LOG_PARAM("gain val param greater than max!\r\n");
        }
    }
    return chRet;
}

/**
 * @fn     GU8 GH3X2X_GetSlotLedTiaGain(GU8 uchSlotIndex, GU8 uchAdcIndex)
 *
 * @brief  Get slot gain config
 *
 * @attention   None
 *
 * @param[in]   uchSlotIndex        slot index [0-7]
 * @param[in]   uchAdcIndex         adc index [0-3]
 * @param[out]  None
 *
 * @return  tia gain val. if param error, return val always equal 0
 */
GU8 GH3X2X_GetSlotLedTiaGain(GU8 uchSlotIndex, GU8 uchAdcIndex)
{
    GU16 usGainRegVal = 0;
    GU16 usSlotTiaGainRegAddr = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((uchSlotIndex < GH3X2X_SLOT_NUM_MAX) && (uchAdcIndex < GH3X2X_SLOT_TIA_GAIN_NUM_MAX))
    {
        usSlotTiaGainRegAddr = GH3X2X_SLOT0_CTRL_4_REG_ADDR + (uchSlotIndex * GH3X2X_SLOT_CTRL_OFFSET);
        GH3X2X_WAIT_CHIP_WAKEUP();
        usGainRegVal = GH3X2X_ReadReg(usSlotTiaGainRegAddr);
        GH3X2X_WAIT_CHIP_DSLP();
    }
    else
    {
        if (uchSlotIndex >= GH3X2X_SLOT_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("slot index param greater than max!\r\n");
        }
        else // uchAdcIndex >= GH3X2X_SLOT_TIA_GAIN_NUM_MAX
        {
            GH3X2X_DEBUG_LOG_PARAM("adc index param greater than max!\r\n");
        }
    }
    return (GU8)(((usGainRegVal) >> (GH3X2X_SLOT_TIA_GAIN_BITS_SIZE * uchAdcIndex)) & GH3X2X_SLOT_TIA_GAIN_BITS_MARK);
}

/**
 * @fn     GS8 GH3X2X_AdcBgcThrConfig(GU8 uchAdcIndex, GU8 uchBgcThrVal)
 *
 * @brief  adc bgc thr config
 *
 * @attention   None
 *
 * @param[in]   uchAdcIndex         adc index [0-3]
 * @param[in]   uchBgcThrVal        bgc thr val [0-7]
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR     return param error
 */
GS8 GH3X2X_AdcBgcThrConfig(GU8 uchAdcIndex, GU8 uchBgcThrVal)
{
    GU16 usBgcThregVal = 0;
    GS8 chRet = GH3X2X_RET_PARAMETER_ERROR;
    GU8 uchKdcCh = (GU8)(GH3X2X_SLOT_KDC_THR_ADC_INDEX_MAX - uchAdcIndex);
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((uchAdcIndex < GH3X2X_SLOT_TIA_GAIN_NUM_MAX) && (uchBgcThrVal < GH3X2X_SLOT_KDC_THR_VAL_MAX))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        usBgcThregVal = GH3X2X_ReadReg(GH3X2X_PPG_DAC_AD_REG1_REG_ADDR);
        GH3X2X_VAL_CLEAR_BIT(usBgcThregVal,
                                (GH3X2X_SLOT_KDC_THR_BITS_MARK << (GH3X2X_SLOT_KDC_THR_BITS_SIZE * uchKdcCh)));
        GH3X2X_VAL_SET_BIT(usBgcThregVal, (((GU16)uchBgcThrVal) << (GH3X2X_SLOT_KDC_THR_BITS_SIZE * uchKdcCh)));
        GH3X2X_WriteReg(GH3X2X_PPG_DAC_AD_REG1_REG_ADDR, usBgcThregVal);
        GH3X2X_WAIT_CHIP_DSLP();
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        if (uchAdcIndex >= GH3X2X_SLOT_TIA_GAIN_NUM_MAX)
        {
            GH3X2X_DEBUG_LOG_PARAM("adc index param greater than max!\r\n");
        }
        else
        {
            GH3X2X_DEBUG_LOG_PARAM("bgc thr val param greater than max!\r\n");
        }
    }
    return chRet;
}

/**
 * @fn     GU8 GH3X2X_GetAdcBgcThr(GU8 uchAdcIndex)
 *
 * @brief  Get adc bgc thr config
 *
 * @attention   None
 *
 * @param[in]   uchAdcIndex         adc index [0-3]
 * @param[out]  None
 *
 * @return  tia gain val. if param error, return val always equal 0
 */
GU8 GH3X2X_GetAdcBgcThr(GU8 uchAdcIndex)
{
    GU16 usBgcThregVal = 0;
    GU8 uchKdcCh = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (uchAdcIndex < GH3X2X_SLOT_TIA_GAIN_NUM_MAX)
    {
        uchKdcCh = (GU8)(GH3X2X_SLOT_KDC_THR_ADC_INDEX_MAX - uchAdcIndex);
        GH3X2X_WAIT_CHIP_WAKEUP();
        usBgcThregVal = GH3X2X_ReadReg(GH3X2X_PPG_DAC_AD_REG1_REG_ADDR);
        GH3X2X_WAIT_CHIP_DSLP();
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("adc index param greater than max!\r\n");
    }
    return (GU8)(((usBgcThregVal) >> (GH3X2X_SLOT_KDC_THR_BITS_SIZE * uchKdcCh)) & GH3X2X_SLOT_KDC_THR_BITS_MARK);
}

/**
 * @fn     GS8 GH3X2X_IrqWidthConfig(GU16 usIrqPulseWidth, GU16 usIrqColdWidth)
 *
 * @brief  Irq width config
 *
 * @attention   Irq val must in [0, 2047]. real_width = (val + 1) * low_clk_tick;
 *              low_clk default 32k, if set ext clk enable that use low_ext_clk;
 *
 * @param[in]   usIrqPulseWidth         irq pulse width val
 * @param[in]   usIrqColdWidth          irq cold width val
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 */
GS8 GH3X2X_IrqWidthConfig(GU16 usIrqPulseWidth, GU16 usIrqColdWidth)
{
    GU16 usIrqPulseWidthVal = usIrqPulseWidth;
    GU16 usIrqColdWidthVal = usIrqColdWidth;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (usIrqPulseWidthVal > GH3X2X_IRQ_WIDTH_VAL_MAX)
    {
        GH3X2X_DEBUG_LOG_PARAM("irq pulse width param greater than max!\r\n");
        usIrqPulseWidthVal = GH3X2X_IRQ_WIDTH_VAL_MAX;
    }
    if (usIrqColdWidthVal > GH3X2X_IRQ_WIDTH_VAL_MAX)
    {
        GH3X2X_DEBUG_LOG_PARAM("irq cold width param greater than max!\r\n");
        usIrqColdWidthVal = GH3X2X_IRQ_WIDTH_VAL_MAX;
    }
    GH3X2X_WAIT_CHIP_WAKEUP();
    GH3X2X_WriteReg(GH3X2X_INT_PWR_REG_ADDR, usIrqPulseWidthVal);
    GH3X2X_WriteReg(GH3X2X_INT_CTR_REG_ADDR, usIrqColdWidthVal);
    GH3X2X_WAIT_CHIP_DSLP();
    return GH3X2X_RET_OK;
}

/**
 * @fn     GS8 GH3X2X_IrqModeConfig(EMIrqModeConfig emIrqMode)
 *
 * @brief  Irq mode config
 *
 * @attention   None
 *
 * @param[in]   emIrqMode         irq mode config @ref EMIrqModeConfig
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 */
GS8 GH3X2X_IrqModeConfig(EMIrqModeConfig emIrqMode)
{
    GU16 usIrqCtrlRegVal = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_WAIT_CHIP_WAKEUP();
    usIrqCtrlRegVal = GH3X2X_ReadReg(GH3X2X_INT_CR_REG_ADDR);
    GH3X2X_VAL_CLEAR_BIT(usIrqCtrlRegVal, (GU16)GH3X2X_INT_CTRL_MODE_MSK_BIT);
    GH3X2X_VAL_SET_BIT(usIrqCtrlRegVal, ((GU16)emIrqMode) & GH3X2X_INT_CTRL_MODE_MSK_BIT);
    GH3X2X_WriteReg(GH3X2X_INT_CR_REG_ADDR, usIrqCtrlRegVal);
    GH3X2X_WAIT_CHIP_DSLP();
    return GH3X2X_RET_OK;
}

/**
 * @fn     void GH3X2X_RegisterResetPinControlFunc(void (*pResetPinLevelControlFunc)(GU8 uchPinLevel))
 *
 * @brief  Register reset pin level control function
 *
 * @attention   Pin level set val should define as 1 [high level] or 0 [low level];
 *
 * @param[in]   pResetPinLevelControlFunc       pointer to set reset pin level function
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_RegisterResetPinControlFunc(void (*pResetPinLevelControlFunc)(GU8 uchPinLevel))
{
    g_pGh3x2xResetPinLevelControlFunc = pResetPinLevelControlFunc;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
}

/**
 * @fn     GS8 GH3X2X_HardReset(void)
 *
 * @brief  Gh3x2x softreset via i2c/spi, can read&write reg with gh3x2x after reset
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    reset pin control function not register
 */
GS8 GH3X2X_HardReset(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (g_pGh3x2xResetPinLevelControlFunc != GH3X2X_PTR_NULL)
    {
        g_pGh3x2xResetPinLevelControlFunc(0);
        GH3X2X_DelayUs(GH3X2X_HARD_RESET_DELAY_X_US); /* hard reset delay 20us. */
        g_pGh3x2xResetPinLevelControlFunc(1);
        chRet = GH3X2X_RET_OK;
    }
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("reset pin ctrl func has not register!\r\n");
    }
    g_uchActiveChipResetFlag = 1;
    if (g_uchResetFromProtocolFlag == 1)
    {
        g_uchResetFromProtocolFlag = 0;
        HOOK_FUNC_CALL(g_pGh3x2xResetByProtocolHookFunc, () );  /* call hook */
    }
    return chRet;
}

/**
 * @fn     void GH3X2X_WearDetectEnable(EMWearDetectEnableType emWearDetectEnable)
 *
 * @brief  enable or disable wear detect
 *
 * @attention   None
 *
 * @param[in]   emWearDetectEnable      enable or disable @ref EMWearDetectEnableType
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_WearDetectEnable(EMWearDetectEnableType emWearDetectEnable)
{
    GU16 usWearStatus = 0;

    GH3X2X_WAIT_CHIP_WAKEUP();
    usWearStatus = GH3X2X_ReadReg(GH3X2X_ADT_WEARON_CR_REG_ADDR);
    
    if (WEAR_DETECT_ENABLE == emWearDetectEnable)
    {
        GH3X2X_VAL_SET_BIT(usWearStatus, GH3X2X_ADT_WEAR_DET_EN_MASK_BIT);
    }
    else
    {
        GH3X2X_VAL_CLEAR_BIT(usWearStatus, (GU16)GH3X2X_ADT_WEAR_DET_EN_MASK_BIT);
    }

    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); 
    GH3X2X_WAIT_CHIP_DSLP();
}

/**
 * @fn     void GH3X2X_ResetHardAdt(void)
 *
 * @brief  Gh3x2x reset hard adt  
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_ResetHardAdt(void)
{
    /* clear int status. restart adt */
    GH3X2X_ExitLowPowerMode();
    GH3X2X_WriteRegBitField(GH3X2X_ADT_WEARON_CR_REG_ADDR,0,0,0);
    GH3X2X_WriteReg(GH3X2X_INT_STR_REG_ADDR, GH3X2X_IRQ_MSK_WEAR_ON_BIT | GH3X2X_IRQ_MSK_WEAR_OFF_BIT);
    GH3X2X_WriteRegBitField(GH3X2X_ADT_WEARON_CR_REG_ADDR,0,0,1);
    GH3X2X_EnterLowPowerMode();
}

/**
 * @fn     GS8 GH3X2X_WearDetectSwitchTo(EMWearDetectType emWearDetectType, EMWearDetectForceSwitch emForceSwitch)
 *
 * @brief  Gh3x2x switch to detect wear on/off type 
 *
 * @attention   Should follow that use WEAR_DETECT_DONT_FORCE_SWITCH @wear on/off irq status process.
 *              If want force switch to detect on/off, should use WEAR_DETECT_FORCE_SWITCH. careful!!!
 *
 * @param[in]   emWearDetectType        switch to detect type @ref EMWearDetectType
 * @param[in]   emForceSwitch           force switch type @ref EMWearDetectForceSwitch
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR       detect type equal last type, don't need to switch
 * @retval  #GH3X2X_RET_PARAMETER_ERROR     return param error
 */
GS8 GH3X2X_WearDetectSwitchTo(EMWearDetectType emWearDetectType, EMWearDetectForceSwitch emForceSwitch)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GU16 usWearStatus = 0;
    GU16 usWearLogicSel = 0;

    GH3X2X_DEBUG_LOG_PARAM("%s:type = %d\r\n", __FUNCTION__, emWearDetectType);
    if (emWearDetectType > WEAR_DETECT_WEAR_OFF)
    {
        GH3X2X_DEBUG_LOG_PARAM("param set error! @ref EMWearDetectType\r\n");
        chRet = GH3X2X_RET_PARAMETER_ERROR;
    }
    else
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        usWearStatus = GH3X2X_ReadReg(GH3X2X_ADT_WEARON_CR_REG_ADDR);
        GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WearDetectSwitchTo] usWearStatus = %d\r\n",(int)usWearStatus);
        if (emWearDetectType == WEAR_DETECT_WEAR_ON) // switch to detect wear on (got wear off evt)
        {
            if ((GH3X2X_CHECK_BIT_SET(usWearStatus, GH3X2X_ADT_WEAR_STATUS_MASK_BIT)) \
                || (emForceSwitch != WEAR_DETECT_DONT_FORCE_SWITCH)) // system is wear on
            {
                GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WearDetectSwitchTo] WEAR_DETECT_WEAR_ON\r\n");
#if (GH3X2X_WEAR_ON_FORCE_SWITCH_WITH_LOGIC_SEL) // wear on force switch with adt logic sel
                GH3X2X_VAL_CLEAR_BIT(usWearStatus, (GU16)GH3X2X_ADT_WEAR_STATUS_MASK_BIT);
                if (emForceSwitch != WEAR_DETECT_DONT_FORCE_SWITCH)
                {
                    usWearLogicSel = GH3X2X_ReadReg(GH3X2X_ADT_WEARON_LOGIC_SEL_REG_ADDR);
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_LOGIC_SEL_REG_ADDR, 0); // clear all logic
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); // set system into wear off
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, \
                                    GH3X2X_SET_BIT(usWearStatus, GH3X2X_ADT_WEAR_CR_WEAR_ON_VAL));
                    GH3X2X_DelayUs(GH3X2X_WEAR_DETECT_SWITCH_WAIT_X_US);
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_LOGIC_SEL_REG_ADDR, usWearLogicSel); // re-sel wear on logic
                }
                else
                {
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); // set system into wear off
                }
#else
                GH3X2X_VAL_CLEAR_BIT(usWearStatus, (GU16)GH3X2X_ADT_WEAR_STATUS_MASK_BIT);
                if ((GH3X2X_CHECK_BIT_SET(usWearStatus, GH3X2X_ADT_WEAR_DET_EN_MASK_BIT))
                    && (emForceSwitch != WEAR_DETECT_DONT_FORCE_SWITCH))
                {
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, \
                                        GH3X2X_CLEAR_BIT(usWearStatus, GH3X2X_ADT_WEAR_DET_EN_MASK_BIT));

                              GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WearDetectSwitchTo] write det en to 0 !!!\r\n");
                }
                GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); // set system into wear off
                GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WearDetectSwitchTo] usWearStatus = %d\r\n",(int)usWearStatus);
#endif
                chRet = GH3X2X_RET_OK;
            } // end of if (GH3X2X_CHECK_BIT_SET(usWearStatus, GH3X2X_ADT_WEAR_STATUS_MASK_BIT))
        } // end of if (emWearDetectType == WEAR_DETECT_WEAR_ON)
        else // switch to detect wear off (got wear on evt)
        {
            if (GH3X2X_CHECK_BIT_NOT_SET(usWearStatus, GH3X2X_ADT_WEAR_STATUS_MASK_BIT) \
                || (emForceSwitch != WEAR_DETECT_DONT_FORCE_SWITCH)) // system is wear off
            {
                GH3X2X_DEBUG_LOG_PARAM("[GH3X2X_WearDetectSwitchTo] WEAR_DETECT_WEAR_OFF\r\n");
                GH3X2X_VAL_SET_BIT(usWearStatus, GH3X2X_ADT_WEAR_STATUS_MASK_BIT);
                if (emForceSwitch != WEAR_DETECT_DONT_FORCE_SWITCH)
                {
                    usWearLogicSel = GH3X2X_ReadReg(GH3X2X_ADT_WEAROFF_LOGIC_SEL_REG_ADDR);
                    GH3X2X_WriteReg(GH3X2X_ADT_WEAROFF_LOGIC_SEL_REG_ADDR, 0); // clear all logic
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); // set system into wear on
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, \
                                    GH3X2X_SET_BIT(usWearStatus, GH3X2X_ADT_WEAR_CR_WEAR_OFF_VAL));
                    GH3X2X_DelayUs(GH3X2X_WEAR_DETECT_SWITCH_WAIT_X_US);
                    GH3X2X_WriteReg(GH3X2X_ADT_WEAROFF_LOGIC_SEL_REG_ADDR, usWearLogicSel); // re-sel wear off logic
                }
                else
                {
                    GH3X2X_WriteReg(GH3X2X_ADT_WEARON_CR_REG_ADDR, usWearStatus); // set system into wear on
                }
                chRet = GH3X2X_RET_OK;
            }
        }
        GH3X2X_WAIT_CHIP_DSLP();
    } // end of else if (emWearDetectType > WEAR_DETECT_WEAR_OFF)
    return chRet;
}

/**
 * @fn     EMSkinColorStatusType GH3X2X_GetSkinStatus(void)
 *
 * @brief  Get gh3x2x skin color status
 *
 * @attention   Must using when chip hard wear detect module enable, if not always return 0
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  skin color status @ref EMSkinColorStatusType
 */
EMSkinColorStatusType GH3X2X_GetSkinStatus(void)
{
    GU16 usSkinStatus = 0;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    GH3X2X_WAIT_CHIP_WAKEUP();
    usSkinStatus = GH3X2X_ReadReg(GH3X2X_SKIN_STR_REG_ADDR);
    GH3X2X_WAIT_CHIP_DSLP();
    return ((EMSkinColorStatusType) GH3X2X_VAL_GET_BIT(usSkinStatus, GH3X2X_SKIN_COLOR_STATUS_MASK_BIT));
}

/**
 * @fn     GS8 GH3X2X_HsiClockCalibration(void)
 *
 * @brief  Calibration Hsi(high-speed internal) clock
 *
 * @attention   must use 4M clock input @OSCIN pin
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR       input clock is unstable, or sampling was started
 */
GS8 GH3X2X_HsiClockCalibration(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (g_uchGh3x2xStatus < GH3X2X_STATUS_STARTED)
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        GH3X2X_WriteReg(GH3X2X_OSC_THR_REG_ADDR, GH3X2X_HSI_CALI_THR_CNT_VAL);
        GH3X2X_WriteReg(GH3X2X_OSC_CR_REG_ADDR, GH3X2X_HSI_CALI_CTRL_EN_VAL);
        GH3X2X_DelayUs(GH3X2X_HSI_CALI_DELAY_VAL);
        if (GH3X2X_CHECK_BIT_SET(GH3X2X_ReadReg(GH3X2X_OSC_FLAG_REG_ADDR), GH3X2X_OSC_CALI_CODE_LOACKED))
        {
            chRet = GH3X2X_RET_OK;
        }
        GH3X2X_DEBUG_LOG_PARAM("Hsi: 0x%.4x\r\n", GH3X2X_ReadReg(GH3X2X_OSC13M_TUNE_REG_ADDR));
        GH3X2X_WriteReg(GH3X2X_OSC_CR_REG_ADDR, GH3X2X_OSC_CALI_CTRL_DIS_VAL);
        GH3X2X_WAIT_CHIP_DSLP();
    }
    
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_LsiClockCalibration(void)
 *
 * @brief  Calibration Lsi(low-speed internal) clock
 *
 * @attention   must use 4M clock input @OSCIN pin, take case that will consume about 100ms
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  errcode
 * @retval  #GH3X2X_RET_OK                  return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR       input clock is unstable, or sampling was started
 */
GS8 GH3X2X_LsiClockCalibration(void)
{
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;
    GU16 usCaliVal = 0;
    GS16 sFreqErrVal = 0;
    GS16 sLastFreqErrVal = GH3X2X_LSI_CALI_ERR_MAX_VAL;
    GU8 uchIndex = 0;
    GU8 uchMinVal = 0;
    GU8 uchMaxVal = GH3X2X_LSI_CALI_FINE_VAL_MAX;
    GU8 uchCalcVal = 0;
    GU8 uchLastCalcVal = GH3X2X_LSI_CALI_FINE_VAL_MAX;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if (g_uchGh3x2xStatus < GH3X2X_STATUS_STARTED)
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        GH3X2X_WriteReg(GH3X2X_OSC_THR_REG_ADDR, GH3X2X_LSI_CALI_THR_CNT_VAL);
        GH3X2X_WriteReg(GH3X2X_OSC_CR_REG_ADDR, GH3X2X_LSI_CALI_CTRL_C_EN_VAL);
        GH3X2X_DelayUs(GH3X2X_LSI_COR_CALI_DELAY_VAL);
        if (GH3X2X_CHECK_BIT_SET(GH3X2X_ReadReg(GH3X2X_OSC_FLAG_REG_ADDR), GH3X2X_OSC_CALI_CODE_LOACKED))
        {
            usCaliVal = GH3X2X_ReadReg(GH3X2X_OSC32K_TUNE_REG_ADDR);
            uchCalcVal = GH3X2X_GET_HIGH_BYTE_FROM_WORD(usCaliVal);
            for (uchIndex = 0; uchIndex < GH3X2X_LSI_CALI_FINE_TUNE_MAX; uchIndex++)
            {
                usCaliVal = GH3X2X_MAKEUP_WORD(uchCalcVal, GH3X2X_GET_LOW_BYTE_FROM_WORD(usCaliVal));
                GH3X2X_WriteReg(GH3X2X_OSC32K_TUNE_REG_ADDR, usCaliVal);
                GH3X2X_WriteReg(GH3X2X_OSC_CR_REG_ADDR, GH3X2X_LSI_CALI_CTRL_F_EN_VAL);
                GH3X2X_DelayUs(GH3X2X_LSI_FINE_CALI_DELAY_VAL);
                sFreqErrVal = GH3X2X_ReadReg(GH3X2X_OSC_FREQ_ERR_UR_REG_ADDR);
                if (sFreqErrVal > GH3X2X_LSI_CALI_ERR_MAX_VAL)
                {
                    sFreqErrVal = (GS16)(GH3X2X_LSI_CALI_ERR_FIXED_VAL - sFreqErrVal);
                    uchMinVal = uchCalcVal;
                }
                else
                {
                    uchMaxVal = uchCalcVal;
                }
                uchCalcVal = ((GU16)uchMaxVal + (GU16)uchMinVal) / GH3X2X_LSI_CALI_FINE_DIV_NUM;
                if (uchLastCalcVal == uchCalcVal)
                {
                    chRet = GH3X2X_RET_OK;
                    if (sLastFreqErrVal < sFreqErrVal)
                    {
                        usCaliVal = GH3X2X_MAKEUP_WORD(uchMaxVal, GH3X2X_GET_LOW_BYTE_FROM_WORD(usCaliVal));
                        GH3X2X_WriteReg(GH3X2X_OSC32K_TUNE_REG_ADDR, usCaliVal);
                    }
                    break;
                }
                uchLastCalcVal = uchCalcVal;
                if (sLastFreqErrVal > sFreqErrVal)
                {
                    sLastFreqErrVal = sFreqErrVal;
                }
            } // end of for (uchIndex = 0; uchIndex < GH3X2X_LSI_CALI_FINE_TUNE_MAX; uchIndex++)
        } // end of if (GH3X2X_CHECK_BIT_SET(GH3X2X_ReadReg(GH3X2X_OSC_FLAG_REG_ADDR), GH3X2X_OSC_CALI_CODE_LOACKED))
        GH3X2X_DEBUG_LOG_PARAM("Lsi: 0x%.4x\r\n", GH3X2X_ReadReg(GH3X2X_OSC32K_TUNE_REG_ADDR));
        GH3X2X_WriteReg(GH3X2X_OSC_CR_REG_ADDR, GH3X2X_OSC_CALI_CTRL_DIS_VAL);
        GH3X2X_WAIT_CHIP_DSLP();
    } // end of if (g_uchGh3x2xStatus < GH3X2X_STATUS_STARTED)
    return chRet;
}

/**
 * @fn     void GH3X2X_RegisterReadPinStatusFunc(GU8 (*pReadIntPinStatusFunc)(void),
 *                                     GU8 (*pReadResetPinStatusFunc)(void),
 *                                     GU8 (*pReadSpcsPinStatusFunc)(void),
 *                                     GU8 (*pReadSpdoPinStatusFunc)(void))
 *
 * @brief  Register read pin status function
 *
 * @attention   Only use for debug, read gh3x2x some pin status can debug some hardware errors;
 *              Pin status return should define as 1 [high level] or 0 [low level];
 *
 * @param[in]   pReadIntPinStatusFunc       pointer to read int pin staus function
 * @param[in]   pReadResetPinStatusFunc     pointer to read reset pin staus function
 * @param[in]   pReadSpcsPinStatusFunc      pointer to read spcs pin staus function
 * @param[in]   pReadSpdoPinStatusFunc      pointer to read spdo pin staus function
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_RegisterReadPinStatusFunc(GU8 (*pReadIntPinStatusFunc)(void),
                                      GU8 (*pReadResetPinStatusFunc)(void),
                                      GU8 (*pReadSpcsPinStatusFunc)(void),
                                      GU8 (*pReadSpdoPinStatusFunc)(void))
{
    g_pGh3x2xReadIntPinStatusFunc = pReadIntPinStatusFunc;
    g_pGh3x2xReadResetPinStatusFunc = pReadResetPinStatusFunc;
    g_pGh3x2xReadSpcsPinStatusFunc = pReadSpcsPinStatusFunc;
    g_pGh3x2xReadSpdoPinStatusFunc = pReadSpdoPinStatusFunc;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
}

/**
 * @fn     GCHAR *GH3X2X_GetDriverLibVersion(void)
 *
 * @brief  Get driver version
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  library version string
 */
GCHAR *GH3X2X_GetDriverLibVersion(void)
{
    return (GCHAR *)GH3X2X_VERSION_STRING;
}

/**
 * @fn     void GH3X2X_SetMaxNumWhenReadFifo(GU16 usMaxNum)
 *
 * @brief  Set max number of rawdata read from fifo every time
 *
 * @attention   None.
 *
 * @param[in]   usMaxNum        max number of rawdata read from fifo
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_SetMaxNumWhenReadFifo(GU16 usMaxNum)
{
    if (usMaxNum <= GH3X2X_FIFO_DATA_BYTES_MAX_LEN)
    {
        g_usMaxNumReadFromFifo = (usMaxNum/4) * 4;
    }    
}

/**
 * @fn     GU16 GH3X2X_GetCurrentFifoWaterLine(void)
 *
 * @brief  get current fifo water line setting
 *
 * @attention   None.
 *
 * @param[in]   None
 * @param[out]  fifo water line
 *
 * @return  None
 */
GU16 GH3X2X_GetCurrentFifoWaterLine(void)
{
    return g_usCurrentFiFoWaterLine;
}

const GU8 g_uchLedCurrentFullScal[4] = {25,50,100,200};

/**
 * @fn      GU8  Gh3x2x_GetLedCurrentFullScal(void)
 
 *
 * @brief  
 *
 * @attention   None.
 *
 * @param[in]   None
 * @param[out]  Drv 0/1Current fs (mA)
 *
 * @return  None
 */
void  GH3x2x_GetLedCurrentFullScal(GU8 *puchDrv0Fs, GU8 *puchDrv1Fs)
{
    (*puchDrv0Fs)= GH3X2X_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV0_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV0_FULL_SCAL_CURRENT_MSB);
    (*puchDrv1Fs)= GH3X2X_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV1_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV1_FULL_SCAL_CURRENT_MSB);
    (*puchDrv0Fs) = g_uchLedCurrentFullScal[*puchDrv0Fs];
    (*puchDrv1Fs) = g_uchLedCurrentFullScal[*puchDrv1Fs];
}

void GH3x2x_DisableHardwareAgc(void)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt ++)
    {
        GH3X2X_WriteRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotCnt,GH3X2X_SLOT_AGC_EN_LSB,GH3X2X_SLOT_AGC_EN_MSB, 0);
    }
}

void GH3x2x_GetBgLevel(GU8 *puchBglevel)
{    
    for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt ++)
    {
        puchBglevel[uchSlotCnt] = GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotCnt,GH3X2X_SLOT_BG_LEVEL_LSB,GH3X2X_SLOT_BG_LEVEL_MSB);
    }
}

void GH3x2x_GetBgCancel(GU8 *puchBgCancel)
{    
    for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt ++)
    {
        puchBgCancel[uchSlotCnt] = GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotCnt,GH3X2X_SLOT_BG_CANCEL_LSB,GH3X2X_SLOT_BG_CANCEL_MSB);
    }
}

void GH3x2x_SetIntTime(GU8 uchSlotNo, GU8 uchIntTimeIndex)
{    
    GH3X2X_WriteRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo,GH3X2X_SLOT_ADC_INT_TIME_LSB,GH3X2X_SLOT_ADC_INT_TIME_MSB, uchIntTimeIndex);
}

void GH3x2x_SetSlotTime(GU8 uchSlotNo, GU16 usSlotTime)
{    
    GH3X2X_WriteReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR + 2*uchSlotNo, usSlotTime);
}

void GH3x2x_SetSlotSampleRate(GU8 uchSlotNo, GU16 usSampleRate)
{    
    GU16 usDivReg;
    if(usSampleRate > 1000)
    {
        usSampleRate = 1000;
    }
    if(usSampleRate < 5)
    {
        usSampleRate = 5;
    }
    usDivReg = (1000/usSampleRate) - 1;
    GH3X2X_WriteRegBitField(GH3X2X_SLOT0_CTRL_0_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo,GH3X2X_SLOT_SR_LSB,GH3X2X_SLOT_SR_MSB, usDivReg);
    GH3X2X_DEBUG_LOG_PARAM("GH3x2x_SetSlotSampleRate: uchSlotNo = %d, usSampleRate = %d, usDivReg = %d \r\n",(int)uchSlotNo,(int)usSampleRate,(int)usDivReg);
}

const GU16 gusSlotTimeList[3][7] = {
{26,    36,    46,    56,    95,    174,    331},
{48,    67,    87,    107,    186,    343,    658},
{69,    99,    128,    158,    276,    512,    985}
};

GU16 GH3x2x_CalSlotTime(GU8 uchBgLevel, GU8 uchBgCancel ,GU8 uchIntTimeIndex)
{
    GU16 usSlotTime;
    if(uchBgLevel > 2)
    {
        uchBgLevel = 2;
    }
    usSlotTime = gusSlotTimeList[uchBgLevel][uchIntTimeIndex];
    if(uchBgCancel > 0)
    {
        usSlotTime += 104;
    }

    usSlotTime += 80;  //fix pd remain problem

    GH3X2X_DEBUG_LOG_PARAM("GH3x2x_CalSlotTime: bg level = %d, bg cancel = %d, IntTimeIndex = %d, TimeSlot = %d \r\n",(int)uchBgLevel,(int)uchBgCancel,(int)uchIntTimeIndex,(int)usSlotTime);

    return usSlotTime;
}

GS8 g_chDrvECode = 0;

void GH3X2X_SetDrvEcode(GS8 chECode)
{
    g_chDrvECode = chECode;
}

#if (0 == __CURRENT_ECODE_CALIBRATE__)
GU16 GH3X2X_SetLedDrvCurrentByEcode(GU8 uchCurrent_mA, GU8 uchDrvNum, GS8 chECODE)
{
    GU8 uchDrFs[2] = {0};
    GH3x2x_GetLedCurrentFullScal(&uchDrFs[0], &uchDrFs[1]);
    GU16 usDrvReg = ((GU16)uchCurrent_mA) * 255 /uchDrFs[uchDrvNum];
    return usDrvReg;
}
void GH3X2X_SetConfigDrvEcode(const STGh3x2xInitConfig *pstGh3x2xInitConfigParam)
{
    (void) pstGh3x2xInitConfigParam;
}
#else
GU16 GH3X2X_SetLedDrvCurrentByEcode(GU8 uchCurrent_mA, GU8 uchDrvNum, GS8 chECODE)
{
    GF32 fEr;
    GF32 fY;
    GU8  uchN;
    GU16 usDrvCode;
    GU16 usDrvScale;
    
    if(uchDrvNum == 0)
    {
        fY = 0.1;
        usDrvScale = GH3X2X_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV0_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV0_FULL_SCAL_CURRENT_MSB);
    }
    else
    {
        fY = 0.01;
        usDrvScale = GH3X2X_ReadRegBitField(GH3X2X_LED_DRV_AD_REG_REG_ADDR,GH3X2X_DRV1_FULL_SCAL_CURRENT_LSB,GH3X2X_DRV1_FULL_SCAL_CURRENT_MSB);
        
    }
    uchN = 1 << (3 - usDrvScale);   

    fEr = (GF32)((GF32)chECODE/256) + 1.0f;
    usDrvCode = (GU16)(0.5f+((GF32)fEr*255*(GF32)uchN)/(0.225f/((GF32)uchCurrent_mA/1000) - fY));
    
    return usDrvCode;
}

void GH3X2X_SetConfigDrvEcode(const STGh3x2xInitConfig *pstGh3x2xInitConfigParam)
{
    if (pstGh3x2xInitConfigParam == GH3X2X_PTR_NULL)
    {
        return;
    }
    GU8 uchDr0Fs = 0;
    GU8 uchDr1Fs = 0;
    GH3x2x_GetLedCurrentFullScal(&uchDr0Fs, &uchDr1Fs);
    for (GU16 usArrCnt = 0; usArrCnt < pstGh3x2xInitConfigParam->usConfigArrLen ; usArrCnt ++)
    {
        GU16 usRegData = pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegData;
        GU16 usRegAddr = pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr;
        if ((pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr & 0xF000) == 0)
        {
            for (int uchSlotIndex = 0; uchSlotIndex < 8; uchSlotIndex++)
            {
                if ((pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr) == 0x011E + (uchSlotIndex * 0x001C) + (0 * 0x0002))
                {
                    GU8 uchCurrent = (GU8)((double)(usRegData&0xFF) * (double)uchDr0Fs / 255 + 0.5);
                    GU16 usDrv0Reg = ((usRegData & 0xFF00) | GH3X2X_SetLedDrvCurrentByEcode(uchCurrent, 0, g_chDrvECode));
                    GH3X2X_INFO_LOG_PARAM("[%s]:slot%d DRV0 addr=0x%04x CURvalue=0x%04x change2 0x%04x\n", __FUNCTION__, uchSlotIndex, usRegAddr, usRegData, usDrv0Reg);
                    GH3X2X_WriteReg(pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr, usDrv0Reg);
                    if (usDrv0Reg != GH3X2X_ReadReg(usRegAddr))
                    {
                        GH3X2X_INFO_LOG_PARAM("[%s]:reg verify error!!!\r\n", __FUNCTION__);
                    }
                }
                else if ((pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr) == 0x011E + (uchSlotIndex * 0x001C) + (1 * 0x0002))
                {
                    GU8 uchCurrent = (GU8)((double)(usRegData&0xFF) * (double)uchDr1Fs / 255 + 0.5);
                    GU16 usDrv1Reg = ((usRegData & 0xFF00) | GH3X2X_SetLedDrvCurrentByEcode(uchCurrent, 1, g_chDrvECode));
                    GH3X2X_INFO_LOG_PARAM("[%s]:slot%d DRV1 addr=0x%04x CURvalue=0x%04x change2 0x%04x\n", __FUNCTION__, uchSlotIndex, usRegAddr, usRegData, usDrv1Reg);
                    GH3X2X_WriteReg(pstGh3x2xInitConfigParam->pstRegConfigArr[usArrCnt].usRegAddr, usDrv1Reg);
                    if (usDrv1Reg != GH3X2X_ReadReg(usRegAddr))
                    {
                        GH3X2X_INFO_LOG_PARAM("[%s]:reg verify error!!!\r\n", __FUNCTION__);
                    }
                }
            }
        }
    }
}
#endif

/**
 * @fn      void GH3x2x_ChangeSampleParmForEngineeringMode(GU32 unFunctionMode, STGh3x2xEngineeringModeSampleParam *pstSampleParaGroup , GU8 uchSampleParaGroupNum)
 *
 * @brief  
 *
 * @attention   None.
 *
 * @param[in]   unFunctionMode: function group exp: GH3X2X_FUNCTION_ADT|GH3X2X_FUNCTION_HR
 * @param[in]   pstSampleParaGroup   the pointer of engineering mode sample param group struct array base
 * @param[in]   uchSampleParaGroupNum   engineering mode sample param group number
 * @param[out]  None
 *
 * @return  None
 */
void GH3x2x_ChangeSampleParmForEngineeringMode(const STGh3x2xFrameInfo * const  pstGh3x2xFrameInfo[], GU32 unFunctionMode, STGh3x2xEngineeringModeSampleParam *pstSampleParaGroup , GU8 uchSampleParaGroupNum)
{
    GU32 unFunctionID;
    GU8 uchFunctionCnt;
    GU8 *puchChnlMap;
    GU8 uchChnlNum;
    GU8 uchDr0Fs;
    GU8 uchDr1Fs;
    GU8 puchBglevel[8];    //slot0 ~ slot1 bg level
    GU8 puchBgCancel[8];   //slot0 ~ slot1 bg cancel

    if(0 == pstSampleParaGroup) //is the point is invalid, do nothing
    {
        return ;
    }

    //read current full scale
    GH3x2x_GetLedCurrentFullScal(&uchDr0Fs, &uchDr1Fs);

    //read bg level
    GH3x2x_GetBgLevel(puchBglevel);

    //read bg cancel
    GH3x2x_GetBgCancel(puchBgCancel);

    //disable all hardware age slot
    GH3x2x_DisableHardwareAgc();

    //process every sample parm group
    for(GU8 uchSampleParaGroupCnt = 0; uchSampleParaGroupCnt < uchSampleParaGroupNum; uchSampleParaGroupCnt ++)
    {
        STGh3x2xEngineeringModeSampleParam *pstTempSampleParam = pstSampleParaGroup + uchSampleParaGroupCnt;
        unFunctionID = pstTempSampleParam->unFunctionID;
        if(0 == unFunctionID)
        {
            continue;
        }
        puchChnlMap = 0;
        uchChnlNum = 0;

        if(unFunctionID == (unFunctionMode&unFunctionID))   //unFunctionMode have cover unFunctionID
        {
            // get channl map and chnl num
            for (uchFunctionCnt = 0 ; uchFunctionCnt < GH3X2X_FUNC_OFFSET_MAX ; uchFunctionCnt ++)
            {
                if(pstGh3x2xFrameInfo[uchFunctionCnt])
                {
                    if(unFunctionID == (((GU32)1) << uchFunctionCnt))
                    {
                        break;
                    }
                }
            }
            if(GH3X2X_FUNC_OFFSET_MAX == uchFunctionCnt)
            {
                GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] can not find valid pstGh3x2xFrameInfo !!! unFunctionID = %d\r\n", (int)unFunctionID);
                continue;
            }
            GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] pstGh3x2xFrameInfo uchFunctionCnt = %d\r\n", (int)uchFunctionCnt);
            puchChnlMap = pstGh3x2xFrameInfo[uchFunctionCnt]->pchChnlMap;
            uchChnlNum = pstGh3x2xFrameInfo[uchFunctionCnt]->pstFunctionInfo->uchChnlNum;

            if((0 != puchChnlMap)&&(0 != uchChnlNum)) //channel map and channel num is all valid
            {
                GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] ParaGroupCnt = %d, uchFunctionID = %d, uchChnlNum = %d\r\n", (int)uchSampleParaGroupCnt, (int)unFunctionID, (int)uchChnlNum);
                for(GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
                {
                    GU8  uchTempSlotNo = GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(puchChnlMap[uchChnlCnt]);
                    GU8  uchTempAdtNo = GH3X2X_BYTE_RAWDATA_GET_ADC_NUM(puchChnlMap[uchChnlCnt]);
                    GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] uchTempSlotNo = %d, uchTempAdtNo = %d \r\n", (int)uchTempSlotNo, (int)uchTempAdtNo);
                    if(pstTempSampleParam->uchIntTimeChangeEn)
                    {    
                        GU16 usTimeSlot;
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] ChangeSampleParm[int time]: need change int time. \r\n");
                        usTimeSlot = GH3x2x_CalSlotTime(puchBglevel[uchTempSlotNo],puchBgCancel[uchTempSlotNo], pstTempSampleParam->uchIntTime);
                        GH3x2x_SetIntTime(uchTempSlotNo,pstTempSampleParam->uchIntTime);
                        GH3x2x_SetSlotTime(uchTempSlotNo,usTimeSlot);
                    }
                    if(pstTempSampleParam->uchSampleRateChangeEn)
                    {    
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] ChangeSampleParm[sample rate]\r\n");

                        GH3x2x_SetSlotSampleRate(uchTempSlotNo,pstTempSampleParam->usSampleRate);
                    }
                    if(pstTempSampleParam->uchTiaGainChangeEn)
                    {
                        GH3X2X_SlotLedTiaGainConfig(uchTempSlotNo, uchTempAdtNo, pstTempSampleParam->uchTiaGain[uchChnlCnt]);
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] ChangeSampleParm[tia gain]: uchSlotNo = %d, uchTempAdtNo = %d, TiaGain = %d \r\n",(int)uchTempSlotNo,(int)uchTempAdtNo,(int)pstTempSampleParam->uchTiaGain[uchChnlCnt]);
                    }
                    if(pstTempSampleParam->uchLedCurrentChangeEn)
                    {    
                        GU16 usDrv0Reg;
                        GU16 usDrv1Reg;
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] ChangeSampleParm[led current]\r\n");

                        usDrv0Reg = GH3X2X_SetLedDrvCurrentByEcode(pstTempSampleParam->uchLedDrv0Current[uchChnlCnt], 0, (GS8)g_chDrvECode);
                        //usDrv0Reg = ((GU16)pstTempSampleParam->uchLedDrv0Current[uchChnlCnt]) * 255 /uchDr0Fs;

                        if(usDrv0Reg > 255)
                        {
                            usDrv0Reg = 255;
                        }

                        usDrv1Reg = GH3X2X_SetLedDrvCurrentByEcode(pstTempSampleParam->uchLedDrv1Current[uchChnlCnt], 1, (GS8)g_chDrvECode);
                        //usDrv1Reg = ((GU16)pstTempSampleParam->uchLedDrv1Current[uchChnlCnt]) * 255 /uchDr1Fs;

                        if(usDrv1Reg > 255)
                        {
                            usDrv1Reg = 255;
                        }

                        GH3X2X_SlotLedCurrentConfig(uchTempSlotNo, 0, usDrv0Reg);
                        GH3X2X_SlotLedCurrentConfig(uchTempSlotNo, 1, usDrv1Reg);
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] uchSlotNo = %d, uchTempAdtNo = %d, Drv0 = %d mA, Drv1 = %d mA \r\n",(int)uchTempSlotNo,(int)uchTempAdtNo,(int)pstTempSampleParam->uchLedDrv0Current[uchChnlCnt],(int)pstTempSampleParam->uchLedDrv1Current[uchChnlCnt]);
                        GH3X2X_DEBUG_LOG_PARAM("[GH3x2x_ChangeSampleParmForEngineeringMode] Drv0_reg = %d, Drv1_reg = %d \r\n",(int)usDrv0Reg,(int)usDrv1Reg);
                    }
                }
            }
        }
    }
}

/**
 * @fn       GU8 GH3x2x_GetActiveChipResetFlag(void)
 *
 * @brief  
 *
 * @attention   None.
 *
 * @param[in]   None
 * @param[out]  ActiveChipResetFlag  1: user have done chip reset(soft reset and hardwear reset) actively    gh3x2x_init will clear it 
 *
 * @return  None
 */
GU8 GH3x2x_GetActiveChipResetFlag(void)
{
    return g_uchActiveChipResetFlag;
}

/**
 * @fn       GU8 GH3x2x_GetChipResetRecoveringFlag(void)
 *
 * @brief  
 *
 * @attention   None.
 *
 * @param[in]   None
 * @param[out]  chip reset recovering flag  0: is not in chip reset recovering flag  1: is in chip reset recovering
 *
 * @return  None
 */
GU8 GH3x2x_GetChipResetRecoveringFlag(void)
{
    return g_uchChipResetRecoveringFlag;
}

/**
 * @fn       GU8 GH3x2x_SetChipResetRecoveringFlag(void)
 *
 * @brief  
 *
 * @attention   None.
 *
 * @param[in]   chip reset recovering flag  0: is not in chip reset recovering flag  1: is in chip reset recovering
 * @param[out]  None
 *
 * @return  None
 */
void GH3x2x_SetChipResetRecoveringFlag(GU8 uchChipResetRecoeringFlag)
{
    g_uchChipResetRecoveringFlag = uchChipResetRecoeringFlag;
}

void GH3x2xCreatTagArray(GU8 *puchTagArray, GU16 usArrayLen,GU32 *unCompeletMask, const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU8  uchChnlNum;
    GU8 *puchChnlMap;
    
    uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    puchChnlMap = pstFrameInfo->pchChnlMap;

    //create tag array
    GH3X2X_Memset(puchTagArray,0xFF,usArrayLen);
    
    for(GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
    {
        GU8 uchTag;
        uchTag = puchChnlMap[uchChnlCnt] >> 3;
        puchTagArray[uchTag] = uchChnlCnt;
    }
    //crete compelet mask
    (*unCompeletMask) = ((((long long)1) << uchChnlNum) - 1)&pstFrameInfo->pstFunctionInfo->unChnlEnForUserSetting;
}

GU16 GH3x2xGetFrameNum(GU8 *puchRawdataBuf, GU16 usRawDataByteLen, const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU32 unTempIncompeletFlag;
    GU8  puchTagArray[32];
    GU32 unCompeletMask;
    GU16 usFrameNum = 0;

    unTempIncompeletFlag = *(pstFrameInfo->punIncompleteChnlMapBit);

    GH3x2xCreatTagArray(puchTagArray, 32,&unCompeletMask, pstFrameInfo);

    //set incomplete flag
    for (GU16 usSearchStartPosi = 0; usSearchStartPosi < usRawDataByteLen; usSearchStartPosi += GH3X2X_FIFO_RAWDATA_SIZE)
    {
        GU8 uchTag;
        GU8 uchChnlId;
        uchTag = puchRawdataBuf[usSearchStartPosi] >> 3;
        uchChnlId =  puchTagArray[uchTag];
        if(0xFF != uchChnlId)
        {        
            unTempIncompeletFlag |= (1 << uchChnlId);
        }

        if(unCompeletMask == (unCompeletMask&unTempIncompeletFlag))  //got one frame
        {
            unTempIncompeletFlag = 0;
            usFrameNum ++;
        }
    }

    return usFrameNum;
    
}

float GH3x2xCalGsensorStep(GU16 usGsDataNum, GU16 usFrameNum)
{
    float fGsensorStep = 0;
    
    if ((usFrameNum > 0) && (usGsDataNum > 0)) // calc gs index
    {
        fGsensorStep = ((GF32)(usGsDataNum)) / (usFrameNum);
    }

    return  fGsensorStep;
}

void GH3x2xSendAllVersion(void)
{
    GU8 uchPacketPayloadArr[240] = {0};
    GU8 uchCmdArr[7 + GH3X2X_FUNC_OFFSET_MAX] = {(0x01),(0x0B),(0x0C),(0x0E),(0x0F),(0x10),(0x11)};
    GU8 uchCmdLen = 7;
    
    if (g_uchAlgoEnableFlag)
    {
        for (int nCnt = 0 ; nCnt < GH3X2X_FUNC_OFFSET_MAX ; nCnt++)
        {
            uchCmdArr[7 + nCnt] = 0x12 + nCnt;
        }
        uchCmdLen = 7 + GH3X2X_FUNC_OFFSET_MAX;
    }
    
    for (GU8 uchOffset = 0 ; uchOffset < uchCmdLen ; uchOffset ++)
    {
        GU8 uchGetVersionType = UPROTOCOL_GET_VER_TYPE_ALGO_VER + uchOffset;
        GCHAR pszVersionString[150] = {0};
        GH3X2X_GetVersion(uchCmdArr[uchOffset], pszVersionString);
        GU8 uchVersionStringLen = (GU8)GH3X2X_Strlen(pszVersionString);
        uchPacketPayloadArr[UPROTOCOL_GET_VER_TYPE_INDEX + 4] = uchGetVersionType;
        uchPacketPayloadArr[UPROTOCOL_GET_VER_STRING_LEN_INDEX + 4] = uchVersionStringLen;
        GH3X2X_Memcpy((GCHAR *)&uchPacketPayloadArr[UPROTOCOL_GET_VER_STRING_INDEX + 4], pszVersionString, uchVersionStringLen);
        GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_BP_DATA_TRANS, uchPacketPayloadArr, &uchPacketPayloadArr[4], uchVersionStringLen + 2);
        Gh3x2xDemoSendProtocolData(uchPacketPayloadArr,usRespondLen);
    }
}

GU8 Gh3x2xFunctionID2Offset(GU32 unFunctionID)
{
    for(GU8 uchOffset = 0; uchOffset < 32; uchOffset ++)
    {
        if(unFunctionID == (((GU32)1) << uchOffset))
        {
            return uchOffset;
        }
    }
    return 0;
}

void Gh3x2xFunctionInfoForUserInit(void)
{
    for(GU8 uchFuncitonCnt = 0; uchFuncitonCnt < GH3X2X_FUNC_OFFSET_MAX; uchFuncitonCnt ++)
    {
        if(g_pstGh3x2xFrameInfo[uchFuncitonCnt])
        {
            g_pstGh3x2xFrameInfo[uchFuncitonCnt] -> pstFunctionInfo -> usSampleRateForUserSetting = 0;
            g_pstGh3x2xFrameInfo[uchFuncitonCnt] -> pstFunctionInfo -> unChnlEnForUserSetting = 0xFFFFFFFF;
        }
    }
}

void GH3x2xHandleFrameData(const STGh3x2xFrameInfo * const pstFrameInfo, GU32 *punTempIncompeletFlag,
                                GU32 *punFunctionID, GU8 uchChnlNum, GU8 *puchRawdataBuf, GU32 *punFrameRawdata,
                                GU8 *puchTag, GS16 *pusGsValueArr, float fGsensorIndex,
                                STCapRawdata* pstCapValueArr,float fCapIndex,STTempRawdata* pstTempValueArr,float fTempIndex)
{
    (void) puchRawdataBuf;
    GU32 unTempIncompeletFlag = *punTempIncompeletFlag;
    GU8 uchTag = *puchTag;
    GU32 unFunctionID = *punFunctionID;
    GU8  uchGyroEnable = g_uchGyroEnable;

    GH3X2X_DEBUG_LOG_PARAM("[%s]got one frame,funcID = %d\r\n",__FUNCTION__,(int)unFunctionID);
    unTempIncompeletFlag = 0;
    GH3X2X_Memset((GU8*)(pstFrameInfo->punFrameFlag), 0, GH3X2X_FRAME_FLAG_NUM*4);       //clear flag
    GH3X2X_Memset((GU8*)(pstFrameInfo ->pstAlgoResult), 0, sizeof(STGh3x2xAlgoResult)); //clear result
    GH3X2X_Memset((GU8*)(pstFrameInfo ->punFrameAgcInfo), 0, uchChnlNum * 4);            //clear agc info

    for(GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
    {
        GU32 unRawdataAndTag;
        unRawdataAndTag = pstFrameInfo->punIncompleteRawdata[uchChnlCnt];
        uchTag = GH3X2X_DWORD_RAWDATA_GET_SLOT_ADC_NUM(unRawdataAndTag);
    
        //rawdata
        punFrameRawdata[uchChnlCnt] = unRawdataAndTag&0x00FFFFFF;

        //flag0 flag1 
        pstFrameInfo->punFrameFlag[0] |= (((GU32)GH3X2X_RAWDATA_GET_ADJ_FLAG0(unRawdataAndTag) << uchChnlCnt));
        pstFrameInfo->punFrameFlag[1] |= (((GU32)GH3X2X_RAWDATA_GET_ADJ_FLAG1(unRawdataAndTag) << uchChnlCnt));
    
        //flag2 bit 0    fast recovery flag
        if ((GH3X2X_FUNCTION_ECG == unFunctionID)||(GH3X2X_FUNCTION_PWTT == unFunctionID))
        {
            if(0 == uchChnlCnt)
            {
                if(GH3X2X_RAWDATA_GET_FAST_RECOVERY_FLAG(unRawdataAndTag))
                {
                    pstFrameInfo->punFrameFlag[2] |= 0x01;
                }
            }
        }

#if GH3X2X_DIVIDE_ZERO_FLAG_UPLODE_EN
        //flag4
        if(g_uchChipDivZeroFlag)
        {
            pstFrameInfo->punFrameFlag[4] |= 0x01;
            GH3X2X_ClearDivZeroFlag();
        }
#endif 

        // agc (gain/drv0/drv1)
        pstFrameInfo->punFrameAgcInfo[uchChnlCnt] |= g_pstGh3x2xAgcInfo->uchTiaGainRecord[uchTag] & 0xF;    //gain
        pstFrameInfo->punFrameAgcInfo[uchChnlCnt] |= ((GU32)g_pstGh3x2xAgcInfo->uchDrvCurrentRecord[(GU8)(uchTag/4)]) << 8; // drv 0/1 current
        pstFrameInfo->punFrameAgcInfo[uchChnlCnt] |= g_pstGh3x2xAgcInfo->uchDcCancelRecord[uchTag] << 24;    //dc cancel
    }

    //g-sensor
    for( int i=0;i<3;i++)
    {
        if(fGsensorIndex != (GU16)fGsensorIndex)
        {
            pstFrameInfo->pusFrameGsensordata[i] = pusGsValueArr[(GU16)fGsensorIndex*(3+3*uchGyroEnable)+i]*(1-fGsensorIndex+(GU16)fGsensorIndex) + pusGsValueArr[((GU16)fGsensorIndex+1)*(3+3*uchGyroEnable)+i]*(fGsensorIndex-(GU16)fGsensorIndex);
        }
        else
        {
            pstFrameInfo->pusFrameGsensordata[i] = pusGsValueArr[(GU16)fGsensorIndex*(3+3*uchGyroEnable)+i];
        }
    }
    if(uchGyroEnable)
    {
        for( int i=0;i<3;i++)
        {
            if(fGsensorIndex != (GU16)fGsensorIndex)
            {
                pstFrameInfo->pusFrameGsensordata[i+3] = pusGsValueArr[(GU16)fGsensorIndex*(3+3*uchGyroEnable)+i+3]*(1-fGsensorIndex+(GU16)fGsensorIndex) + pusGsValueArr[((GU16)fGsensorIndex+1)*(3+3*uchGyroEnable)+i+3]*(fGsensorIndex-(GU16)fGsensorIndex);
            }
            else
            {
                pstFrameInfo->pusFrameGsensordata[i+3] = pusGsValueArr[(GU16)fGsensorIndex*(3+3*uchGyroEnable)+i+3];
            }
        }
    }
    //cap and temp
    if(GH3X2X_GetCapEnableFlag())
    {
        GH3X2X_Memcpy(pstFrameInfo->pstFrameCapdata,&pstCapValueArr[(GU16)fCapIndex],sizeof(STCapRawdata));
    }
    if(GH3X2X_GetTempEnableFlag())
    {
        GH3X2X_Memcpy(pstFrameInfo->pstFrameTempdata,&pstTempValueArr[(GU16)fTempIndex],sizeof(STTempRawdata));
    }

    //flag 2 bit1  first frame flag

    Gh3x2xSetFrameFlag2(pstFrameInfo);
    /*
    pstFrameInfo->punFrameFlag[2] &= (~(((GU32)1) << 1));
    if(0 == (*(pstFrameInfo->punFrameCnt)))
    {
        pstFrameInfo->punFrameFlag[2] |= 0x02;
    }
    */
    if(GH3X2X_FUNCTION_SOFT_ADT_IR == unFunctionID)
    {
        pstFrameInfo->punFrameFlag[2] |= ((GU32)1)<<4;
    }

    GH3X2X_DEBUG_LOG_PARAM("[GetFrameData] function id: 0x%x,  rawdata0 = %d, rawdata1 = %d\r\n",(int)pstFrameInfo->unFunctionID, (int)(pstFrameInfo->punFrameRawdata[0]), (int)(pstFrameInfo->punFrameRawdata[1]));
    GH3X2X_DEBUG_LOG_PARAM("[GetFrameData] TimeStamp = %d\r\n",(int)(*(pstFrameInfo->punFrameCnt)));

    //call algo 2-level interface
    unFunctionID = pstFrameInfo->unFunctionID;
    pstFrameInfo->pstAlgoResult->uchUpdateFlag = 0;
    
    //get algo io data hook
    HOOK_FUNC_CALL(g_pGh3x2xGetIoStructInfoHookFunc, (pstFrameInfo));

    SET_VAL_VIA_PTR(punFunctionID, unFunctionID);
    SET_VAL_VIA_PTR(puchTag, uchTag);
    SET_VAL_VIA_PTR(punTempIncompeletFlag, unTempIncompeletFlag);
}

void GH3x2xGetFrameDataAndProcess(GU8 *puchRawdataBuf, GU16 usRawDataByteLen,
                                        GS16 *pusGsValueArr, GU16 usGsDataNum,STCapRawdata* pstCapValueArr,GU16 usCapDataNum,STTempRawdata* pstTempValueArr,GU16 usTempDataNum,
                                        const STGh3x2xFrameInfo * const pstFrameInfo, float fGensorStep, float fCapStep, float fTempStep, GU16 usFrameNum)
{
    UNUSED_VAR(usGsDataNum);
    UNUSED_VAR(usCapDataNum);
    UNUSED_VAR(usTempDataNum);
    GU32 unTempIncompeletFlag;
    GU8  puchTagArray[32];
    GU8  uchChnlNum;
    GU32 unCompeletMask;
    GU32 *punFrameRawdata;
    float fGsensorIndex = 0;
    float fCapIndex = 0;
    float fTempIndex = 0;
    GU32 unFunctionID;
    GU16 usFrameCnt = 0;

    unTempIncompeletFlag = *(pstFrameInfo->punIncompleteChnlMapBit);
    uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum; 
    punFrameRawdata = pstFrameInfo->punFrameRawdata;
    unFunctionID = pstFrameInfo->unFunctionID;

    GH3x2xCreatTagArray(puchTagArray, 32, &unCompeletMask, pstFrameInfo);

    //set incomplete flag
    for (GU16 usSearchStartPosi = 0; usSearchStartPosi < usRawDataByteLen; usSearchStartPosi += GH3X2X_FIFO_RAWDATA_SIZE)
    {
        GU8 uchTag;
        GU8 uchChnlId;
        uchTag = puchRawdataBuf[usSearchStartPosi] >> 3;
        uchChnlId =  puchTagArray[uchTag];
        if(0xFF != uchChnlId)
        {        
            GU32 unRawdataAndTag;                
            unTempIncompeletFlag |= (1 << uchChnlId);
            unRawdataAndTag = GH3X2X_MAKEUP_DWORD(puchRawdataBuf[usSearchStartPosi],
                                                                    puchRawdataBuf[usSearchStartPosi + 1],
                                                                    puchRawdataBuf[usSearchStartPosi + 2],
                                                                    puchRawdataBuf[usSearchStartPosi + 3]);
            //store incomplete rawdata 
            pstFrameInfo->punIncompleteRawdata[uchChnlId] = unRawdataAndTag;
            
        }

        if(unCompeletMask == (unCompeletMask&unTempIncompeletFlag))  //got one frame
        {
            if((0 == pstFrameInfo->pstDownSampleInfo->uchDownSampleFactor)||(pstFrameInfo->pstDownSampleInfo->uchDownSampleFactor == pstFrameInfo->pstDownSampleInfo->uchDownSampleCnt))
            {
                GH3x2xHandleFrameData(pstFrameInfo, &unTempIncompeletFlag,
                                    &unFunctionID, uchChnlNum, puchRawdataBuf, punFrameRawdata,
                                    &uchTag, pusGsValueArr, fGsensorIndex,
                                    pstCapValueArr,fCapIndex,pstTempValueArr,fTempIndex);

                //send BLE package
#if (__SUPPORT_ZIP_PROTOCOL__)
                Gh2x2xUploadZipDataToMaster(pstFrameInfo, usFrameCnt, usFrameNum, puchTagArray);
#else
                Gh2x2xUploadDataToMaster(pstFrameInfo, usFrameCnt, usFrameNum, puchTagArray);
#endif 
                // timestamp
                (*(pstFrameInfo->punFrameCnt)) ++;
                // bak last gain
                for(GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
                {
                    pstFrameInfo ->puchFrameLastGain[uchChnlCnt] = ((pstFrameInfo->punFrameAgcInfo[uchChnlCnt]) & 0x0000000F);
                }
                usFrameCnt ++;
            }
            unTempIncompeletFlag = 0;
            if(pstFrameInfo->pstDownSampleInfo->uchDownSampleFactor)
            {
                if(pstFrameInfo->pstDownSampleInfo->uchDownSampleFactor <= pstFrameInfo->pstDownSampleInfo->uchDownSampleCnt)
                {
                    pstFrameInfo->pstDownSampleInfo->uchDownSampleCnt = 0;
                }
                else
                {
                    pstFrameInfo->pstDownSampleInfo->uchDownSampleCnt ++;
                }
            }
            //calculate next fGsensorIndex
            fGsensorIndex += fGensorStep;
            fCapIndex  += fCapStep;
            fTempIndex  += fTempStep;
        }
    }
    //save IncompeletFlag
    *(pstFrameInfo->punIncompleteChnlMapBit) = unTempIncompeletFlag;
}

void GH3x2xFunctionProcess(GU8 *puchRawdataBuf, GU16 usRawDataByteLen, GS16 *pusGsValueArr, GU16 usGsDataNum, 
                        STCapRawdata* pstCapValueArr,GU16 usCapDataNum,STTempRawdata* pstTempValueArr,GU16 usTempDataNum,
                        const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU16 usFrameNum;
    float fGsensorStep;
    float fCapStep;
    float fTempStep;

    if(0 == pstFrameInfo)
    {
        return ;
    }

    /*****  check feature value  *********/
    if(GH3X2X_CheckRawdataBuf(puchRawdataBuf, usRawDataByteLen) || pstFrameInfo->pstFunctionInfo->uchChnlNum == 0)
    {
        return;
    }

    /***  cal how many frames in this fifo  ***/
    usFrameNum = GH3x2xGetFrameNum(puchRawdataBuf, usRawDataByteLen, pstFrameInfo);

    /***  cal g-sensor step ***/
    fGsensorStep = GH3x2xCalGsensorStep(usGsDataNum, usFrameNum);
    fCapStep     = GH3x2xCalGsensorStep(usCapDataNum, usFrameNum);
    fTempStep    = GH3x2xCalGsensorStep(usTempDataNum, usFrameNum);

    GH3X2X_DEBUG_LOG_PARAM("[GetFrameNum]FunctionID: %d FrameNum: %d, GsensorStep: %.3f\r\n",(int)pstFrameInfo->unFunctionID, usFrameNum, fGsensorStep);

    /***  get one frame data and process***/
    GH3x2xGetFrameDataAndProcess(puchRawdataBuf,usRawDataByteLen,pusGsValueArr,usGsDataNum,pstCapValueArr,usCapDataNum,pstTempValueArr,usTempDataNum,
                                            pstFrameInfo,fGsensorStep,fCapStep,fTempStep,usFrameNum);
}

void GH3X2X_ZipmodeInit(void)
{
    g_uchOddEvenChangeFlag = 1;
}

GS8 GH3X2X_FunctionStart(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_OK;
    GU32 unFunctionID;
    if(0 == pstFrameInfo)
    {
        return chRet;
    }

    GH3X2X_ZipmodeInit();

    unFunctionID = pstFrameInfo->unFunctionID;

    GH3x2xCalFunctionSlotBit(pstFrameInfo);

    /* if all func off, start sampling */
    if (GH3X2X_NO_FUNCTION == g_unFuncStartedBitmap)
    {
        chRet = GH3X2X_StartSampling();
        GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] function id: 0x%x, start result: %d\r\n",(int)pstFrameInfo->unFunctionID, chRet);
        GH3X2X_RET_ERROR_CHECK_E(chRet, GH3X2X_RET_NO_INITED_ERROR);
    }

#if GH3X2X_ALGORITHM_ECG_SUPPORT && __FUNC_TYPE_ECG_ENABLE__
    if (GH3X2X_FUNCTION_ECG == unFunctionID)
    {
        GH3X2X_LeadOffDetectInit();
        GH3X2X_EcgSampleHookHandle(ECG_SAMPLE_EVENT_TYPE_SAMPLE, ECG_SAMPLE_EVENT_INFO_SAMPLE_START);  //G202008231001 wanpeng
        GH3X2X_ECGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->usSampleRate);
        GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] sample rate: %d\r\n",(int)g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG]->pstFunctionInfo->usSampleRate);
    }
#endif

#if __SUPPORT_PPG_DOWNSAMP__
    if(g_uchPPGDownSampleChnlNumLimit>0)
    {
        if (GH3X2X_FUNCTION_HR == unFunctionID)
        {
            GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstFunctionInfo->usSampleRate);
            GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] sample rate: %d\r\n",(int)g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR]->pstFunctionInfo->usSampleRate);
        }

        else if (GH3X2X_FUNCTION_SPO2 == unFunctionID)
        {
            GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2]->pstFunctionInfo->usSampleRate);
            GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] sample rate: %d\r\n",(int)g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2]->pstFunctionInfo->usSampleRate);
        }
        else if (GH3X2X_FUNCTION_SOFT_ADT_GREEN == unFunctionID)
        {
            GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_GREEN]->pstFunctionInfo->usSampleRate);
            GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] sample rate: %d\r\n",(int)g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_GREEN]->pstFunctionInfo->usSampleRate);
        }
        else if (GH3X2X_FUNCTION_SOFT_ADT_IR == unFunctionID)
        {
            GH3X2X_PPGResampleConfig(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_IR]->pstFunctionInfo->usSampleRate);
            GH3X2X_DEBUG_LOG_PARAM("[FunctionStart] sample rate: %d\r\n",(int)g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_IR]->pstFunctionInfo->usSampleRate);
        }
    }
#endif

    if (GH3X2X_FUNCTION_ADT == unFunctionID)
    {
#if __FUNC_TYPE_ECG_ENABLE__
        GH3X2X_LeadDetEnInHardAdt(1);
#endif
    }

    /* set started bit */
    g_unFuncStartedBitmap |= unFunctionID;

    /* clear incompelet flag */
    (*(pstFrameInfo->punIncompleteChnlMapBit)) = 0;

    /* clear timestamp */
    (*(pstFrameInfo->punFrameCnt)) = 0;

    /* set last gain */
    GH3X2X_Memset(pstFrameInfo->puchFrameLastGain, 0xFF, pstFrameInfo->pstFunctionInfo->uchChnlNum);

    return chRet;
}

GS8 GH3X2X_FunctionStop(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GS8 chRet = GH3X2X_RET_OK;
    GU32 unFunctionID;
    if(0 == pstFrameInfo)
    {
        return chRet;
    }

    unFunctionID = pstFrameInfo->unFunctionID;
    GH3x2xCalFunctionSlotBit(pstFrameInfo);
    
    /* clear started bit */
    g_unFuncStartedBitmap &= (~unFunctionID);

    /* set result flag */
    if (pstFrameInfo->pstAlgoRecordResult != GH3X2X_PTR_NULL)
    {
        pstFrameInfo->pstAlgoRecordResult->uchUpdateFlag = 0;
        pstFrameInfo->pstAlgoRecordResult->usResultBit = 0;
    }

    /* if all func off, stop sampling */
    if (GH3X2X_NO_FUNCTION == g_unFuncStartedBitmap)
    {
        chRet = GH3X2X_StopSampling();
        GH3X2X_DEBUG_LOG_PARAM("[FunctionStop] function id: 0x%x, start result: %d\r\n",(int)pstFrameInfo->unFunctionID, chRet);
        GH3X2X_RET_ERROR_CHECK(chRet);
        /* fixed fifo watermark thr */
        //GH3X2X_FIXED_FIFO_WATERMARK_THR();
    }

    return chRet;
}

GU32 GH3X2X_GetFuncStartedBitmap(void)
{
    return g_unFuncStartedBitmap;
}

GU8* GH3x2xGetFunctionChnlMap(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if(0 == pstFrameInfo)
    {
        return 0;
    }
    else
    {        
        return pstFrameInfo->pchChnlMap;
    }
}

void GH3x2xSetFunctionChnlNum(const STGh3x2xFrameInfo * const pstFrameInfo, GU8 uchChnlNum)
{
    GU8 uchChnlLimit;
    if(pstFrameInfo)
    {
        uchChnlLimit = pstFrameInfo->uchFuntionChnlLimit;
        if(uchChnlNum > uchChnlLimit)
        {
            uchChnlNum = uchChnlLimit;
        }
        pstFrameInfo ->pstFunctionInfo->uchChnlNum = uchChnlNum;
        if (uchChnlNum > 0)
        {
            g_unConfigFuncMode |= pstFrameInfo->unFunctionID;
        }
        GH3X2X_DEBUG_LOG_PARAM("[SetFunctionChnlNum] function id: 0x%x, ChnlNum: %d\r\n",(int)pstFrameInfo->unFunctionID, uchChnlNum);
    }
}

void GH3x2xSetFunctionChnlMap(const STGh3x2xFrameInfo * const pstFrameInfo, GU8 uchChnlId, GU8 uchChnlTag)
{
    if(pstFrameInfo)
    {
        if(uchChnlId < (pstFrameInfo->uchFuntionChnlLimit))
        {
            pstFrameInfo ->pchChnlMap[uchChnlId] = uchChnlTag;

            
            GH3X2X_DEBUG_LOG_PARAM("[SetFunctionChnlMap] function id: 0x%x, SlotAdc: %d\r\n",(int)pstFrameInfo->unFunctionID, uchChnlTag>>3);
        }
        
    }
}

void GH3x2xCalFunctionSlotBit(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    GU8 uchSlotBit = 0;
    GU8 uchChnlTag;
    if(pstFrameInfo)
    {
        for(GU8 uchChnlCnt = 0; uchChnlCnt < pstFrameInfo->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
        {
            if(pstFrameInfo->pstFunctionInfo->unChnlEnForUserSetting & (((GU32)1) << uchChnlCnt))
            {
                uchChnlTag = pstFrameInfo->pchChnlMap[uchChnlCnt];
                uchSlotBit |= (1 << (GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(uchChnlTag)));
            }
        }
    }

    pstFrameInfo ->pstFunctionInfo->uchSlotBit = uchSlotBit;

    GH3X2X_DEBUG_LOG_PARAM("[GH3x2xCalFunctionSlotBit] function id: 0x%x, slot bit: 0x%x\r\n",(int)pstFrameInfo->unFunctionID, pstFrameInfo->pstFunctionInfo->uchSlotBit);
}

/**
 * @fn     GU32 GH3X2X_GetSoftEvent(void)
 * @brief  Get soft event
 * @param[in]   None
 * @param[out]  Soft event
 *
 * @return  soft event
 */
GU32 GH3X2X_GetSoftEvent(void)
{
    return g_unSoftEvent;
}

/**
 * @fn     void GH3X2X_SetSoftEvent(GU32 unEvent)
 * @brief  set soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X2X_SetSoftEvent(GU32 unEvent)
{
    g_unSoftEvent |= unEvent;
}

/**
 * @fn     void GH3X2X_ClearSoftEvent(GU32 unEvent)
 * @brief  clear soft event
 * @param[in]   uchEvent
 * @param[out]  None
 * @return  None
 */
void GH3X2X_ClearSoftEvent(GU32 unEvent)
{
    g_unSoftEvent &= (~unEvent);
}

/**
 * @fn     GU8 GH3X2X_GetGsensorEnableFlag(void)
 *
 * @brief  Inquire if need gsensor data
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  1: need gsensor data  0: not need gsensor data
 */
GU8 GH3X2X_GetGsensorEnableFlag(void)
{
    return g_uchGsensorEnable;
}

#if (0 == __CAP_ENABLE__)
GU8 GH3X2X_GetCapEnableFlag(void)
{
    return 0;
}
#else
GU8 GH3X2X_GetCapEnableFlag(void)
{
    return g_uchCapEnable;
}
#endif

#if (0 == __TEMP_ENABLE__)
GU8 GH3X2X_GetTempEnableFlag(void)
{
    return 0;
}
#else
GU8 GH3X2X_GetTempEnableFlag(void)
{
    return g_uchTempEnable;
}
#endif

void Gh3x2xFunctionInit(void)
{
    for(GU8 uchFuncitonCnt = 0; uchFuncitonCnt < GH3X2X_FUNC_OFFSET_MAX; uchFuncitonCnt ++)
    {
        if(g_pstGh3x2xFrameInfo[uchFuncitonCnt])
        {
            GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[uchFuncitonCnt], 0);   //chnl num
            g_pstGh3x2xFrameInfo[uchFuncitonCnt] -> pstFunctionInfo -> usSampleRate = 0; //set sample rate to invalid value
            g_pstGh3x2xFrameInfo[uchFuncitonCnt] -> pstDownSampleInfo -> uchDownSampleFactor = 0; 
            g_pstGh3x2xFrameInfo[uchFuncitonCnt] -> pstDownSampleInfo -> uchDownSampleCnt = 0; 
        }
    }
}

/**
 * @fn     void GH3X2X_ReinitAllSwModuleParam(GU16 usReinitFlag)
 *
 * @brief  reinit all software param config
 *
 * @attention   None
 *
 * @param[in]   usReinitFlag        reinit flag, #0 don't reinit, #others reinit all
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_ReinitAllSwModuleParam(GU16 usReinitFlag)
{
    if (usReinitFlag != 0) // if != 0, that need reinit all software module param
    {

        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HR], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_HRV], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SPO2], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ECG], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_GREEN], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_SOFT_ADT_IR], 0);
        GH3x2xSetFunctionChnlNum(g_pstGh3x2xFrameInfo[GH3X2X_FUNC_OFFSET_ADT], 0);

#if __SUPPORT_SOFT_AGC_CONFIG__
        GH3X2X_LedAgcReset();
#endif

#if __FUNC_TYPE_ECG_ENABLE__ 
        GH3X2X_SlaverSoftLeadPramInit();// lead moudle param reinit
#endif

        GH3X2X_DumpModeSet(0);          // dump module param reinit

#if __FUNC_TYPE_ECG_ENABLE__
        GH3X2X_AdtPramInit();   //adt param reinit
#endif
        g_uchGsensorEnable = 0;
        // soft adt param reinit
    }
}

/* util function, memcpy & memset & strlen */
/**
 * @fn     void *GH3X2X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
 *
 * @brief  memcpy() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pSrc        pointer to source buffer
 * @param[in]   unByteSize  source buffer byte size
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X2X_Memcpy(void *pDest, const void *pSrc, GU32 unByteSize)
{
    GU8 *puchSrc = (GU8 *)pSrc;
    GU8 *puchDest = (GU8 *)pDest;
    GU32 unAlign = ((GU32)puchDest | (GU32)puchSrc) << GH3X2X_UPROTOCOL_ALIGN_LEFTSHIFT;

    if ((pDest == GH3X2X_PTR_NULL) || (pSrc == GH3X2X_PTR_NULL) || (0 == unByteSize))
    {
        return GH3X2X_PTR_NULL;
    }
    if (unAlign == 0) // if align 4
    {
        while (unByteSize >= GH3X2X_UPROTOCOL_SIZE_T)
        {
            *(GU32 *)puchDest = *(GU32 *)puchSrc;
            puchSrc += GH3X2X_UPROTOCOL_SIZE_T;
            puchDest += GH3X2X_UPROTOCOL_SIZE_T;
            unByteSize -= GH3X2X_UPROTOCOL_SIZE_T;
        }
    }
    while (unByteSize) // bytes
    {
        *(GU8 *)puchDest = *(GU8 *)puchSrc;
        puchSrc++;
        puchDest++;
        unByteSize--;
    }
    return pDest;
}

/**
 * @fn     void *GH3X2X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
 *
 * @brief  memset() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   chVal       char val for set
 * @param[in]   unByteSize       source buffer len
 * @param[out]  pDest       pointer to destination buffer
 *
 * @return  pointer to destination buffer
 */
void *GH3X2X_Memset(void* pDest, GCHAR chVal, GU32 unByteSize)
{
    GU8 *puchDest = (GU8 *)pDest;
    GU32 unAlign = ((GU32)puchDest) << GH3X2X_UPROTOCOL_ALIGN_LEFTSHIFT;
    GU32 unWordVal = GH3X2X_MAKEUP_DWORD(chVal, chVal, chVal, chVal);

    if (pDest == GH3X2X_PTR_NULL)
    {
        return GH3X2X_PTR_NULL;
    }
    if (unAlign == 0) // if align 4
    {
        while (unByteSize >= GH3X2X_UPROTOCOL_SIZE_T)
        {
            *(GU32 *)puchDest = unWordVal;
            puchDest += GH3X2X_UPROTOCOL_SIZE_T;
            unByteSize -= GH3X2X_UPROTOCOL_SIZE_T;
        }
    }
    while (unByteSize) // bytes
    {
        *(GU8 *)puchDest = (GU8)chVal;
        puchDest++;
        unByteSize--;
    }
    return pDest;
}

/**
 * @fn     GU32 GH3X2X_Strlen(const GCHAR *pszSrc)
 *
 * @brief  strlen() Re-implementation
 *
 * @attention   None
 *
 * @param[in]   pszSrc      pointer to string
 * @param[out]  None
 *
 * @return  string len
 */
GU32 GH3X2X_Strlen(const GCHAR *pszSrc)
{
    GU32 unCnt = 0;
    if (pszSrc != GH3X2X_PTR_NULL)
    {
        while ((*pszSrc) != 0) // if not equal '\0'
        {
            unCnt++;
            pszSrc++;
        }
    }
    return unCnt;
}

GU8 GH3X2X_GetAlgoEnableFlag(void)
{
    return g_uchAlgoEnableFlag;
}

void GH3X2X_SetAlgoEnableFlag(GU8 uchAlgoEnableFlag)
{
    g_uchAlgoEnableFlag = uchAlgoEnableFlag;
}

GU8 GH3X2X_GetSingleChipModeEnableFlag(void)
{
    return g_uchSingleChipModeEnableFlag;
}

void GH3X2X_SetSingleChipModeEnableFlag(GU8 uchAlgoEnableFlag)
{
    g_uchSingleChipModeEnableFlag = uchAlgoEnableFlag;
}

/**
 * @fn     void GH3X2X_ModifyFunctionFrequency(GU8 uchFunctionID, GU16 usFrequencyValue) 
 *
 * @brief  Modify fs for each function
 *
 * @attention   None
 *
 * @param[in]   uchFunctionID               function offset
 * @param[in]   usFrequencyValue         5Hz-1000Hz
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_ModifyFunctionFrequency(GU8 uchFunctionID, GU16 usFrequencyValue)
{
    GU8 uchTempSlotNo = 0;
    GU8 uchTempSlotLastNo = 0xff;
    GU8 uchChnlCnt = 0;
    for(uchChnlCnt = 0; uchChnlCnt < g_pstGh3x2xFrameInfo[uchFunctionID]->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
    {
        uchTempSlotNo = GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(g_pstGh3x2xFrameInfo[uchFunctionID]->pchChnlMap[uchChnlCnt]);
        if (uchTempSlotLastNo != uchTempSlotNo)
        {
            GH3x2x_SetSlotSampleRate(uchTempSlotNo, usFrequencyValue);
            uchTempSlotLastNo = uchTempSlotNo;
        }
    }
    g_pstGh3x2xFrameInfo[uchFunctionID]->pstFunctionInfo->usSampleRate = usFrequencyValue;
}

/**
 * @fn     void GH3X2X_ModifyFunctionLedCurrent(GU8 uchFunctionID, GU16 usLedDrv0Current, GU16 usLedDrv1Current)
 *
 * @brief  Modify led current for each function
 *
 * @attention   None
 *
 * @param[in]   uchFunctionID               function offset
 * @param[in]   usLedDrv0Current         0mA-200mA
 * @param[in]   usLedDrv1Current         0mA-200mA
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_ModifyFunctionLedCurrent(GU8 uchFunctionID, GU16 usLedDrv0Current, GU16 usLedDrv1Current)
{
    GU8 uchTempSlotNo = 0;
    GU8 uchTempSlotLastNo = 0xff;
    GU8 uchChnlCnt = 0;
    GU16 usDrv0Reg = 0;
    GU16 usDrv1Reg = 0;
    GU16 usDr0Fs = 0;
    GU16 usDr1Fs = 0;

    GH3x2x_GetLedCurrentFullScal((GU8*)&usDr0Fs, (GU8*)&usDr1Fs);
    
    for(uchChnlCnt = 0; uchChnlCnt < g_pstGh3x2xFrameInfo[uchFunctionID]->pstFunctionInfo->uchChnlNum; uchChnlCnt ++)
    {
        uchTempSlotNo = GH3X2X_BYTE_RAWDATA_GET_SLOT_NUM(g_pstGh3x2xFrameInfo[uchFunctionID]->pchChnlMap[uchChnlCnt]);
        if (uchTempSlotLastNo != uchTempSlotNo)
        {
            usDrv0Reg = (GU16)usLedDrv0Current * 255 /usDr0Fs;
            if(usDrv0Reg > 255)
            {
                usDrv0Reg = 255;
            }
            usDrv1Reg = (GU16)usLedDrv1Current * 255 /usDr1Fs;
            if(usDrv1Reg > 255)
            {
                usDrv1Reg = 255;
            }

            GH3X2X_SlotLedCurrentConfig(uchTempSlotNo, 0, usDrv0Reg);
            GH3X2X_SlotLedCurrentConfig(uchTempSlotNo, 1, usDrv1Reg);
            uchTempSlotLastNo = uchTempSlotNo;
        }
    }
}

GU32 GH3X2X_GetConfigFuncMode(void)
{
    return g_unConfigFuncMode;
}

void GH3X2X_SetConfigFuncMode(GU32 unConfigFuncMode)
{
    g_unConfigFuncMode = unConfigFuncMode;
}

void Gh3x2xFifoRecovery(void)
{
    GU16 uchSampleStateBak;
    uchSampleStateBak = GH3X2X_ReadRegBitField(0x0000, 0, 0);
    GH3X2X_WriteRegBitField(0x0000, 0, 0, 0);
    Gh3x2x_BspDelayMs(10);
    GH3X2X_WriteRegBitField(0x0000, 0, 0, uchSampleStateBak);
}

#if (GH3X2X_FIFO_MONITOR_EN&(__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
void Gh3x2xModifiyDecimation(GU8 uchDecimationChnl, GU8 uchSlotAdc, GU8 uchOption)
{
    GU16 uchSampleStateBak;
    uchSampleStateBak = GH3X2X_ReadRegBitField(0x0000, 0, 0);
    GH3X2X_WriteRegBitField(0x0000, 0, 0, 0);
    Gh3x2x_BspDelayMs(10);

    GH3X2X_WriteRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_EN_LSB + uchDecimationChnl, GH3X2X_DCMT_EN_LSB + uchDecimationChnl, 1);
    GH3X2X_WriteRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_DNSAMPLE_DIS_LSB + uchDecimationChnl, GH3X2X_DCMT_DNSAMPLE_DIS_LSB + uchDecimationChnl, 0);

    GH3X2X_WriteRegBitField(GH3X2X_DCMT_CTRL1_REG_ADDR + uchDecimationChnl * 2, GH3X2X_DCMT_HEAD_LSB, GH3X2X_DCMT_HEAD_MSB, uchSlotAdc);
    GH3X2X_WriteRegBitField(GH3X2X_DCMT_CTRL1_REG_ADDR + uchDecimationChnl * 2, GH3X2X_DCMT_RND_SEL_LSB, GH3X2X_DCMT_RND_SEL_MSB, uchOption);

    GH3X2X_WriteRegBitField(0x0000, 0, 0, uchSampleStateBak);
}

/// @brief get channel output en
/// @param  
/// @return output enable flagx, 0: slot0 adc0 output flag  1: slot0 adc1 output flag ... 31: slot7 adc3 output flag
GU32 Gh3x2xGetChnlOutputEn(void)
{
    GU32 unChnlEn = 0;
    GU8 uchSlotCfgEn = GH3X2X_ReadRegBitField(GH3X2X_SLOT_ENABLE_CFG_REG_ADDR, GH3X2X_SLOT_ENABLE_CFG_REG_LSB, GH3X2X_SLOT_ENABLE_CFG_REG_MSB);
    //get channel en, need eliminate no-output channel
    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X2X_SLOT_NUM_MAX; uchSlotCnt ++)
    {
        if(uchSlotCfgEn&(1 << uchSlotCnt))
        {
            GU8 uchRxEnFifoEn = GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_0_REG_ADDR + ((GU16)uchSlotCnt * GH3X2X_SLOT_CTRL_ADDR_INTERVAL), GH3X2X_SLOT_RX_EN_FIFO_EN_LSB, GH3X2X_SLOT_RX_EN_FIFO_EN_MSB);
            if(uchRxEnFifoEn & (1 << (GH3X2X_SLOT_FIFO_EN_LSB - GH3X2X_SLOT_RX_EN_FIFO_EN_LSB)))  // if fifo en = 1
            {
                for(GU8 uchRxCnt = 0; uchRxCnt < GH3X2X_RX_NUM_MAX; uchRxCnt ++)
                {
                    if(uchRxEnFifoEn&(1 << uchRxCnt))
                    {
                        unChnlEn |= 1 << (uchSlotCnt * GH3X2X_RX_NUM_MAX + uchRxCnt);
                    }
                }
            }
        }
    }
    return unChnlEn; 
}

/// @brief get sample rate factor for every slot
/// @param puchSlotSampleRateFactor   [0]: slot0 sample rate factor  [1]: slot1 sample rate factor ... [7]: slot7 sample rate factor
void Gh3x2xGetSlotSampleRateFactor(GU8 *puchSlotSampleRateFactor)
{
    for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X2X_SLOT_NUM_MAX; uchSlotCnt ++)
    {
        puchSlotSampleRateFactor[uchSlotCnt] = GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_0_REG_ADDR + ((GU16)uchSlotCnt * GH3X2X_SLOT_CTRL_ADDR_INTERVAL), GH3X2X_SLOT_SR_MULTIPLIER_LSB, GH3X2X_SLOT_SR_MULTIPLIER_MSB);
    }
}

    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
/// @brief get decimation enable (decimation en = 1 and downsample disable = 0)
/// @param  none
/// @return decimation
GU8 Gh3x2xGetDecimationEn(void)
{
    GU8 uchDecimationEn;
    GU8 uchDownSampleEn;
    uchDecimationEn = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_EN_LSB, GH3X2X_DCMT_EN_MSB);
    uchDownSampleEn = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_DNSAMPLE_DIS_LSB, GH3X2X_DCMT_DNSAMPLE_DIS_MSB);
    return uchDecimationEn &= (~uchDownSampleEn);
}
    #else

/// @brief get decimaiton information in current chip registers
/// @param pstDecimationInfo the decimation infomation that we get
void Gh3x2xGetDecimationInfo(STGh3x2xDecimationInfo *pstDecimationInfo)
{
    GU8 uchDecimationEn;
    GU8 uchDownSampleEn;
    GU8 uchValidChnlCnt;
    uchDecimationEn = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_EN_LSB, GH3X2X_DCMT_EN_MSB);
    uchDownSampleEn = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL0_REG_ADDR, GH3X2X_DCMT_DNSAMPLE_DIS_LSB, GH3X2X_DCMT_DNSAMPLE_DIS_MSB);

    uchDecimationEn &= (~uchDownSampleEn);  //we need channel which uchDecimationEn = 1 and uchDownSampleEn = 0
    uchValidChnlCnt = 0;
    for(GU8 uchChnlCnt = 0; uchChnlCnt < GH3X2X_DECIMAION_CHNL_MAX; uchChnlCnt ++)
    {
        if(uchDecimationEn&(1 << uchChnlCnt))
        {
            pstDecimationInfo->puchSlotAdcIdx[uchValidChnlCnt] = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL1_REG_ADDR + uchChnlCnt * 2, GH3X2X_DCMT_HEAD_LSB, GH3X2X_DCMT_HEAD_MSB);
            pstDecimationInfo->puchDecimationOption[uchValidChnlCnt] = GH3X2X_ReadRegBitField(GH3X2X_DCMT_CTRL1_REG_ADDR + uchChnlCnt * 2, GH3X2X_DCMT_RND_SEL_LSB, GH3X2X_DCMT_RND_SEL_MSB);
            uchValidChnlCnt ++;
        }
    }
    pstDecimationInfo->uchDecimaionChnlNum = uchValidChnlCnt;
}

/// @brief get decimaion option of a specific channel
/// @param uchSlotAdcIdx 
/// @return Decimaion option.   0: no decimtion  1: 2x decimation  2: 4x decimation 3: 8x decimation
GU8 Gh3x2xGetChannelDecimationOption(GU8 uchSlotAdcIdx, STGh3x2xDecimationInfo *pstDecimationInfo)
{
    for(GU8 uchChnlCnt = 0; uchChnlCnt < pstDecimationInfo->uchDecimaionChnlNum; uchChnlCnt ++)
    {
        if(pstDecimationInfo->puchSlotAdcIdx[uchChnlCnt] == uchSlotAdcIdx)
        {
            return pstDecimationInfo->puchDecimationOption[uchChnlCnt];
        }
    }
    return 0;
}
    #endif

    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
GU16 Gh3x2xGetDividerOfChnl(GU8 uchSlotAdcIdx, GU8 *puchSlotSampleRateFactor)
    #else
GU16 Gh3x2xGetDividerOfChnl(GU8 uchSlotAdcIdx, STGh3x2xDecimationInfo *pstDecimationInfo, GU8 *puchSlotSampleRateFactor)
    #endif
{
    GU16 usTempDivider;
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    usTempDivider = ((GU16)puchSlotSampleRateFactor[uchSlotAdcIdx >> 2] + 1);
    #else
    GU8 uchDecimationOption;
    uchDecimationOption = Gh3x2xGetChannelDecimationOption(uchSlotAdcIdx, pstDecimationInfo);
    usTempDivider = ((GU16)puchSlotSampleRateFactor[uchSlotAdcIdx >> 2] + 1) << uchDecimationOption;
    #endif
    return usTempDivider;
}

/// @brief get slot adc index of the lowest frequence channel
/// @param pstDecimationInfo 
/// @param puchSlotSampleRateFactor the register value of rg_slotX_sr_multiplier(X = 0~7), the array length is 8
/// @param unSlotAdcChnlEn bi0: slot0adc0 is en or not   bit1: slot0adc1 is en or not  ...  bit31: is slot7adc3 is en or not
/// @return slot adc index of this channel
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
GU8 Gh3x2xGetLowestChnl(GU8 *puchSlotSampleRateFactor, GU32 unSlotAdcChnlEn)
    #else
GU8 Gh3x2xGetLowestChnl(STGh3x2xDecimationInfo *pstDecimationInfo, GU8 *puchSlotSampleRateFactor, GU32 unSlotAdcChnlEn)
    #endif
{
    GU16 usLowestChnlDivider = 0;
    GU8  uchLowestChnlSlotAdc = 0xFF;
    for(GU8 uchChnlCnt = 0; uchChnlCnt < (GH3X2X_RX_NUM_MAX * GH3X2X_SLOT_NUM_MAX); uchChnlCnt ++)
    {
        if(unSlotAdcChnlEn&(1 << uchChnlCnt))
        {

            GU16 usTempDivider;
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
            usTempDivider = Gh3x2xGetDividerOfChnl(uchChnlCnt, puchSlotSampleRateFactor);
    #else
            usTempDivider = Gh3x2xGetDividerOfChnl(uchChnlCnt, pstDecimationInfo, puchSlotSampleRateFactor);
    #endif
            GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] slot%d, adc%d, usTempDivider = %d.\r\n",(int)(GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchChnlCnt)), (int)(GH3X2X_GET_ADC_FROM_CHANNEL_NUM(uchChnlCnt)), (int)usTempDivider);
            if(usTempDivider > usLowestChnlDivider)
            {
                usLowestChnlDivider = usTempDivider;
                uchLowestChnlSlotAdc = uchChnlCnt;
            }
        }
    }
    return uchLowestChnlSlotAdc;
}

/// @brief get sample rate radio(integer part) of target channel and reference channel
/// @param pstDecimationInfo 
/// @param puchSlotSampleRateFactor the register value of rg_slotX_sr_multiplier(X = 0~7), the array length is 8
/// @param uchTargetChnlSlotAdcIdx slot adc index of target channel
/// @param uchRefChnlSlotAdcIdx slot adc index of reference channel
/// @param puchDecimalFlag if decimal part is not zero, the flag will be set
/// @return 
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
GU16 Gh3x2xGetTargetChnlSampleRateRadio(GU8 *puchSlotSampleRateFactor, GU8 uchTargetChnlSlotAdcIdx, GU8 uchRefChnlSlotAdcIdx)
    #else
GU16 Gh3x2xGetTargetChnlSampleRateRadio(STGh3x2xDecimationInfo *pstDecimationInfo, GU8 *puchSlotSampleRateFactor, GU8 uchTargetChnlSlotAdcIdx, GU8 uchRefChnlSlotAdcIdx, GU8 *puchDecimalFlag)
    #endif
{
    GU16 usTargetChnlDivder;
    GU16 usRefChnlDivder;
    GU16 usSampleRateRadio;
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    usTargetChnlDivder = Gh3x2xGetDividerOfChnl(uchTargetChnlSlotAdcIdx, puchSlotSampleRateFactor);
    usRefChnlDivder = Gh3x2xGetDividerOfChnl(uchRefChnlSlotAdcIdx, puchSlotSampleRateFactor);
    #else
    usTargetChnlDivder = Gh3x2xGetDividerOfChnl(uchTargetChnlSlotAdcIdx, pstDecimationInfo, puchSlotSampleRateFactor);
    usRefChnlDivder = Gh3x2xGetDividerOfChnl(uchRefChnlSlotAdcIdx, pstDecimationInfo, puchSlotSampleRateFactor);
    #endif
    usSampleRateRadio = usRefChnlDivder/usTargetChnlDivder;
    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    puchDecimalFlag[0] = 0;
    if(usSampleRateRadio*usTargetChnlDivder < usRefChnlDivder)
    {
        puchDecimalFlag[0] = 1;
    }
    #endif
    return usSampleRateRadio;
}

/// @brief fifo monitor module init
/// @param pstMonitorInfo 
void Gh3x2xFifoMonitorInit(STGh3x2xFifoMonitorInfo *pstMonitorInfo)
{
    GU32 unChnlEn;
    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    STGh3x2xDecimationInfo stDecimationInfo;
    GU8 uchDecimalFlag;
    #endif
    GU8 puchSlotSampleRateFactor[GH3X2X_SLOT_NUM_MAX];
    GU16 usSampleRateRadio;

    GH3X2X_Memset((void*)pstMonitorInfo, 0, sizeof(STGh3x2xFifoMonitorInfo));

    unChnlEn = Gh3x2xGetChnlOutputEn();
    GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] unChnlEn = 0x%x.\r\n",unChnlEn);
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    if(Gh3x2xGetDecimationEn())
    {
        GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] error! decimation is detected, please set GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT to 1\r\n");
        while(1);
    }
    #else
    Gh3x2xGetDecimationInfo(&stDecimationInfo);
    #endif
    Gh3x2xGetSlotSampleRateFactor(puchSlotSampleRateFactor);

    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    pstMonitorInfo->uchLowestChnlSlotAdcIdx = Gh3x2xGetLowestChnl(puchSlotSampleRateFactor, unChnlEn);
    #else
    pstMonitorInfo->uchLowestChnlSlotAdcIdx = Gh3x2xGetLowestChnl(&stDecimationInfo, puchSlotSampleRateFactor, unChnlEn);
    #endif

    GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] lowest chnl is slot%d, adc%d.\r\n",(int)(GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(pstMonitorInfo->uchLowestChnlSlotAdcIdx)), (int)(GH3X2X_GET_ADC_FROM_CHANNEL_NUM(pstMonitorInfo->uchLowestChnlSlotAdcIdx)));
    for(GU8 uchChnlCnt = 0; uchChnlCnt < (GH3X2X_RX_NUM_MAX * GH3X2X_SLOT_NUM_MAX); uchChnlCnt ++)
    {
    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
        uchDecimalFlag = 0;
    #endif
        if(unChnlEn & (1 << uchChnlCnt))
        {
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
            usSampleRateRadio = Gh3x2xGetTargetChnlSampleRateRadio(puchSlotSampleRateFactor, uchChnlCnt, pstMonitorInfo->uchLowestChnlSlotAdcIdx);
    #else
            usSampleRateRadio = Gh3x2xGetTargetChnlSampleRateRadio(&stDecimationInfo, puchSlotSampleRateFactor, uchChnlCnt, pstMonitorInfo->uchLowestChnlSlotAdcIdx, &uchDecimalFlag);
    #endif
            
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
            pstMonitorInfo -> usEstiSampleCntlimit[GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchChnlCnt)] += usSampleRateRadio;
    #endif
            pstMonitorInfo -> usEstiAllChnlSampleCntlimit += usSampleRateRadio;
            
    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
        #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
            pstMonitorInfo -> uchEstiSampleCntUplimit[GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchChnlCnt)] += usSampleRateRadio + uchDecimalFlag;
        #endif
            pstMonitorInfo -> usEstiAllChnlSampleCntUplimit += usSampleRateRadio + uchDecimalFlag;
    #endif

    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
            GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] slot%dadc%d usSampleRateRadio = %d, uchDecimalFlag = %d.\r\n",
            (int)(GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchChnlCnt)), 
            (int)(GH3X2X_GET_ADC_FROM_CHANNEL_NUM(uchChnlCnt)),
            (int)(usSampleRateRadio), 
            (int)(uchDecimalFlag)
            );
    #else
            GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorInit] slot%dadc%d usSampleRateRadio = %d.\r\n",
            (int)(GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchChnlCnt)), 
            (int)(GH3X2X_GET_ADC_FROM_CHANNEL_NUM(uchChnlCnt)),
            (int)(usSampleRateRadio)
            );
    #endif
        }
    }

    pstMonitorInfo -> usNeedSkipFifoByteNum = GH3X2X_ReadReg(GH3X2X_INT_FIFO_UR_REG_ADDR) * GH3X2X_FIFO_RAWDATA_SIZE;
}


/// @brief recovry process function which is using after abnormal fifo data is monitored
/// @param pstMonitorInfo 
void Gh3x2xFifoMonitorRecovery(STGh3x2xFifoMonitorInfo *pstMonitorInfo)
{
    Gh3x2xFifoRecovery();
    Gh3x2xFifoMonitorInit(pstMonitorInfo);
}

extern volatile GU32 g_unDebugFifoMonitor;

/// @brief fifo monitor module process function
/// @param pstMonitorInfo 
/// @param pFifo fifo data byte stream
/// @param FifoByteNum  fifo date byte number
void Gh3x2xFifoMonitorPro(STGh3x2xFifoMonitorInfo *pstMonitorInfo, GU8 *pFifo, GU16 FifoByteNum)
{
    GU8 uchSlotAdc;
    GU16 usAllChnlUpLimit;
    GU16 usAllChnlDownLimit;
    GU8 uchIsAbnormal = 0;
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
    GU8 uchSlotIdx;
    GU16 *pusUpLimit;
    GU16 *pusDownLimit;
    pusDownLimit = pstMonitorInfo ->usEstiSampleCntlimit;
        #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    pusUpLimit = pstMonitorInfo ->usEstiSampleCntlimit;
        #else
    pusUpLimit = pstMonitorInfo ->uchEstiSampleCntUplimit;
        #endif
    #endif
    usAllChnlDownLimit = pstMonitorInfo ->usEstiAllChnlSampleCntlimit;
    #if  (0 == GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
    usAllChnlUpLimit = pstMonitorInfo ->usEstiAllChnlSampleCntlimit;
    #else
    usAllChnlUpLimit = pstMonitorInfo ->usEstiAllChnlSampleCntUplimit;
    #endif

    for(GU16 usFifoByteCnt = 0; usFifoByteCnt < FifoByteNum; usFifoByteCnt += GH3X2X_FIFO_RAWDATA_SIZE)
    {
        if(0 == pstMonitorInfo ->usNeedSkipFifoByteNum)
        {
            uchSlotAdc = GH3X2X_BYTE_RAWDATA_GET_SLOT_ADC_NUM(pFifo[usFifoByteCnt]);
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
            uchSlotIdx = GH3X2X_GET_SLOT_FROM_CHANNEL_NUM(uchSlotAdc);
    #endif
            //GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]Got data :Slot%dAdc%d.\r\n",(int)uchSlotIdx, (int)(GH3X2X_GET_ADC_FROM_CHANNEL_NUM(uchSlotAdc)));
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
            pstMonitorInfo ->usRealTimeSampleCnt[uchSlotIdx] += 1;
    #endif
            pstMonitorInfo ->usAllChnlRealTimeSampleCnt += 1;
    #if GH3X2X_FIFO_MONITOR_DEBUG
            pstMonitorInfo ->usEveryChnlRealTimeSampleCnt[uchSlotIdx][GH3X2X_GET_ADC_FROM_CHANNEL_NUM(uchSlotAdc)] += 1;
    #endif
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
            if(pstMonitorInfo -> usRealTimeSampleCnt[uchSlotIdx] > pusUpLimit[uchSlotIdx])  // is abnormal
            {
                uchIsAbnormal = 1;
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]RealTimeSampleCnt is too big,uchSlotIdx = %d,RealTimeSampleCnt = %d, limit = %d\r\n",(int)uchSlotIdx, (int)pstMonitorInfo -> usRealTimeSampleCnt[uchSlotIdx], pusUpLimit[uchSlotIdx]);
            }
            
            if((pstMonitorInfo ->uchLowestChnlAppearedFlag) &&(uchSlotAdc == pstMonitorInfo->uchLowestChnlSlotAdcIdx) && (uchSlotAdc == pstMonitorInfo->uchLowestChnlSlotAdcIdx))
            {
                for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X2X_SLOT_NUM_MAX; uchSlotCnt ++)
                {
                    if(pstMonitorInfo -> usRealTimeSampleCnt[uchSlotCnt] < pusDownLimit[uchSlotCnt])
                    {
                        uchIsAbnormal = 1;
                        GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]RealTimeSampleCnt is too small,uchSlotCnt = %d,RealTimeSampleCnt = %d, limit = %d\r\n",(int)uchSlotCnt, (int)pstMonitorInfo -> usRealTimeSampleCnt[uchSlotCnt], pusDownLimit[uchSlotCnt]);
                    }
                }
            }
    #endif

            if(pstMonitorInfo -> usAllChnlRealTimeSampleCnt > usAllChnlUpLimit)  // is abnormal
            {
                uchIsAbnormal = 2;
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]AllChnlRealTimeSampleCnt is too big,RealTimeSampleCnt = %d, limit = %d.\r\n", (int)pstMonitorInfo -> usAllChnlRealTimeSampleCnt, usAllChnlUpLimit);
            }
            if((pstMonitorInfo ->uchLowestChnlAppearedFlag)&&(uchSlotAdc == pstMonitorInfo->uchLowestChnlSlotAdcIdx) && (pstMonitorInfo -> usAllChnlRealTimeSampleCnt < usAllChnlDownLimit))  // is abnormal
            {
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]AllChnlRealTimeSampleCnt is too samll,RealTimeSampleCnt = %d, limit = %d.\r\n", (int)pstMonitorInfo -> usAllChnlRealTimeSampleCnt, usAllChnlDownLimit);
                uchIsAbnormal = 2;
            }

            if(2 == uchIsAbnormal)
            {
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] ***************every channel information START************\r\n");
                for(GU8 uchSlotCnt = 0; uchSlotCnt < GH3X2X_SLOT_NUM_MAX; uchSlotCnt ++)
                {
    #if GH3X2X_FIFO_MONITOR_DEBUG
                    GH3X2X_DEBUG_LOG_PARAM(
                    "[Gh3x2xFifoMonitorPro] Real time every channel sample num: Slot%d: %d, %d, %d, %d.\r\n", 
                    (int)uchSlotCnt, 
                    (int)pstMonitorInfo ->usEveryChnlRealTimeSampleCnt[uchSlotCnt][EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX0_INDEX],
                    (int)pstMonitorInfo ->usEveryChnlRealTimeSampleCnt[uchSlotCnt][EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX1_INDEX],
                    (int)pstMonitorInfo ->usEveryChnlRealTimeSampleCnt[uchSlotCnt][EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX2_INDEX],
                    (int)pstMonitorInfo ->usEveryChnlRealTimeSampleCnt[uchSlotCnt][EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX3_INDEX]
                    );
    #endif

    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
                    GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] Real time slot sample num up: Slot%d: %d.\r\n", (int)uchSlotCnt, (int)pstMonitorInfo ->usRealTimeSampleCnt[uchSlotCnt]);
                    GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] slot sample num up limit: Slot%d: %d.\r\n", (int)uchSlotCnt, (int)pstMonitorInfo ->uchEstiSampleCntUplimit[uchSlotCnt]);
                    GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] slot sample num down limit: Slot%d: %d.\r\n", (int)uchSlotCnt, (int)pstMonitorInfo ->usEstiSampleCntlimit[uchSlotCnt]);
    #endif

                }
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] Real time all-chnl sample num: %d.\r\n", (int)pstMonitorInfo ->usAllChnlRealTimeSampleCnt);
    #if  (GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT)
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] all-chnl sample up limit num: %d.\r\n", (int)pstMonitorInfo ->usEstiAllChnlSampleCntUplimit);
    #endif
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] all-chnl sample down limit num: %d.\r\n", (int)pstMonitorInfo ->usEstiAllChnlSampleCntlimit);
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro] ***************every channel information END************\r\n");
            }

            if(2 == uchIsAbnormal)
            {

                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]recover fifo start...\r\n");
                Gh3x2xFifoMonitorRecovery(pstMonitorInfo);  //go to recovery process
                GH3X2X_DEBUG_LOG_PARAM("[Gh3x2xFifoMonitorPro]recover fifo end.\r\n");
                break;
            }

            if(uchSlotAdc == pstMonitorInfo->uchLowestChnlSlotAdcIdx)
            {
                pstMonitorInfo ->uchLowestChnlAppearedFlag = 1;
    #if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
                GH3X2X_Memset((void*)pstMonitorInfo -> usRealTimeSampleCnt, 0, sizeof(pstMonitorInfo -> usRealTimeSampleCnt));
    #endif
                pstMonitorInfo -> usAllChnlRealTimeSampleCnt = 0;
    #if GH3X2X_FIFO_MONITOR_DEBUG
                GH3X2X_Memset((void*)pstMonitorInfo -> usEveryChnlRealTimeSampleCnt, 0, sizeof(pstMonitorInfo -> usEveryChnlRealTimeSampleCnt));
    #endif
                pstMonitorInfo->usAllChnlRealTimeSampleCnt = 0;
            }
        }
        else
        {
            if(pstMonitorInfo ->usNeedSkipFifoByteNum < GH3X2X_FIFO_RAWDATA_SIZE)
            {
                pstMonitorInfo ->usNeedSkipFifoByteNum = 0;
            }
            else
            {
                pstMonitorInfo ->usNeedSkipFifoByteNum -= GH3X2X_FIFO_RAWDATA_SIZE;
            }
        }
    }
}
#endif

#if __DRIVER_OPEN_ALL_SOURCE__
void GH3X2X_CalRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen)
{
    UNUSED_VAR(puchRawdata);
    UNUSED_VAR(usRawdataLen);
}

GU8 GH3X2X_CheckRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen)
{
    return 0;
}

/**
 * @fn     GS16 GH3X2X_ReadFifodata(GU8 *puchGh3x2xReadFifoData, GU16* pusReadFifoDataLen)
 *
 * @brief  Read Gh3x2x Fifo Data for open source
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  puchGh3x2xReadFifoData      pointer to Gh3x2x fifo data
 * @param[out]  pusReadFifoDataLen          pointer to Gh3x2x fifo data length in byte
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return read fifo successful
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 * @retval  #GH3X2X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 */
GS16 GH3X2X_ReadFifodata(GU8 *puchGh3x2xReadFifoData, GU16* pusReadFifoDataLen ,GU16 usFifoReadByteNum)
{
    //GU16 usFifoLength = 0;
    GS16 sRet = GH3X2X_RET_OK;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((puchGh3x2xReadFifoData != GH3X2X_PTR_NULL) && (pusReadFifoDataLen != GH3X2X_PTR_NULL))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
        /*
        usFifoLength = GH3X2X_FIFO_CNT_CALC(GH3X2X_ReadReg(GH3X2X_INT_FIFO_UR_REG_ADDR));
        */
        if (usFifoReadByteNum > g_usMaxNumReadFromFifo)
        {
            usFifoReadByteNum = g_usMaxNumReadFromFifo;
            sRet = GH3X2X_RET_READ_FIFO_CONTINUE;
        }

        if ((usFifoReadByteNum > 0) && (usFifoReadByteNum <= GH3X2X_FIFO_DATA_BYTES_MAX_LEN))
        {
            GH3X2X_ReadFifo(puchGh3x2xReadFifoData, usFifoReadByteNum);
        }
        else
        {
            if (usFifoReadByteNum > GH3X2X_FIFO_DATA_BYTES_MAX_LEN)
            {
                usFifoReadByteNum = 0;
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo len greater than max, pelease check i2c/spi!\r\n");
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo equl 0!\r\n");
            }
        }
        
        /* call hook */
        HOOK_FUNC_CALL(g_pGh3x2xGetRawdataHookFunc, (puchGh3x2xReadFifoData, usFifoReadByteNum) );
        GH3X2X_WAIT_CHIP_DSLP();
        (*pusReadFifoDataLen) = usFifoReadByteNum;
    }// if (pstGh3x2xRawdata->puchReadBuffer != GH3X2X_PTR_NULL)
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("get rawdata error that pointer is null!\r\n");
        sRet = GH3X2X_RET_PARAMETER_ERROR;
    }

    //SET_VAL_VIA_PTR(usFifoReadByteNum, usFifoLength);
    return sRet;
}

GS16 GH3X2X_CascadeReadFifodata(GU8 *puchGh3x2xReadFifoDataCascade, GU8 *puchGh3x2xReadFifoDataSingleBuf, GU16* pusReadFifoDataLen ,GU16 usFifoReadByteNum, GU8 uchChip2ChannelMap)
{
    //GU16 usFifoLength = 0;
    GS16 sRet = GH3X2X_RET_OK;
    GU16 usIndex = 0;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    if ((puchGh3x2xReadFifoDataCascade != GH3X2X_PTR_NULL) && (puchGh3x2xReadFifoDataSingleBuf != GH3X2X_PTR_NULL) \
                                                            && (pusReadFifoDataLen != GH3X2X_PTR_NULL))
    {
        GH3X2X_WAIT_CHIP_WAKEUP();
    /*
        usFifoLength = GH3X2X_FIFO_CNT_CALC(GH3X2X_ReadReg(GH3X2X_INT_FIFO_UR_REG_ADDR));
        */
        if (usFifoReadByteNum > g_usMaxNumReadFromFifo)
        {
            usFifoReadByteNum = g_usMaxNumReadFromFifo;
            sRet = GH3X2X_RET_READ_FIFO_CONTINUE;
        }
        
        if ((usFifoReadByteNum > 0) && (usFifoReadByteNum <= GH3X2X_FIFO_DATA_BYTES_MAX_LEN))
        {
            //GH3X2X_CascadeOperationMasterChip();
            GH3X2X_ReadFifo(puchGh3x2xReadFifoDataSingleBuf, (usFifoReadByteNum / 4) * 4);
            GU32* punGh3x2xReadFifoDataCascade = (GU32* )puchGh3x2xReadFifoDataCascade;
            GU32* punGh3x2xReadFifoDataSingleBuf =(GU32* ) puchGh3x2xReadFifoDataSingleBuf;
            for (usIndex = 0; usIndex< usFifoReadByteNum / 4; usIndex++)
            {
                punGh3x2xReadFifoDataCascade[2*usIndex] = punGh3x2xReadFifoDataSingleBuf[usIndex];
            }
            
            GH3X2X_CascadeOperationSlaverChip();
            GH3X2X_ReadFifo(puchGh3x2xReadFifoDataSingleBuf, (usFifoReadByteNum / 4) * 4);
            GH3X2X_CascadeOperationMasterChip();
            for (usIndex = 0; usIndex< usFifoReadByteNum / 4; usIndex++)
            {
                punGh3x2xReadFifoDataSingleBuf[usIndex] &= 0xffffff07;
                punGh3x2xReadFifoDataSingleBuf[usIndex] |= uchChip2ChannelMap;
                punGh3x2xReadFifoDataCascade[2*usIndex + 1] = punGh3x2xReadFifoDataSingleBuf[usIndex];
            }
        }
        else
        {
            if (usFifoReadByteNum > GH3X2X_FIFO_DATA_BYTES_MAX_LEN)
            {
                usFifoReadByteNum = 0;
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo len greater than max, pelease check i2c/spi!\r\n");
            }
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("get rawdata fifo equl 0!\r\n");
            }
        }
        
        /* call hook */
        HOOK_FUNC_CALL(g_pGh3x2xGetRawdataHookFunc, (puchGh3x2xReadFifoDataCascade, usFifoReadByteNum * 2) );
        GH3X2X_WAIT_CHIP_DSLP();
    }// if (pstGh3x2xRawdata->puchReadBuffer != GH3X2X_PTR_NULL)
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("get rawdata error that pointer is null!\r\n");
        sRet = GH3X2X_RET_PARAMETER_ERROR;
    }

    //SET_VAL_VIA_PTR(usFifoReadByteNum, usFifoLength);
    (*pusReadFifoDataLen) = usFifoReadByteNum * 2;
    return sRet;
}
#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
