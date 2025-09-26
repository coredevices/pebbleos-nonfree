/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_dump.h
 *
 * @brief   gh3x2x dump data define & declaration
 *
 * @version ref gh3x2x_drv_version.h
 *
 */

#ifndef _GH3X2X_DRV_DUMP_H_
#define _GH3X2X_DRV_DUMP_H_


#include "gh_drv_config.h"
#include "gh_drv.h"


/**
 * @brief dump data bg lvl select enum
 */
enum EMGh3x2xDumpRawdataBgLvlSelect
{
    GH3X2X_DUMP_RAWDATA_BGLVL0 = 0,         /**< dump rawdata bg 0 lvl */
    GH3X2X_DUMP_RAWDATA_BGLVL1,             /**< dump rawdata bg 1 lvl */
    GH3X2X_DUMP_RAWDATA_BGLVL2,             /**< dump rawdata bg 2 lvl */
    GH3X2X_DUMP_RAWDATA_BGLVL2X2,           /**< dump rawdata bg 2x2 lvl */
};

/**
 * @brief channnel dump data struct type
 */
typedef struct
{
    GU32 *punPkgNoAndCurrentArr;        /**< pointer to buffer store PkgNo&current data, map e.g. ch0 ch1...chx ch0 */
    GU32 *punChBgRawdataArr;            /**< pointer to buffer store channel bg data, map e.g. ch0 ch1...chx ch0 */
    GU32 *punChRawdataArr;              /**< pointer to buffer store channel data, map e.g. ch0 ch1...chx ch0 */
    GU16 usChRawdataCnt;                /**< channel rawdata count output */
    GU32 (*punIncompleteChRawdataArr)[6];    /**< pointer to last incomplete packet buffer */
    GU8  uchChIncompleteFlagArr[32];    /**< channel incomplete flag, max 32 channel */
} STGh3x2xChannelDumpData;












/// rawdata filter array length in byte
#define RAWDATA_FILTER_ARRAY_BYTE_LENGTH            (200 * GH3X2X_FIFO_RAWDATA_SIZE)

/// dump data max num
#define DUMP_DATA_MAX_NUM                           (6)

/// max slot num
#define   GH3X2X_MAX_SLOT_NUM                       (8)    

/// ADC num
#define   GH3X2X_ADC_NUM                            (4)

/// offset of slot0 ~ slot8 regs address
#define   SLOT_ADDR_OFFSET                          (0x001C)

/// mask of adc0 ~ adc3 tia gain
#define   GH3X2X_DRV_CURRENT_BIT_MASK               (0xFF)

/// left shift bit of drv0 current in drv current data
#define   GH3X2X_DRV0_CURRENT_LEFT_SHIFT_BIT_NUM    (0x0)

/// left shift bit of drv1 current in drv current data
#define   GH3X2X_DRV1_CURRENT_LEFT_SHIFT_BIT_NUM    (0x8)

/// bit num of adc0 ~ adc3 bg cancel
#define   GH3X2X_ADC_TIA_GAIN_BIT_NUM               (0x4)

/// mask of adc0 ~ adc3 tia gain
#define   GH3X2X_ADC_TIA_GAIN_BIT_MASK              (0xF)

/// bit num of adc0 ~ adc3 bg cancel
#define   GH3X2X_ADC_BG_CANCEL_BIT_NUM              (0x2)

/// mask of adc0 ~ adc3 bg cancel
#define   GH3X2X_ADC_BG_CANCEL_BIT_MASK             (0x3)

/// left shift bit of gain in gain&bgcancel data
#define   GH3X2X_ADC_TIA_GAIN_LEFT_SHIFT_BIT_NUM    (0x0)

/// left shift bit of bg cancel in gain&bgcancel data
#define   GH3X2X_ADC_BG_CANCEL_LEFT_SHIFT_BIT_NUM   (0x4)

/// mask of slot bg level
#define   GH3X2X_SLOT_BG_LEVEL_BIT_MASK             (0x03)    

/// dump data offset
#define   GH3X2X_DUMP_DATA_OFFSET                   (8388608)

/// max dump data num of every channel
#define   GH3X2X_MAX_DUMP_DATA_NUM_CHANNEL          (4)  

/// max dump data num of every channel that will upload
#define   GH3X2X_MAX_DUMP_DATA_NUM_UPLOAD           (3)  

/// dump gain & current enable
#define   GH3X2X_DUMP_GAIN_CURRENT_ENABLE_BIT       (0)      

