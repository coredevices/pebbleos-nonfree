/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_control.h
 *
 * @brief   gh3x2x control define & declaration
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#ifndef _GH3X2X_DRV_CONTROL_H_
#define _GH3X2X_DRV_CONTROL_H_

#include "gh_drv_config.h"
#include "gh_drv.h"
#include "gh_drv_registers.h"
#include "gh_demo_config.h"

/**
 * @brief status flag enum
 */
enum EMGh3x2xStatus
{
    GH3X2X_STATUS_NO_INIT = 0,  /**< no init status */
    GH3X2X_STATUS_INITED,       /**< inited status */
    GH3X2X_STATUS_STARTED,      /**< started status */
    GH3X2X_STATUS_INVALID,      /**< invaild status */
};


/* function ptr typedef */

typedef void (*pfnNormalHookFunc)(void);                                                 /**< normal hook type */
typedef void (*pfnGetRawdataHookFunc)(GU8 *puchDataBuffer, GU16 usLen);                  /**< get rawdata hook type */
typedef GU8 (*pfnReadPinStatus)(void);                                                   /**< read pin status type */
typedef void (*pfnSetPinLevel)(GU8 uchPinLevel);                                         /**< set pin status type */
typedef void (*pfnGetIoStructInfo)(const STGh3x2xFrameInfo * const pstFrameInfo);
typedef void (*pfnWriteAlgConfigWithVirtualReg)(GU16 usVirtualRegAddr, GU16 usVirtualRegValue);

extern GU8 g_uchGh3x2xChipOperateIndexRealtime;

/// send wakeup(resume) cmd
#define   GH3X2X_SEND_WAKEUP_CMD()          do { GH3X2X_SendCmd(GH3X2X_CMD_RESUME); } while (0)

/// send sleep cmd
#define   GH3X2X_SEND_DSLEEP_CMD()          do { GH3X2X_SendCmd(GH3X2X_CMD_SLEEP); } while (0)

/// send reset cmd
#define   GH3X2X_SEND_RESET_CMD()           do { GH3X2X_SendCmd(GH3X2X_CMD_RESET); } while (0)

/// wait send cmd done
#define   GH3X2X_SEND_WAIT_CMD_DONE()       do { GH3X2X_DelayUs(GH3X2X_CMD_DELAY_X_US); } while (0)

/// wait send sleep done
#define   GH3X2X_SEND_WAIT_SLEEP_CMD_DONE() do { GH3X2X_DelayUs(50); } while (0)

#define GH3X2X_CHECK_0x0088()             GH3X2X_SEND_WAKEUP_CMD();\
                                          GH3X2X_SEND_WAIT_CMD_DONE();\
                                          GH3X2X_DEBUG_LOG_PARAM("Master: 0x088:%04x\r\n",GH3X2X_ReadReg(0x88));
#if GH3X2X_WAKE_UP_CONFIRM_EN
/// wake up confirm
#define  GH3X2X_WAKE_UP_CONFIRM(ret)    do {\
                                            (ret) = GH3X2X_RET_COMM_ERROR;\
                                            if (GH3X2X_ReadReg(GH3X2X_CHIP_READY_CODE_REG_ADDR) == GH3X2X_CHIP_READY_REG_VAL)\
                                            {\
                                                (ret) = GH3X2X_RET_OK;\
                                            }\
                                            else \
                                            {\
                                                GH3X2X_INFO_LOG("[GH3X2X_WakeUpConfirm] Chip wake up fail!!! Try wake up again.\r\n");\
                                                GH3X2X_SEND_WAKEUP_CMD();\
                                                GH3X2X_SEND_WAIT_CMD_DONE();\
                                                if (GH3X2X_ReadReg(GH3X2X_CHIP_READY_CODE_REG_ADDR) == GH3X2X_CHIP_READY_REG_VAL)\
                                                {\
                                                    (ret) = GH3X2X_RET_OK;\
                                                }\
                                                else \
                                                {\
                                                    GH3X2X_INFO_LOG("[GH3X2X_WakeUpConfirm] Chip wake up fail!!!\r\n");\
                                                }\
                                            }\
                                        }while (0)