/// dump ambiance enable
#define   GH3X2X_DUMP_AMBIANCE_ENABLE_BIT           (1)

/// dump electrode wear enable
#define   GH3X2X_DUMP_ELECTR_WEAR_ENABLE_BIT        (2)

/// ECG enable bit
#define   GH3X2X_ECG_ENABLE_BIT                     (0x1)

/// ECG ADC index
#define   GH3X2X_ECG_ADC_INDEX                      (0x3)

/// dump electrode wear status bit field
#define   GH3X2X_ELECTR_WEAR_STATUS_BIT_FIELD       (0x1)

/// dump electrode wear status bit in data tag of rawdata
#define   GH3X2X_ELECTR_WEAR_STATUS_BIT_IN_DATA_TAG (0x1)

/// dump data ppg index in buffer after filter
#define   GH3X2X_DUMP_DATA_PPG_INDEX                (0)

/// dump data gain current index in buffer after filter
#define   GH3X2X_DUMP_DATA_GAIN_CURRENT_INDEX       (1)

/// dump data ambiance index in buffer after filter
#define   GH3X2X_DUMP_DATA_AMBIANCE_INDEX           (2)

/// gain & current data size
#define   GH3X2X_DUMP_GAIN_CURRENT_DATA_SIZE        (4)

/// ambiance data size
#define   GH3X2X_DUMP_AMBIANCE_DATA_SIZE            (3)

/// gain & current data calc
#define   GH3X2X_DUMP_GAIN_CURRENT_CALC(x)          ((x) * GH3X2X_DUMP_GAIN_CURRENT_DATA_SIZE)

/// ambiance data calc
#define   GH3X2X_DUMP_AMBIANCE_CALC(x)              ((x) * GH3X2X_DUMP_AMBIANCE_DATA_SIZE)

/// gain value in dump data
#define DUMP_GAIN(x)      ((((GU8*)(&(x)))[1]) >> 4)

/// current0 in dump data
#define DUMP_CUREENT0(x)   (((GU8*)(&(x)))[3])

/// current1 in dump data
#define DUMP_CUREENT1(x)   (((GU8*)(&(x)))[2])

/// PPG data in dump data
#define DUMP_PPG_DATA(x)   ((((GU32)(((GU8*)(&(x)))[1])) << 16) + (((GU32)(((GU8*)(&(x)))[2])) << 8)\
                                                                    + (((GU32)(((GU8*)(&(x)))[3])) << 0))

/// current data index in raw data buf
extern GU16 g_usGh3x2xDataIndexInBuf;

/// current data index in raw data buf of every channel
extern GU16 g_usRawdataByteIndexArr[];

/// packet payload
//extern GU8 g_uchPacketPayloadArr[];

/// current data index in raw data buf
extern GF32 g_fGsDataIndexInBuf;

/// packet max len support
extern GU8 g_uchPacketMaxLenSupport;

/// function started bitmap, use for sampling control
extern GU32 g_unFuncStartedBitmap;

//save bg cancel value of every channel
extern GU8  g_puchGainBgCancelRecord[];

//save current of drv0 and drv1
extern STGh3x2xAgcInfo *g_pstGh3x2xAgcInfo;

/**
 * @fn     void GH3X2X_DumpInit(void)
 *
 * @brief  init dump moudle
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_DumpInit(void);

/**
 * @fn     void GH3X2X_DumpModeSet(GU16 usRegVal)
 *
 * @brief  set dump mode
 *
 * @attention   None
 *
 * @param[in]   usRegVal          reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_DumpModeSet(GU16 usRegVal);

/**
 * @fn     GU16 GH3X2X_DumpModeGet(void)
 *
 * @brief  get dump mode
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  dump mode
 */
GU16 GH3X2X_DumpModeGet(void);

/**
 * @fn     void GH3X2X_BgLevelSet(GU16 usRegVal)
 *
 * @brief  set bg level of every slot
 *
 * @attention   None
 *
 * @param[in]   usRegVal          reg value
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_BgLevelSet(GU16 usRegVal);

/**
 * @fn     void GH3X2X_RecordDumpData(void)
 *
 * @brief  read some dump data, and save them
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void GH3X2X_RecordDumpData(void);

/**
 * @fn     void GH3X2X_ReadElectrodeWearDumpData(void)
 *
 * @brief  read eletrode wear status
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  dump mode
 */
void GH3X2X_ReadElectrodeWearDumpData(void);