/// chip wakeup
#define   GH3X2X_CHIP_WAKEUP(ret)       do {\
                                            GH3X2X_SEND_WAKEUP_CMD();\
                                            GH3X2X_SEND_WAIT_CMD_DONE();\
                                            GH3X2X_WAKE_UP_CONFIRM(ret);\
                                            GH3X2X_PMU_FIFO_POWER_RECOVERY();\
                                            (ret) = GH3X2X_RET_OK;\
                                        } while (0)
#else
/// chip wakeup
#define   GH3X2X_CHIP_WAKEUP(ret)       do {\
                                            GH3X2X_SEND_WAKEUP_CMD();\
                                            GH3X2X_SEND_WAIT_CMD_DONE();\
                                            GH3X2X_PMU_FIFO_POWER_RECOVERY();\
                                            (ret) = GH3X2X_RET_OK;\
                                        } while (0)										
#endif
/// chip sleep
#define   GH3X2X_CHIP_SLEEP(ret)        do {\
                                            GH3X2X_PMU_FIFO_POWER_OFF();\
                                            GH3X2X_SEND_DSLEEP_CMD();\
                                            GH3X2X_SEND_WAIT_SLEEP_CMD_DONE();\
                                            (ret) = GH3X2X_RET_OK;\
                                        } while (0)

/// chip reset
#define   GH3X2X_CHIP_RESET(ret)        do {\
                                            GH3X2X_SEND_RESET_CMD();\
                                            (ret) = GH3X2X_RET_OK;\
                                        } while (0)

/// chip wakeup
#define   GH3X2X_CHIP_WAKEUP_NORET()    

/// chip sleep
#define   GH3X2X_CHIP_SLEEP_NORET()     do {\
                                            GH3X2X_PMU_FIFO_POWER_OFF();\
                                            GH3X2X_SEND_DSLEEP_CMD();\
                                            GH3X2X_SEND_WAIT_SLEEP_CMD_DONE();\
                                            GH3X2X_SetChipSleepFlag(g_uchGh3x2xChipOperateIndexRealtime);\
                                        } while (0)

/// chip wakeup
#define   GH3X2X_CHIP_WAKEUP_PROTOCOL()

/// chip sleep
#define   GH3X2X_CHIP_SLEEP_PROTOCOL()  do {\
                                            GH3X2X_SEND_DSLEEP_CMD();\
                                            GH3X2X_SEND_WAIT_SLEEP_CMD_DONE();\
                                            GH3X2X_SetChipSleepFlag(g_uchGh3x2xChipOperateIndexRealtime);\
                                        } while (0)

/// chip reset
#define   GH3X2X_CHIP_RESET_NORET()     do {\
                                            GH3X2X_SEND_RESET_CMD();\
                                        } while (0)

/// chip wakeup
#define   GH3X2X_WAIT_CHIP_WAKEUP()

/// chip sleep
#define   GH3X2X_WAIT_CHIP_DSLP()
#if GH3X2X_PMU_WAIT_PULSE_OFF_ENABLED

/// wait pulse mode off
#define GH3X2X_PMU_WAIT_PULSE_MODE_OFF()    do {\
                                                GH3X2X_SEND_DSLEEP_CMD();\
                                                GH3X2X_SEND_WAIT_SLEEP_CMD_DONE();\
                                                GH3X2X_SEND_WAKEUP_CMD();\
                                                GH3X2X_SEND_WAIT_CMD_DONE();\
                                                GH3X2X_SEND_WAIT_CMD_DONE();\
                                            } while (0)

#else

/// wait pulse mode off
#define GH3X2X_PMU_WAIT_PULSE_MODE_OFF()    do {} while (0)

#endif

/* reg addr control bit macro */