/**
 * @fn     void GH3X2X_BgLevelDecode(GU8 puchBgLvArr[], GU8 uchChannelCnt, GU8 *puchChannelMapArray, GU16 usBgLevel)
 *
 * @brief  decode function's bg level array according to channel map and slot bg level
 *
 * @attention   if there is 2x2 bg level,set the first slot to level 2
 *
 * @param[in]   uchChannelCnt                channel cnt
 * @param[in]   puchChannelMapArray          channel map array
 * @param[in]   usBgLevel                    bg level of slot
 * @param[out]  puchBgLvArr                  bg level array
 *
 * @return  None
 */
void GH3X2X_BgLevelDecode(GU8 puchBgLvArr[], GU8 uchChannelCnt, GU8 *puchChannelMapArray, GU16 usBgLevel);

/**
 * @fn     GS8 GH3X2X_GetDumpDataInfo(GF32 *pfGsFloatIncVal, GU16 *pusChannelDataCnt,
 *                                    GU8 *puchNextIncompleteFlag,GU8 *puchDumpDataIndexArr,
 *                                    GU8 *puchIncompleteFlagArr, GU16 usFifoLength, GU8 *puchDataBuffer,
 *                                    GU8 uchChannelCnt, GU8 *puchChannelMapArray, GU8 *puchChannelBgLvlArr, 
 *                                    GU16 usReadGsensorArrCnt)
 *
 * @brief  Get information from rawdata
 *
 * @attention   None
 *
 * @param[out]  pfGsFloatIncVal             pointer to gsensor inc value output
 * @param[out]  pusChannelDataCnt           pointer to slot data cnt output
 * @param[out]  puchNextIncompleteFlag      pointer to next incomplete flag arr output
 * @param[out]  puchDumpDataIndexArr        pointer to dump data index output
 * @param[in]   puchIncompleteFlagArr       incomplete flag array of last rawdata frame
 * @param[in]   usFifoLength                read fifo buffer data len
 * @param[in]   puchDataBuffer              pointer to read fifo buffer
 * @param[in]   uchChannelCnt               channel map arr cnt
 * @param[in]   puchChannelMapArray         pointer to channel map arr
 * @param[in]   puchChannelBgLvlArr         pointer to bg lvl arr
 * @param[in]   usReadGsensorArrCnt         read gsensor array cnt
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK               return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR    generic error when size is 0
 * @retval  #GH3X2X_RET_RESOURCE_ERROR   resource error when doesn't correspond to channel map
 */
GS8 GH3X2X_GetDumpDataInfo(GF32 *pfGsFloatIncVal, GU16 *pusChannelDataCnt,
                            GU8 *puchNextIncompleteFlag, GU8 *puchDumpDataIndexArr,
                            GU8 *puchIncompleteFlagArr, GU16 usFifoLength, GU8 *puchDataBuffer,
                            GU8 uchChannelCnt, GU8 *puchChannelMapArray, GU8 *puchChannelBgLvlArr, 
                            GU16 usReadGsensorArrCnt);

/**
 * @fn     GS8 GH3X2X_UnpackDumpDataWithChannelMap(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData,
 *                    GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr,
 *                    GU8 *puchChannelBgLvlArr)
 *
 * @brief  Unpack to channel dump data from read fifo data buffer;
 *         if dump data incomplete, should change fifo watermark
 *
 * @attention   This function should use in get rawdata hook
 * 
 * @param[in]   puchReadRawdataBuffer           pointer to read data buffer
 * @param[in]   usReadRawdataLen                read data length
 * @param[in]   uchChannelMapCnt                channel map array cnt, max:32
 * @param[in]   puchChannelMapArr               pointer to channel map array
 * @param[in]   puchChannelBgLvlArr             pointer to channel lvl array, @ref EMGh3x2xDumpRawdataBgLvlSelect
 * @param[out]  pstGh3x2xChannelDumpData        pointer to channel rawdata struct
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 */
GS8 GH3X2X_UnpackDumpDataWithChannelMap(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData,
                    GU8 *puchReadRawdataBuffer, GU16 usReadRawdataLen, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr,
                    GU8 *puchChannelBgLvlArr);