/// reg addr[11:0] valid
#define   GH3X2X_GET_REG_REAL_ADRR(x)       ((x) & 0x0FFF)

/* virtual reg addr mcaro */

/// software config reg end addr, software addr map [0x0000~0x09FE]
#define   GH3X2X_VIRTUAL_REG_SW_CONFIG_END_ADDR         (0x29FF)

/// algorithm config reg end addr, algorithm addr map [0x0A00~0x0EFE]
#define   GH3X2X_VIRTUAL_REG_ALGO_CONFIG_END_ADDR       (0x2EFF)

/// write algorithm config function
#define   GH3X2X_FUNCTION_CONFIG_WRITE(...)       do { GH3X2X_WriteFunctionConfigWithVirtualReg(__VA_ARGS__); } while (0)
/// write algorithm config function
#define   GH3X2X_FUNCTION_CONFIG_READ(...)      //(GH3X2X_ReadAlgorithmConfigWithVirtualReg(__VA_ARGS__))

/// write software config function
#define   GH3X2X_SW_CONFIG_WRITE(...)         do { GH3X2X_WriteSwConfigWithVirtualReg(__VA_ARGS__); } while (0)
/// write software config function
#define   GH3X2X_SW_CONFIG_READ(...)        //(GH3X2X_ReadSwConfigWithVirtualReg(__VA_ARGS__))

/* reg instructions chip inited macro */
#if   GH3X2X_INSTRUCTIONS_CHIP_INIED_ENABLED

/// set chip inited flag into reg
#define   GH3X2X_SET_CHIP_INIED()    do {\
                                        GU16 usChipIniedRegValTmp =\
                                            GH3X2X_ReadReg(GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_ADDR);\
                                            GH3X2X_WriteReg(GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_ADDR, \
                                                usChipIniedRegValTmp | GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_VAL);\
                                     } while (0)

/// read chip inited flag from reg
#define   GH3X2X_IS_CHIP_INIED()    ((GH3X2X_ReadReg(GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_ADDR) &\
                                        GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_VAL) == \
                                        GH3X2X_INSTRUCTIONS_CHIP_INIED_REG_VAL)
#else

/// set chip inited flag into reg
#define   GH3X2X_SET_CHIP_INIED()

/// read chip inited flag from reg
#define   GH3X2X_IS_CHIP_INIED()    (1)

#endif

#if     GH3X2X_PMU_FIFO_POWER_CTRL_ENABLED

/// store fifo power status
#define  GH3X2X_PMU_FIFO_POWER_OFF()    do {\
                                            if (g_puchPmuFifoModuleCtrlCacheStatus[g_uchGh3x2xChipOperateIndexRealtime] == 0)\
                                            {\
                                                if ((g_uchGh3x2xStatus == GH3X2X_STATUS_INITED) || \
                                                    (g_uchGh3x2xStatus == GH3X2X_STATUS_NO_INIT)) \
                                                {\
                                                    GH3X2X_WriteReg(GH3X2X_PMU_CTRL4_REG_ADDR, \
                                                     (g_pusPmuFifoModuleCtrlVal[g_uchGh3x2xChipOperateIndexRealtime] | GH3X2X_PMU_CTRL4_FIFO_DISABLE_MASK));\
                                                    g_puchPmuFifoModuleCtrlCacheStatus[g_uchGh3x2xChipOperateIndexRealtime] = 1;\
                                                    GH3X2X_DEBUG_LOG_INTERNAL("save pmu fifo module status!\r\n");\
                                                }\
                                            }\
                                        } while (0)

/// set chip inited flag into reg
#define  GH3X2X_PMU_FIFO_POWER_RECOVERY()       do {\
                                                    if (g_puchPmuFifoModuleCtrlCacheStatus[g_uchGh3x2xChipOperateIndexRealtime] == 1)\
                                                    {\
                                                        GH3X2X_WriteReg(GH3X2X_PMU_CTRL4_REG_ADDR, g_pusPmuFifoModuleCtrlVal[g_uchGh3x2xChipOperateIndexRealtime]);\
                                                        GH3X2X_PMU_WAIT_PULSE_MODE_OFF();\
                                                        g_puchPmuFifoModuleCtrlCacheStatus[g_uchGh3x2xChipOperateIndexRealtime] = 0;\
                                                        GH3X2X_DEBUG_LOG_INTERNAL("recovery pmu fifo module status!\r\n");\
                                                    }\
                                                } while (0)
#else
/// store fifo power status
#define   GH3X2X_PMU_FIFO_POWER_OFF()

/// set chip inited flag into reg
#define   GH3X2X_PMU_FIFO_POWER_RECOVERY()
#endif

/// wear detect force switch delay
#define GH3X2X_WEAR_DETECT_SWITCH_WAIT_X_US             (150)

/// hard reset delay
#define   GH3X2X_HARD_RESET_DELAY_X_US                  (100)

/// fifo watermark thr min val
#define   GH3X2X_FIFO_WATERMARK_THR_MIN                 (3)

/// fifo watermark thr max val
#define   GH3X2X_FIFO_WATERMARK_THR_MAX                 (800)

/// irq width val
#define   GH3X2X_IRQ_WIDTH_VAL_MAX                      (2047)

/* osc cali */

/// osc calibration code lock flag
#define   GH3X2X_OSC_CALI_CODE_LOACKED                  (0x0001)

/// osc calibration Ctrl disable
#define   GH3X2X_OSC_CALI_CTRL_DIS_VAL                  (0x0000)

/// hsi osc calibration delay
#define   GH3X2X_HSI_CALI_DELAY_VAL                     (1000u)

/// hsi osc calibration theroy count
#define   GH3X2X_HSI_CALI_THR_CNT_VAL                   (0x0196)

/// hsi osc calibration Ctrl enable
#define   GH3X2X_HSI_CALI_CTRL_EN_VAL                   (0x0271)

/// lsi osc calibration theroy count
#define   GH3X2X_LSI_CALI_THR_CNT_VAL                   (0x1F40)

/// lsi osc calibration ctrl coarse enable
#define   GH3X2X_LSI_CALI_CTRL_C_EN_VAL                 (0x09D7)

/// lsi osc calibration ctrl fine enable
#define   GH3X2X_LSI_CALI_CTRL_F_EN_VAL                 (0x19D3)

/// lsi osc calibration error val max
#define   GH3X2X_LSI_CALI_ERR_MAX_VAL                   (0x3FFF)

/// lsi osc calibration error val fixed val
#define   GH3X2X_LSI_CALI_ERR_FIXED_VAL                 (0x8000)

/// lsi osc calibration fine error max
#define   GH3X2X_LSI_CALI_FINE_VAL_MAX                  (255u)

/// lsi osc calibration fine tune max cnt
#define   GH3X2X_LSI_CALI_FINE_TUNE_MAX                 (9u)

/// lsi osc calibration fine div num
#define   GH3X2X_LSI_CALI_FINE_DIV_NUM                  (2u)

/// lsi osc calibration delay
#define   GH3X2X_LSI_COR_CALI_DELAY_VAL                 (50000u)

/// lsi osc calibration delay
#define   GH3X2X_LSI_FINE_CALI_DELAY_VAL                (5000u)

/**< rawdata buffer size in byte */
#define   GH3X2X_RAWDATA_BUFFER_SIZE                    (200*4)

/// get slot & adc num from rawdata flag byte, equal channel map formats
#define   GH3X2X_CHANNEL_MAP_GET(x)                     ((x) & GH3X2X_SLOT_ADC_NUM_BITS)

// fifo monitor start