/**
 * @fn     GS16 GH3X2X_GetDumpdataWithChannelMap(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData, 
 *                       GU8 *puchReadRawdataBuffer, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr, GU8 *puchBglvlArr)
 *
 * @brief  Get dump data from fifo with channel map
 *
 * @attention   None
 *
 * @param[in]   puchReadRawdataBuffer       pointer to read data buffer
 * @param[in]   uchChannelMapCnt            channel map array cnt, max:32
 * @param[in]   puchChannelMapArr           pointer to channel map array
 * @param[in]   puchBglvlArr                pointer to bg lvl array, @ref EMGh3x2xDumpRawdataBgLvlSelect
 * @param[out]  pstGh3x2xChannelDumpData    pointer to channel rawdata struct output
 *
 * @return  GH3X2X_RET_OK                       return get data successful
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 * @retval  #GH3X2X_RET_READ_FIFO_CONTINUE      return fifo is not empty
 */
GS16 GH3X2X_GetDumpdataWithChannelMap(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData, 
                        GU8 *puchReadRawdataBuffer, GU8 uchChannelMapCnt, GU8 *puchChannelMapArr, GU8 *puchBglvlArr);

/**
 * @fn     GS8 GH3X2X_ChannelMapDumpDataClear(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData)
 *
 * @brief  clear channel map dump data struct
 *
 * @attention   This function must use before get dump data by one kind channel map
 *              e.g. channel map start/stop need clear
 *
 * @param[in]   pstGh3x2xChannelDumpData     pointer to channel dump data struct
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_PARAMETER_ERROR         return param error
 */
GS8 GH3X2X_ChannelMapDumpDataClear(STGh3x2xChannelDumpData *pstGh3x2xChannelDumpData);

/**
 * @fn       void GH3X2X_ElectrWearDumpDataPro(GU8* puchRawdataTag, GU8 uchChMapIndex)
 *
 * @brief    electrode wear dump data pro
 *
 * @attention    None
 *
 * @param[in]    puchRawdataTag      data tag of rawdata
 * @param[in]    uchChMapIndex       channel map index
 * @param[out]   None
 *
 * @return    None
 */
void GH3X2X_ElectrWearDumpDataPro(GU8* puchRawdataTag, GU8 uchChMapIndex);

/**
 * @fn       GS8 GH3X2X_SendSpo2DumpDataPkg(GU8 *puchRespondBuffer, GU16 *pusRespondLen, GU8 *puchFifoRawdata,
                                            GS32 nAlgoResultArr[], STGsensorRawdata *pstGsAxisValueArr,
                                            GU16 usGsDataNum, EMGsensorSensitivity emGsSensitivity)
 *                    
 * @brief  pack GH3X2X hb rawdata and other type of data according to specific protocol
 *
 * @attention    None
 *
 * @param[out]    puchRespondBuffer        pointer to frame data package
 * @param[out]    pusRespondLen            pointer to data length of package
 * @param[in]     puchFifoRawdata          pointer to encrypted rawdata
 * @param[in]     nAlgoResultArr           pointer to algorithm result
 * @param[in]     pstGsAxisValueArr        pointer to g sensor data
 * @param[in]     usGsDataNum              number of g sensor data
 * @param[in]     emGsSensitivity          sensitivity of g sensor
 *
 * @return    error code
 * @retval    #GH3X2X_RET_OK                       return successfully
 * @retval    #GH3X2X_RET_GENERIC_ERROR            return func start error
 * @retval    #GH3X2X_RET_NO_INITED_ERROR          gh3x2x has not inited
 */

/**
 * @fn     GS8 GH3X2X_SendHbDumpDataPkg(GU8 *puchRespondBuffer, GU16 *pusRespondLen, GU8 *puchFifoRawdata,
                                        GS32 nAlgoResultArr[], STGsensorRawdata *pstGsAxisValueArr,
                                        GU16 usGsDataNum, EMGsensorSensitivity emGsSensitivity)
 *                  
 * @brief  pack GH3X2X hb rawdata and other type of data according to specific protocol
 *
 * @attention   None
 *
 * @param[out]  puchRespondBuffer       pointer to frame data package
 * @param[out]  pusRespondLen           pointer to data length of package
 * @param[in]   puchFifoRawdata         pointer to encrypted rawdata
 * @param[in]   nAlgoResultArr          pointer to algorithm result
 * @param[in]   pstGsAxisValueArr       pointer to g sensor data
 * @param[in]   usGsDataNum             number of g sensor data
 * @param[in]   emGsSensitivity         sensitivity of g sensor
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_GENERIC_ERROR           return func start error
 * @retval  #GH3X2X_RET_NO_INITED_ERROR         gh3x2x has not inited
 */







#endif /* _GH3X2X_DRV_DUMP_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