typedef struct
{
    GU8 uchDecimaionChnlNum;   //0~8
    GU8 puchSlotAdcIdx[GH3X2X_DECIMAION_CHNL_MAX];        //slot and adc index for valid decimation channel
    GU8 puchDecimationOption[GH3X2X_DECIMAION_CHNL_MAX];   //1: 2x decimation   2: 4x decimaiton  3: 8x deccimaion  others: is illegal
} STGh3x2xDecimationInfo;

#define EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX0_INDEX 0
#define EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX1_INDEX 1
#define EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX2_INDEX 2
#define EVERY_CHNL_REAL_TIME_SAMPLE_CNT_RX3_INDEX 3
#define GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE       1
#define GH3X2X_FIFO_MONITOR_DEBUG              0

typedef struct
{
    GU8  uchLowestChnlSlotAdcIdx;   // slot and adc index of lowest channel,  0xFF means: there is no channel output to fifo
    GU8  uchLowestChnlAppearedFlag;   //0£ºnever appeared   1: appeared
    GU16 usNeedSkipFifoByteNum;      //the number which we need skip before formal monitor
#if 0 == GH3X2X_FIFO_MONITOR_RAM_OPTIMIZE
    GU16 usRealTimeSampleCnt[GH3X2X_SLOT_NUM_MAX];   //real time counter of sample of every slot between two lowest channels sample
    #if GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT
    GU16 uchEstiSampleCntUplimit[GH3X2X_SLOT_NUM_MAX];   //estimate up limit counter of sample of every slot between two lowest channels sample
    #endif
    GU16 usEstiSampleCntlimit [GH3X2X_SLOT_NUM_MAX];   //estimate down limit counter of sample of every slot between two lowest channels sample
#endif
    GU16 usAllChnlRealTimeSampleCnt;
    #if GH3X2X_FIFO_MONITOR_DECIMATION_SUPPORT
    GU16 usEstiAllChnlSampleCntUplimit;
    #endif
    GU16 usEstiAllChnlSampleCntlimit;
#if GH3X2X_FIFO_MONITOR_DEBUG
    GU16 usEveryChnlRealTimeSampleCnt[GH3X2X_SLOT_NUM_MAX][GH3X2X_RX_NUM_MAX];
#endif
} STGh3x2xFifoMonitorInfo;

/* extern variables declaration */

/// get rawdata hook func ptr
extern pfnGetRawdataHookFunc g_pGh3x2xGetRawdataHookFunc;

/// read int pin status func ptr
extern pfnReadPinStatus g_pGh3x2xReadIntPinStatusFunc;

/// read reset pin status func ptr
extern pfnReadPinStatus g_pGh3x2xReadResetPinStatusFunc;

/// read spcs pin status func ptr
extern pfnReadPinStatus g_pGh3x2xReadSpcsPinStatusFunc;

/// read spdo pin status func ptr
extern pfnReadPinStatus g_pGh3x2xReadSpdoPinStatusFunc;

/// set algo config hook func ptr
extern pfnWriteAlgConfigWithVirtualReg g_pGh3x2xWriteAlgConfigWithVirtualRegHookFunc;

/// config start set hook func ptr
extern pfnNormalHookFunc g_pGh3x2xConfigSetStartHookFunc;

/// config stop set hook func ptr
extern pfnNormalHookFunc g_pGh3x2xConfigSetStopHookFunc;

#if GH3X2X_PMU_FIFO_POWER_CTRL_ENABLED

/// fifo power ctrl fifo control reg cache var
extern GU16 g_pusPmuFifoModuleCtrlVal[];

/// for fifo power ctrl cache status
extern GU8 g_puchPmuFifoModuleCtrlCacheStatus[];

#endif

/// gh3x2x status
extern GU8 g_uchGh3x2xStatus;

/// max rawdata num read from fifo every time
extern GU16 g_usMaxNumReadFromFifo;

extern GU8 g_uchGsensorEnable;
/// Cap enable flag
extern GU8 g_uchCapEnable;

/// Temp enable flag
extern GU8 g_uchTempEnable;

/// function started bitmap, use for sampling control
extern GU32 g_unFuncStartedBitmap ;

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
GU16 GH3X2X_FindGu16MinVal(GU16 *pusBuffer, GU8 uchLen);

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
GU16 GH3X2X_FindGu16MaxVal(GU16 *pusBuffer, GU8 uchLen);

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
void GH3X2X_FindGu16MaxMinVal(GU16 *pusMaxVal, GU16 *pusMinVal, GU16 *pusBuffer, GU8 uchLen);

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
GU8 GH3x2x_GetChipResetRecoveringFlag(void);

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
void GH3X2X_ReinitAllSwModuleParam(GU16 usReinitFlag);

GU8 GH3x2xGetProtocolMode(void);
void GH3x2xCreatTagArray(GU8 *puchTagArray, GU16 usArrayLen,GU32 *unCompeletMask, const STGh3x2xFrameInfo * const pstFrameInfo);
void GH3x2xHandleFrameData(const STGh3x2xFrameInfo * const pstFrameInfo, GU32 *punTempIncompeletFlag,
                                GU32 *punFunctionID, GU8 uchChnlNum, GU8 *puchRawdataBuf, GU32 *punFrameRawdata,
                                GU8 *puchTag, GS16 *pusGsValueArr, float fGsensorIndex,
                                STCapRawdata* pstCapValueArr,float fCapIndex,STTempRawdata* pstTempValueArr,float fTempIndex);

GU8 GH3X2X_CheckRawdataBuf(GU8 *puchRawdata, GU16 usRawdataLen);
float GH3x2xCalGsensorStep(GU16 usGsDataNum, GU16 usFrameNum);
void GH3X2X_SendOverflowLengthBuffer(GU8 *puchBuffer, GS32 nBufferLen, GU8 uchCmd, GU8 uchSignID);
GU8 GH3X2X_ReceiveOverflowLengthBuffer(GU8 *puchRespondBuffer, GU16 *pusRespondLen, GU8 *puchReceiveBuffer, GS32 *pnReceiveBufferLen);

/**
 * @fn     GU32 DRV_DSP_RawdataCalc(GU32 *punRawdata)
 *
 * @brief  calc byte check rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata  one rawdata addr
 * @param[out]  None
 *
 * @return  target rawdata
 */
GU32 DRV_DSP_RawdataCalc(GU32 unRawdata);

/**
 * @fn     GU8 DRV_DSP_RawdataCheck(GU32 unRawdata)
 *
 * @brief  rawdata check last byte in algo lib
 *
 * @attention   none
 *
 * @param[in]   unRawdata   single rawdata
 * @param[out]  None
 *
 * @return  errcode
 * @retval  0               check success
 * @retval  1               check failed
 */
GU8 DRV_DSP_RawdataCheck(GU32 unRawdata);

/**
 * @fn     void DRV_DSP_RawdataTransform(GU32 *punRawdata)
 *
 * @brief  calc new rawdata
 *
 * @attention
 *
 * @param[in]   punRawdata
 *
 * @return  none
 */
void DRV_DSP_RawdataTransform(GU32 *punRawdata);

/**
 * @fn     GU8 GH3X2X_GetSoftWearColor(void)
 *
 * @brief  GetSoftWearColor
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  SoftWearColor 0: green  1:ir
 */

GU8 GH3X2X_GetSoftWearColor(void);
void GH3X2X_SetConfigDrvEcode(const STGh3x2xInitConfig *pstGh3x2xInitConfigParam);
void GH3X2X_SetDrvEcode(GS8 chECode);

#if (GH3X2X_FIFO_MONITOR_EN&(__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_I2C__))
void Gh3x2xFifoMonitorInit(STGh3x2xFifoMonitorInfo *pstMonitorInfo);
void Gh3x2xFifoMonitorPro(STGh3x2xFifoMonitorInfo *pstMonitorInfo, GU8 *pFifo, GU16 FifoByteNum);
#endif

#endif /* _GH3X2X_DRV_CONTROL_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
