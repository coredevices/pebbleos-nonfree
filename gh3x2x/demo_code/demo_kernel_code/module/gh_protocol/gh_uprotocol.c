/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_drv_uprotocol.c
 *
 * @brief   gh3x2x universal protocol functions
 *
 * @version ref gh3x2x_drv_version.h
 *
 */


#include <stdio.h>
#include "gh_drv_version.h"
#include "gh_drv_common.h"
#include "gh_drv.h"
#include "gh_uprotocol.h"
#include "gh_drv_interface.h"
#include "gh_drv_control.h"
#include "gh_drv_dump.h"
#include "gh_demo_config.h"
#include "gh_agc.h"

/// packet payload
//GU8 g_pstGh3x2xProtocolData->puchPacketPayloadArr[GH3X2X_UPROTOCOL_PAYLOAD_LEN_MAX] = {0};


// Zipmode Tempdata
#if 0
STZipLastDataTemp g_stZipTempData;
#endif


#define GH3X2X_PROTOCOL_VERSION_STRING "Health_Uprotocol_v0.4"
extern GU8 g_uchFifoPackageID;
extern GU8 g_uchFifoPackageMode;
extern GU8 g_uchResetFromProtocolFlag;

/// packet payload
GU8 g_uchPacketPayloadIndex = 0;

/// packet payload
GU8 g_uchPacketPayloadLen = 0;

/// packet payload
GU8 g_uchPacketCmd = 0;

/// packet crc8 val
GU8 g_uchPacketCalcCrc8Val = GH3X2X_CRC8_CLEAR_VAL;

/// packet payload
EMUprotocolParseStatus g_emPacketParseStatus = UPROTOCOL_STATUS_IDLE;

/// packet max len support
GU8 g_uchPacketMaxLenSupport = GH3X2X_UPROTOCOL_PACKET_LEN_MAX;

/// chip version string
GCHAR  g_szChipVerString[GH3X2X_CHIP_VER_STR_LEN] = {0};

/// get firmware version func ptr
static pfnGetFirmwareVersion g_pGh3x2xGetFirmwareVersionFunc = GH3X2X_PTR_NULL;
static pfnGetFirmwareVersion g_pGh3x2xGetDemoVersionFunc = GH3X2X_PTR_NULL;
static pfnGetFirmwareVersion g_pGh3x2xGetBootloaderVersionFunc = GH3X2X_PTR_NULL;

/// current data index in raw data buf
GU16 g_usGh3x2xDataIndexInBuf = 0;

GU8  g_uchGh3x2xUploadStatus = 0;
GU8  g_uchGh3x2xUploadBufUse = 0;

/// current data index in raw data buf of every channel
GU16 g_usRawdataByteIndexArr[GH3X2X_CHANNEL_MAP_MAX_CH];

/// current g sensor data index in g sensor data buf
GF32 g_fGsDataIndexInBuf = 0;

/// current algo result index
GU16 g_usAlgoResIndex = 0;

///frame id array
//GU8 g_uchFrameIdArr[CHANNEL_MAP_ID_NUM] = {0};

/// all function mode available
GU32 g_unAllFuncMode = 0;

/// already opened function mode
GU32 g_unFuncModeOpened = 0;

/// function mode will be closed or opened
GU32 g_unTargetFuncMode = 0;

/// LastIncompleteMark
GU32 g_unLastIncompleteMark = 0;

/// event need report
GU16 g_usGh3x2xEventNeedReport = 0;

/// event report id
GU8 g_uchEventReportId = 0;

/// received event report id
GU8 g_uchEventReportIdReceive = UPROTOCOL_REPORT_EVENT_ID_INIT;

/// crc8 table
static const GU8 g_uchCrc8TabArr[] = 
{
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

GCHAR *GH3X2X_GetProtocolVersion(void)
{
    return (GCHAR *)GH3X2X_PROTOCOL_VERSION_STRING;
}

/**
 * @fn     GCHAR *GH3X2X_GetChipVersion(void)
 *
 * @brief  Get chip version, only for protocol, format like v11_ev01(11: chip id, 01 efuse ver)
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  chip version string
 */
GCHAR *GH3X2X_GetChipVersion(void)
{
    GU8 uchChipVer = 0;
    GU8 uchEfuseVer = 0;

    GH3X2X_CHIP_WAKEUP_NORET();
    uchChipVer = GH3X2X_GET_LOW_BYTE_FROM_WORD(GH3X2X_ReadReg(GH3X2X_CHIP_ID_REG_ADDR));
    uchEfuseVer = GH3X2X_GET_LOW_BYTE_FROM_WORD(GH3X2X_ReadReg(GH3X2X_EFUSE_CTRL_EFUSE0_AUTOLOAD_3_ADDR));
    GH3X2X_CHIP_SLEEP_NORET();
    
    if(g_pSnprintfUserIn)
    {
        g_pSnprintfUserIn(g_szChipVerString, GH3X2X_CHIP_VER_STR_LEN, "v%.2x_ev%.2x\r\n", uchChipVer, uchEfuseVer);
        return (GCHAR *)g_szChipVerString;
    }
    return "no snprintf";
    
}

extern void GH3X2X_AlgoVersion(GU8 uchFunctionID, GCHAR version[150]);
/**
 * @fn     static void GH3X2X_UprotocolHandleGetVerCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle get version cmd
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
void GH3X2X_GetVersion(GU8 uchGetVersionType, GCHAR pszVersionString[150])
{
    //GCHAR *pszVersionString = GH3X2X_PTR_NULL;
    GH3X2X_Memcpy(pszVersionString, UPROTOCOL_GET_VER_DEFAULT_VER_STRING, GH3X2X_Strlen((const GCHAR *)UPROTOCOL_GET_VER_DEFAULT_VER_STRING) + 1);
    switch (uchGetVersionType)
    {
    case UPROTOCOL_GET_VER_TYPE_FW_VER:
        if (g_pGh3x2xGetFirmwareVersionFunc != GH3X2X_PTR_NULL)
        {
            GH3X2X_Memcpy(pszVersionString, g_pGh3x2xGetFirmwareVersionFunc(), GH3X2X_Strlen((const GCHAR *)g_pGh3x2xGetFirmwareVersionFunc()) + 1);
        }
        break;
    case UPROTOCOL_GET_VER_TYPE_DEMO_VER:
        if (g_pGh3x2xGetDemoVersionFunc != GH3X2X_PTR_NULL)
        {
            GH3X2X_Memcpy(pszVersionString, g_pGh3x2xGetDemoVersionFunc(), GH3X2X_Strlen((const GCHAR *)g_pGh3x2xGetDemoVersionFunc()) + 1);
        }
        break;
    case UPROTOCOL_GET_VER_TYPE_BOOTLOADER_VER:
        if (g_pGh3x2xGetBootloaderVersionFunc != GH3X2X_PTR_NULL)
        {
            GH3X2X_Memcpy(pszVersionString, g_pGh3x2xGetBootloaderVersionFunc(), GH3X2X_Strlen((const GCHAR *)g_pGh3x2xGetBootloaderVersionFunc()) + 1);
        }
        break;
    case UPROTOCOL_GET_VER_TYPE_FUNC_SUPPORT:
        break;
    case UPROTOCOL_GET_VER_TYPE_CHIP_VER:
        GH3X2X_Memcpy(pszVersionString, GH3X2X_GetChipVersion(), GH3X2X_Strlen((const GCHAR *)GH3X2X_GetChipVersion()) + 1);
        break;
    case UPROTOCOL_GET_VER_TYPE_PROTOCOL_VER:
        GH3X2X_Memcpy(pszVersionString, GH3X2X_GetProtocolVersion(), GH3X2X_Strlen((const GCHAR *)GH3X2X_GetProtocolVersion()) + 1);
        break;
    case UPROTOCOL_GET_VER_TYPE_VIRTUAL_REG_VER:
        GH3X2X_Memcpy(pszVersionString, GH3X2X_GetVirtualRegVersion(), GH3X2X_Strlen((const GCHAR *)GH3X2X_GetVirtualRegVersion()) + 1);
        break;
    case UPROTOCOL_GET_VER_TYPE_DRV_VER:
        GH3X2X_Memcpy(pszVersionString, GH3X2X_GetDriverLibVersion(), GH3X2X_Strlen((const GCHAR *)GH3X2X_GetDriverLibVersion()) + 1);
        break;
    default:
        //#if __GOODIX_ALGO_CALL_MODE__
        if (uchGetVersionType >= UPROTOCOL_GET_VER_TYPE_ALGO_VER && uchGetVersionType <= UPROTOCOL_GET_VER_TYPE_ALGO_VER_END)
        {
            GU32 unFunctionOffset = (GU32)(uchGetVersionType - UPROTOCOL_GET_VER_TYPE_ALGO_VER);
            GCHAR uchVersion[150] = {0};
            GH3X2X_AlgoVersion(unFunctionOffset, uchVersion);
            GU32 len = GH3X2X_Strlen((const GCHAR *)uchVersion) + 1;
            GH3X2X_Memcpy(pszVersionString, uchVersion, len);
        }
        //#endif
        break;
    }
}

/**
 * @fn     void  GH3X2X_CalcByteCrc8Val(GU8 *puchCrc8Val, GU8  uchNeedCalcByte)
 *
 * @brief  real time calc crc8 val, only for protocol,
 *
 * @attention   None
 *
 * @param[in]   uchNeedCalcByte      data need calc crc8
 * @param[out]  puchCrc8Val          pointer to crc8 output & last val input
 *
 * @return  crc8 value
 */
void  GH3X2X_CalcByteCrc8Val(GU8 *puchCrc8Val, GU8  uchNeedCalcByte)
{
    GU8 uchCrc8TabIndex;
    uchCrc8TabIndex = ((*puchCrc8Val) ^  uchNeedCalcByte) & GH3X2X_CRC8_CLEAR_VAL;
    SET_VAL_VIA_PTR(puchCrc8Val, g_uchCrc8TabArr[uchCrc8TabIndex]);
}

/**
 * @fn     GU8  GH3X2X_CalcArrayCrc8Val(GU8 uchDataArr[], GU16 usDataIndex, GU16 usDataLen)
 *
 * @brief  Get crc8 data, only for protocol
 *
 * @attention   None
 *
 * @param[in]   uchDataArr      pointer to data array
 * @param[in]   usDataIndex     data index need to calc crc8
 * @param[in]   usDataLen       data len need to calc crc8
 * @param[out]  None
 *
 * @return  crc8 value
 */
GU8  GH3X2X_CalcArrayCrc8Val(GU8 uchDataArr[], GU16 usDataIndex, GU16 usDataLen)
{
    GU8 uchCrc8TabIndex;
    GU8 uchCrc8 = GH3X2X_CRC8_CLEAR_VAL;
    GU16 usIndex = 0;
    for (usIndex = usDataIndex; usIndex < usDataLen; usIndex++)
    {
        uchCrc8TabIndex = (uchCrc8 ^ (uchDataArr[GH3X2X_GET_LOW_BYTE_FROM_WORD(usIndex)])) & GH3X2X_CRC8_CLEAR_VAL;
        uchCrc8 = g_uchCrc8TabArr[uchCrc8TabIndex];
    }
    return uchCrc8;
}

/**
 * @fn     GU8 GH3X2X_UprotocolPacketFormat(GU8 uchCmd, GU8 *puchPacketBuffer,
 *                                          GU8 *puchPayloadData, GU8 uchPayloadDataLen)
 *
 * @brief  universal protocol format packet
 *
 * @attention   None
 *
 * @param[out]   puchPacketBuffer    pointer to respond packet buffer
 * @param[in]    uchCmd              cmd id
 * @param[in]    puchPayloadData     pointer to payload buffer
 * @param[in]    uchPayloadDataLen   payload len
 *
 * @return  len after make packet
 */
GU8 GH3X2X_UprotocolPacketFormat(GU8 uchCmd, GU8 *puchPacketBuffer, GU8 *puchPayloadData, GU8 uchPayloadDataLen)
{
    GU8 uchPacketIndex = 0;
    GU8 uchPacketPayloadRealLen = GH3X2X_UPROTOCOL_GET_PAYLOAD_LEN_SUPPORT(g_uchPacketMaxLenSupport);

    /* fixed real len */
    if (uchPacketPayloadRealLen > uchPayloadDataLen)
    {
        uchPacketPayloadRealLen = uchPayloadDataLen;
    }

    puchPacketBuffer[GH3X2X_UPROTOCOL_PACKET_FIXED_INDEX] = GH3X2X_UPROTOCOL_FIXED_HEADER;
    puchPacketBuffer[GH3X2X_UPROTOCOL_PACKET_VER_INDEX]   = GH3X2X_UPROTOCOL_VERSION;
    puchPacketBuffer[GH3X2X_UPROTOCOL_PACKET_CMD_INDEX]   = uchCmd;
    puchPacketBuffer[GH3X2X_UPROTOCOL_PACKET_LEN_INDEX]   = uchPacketPayloadRealLen;
    uchPacketIndex += GH3X2X_UPROTOCOL_PACKET_HEADER_LEN;
    if (uchPacketPayloadRealLen != 0)
    {
        GH3X2X_Memcpy(&puchPacketBuffer[uchPacketIndex], puchPayloadData, uchPacketPayloadRealLen);
        uchPacketIndex += uchPacketPayloadRealLen;
    }
    puchPacketBuffer[uchPacketIndex] = GH3X2X_CalcArrayCrc8Val(puchPacketBuffer, 0,
                                uchPacketPayloadRealLen + GH3X2X_UPROTOCOL_PACKET_HEADER_LEN);
    uchPacketIndex++;

    return (uchPacketIndex);
}

#if (__SUPPORT_PROTOCOL_ANALYZE__)
/**
 * @fn     static GU8 GH3X2X_UprotocolParseBuffer(GU16 *pusHasParseLen, GU8 uchRecvDataBufferArr[], GU16 usDataLen)
 *
 * @brief  parse recv buffer data
 *
 * @attention   only support parse 1 cmd
 *
 * @param[in]   uchRecvDataBufferArr    pointer to data array
 * @param[in]   usDataLen               data length
 * @param[out]  pusHasParseLen           pointer to has parse len of buffer
 *
 * @return  got cmd or not
 * @retval  #GH3X2X_RET_GENERIC_ERROR   no cmd has parse done
 * @retval  #GH3X2X_RET_OK              a cmd has parse done
 */
static GS8 GH3X2X_UprotocolParseBuffer(GU16 *pusHasParseLen, GU8 uchRecvDataBufferArr[], GU16 usDataLen)
{
    GU16 usIndex  = 0;
    GS8 chRet = GH3X2X_RET_GENERIC_ERROR;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    for (usIndex = 0; usIndex < usDataLen; usIndex++)
    {
        switch (g_emPacketParseStatus)
        {
        case UPROTOCOL_STATUS_IDLE:
            if (uchRecvDataBufferArr[usIndex] == GH3X2X_UPROTOCOL_FIXED_HEADER)
            {
                GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
                g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_FIXED;
            }
            break;

        case UPROTOCOL_STATUS_GOT_FIXED:
            if (uchRecvDataBufferArr[usIndex] == GH3X2X_UPROTOCOL_VERSION)
            {
                GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
                g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_VER;
            }
            else
            {
                GH3X2X_CRC8_VAL_CLEAR(g_uchPacketCalcCrc8Val);
                g_emPacketParseStatus = UPROTOCOL_STATUS_IDLE;
            }
            break;

        case UPROTOCOL_STATUS_GOT_VER:
            g_uchPacketCmd = uchRecvDataBufferArr[usIndex];
            GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
            g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_CMD;
            break;

        case UPROTOCOL_STATUS_GOT_CMD:
            g_uchPacketPayloadIndex = 0;
            if (uchRecvDataBufferArr[usIndex] == 0)
            {
                g_uchPacketPayloadLen = 0;
                GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
                g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_PAYLOAD; // if payload len equl 0, goto verify crc8
            }
            else if (uchRecvDataBufferArr[usIndex] <= GH3X2X_UPROTOCOL_PAYLOAD_LEN_MAX)
            {
                g_uchPacketPayloadLen = uchRecvDataBufferArr[usIndex];
                GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
                g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_LEN;
            }
            else
            {
                GH3X2X_CRC8_VAL_CLEAR(g_uchPacketCalcCrc8Val);
                g_emPacketParseStatus = UPROTOCOL_STATUS_IDLE;
            }
            break;

        case UPROTOCOL_STATUS_GOT_LEN:
            g_pstGh3x2xProtocolData->puchPacketPayloadArr[g_uchPacketPayloadIndex] = uchRecvDataBufferArr[usIndex];
            g_uchPacketPayloadIndex++;
            GH3X2X_CalcByteCrc8Val(&g_uchPacketCalcCrc8Val, uchRecvDataBufferArr[usIndex]);
            if (g_uchPacketPayloadIndex >= g_uchPacketPayloadLen)
            {
                g_emPacketParseStatus = UPROTOCOL_STATUS_GOT_PAYLOAD;
            }
            break;

        case UPROTOCOL_STATUS_GOT_PAYLOAD:
            if (uchRecvDataBufferArr[usIndex] == g_uchPacketCalcCrc8Val)
            {
                chRet = GH3X2X_RET_OK;
            }
            GH3X2X_CRC8_VAL_CLEAR(g_uchPacketCalcCrc8Val);
            g_emPacketParseStatus = UPROTOCOL_STATUS_IDLE;
            break;

        default:
            GH3X2X_CRC8_VAL_CLEAR(g_uchPacketCalcCrc8Val);
            g_emPacketParseStatus = UPROTOCOL_STATUS_IDLE;
            GH3X2X_DEBUG_LOG_PARAM("goto default error, crc8:0x%x\r\n", g_uchPacketCalcCrc8Val);
            break;
        } // end switch (stParseStatus.ParseState)

        /* if parse cmd done, break */
        if (chRet == GH3X2X_RET_OK)
        {
            break;
        }
    } // end for (usIndex = 0; usIndex < usDataLen; usIndex++)

    SET_VAL_VIA_PTR(pusHasParseLen, (GU16)(usIndex + 1)); // set has parse length

    return chRet;
}

/**
 * @fn     static void GH3X2X_UprotocolHandleRegRwCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle reg rw cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleRegRwCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    if (g_uchPacketPayloadLen >= UPROTOCOL_REG_RW_HEADER_LENGTH)
    {
        GU8 uchIndex;
        GU16 usRegDataTmp = 0;
        GU8 uchRegRwMode =  g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_MODE_INDEX];
        GU8 uchRegRwLen =  g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_CNT_INDEX];
        GU16 usRegAddr = GH3X2X_MAKEUP_WORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_ADDR_H_INDEX],
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_ADDR_L_INDEX]);

        if (uchRegRwMode ==  UPROTOCOL_REG_RW_WRITE_MODE)
        {
            if ((uchRegRwLen > 0)
                && ((uchRegRwLen * GH3X2X_REG_ADDR_SIZE) == (g_uchPacketPayloadLen - UPROTOCOL_REG_RW_HEADER_LENGTH)))
            {
                switch (GH3X2X_GET_BIT_IS_SET(usRegAddr))
                {
                case GH3X2X_REG_IS_VIRTUAL0X0_BIT:
                    GH3X2X_CHIP_WAKEUP_PROTOCOL();
                    for (uchIndex = 0; uchIndex < uchRegRwLen; uchIndex++)
                    {
                        usRegDataTmp = \
                            GH3X2X_MAKEUP_WORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX \
                                                                    + (uchIndex * GH3X2X_REG_ADDR_SIZE)],
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + 1 \
                                                                    + (uchIndex * GH3X2X_REG_ADDR_SIZE)]);
                        GH3X2X_WriteReg(GH3X2X_GET_REG_REAL_ADRR(usRegAddr + (uchIndex * GH3X2X_REG_ADDR_SIZE)), \
                                        usRegDataTmp);
                        GU16 usReadRegVal = GH3X2X_ReadReg(GH3X2X_GET_REG_REAL_ADRR(usRegAddr + (uchIndex * GH3X2X_REG_ADDR_SIZE)));
                        if (usReadRegVal != usRegDataTmp)
                        {
                                GH3X2X_DEBUG_LOG_PARAM("reg verify error! addr:0x%.4x,w_val:0x%.4x,r_val:0x%.4x\r\n",
                                                                                GH3X2X_GET_REG_REAL_ADRR(usRegAddr + (uchIndex * GH3X2X_REG_ADDR_SIZE)),
                                                                                usRegDataTmp, usReadRegVal);
                        }
                    }
                    GH3X2X_CHIP_SLEEP_PROTOCOL();
                    break;                
                default:
                    for (uchIndex = 0; uchIndex < uchRegRwLen; uchIndex++)
                    {
                        usRegDataTmp = \
                            GH3X2X_MAKEUP_WORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX \
                                                                    + (uchIndex * GH3X2X_REG_ADDR_SIZE)],
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + 1 \
                                                                    + (uchIndex * GH3X2X_REG_ADDR_SIZE)]);
                        GH3X2X_WriteVirtualReg(usRegAddr + (uchIndex * GH3X2X_REG_ADDR_SIZE),
                                               usRegDataTmp);
                    }
                    break;
                }
            } // end of if ((uchRegRwLen > 0) && ...
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("reg rw write len error!\r\n");
            }

            /* respond packet format */
            SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr, g_uchPacketPayloadLen));
        } // end of if (uchRegRwMode ==  UPROTOCOL_REG_RW_WRITE_MODE)
        else if (uchRegRwMode ==  UPROTOCOL_REG_RW_READ_MODE)
        {
            if (uchRegRwLen > 0)
            {
                GU8 uchMaxRegLen = (GH3X2X_UPROTOCOL_GET_PAYLOAD_LEN_SUPPORT(g_uchPacketMaxLenSupport)
                                    - UPROTOCOL_REG_RW_HEADER_LENGTH) / GH3X2X_REG_ADDR_SIZE;
                GU16 usRegType = 0;
                if (uchRegRwLen > uchMaxRegLen)
                {
                    uchRegRwLen = uchMaxRegLen; // fixed max len
                    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_CNT_INDEX] = uchRegRwLen;
                }
                usRegType = GH3X2X_GET_BIT_IS_SET(usRegAddr);
                if(0xAAAA == usRegAddr)
                {
                    usRegType = GH3X2X_REG_IS_VIRTUAL0X0_BIT;
                }
                switch (usRegType)
                {
                case GH3X2X_REG_IS_VIRTUAL0X0_BIT:
                    GH3X2X_CHIP_WAKEUP_PROTOCOL();

                    if(0xAAAA == usRegAddr)
                    {
                        GH3X2X_ReadFifo(&(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX]), uchRegRwLen*2);
                    }
                    else
                    {
                        for (uchIndex = 0; uchIndex < uchRegRwLen; uchIndex++)
                        {
                            
                            usRegDataTmp = GH3X2X_ReadReg(GH3X2X_GET_REG_REAL_ADRR(usRegAddr\
                                                            + (uchIndex * GH3X2X_REG_ADDR_SIZE)));
                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + (uchIndex * GH3X2X_REG_ADDR_SIZE)] = \
                                                                        GH3X2X_GET_HIGH_BYTE_FROM_WORD(usRegDataTmp);
                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + 1 + (uchIndex * GH3X2X_REG_ADDR_SIZE)] = \
                                                                        GH3X2X_GET_LOW_BYTE_FROM_WORD(usRegDataTmp);
                        }
                    }
                    GH3X2X_CHIP_SLEEP_PROTOCOL();
                    break;
                default:
                #if GH3X2X_SUPPORT_READ_BACK_VIRTUAL_REG
                    for (uchIndex = 0; uchIndex < uchRegRwLen; uchIndex++)
                    {
                        usRegDataTmp = GH3X2X_ReadVirtualReg(usRegAddr\
                                                               + (uchIndex * GH3X2X_REG_ADDR_SIZE));
                        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + (uchIndex * GH3X2X_REG_ADDR_SIZE)] = \
                                                                    GH3X2X_GET_HIGH_BYTE_FROM_WORD(usRegDataTmp);
                        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REG_RW_DATA_INDEX + 1 + (uchIndex * GH3X2X_REG_ADDR_SIZE)] = \
                                                                    GH3X2X_GET_LOW_BYTE_FROM_WORD(usRegDataTmp);
                    }
                #endif
                    break;
                }
            } // end of if (uchRegRwLen > 0)
            else
            {
                GH3X2X_DEBUG_LOG_PARAM("reg rw read len error!\r\n");
            }

            /* respond packet format */
            SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                        g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_REG_RW_HEADER_LENGTH + uchRegRwLen * GH3X2X_REG_ADDR_SIZE));
        } // end of else if (uchRegRwMode ==  UPROTOCOL_REG_RW_WRITE_MODE)
    } // end if (g_uchPacketPayloadLen >= UPROTOCOL_REG_RW_HEADER_LENGTH)
    else
    {
        GH3X2X_DEBUG_LOG_PARAM("reg rw payload error!\r\n");
    }
}

/**
 * @fn     void GH3X2X_RegisterGetFirmwareVersionFunc(GCHAR* (*pGetFirmwareVersionFunc)(void))
 *
 * @brief  Register get firmware version function
 *
 * @attention   None
 *
 * @param[in]    pGetFirmwareVersionFunc    pointer to get firmware versionn function
 * @param[out]   None
 *
 * @return  None
 */
void GH3X2X_RegisterGetFirmwareVersionFunc(GCHAR* (*pGetFirmwareVersionFunc)(void),
                                           GCHAR* (*pGetDemoVersionFunc)(void),
                                           GCHAR* (*pGetBootloaderVersionFunc)(void))
{
    g_pGh3x2xGetFirmwareVersionFunc = pGetFirmwareVersionFunc;
    g_pGh3x2xGetDemoVersionFunc = pGetDemoVersionFunc;
    g_pGh3x2xGetBootloaderVersionFunc = pGetBootloaderVersionFunc;
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
}

static void GH3X2X_UprotocolHandleGetOpenedFuncCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_MAX_LEN_RESP_VAL_INDEX] = g_uchPacketMaxLenSupport;

    GU32 unFuncModeOpened = GH3X2X_GetFuncModeOpened();
    GU8 uchPayload[5];
    uchPayload[0] = 0;
    uchPayload[1] = GH3X2X_GET_BYTE3_FROM_DWORD(unFuncModeOpened);
    uchPayload[2] = GH3X2X_GET_BYTE2_FROM_DWORD(unFuncModeOpened);
    uchPayload[3] = GH3X2X_GET_BYTE1_FROM_DWORD(unFuncModeOpened);
    uchPayload[4] = GH3X2X_GET_BYTE0_FROM_DWORD(unFuncModeOpened);

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                uchPayload, 5));
}

static void GH3X2X_UprotocolHandleGetVerCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchGetVersionType = g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_VER_TYPE_INDEX];
    GCHAR pszVersionString[150] = {0};
    GU8 uchVersionStringLen = 0;
    if (g_uchPacketPayloadLen == 0)
    {
        uchGetVersionType = UPROTOCOL_GET_VER_TYPE_FW_VER; // fixed to get firmware version
    }
    GH3X2X_GetVersion(uchGetVersionType, (GCHAR*)pszVersionString);
    uchVersionStringLen = (GU8)GH3X2X_Strlen(pszVersionString);
    if (uchVersionStringLen > (GH3X2X_UPROTOCOL_GET_PAYLOAD_LEN_SUPPORT(g_uchPacketMaxLenSupport) \
                                   - UPROTOCOL_GET_VER_STRING_INDEX))
    {
        uchVersionStringLen = GH3X2X_UPROTOCOL_GET_PAYLOAD_LEN_SUPPORT(g_uchPacketMaxLenSupport) \
                                   - UPROTOCOL_GET_VER_STRING_INDEX; // fixed max len
    }
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_VER_TYPE_INDEX] = uchGetVersionType;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_VER_STRING_LEN_INDEX] = uchVersionStringLen;
    GH3X2X_Memcpy((GCHAR *)&g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_VER_STRING_INDEX],
                            pszVersionString, uchVersionStringLen);

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_GET_VER_STRING_INDEX + uchVersionStringLen));
}

/**
 * @fn     static void GH3X2X_UprotocolHandleFifoThrConfig(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle fifo thr config cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleFifoThrConfig(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    if (g_uchPacketPayloadLen == GH3X2X_REG_ADDR_SIZE)
    {
        GH3X2X_CHIP_WAKEUP_NORET();
        GH3X2X_FifoWatermarkThrConfig(GH3X2X_MAKEUP_WORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FIFO_THR_CFG_H_INDEX],
                                      g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FIFO_THR_CFG_L_INDEX]));
        GH3X2X_CHIP_SLEEP_NORET();
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FIFO_THR_CFG_RESP_STATUS_INDEX] = UPROTOCOL_FIFO_THR_CFG_RESP_CFG_SUCCESS;
    }
    else
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FIFO_THR_CFG_RESP_STATUS_INDEX] = UPROTOCOL_FIFO_THR_CFG_RESP_CFG_FAIL;
    }

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_FIFO_THR_CFG_RESP_LEN));
}

STAgcRegConfig g_stAgcRegConfig;
/**
 * @fn     static void GH3X2X_UprotocolHandleChipCtrl(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle chip ctrl cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleChipCtrl(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    if (g_uchPacketPayloadLen == 1)
    {
        GU8 uchCtrlType = g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_VAL_INDEX];
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_NULL;
        switch (uchCtrlType)
        {
        case UPROTOCOL_CHIP_CTRL_TYPE_HARD_RESET:
#if __SUPPORT_SOFT_AGC_CONFIG__
            GH3x2xAgcRegvalReset(&g_stAgcRegConfig);
#endif
            g_uchResetFromProtocolFlag = 1;
            if (GH3X2X_HardReset() == GH3X2X_RET_OK)
            {
                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_SUCCESS;
            }
            break;
        case UPROTOCOL_CHIP_CTRL_TYPE_SOFT_RESET:
#if __SUPPORT_SOFT_AGC_CONFIG__
            GH3x2xAgcRegvalReset(&g_stAgcRegConfig);
#endif
            g_uchResetFromProtocolFlag = 1;
            if (GH3X2X_SoftReset() == GH3X2X_RET_OK)
            {
                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_SUCCESS;
            }
            break;

#if (GH3X2X_UPROTOCOL_CHIP_CTRL_CMD_WAKEUP_SLEEP_SUP)
        case UPROTOCOL_CHIP_CTRL_TYPE_WAKEUP:
            if (GH3X2X_ExitLowPowerMode() == GH3X2X_RET_OK)
            {
                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_SUCCESS;
            }
            break;

        case UPROTOCOL_CHIP_CTRL_TYPE_SLEEP:
            if (GH3X2X_EnterLowPowerMode() == GH3X2X_RET_OK)
            {
                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_SUCCESS;
            }
            break;
#endif

        default:
            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_FAIL;
            break;
        } // end of switch (uchCtrlType)
    } // end of if (g_uchPacketPayloadLen == 1)
    else
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_CTRL_RESP_STATUS_INDEX] = UPROTOCOL_CHIP_CTRL_RESP_CFG_FAIL;
    }

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_CHIP_CTRL_RESP_LEN));
}

/**
 * @fn     static void GH3X2X_UprotocolHandleGetSupprtLenCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle get max len support cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleGetSupprtLenCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_GET_MAX_LEN_RESP_VAL_INDEX] = g_uchPacketMaxLenSupport;

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_GET_MAX_LEN_RESP_LEN));
}

/**
 * @fn     static void GH3X2X_UprotocolHandleSwFunctionCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle switch slot and function cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static EMUprotocolParseCmdType GH3X2X_UprotocolHandleSwFunctionCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    EMUprotocolParseCmdType emUprotocolCmdType = UPROTOCOL_CMD_IGNORE;
        
    g_unTargetFuncMode = GH3X2X_MAKEUP_DWORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_FUNC_INDEX + 3], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_FUNC_INDEX + 2], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_FUNC_INDEX + 1], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_FUNC_INDEX]);

    if (g_unAllFuncMode == (g_unTargetFuncMode | g_unAllFuncMode))
    {
        if (UPROTOCOL_START_SAMPLING == g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_ONOFF_INDEX])
        {
            g_unFuncModeOpened |= g_unTargetFuncMode;
            emUprotocolCmdType = UPROTOCOL_CMD_START;
        }
        else
        {
            g_unFuncModeOpened &= (~g_unTargetFuncMode);
            emUprotocolCmdType = UPROTOCOL_CMD_STOP;
        }
        
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_RESP_INDEX] = UPROTOCOL_SW_FUNC_SUCCESS;
    }
    else
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_SW_FUNC_RESP_INDEX] = UPROTOCOL_SW_FUNC_FAIL;
    }   
    
    GH3X2X_DEBUG_LOG_PARAM("[%s]:g_unTargetFuncMode = 0x%08x\r\n", __FUNCTION__, g_unTargetFuncMode);

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_SW_FUNC_RESP_LEN));
    return emUprotocolCmdType;
}

/**
 * @fn     static void GH3X2X_UprotocolHandleLoadRegListCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle load reg list cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleLoadRegListCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchReglistCnt = g_uchPacketPayloadLen / UPROTOCOL_LOAD_REG_LIST_SIZEOF;
    STGh3x2xReg *pstReglist = (STGh3x2xReg *)g_pstGh3x2xProtocolData->puchPacketPayloadArr;
    GH3X2X_SwapLittleBigEndianData(g_pstGh3x2xProtocolData->puchPacketPayloadArr, g_uchPacketPayloadLen);
    GH3X2X_CHIP_WAKEUP_PROTOCOL();
    if (GH3X2X_LoadNewRegConfigArr(pstReglist, uchReglistCnt, 0, &g_stAgcRegConfig) == GH3X2X_RET_OK)
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_LOAD_REG_LIST_RESP_STATUS_INDEX] = \
                                                                UPROTOCOL_LOAD_REG_LIST_RESP_CFG_SUCCESS;
    }
    else
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_LOAD_REG_LIST_RESP_STATUS_INDEX] = UPROTOCOL_LOAD_REG_LIST_RESP_CFG_FAIL;
    }
    GH3X2X_CHIP_SLEEP_PROTOCOL();

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_LOAD_REG_LIST_RESP_LEN));
}

/**
 * @fn     static void GH3X2X_UprotocolHandleDebugStatusCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle debug status cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleDebugStatusCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchDebugType = g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_TYPE_INDEX];
    GU8 uchBytesLen = UPROTOCOL_DEBUG_STATUS_RESP_BYTES_8;
    GH3X2X_Memset(&g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_BYTES_INDEX], 0,
                                                                        UPROTOCOL_DEBUG_STATUS_RESP_LEN - 1);
    switch (uchDebugType)
    {
    case UPROTOCOL_DEBUG_STATUS_READ_INT_PIN:
        READ_PIN_STATUS_CALL(g_pGh3x2xReadIntPinStatusFunc,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX]);
        break;
    case UPROTOCOL_DEBUG_STATUS_READ_RST_PIN:
        READ_PIN_STATUS_CALL(g_pGh3x2xReadResetPinStatusFunc,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX]);
        break;
    case UPROTOCOL_DEBUG_STATUS_READ_SPCS_PIN:
        READ_PIN_STATUS_CALL(g_pGh3x2xReadSpcsPinStatusFunc,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX]);
        break;
    case UPROTOCOL_DEBUG_STATUS_READ_SPDO_PIN:
        READ_PIN_STATUS_CALL(g_pGh3x2xReadSpdoPinStatusFunc,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX]);
        break;
    case UPROTOCOL_DEBUG_STATUS_COMM_CONFIRM:
        GH3X2X_CHIP_WAKEUP_NORET();
        if (GH3X2X_CommunicateConfirm() == GH3X2X_RET_OK)
        {
            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX] = 0x01;
        }
        GH3X2X_CHIP_SLEEP_NORET();
        break;

    default: // include UPROTOCOL_DEBUG_STATUS_READ_WORK_STATUS
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_DATA_INDEX] = g_uchGh3x2xStatus;
        break;
    } // end of switch (uchDebugType)
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_DEBUG_STATUS_RESP_BYTES_INDEX] = uchBytesLen;

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_DEBUG_STATUS_RESP_LEN));
}

/**
 * @fn     static EMUprotocolParseCmdType GH3X2X_UprotocolHandleStartCtrlCmd(GU8 *puchRespondBuffer,
 *                              GU16 *pusRespondLen)
 *
 * @brief  handle start ctrl cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  @ref EMUprotocolParseCmdType
 */
static EMUprotocolParseCmdType GH3X2X_UprotocolHandleStartCtrlCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    EMUprotocolParseCmdType emUprotocolCmdType = UPROTOCOL_CMD_IGNORE;
    g_unTargetFuncMode = GH3X2X_MAKEUP_DWORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_FUNCTION_INDEX + 3], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_FUNCTION_INDEX + 2], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_FUNCTION_INDEX + 1], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_FUNCTION_INDEX]);

    if (g_unAllFuncMode == (g_unTargetFuncMode | g_unAllFuncMode))
    {
        if (UPROTOCOL_START_SAMPLING == g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_INDEX])
        {
            g_unFuncModeOpened |= g_unTargetFuncMode;
            emUprotocolCmdType = UPROTOCOL_CMD_START;
        }
        else
        {
            g_unFuncModeOpened &= (~g_unTargetFuncMode);
            emUprotocolCmdType = UPROTOCOL_CMD_STOP;
        }
        
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_RESP_INDEX] = UPROTOCOL_START_CTRL_SUCCESS;
    }
    else
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_START_CTRL_RESP_INDEX] = UPROTOCOL_START_CTRL_FAIL;
    }   

    GH3X2X_DEBUG_LOG_PARAM("[%s]:g_unTargetFuncMode = 0x%08x, emUprotocolCmdType = %d\r\n", __FUNCTION__, g_unTargetFuncMode, emUprotocolCmdType);

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_START_CTRL_RESP_LEN));
    return emUprotocolCmdType;
}

/**
 * @fn     static EMUprotocolParseCmdType GH3X2X_UprotocolHandleWorkModeCmd(GU8 *puchRespondBuffer,
 *              GU16 *pusRespondLen)
 *
 * @brief  handle work mode cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  @ref EMUprotocolParseCmdType
 */
static EMUprotocolParseCmdType GH3X2X_UprotocolHandleWorkModeCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    EMUprotocolParseCmdType emUprotocolCmdType = UPROTOCOL_CMD_IGNORE;

    emUprotocolCmdType = (EMUprotocolParseCmdType)(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_WORK_MODE_INDEX] + \
                                                    UPROTOCOL_CMD_EVK_MODE);

    g_unAllFuncMode = GH3X2X_MAKEUP_DWORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FUNC_MODE_INDEX + 3], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FUNC_MODE_INDEX + 2], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FUNC_MODE_INDEX + 1], \
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_FUNC_MODE_INDEX]);

    GH3X2X_DEBUG_LOG_PARAM("[%s]:work mode = %d, function_enable = 0x%08x\r\n", __FUNCTION__, g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_WORK_MODE_INDEX], (int)g_unAllFuncMode);

    g_unFuncModeOpened = 0;
    g_unTargetFuncMode = 0;

    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_WORK_MODE_RESP_INDEX] = UPROTOCOL_WORK_MODE_SET_SUCCESS;

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer, \
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_WORK_MODE_RESP_LEN));

    return emUprotocolCmdType;
}

/**
 * @fn     static void GH3X2X_UprotocolHandleReportEventSetCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle event report set cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleReportEventSetCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    g_usGh3x2xEventNeedReport = GH3X2X_MAKEUP_WORD(g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REPORT_EVENT_SET_INDEX], \
                                                    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REPORT_EVENT_SET_INDEX + 1]);

    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REPORT_EVENT_SET_RESP_INDEX] = UPROTOCOL_REPORT_EVENT_SET_SUCCESS;

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_REPORT_EVENT_SET_RESP_LEN));
}

/**
 * @fn     static void GH3X2X_UprotocolPackageTestCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle test package
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolPackageTestCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchPackageLen = g_pstGh3x2xProtocolData->puchPacketPayloadArr[0];

    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, uchPackageLen));
}

/**
 * @fn     static void GH3X2X_UprotocolEcgClassificationCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle test package
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
extern GU8 GH3x2xEcgAlgoClassification(void);
static void GH3X2X_UprotocolEcgClassificationCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    //if (g_pstGh3x2xProtocolData->puchPacketPayloadArr[0] == 1)
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CONN_ECG_CLASSIFICATION_INDEX] = GH3x2xEcgAlgoClassification();
        /* respond packet format */
        SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                    g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_CONN_ECG_CLASSIFICATION_LEN));
    }
    //else if (puchRespondBuffer[0] == 2)
    {
        
    }
}

static void GH3X2X_UprotocolEcgCascadeCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchOpCmd = g_pstGh3x2xProtocolData->puchPacketPayloadArr[0];
    switch (uchOpCmd)
    {
        //op0x01: set ecg cascade en; 1: enable, 0 :disable
        case 0x01:
            //GH3X2X_CascadeSetEcgEnFlag(g_pstGh3x2xProtocolData->puchPacketPayloadArr[1]);
            GH3X2X_DEBUG_LOG_PARAM("pc set g_uchEcgCascadeEn:%d\r\n",GH3X2X_CascadeGetEcgEnFlag());
            if(g_pstGh3x2xProtocolData->puchPacketPayloadArr[1])
            {
                Gh3x2xEnterCascadeMode();
            }
            else
            {
                Gh3x2xExitCascadeMode();
            }
            break;
        case 0x02:
            GH3X2X_CascadeSetLogicMasterChip(0);   //chip0
            break;
        case 0x03:
            GH3X2X_CascadeSetLogicMasterChip(1);   //chip1
            break;
        default:
            break;
    }

    /* respond packet format */
//    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,\
//                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_CONN_ECG_CLASSIFICATION_LEN));
}

/**
 * @fn     static void GH3X2X_UprotocolFpbpDataReceiveCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle test package
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
void GH3X2X_UprotocolFpbpDataReceiveCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    //GH3x2xBpAlgoInitSetting(g_pstGh3x2xProtocolData->puchPacketPayloadArr);

    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_REPORT_EVENT_SET_RESP_INDEX] = UPROTOCOL_REPORT_EVENT_SET_SUCCESS;
    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_WORK_MODE_RESP_LEN));
}

/**
 * @fn     static EMUprotocolParseCmdType GH3X2X_UprotocolHandleEventReportAckCmd(GU8 *puchRespondBuffer,
 *                                                  GU16 *pusRespondLen)
 *
 * @brief  handle event report cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static EMUprotocolParseCmdType GH3X2X_UprotocolHandleEventReportAckCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    if(Gh3x2xGetProtocolEventReportId() == g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_EVENT_REPORT_ACK_INDEX])
    {
        Gh3x2xSetProtocolEventAck();
    }
    SET_VAL_VIA_PTR(pusRespondLen, 0);
    return UPROTOCOL_CMD_IGNORE;
}

extern void Gh3x2xSetProtocolEventAck(void);
extern GU8 Gh3x2xGetProtocolEventReportId(void);

/**
 * @fn     static void GH3X2X_UprotocolHandleChipSetConnStaCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle get chipset connect status cmd
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  None
 */
static void GH3X2X_UprotocolHandleChipSetConnStaCmd(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    GU8 uchRet = UPROTOCOL_CONN_STA_FAIL;

    GH3X2X_CHIP_WAKEUP_NORET();
    if (GH3X2X_RET_OK == GH3X2X_CommunicateConfirm())
    {
        uchRet = UPROTOCOL_CONN_STA_SUCCESS;
    }
    GH3X2X_CHIP_SLEEP_NORET();
    
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CONN_STA_RESP_INDEX] = uchRet;
    
    /* respond packet format */
    SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_CONN_STA_RESP_LEN));
}

/**
 * @fn     EMUprotocolParseCmdType GH3X2X_UprotocolCmdHandler(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
 *
 * @brief  handle all cmds
 *
 * @attention   None
 *
 * @param[in]    None
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 *
 * @return  @ref EMUprotocolParseCmdType
 */
EMUprotocolParseCmdType GH3X2X_UprotocolCmdHandler(GU8 *puchRespondBuffer, GU16 *pusRespondLen)
{
    EMUprotocolParseCmdType emUprotocolCmdType = UPROTOCOL_CMD_IGNORE;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    switch (g_uchPacketCmd)
    {
    case GH3X2X_UPROTOCOL_CMD_REG_RW:
        GH3X2X_UprotocolHandleRegRwCmd(puchRespondBuffer, pusRespondLen);
        break;

    case GH3X2X_UPROTOCOL_CMD_GET_VER:
        GH3X2X_UprotocolHandleGetVerCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_GET_OPENED_FUNC:
        GH3X2X_UprotocolHandleGetOpenedFuncCmd(puchRespondBuffer, pusRespondLen);
        break;
    // case GH3X2X_UPROTOCOL_CMD_LOAD_CONFIG:
    //     break;
    case GH3X2X_UPROTOCOL_CMD_START_CTRL:
        emUprotocolCmdType = GH3X2X_UprotocolHandleStartCtrlCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_WORK_MODE:
        emUprotocolCmdType = GH3X2X_UprotocolHandleWorkModeCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_REPORT_EVENT_SET:
        GH3X2X_UprotocolHandleReportEventSetCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_GET_CHIPSET_CONN_STA:
        GH3X2X_UprotocolHandleChipSetConnStaCmd(puchRespondBuffer, pusRespondLen);
        break;
#if (GH3X2X_UPROTOCOL_FIFO_THR_CFG_CMD_SUP)
    case GH3X2X_UPROTOCOL_CMD_FIFO_THR_CONFIG:
        GH3X2X_UprotocolHandleFifoThrConfig(puchRespondBuffer, pusRespondLen);
        break;
#endif

    // case GH3X2X_UPROTOCOL_CMD_FUNC_MAP:
    //     break;
    case GH3X2X_UPROTOCOL_CMD_CHIP_EVENT_REPORT:
        emUprotocolCmdType = GH3X2X_UprotocolHandleEventReportAckCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_CHIP_CTRL:
        GH3X2X_UprotocolHandleChipCtrl(puchRespondBuffer, pusRespondLen);
        break;

    case GH3X2X_UPROTOCOL_CMD_GET_MAX_LEN:
        GH3X2X_UprotocolHandleGetSupprtLenCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_SWITCH_SLOT_FUNC:
        emUprotocolCmdType = GH3X2X_UprotocolHandleSwFunctionCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_LOAD_REG_LIST:
        GH3X2X_UprotocolHandleLoadRegListCmd(puchRespondBuffer, pusRespondLen);
        break;

    case GH3X2X_UPROTOCOL_CMD_DEBUG_STATUS:
        GH3X2X_UprotocolHandleDebugStatusCmd(puchRespondBuffer, pusRespondLen);
        break;

    case GH3X2X_UPROTOCOL_CMD_PACKAGE_TEST:
        GH3X2X_UprotocolPackageTestCmd(puchRespondBuffer, pusRespondLen);
        break;

    case GH3X2X_UPROTOCOL_CMD_ECG_CLASSIFICATION:
        GH3X2X_UprotocolEcgClassificationCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_SET_ECG_CASCADE:
        GH3X2X_UprotocolEcgCascadeCmd(puchRespondBuffer, pusRespondLen);
        break;
    case GH3X2X_UPROTOCOL_CMD_BP_DATA_TRANS:
        GH3X2X_UprotocolFpbpDataReceiveCmd(puchRespondBuffer, pusRespondLen);
        break;

    default:
        GH3X2X_DEBUG_LOG_PARAM("unsupported cmd error!\r\n");

        /* respond packet format */
        SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(g_uchPacketCmd, puchRespondBuffer,
                                            g_pstGh3x2xProtocolData->puchPacketPayloadArr, 0));
        break;
    } // end of switch (g_uchPacketCmd)
    return emUprotocolCmdType;
}

/**
 * @fn     EMUprotocolParseCmdType GH3X2X_UprotocolParseHandler(GU8 *puchRespondBuffer, GU16 *pusRespondLen,
 *                                                     GU8 *puchRecvDataBuffer, GU16 usRecvLen)
 *
 * @brief  universal protocol parse handler, parse protocol receive data
 *
 * @attention   None
 *
 * @param[out]   puchRespondBuffer    pointer to respond buffer
 * @param[out]   pusRespondLen        pointer to respond length
 * @param[in]    puchRecvDataBuffer   pointer to receive data buffer
 * @param[in]    usRecvLen            receive data len
 *
 * @return  @ref EMUprotocolParseCmdType
 */
EMUprotocolParseCmdType GH3X2X_UprotocolParseHandler(GU8 *puchRespondBuffer, GU16 *pusRespondLen,
                                                     GU8 *puchRecvDataBuffer, GU16 usRecvLen)
{
    EMUprotocolParseCmdType emUprotocolCmdType = UPROTOCOL_CMD_IGNORE;
    GU16 usParseIndex = 0;
    GU16 usHasParseLenOfBuffer = 0;
    GU16 usRespondIndex = 0;
    GU16 usCmdRespondLen = 0;

    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);
    while (usParseIndex < usRecvLen)
    {
        if (GH3X2X_UprotocolParseBuffer(&usHasParseLenOfBuffer, &puchRecvDataBuffer[usParseIndex],
                                        usRecvLen - usParseIndex) == GH3X2X_RET_OK)
        {
            emUprotocolCmdType = GH3X2X_UprotocolCmdHandler(&puchRespondBuffer[usRespondIndex], &usCmdRespondLen);
            usRespondIndex += usCmdRespondLen;
        }
        else
        {
            if (g_emPacketParseStatus == UPROTOCOL_STATUS_IDLE)
            {
                GH3X2X_DEBUG_LOG_PARAM("protocol parse buffer error!");
            }
        }
        usParseIndex += usHasParseLenOfBuffer;
    } // end while (usParseIndex < usRecvLen)

    SET_VAL_VIA_PTR(pusRespondLen, usRespondIndex);

    return emUprotocolCmdType;
}

/**
 * @fn     GU8 GH3X2X_UprotocolPacketMaxLenConfig(GU8 uchPacketMaxLen)
 *
 * @brief  packet max len support, default len is 243
 *
 * @attention   uchPacketMaxLen val must at [20, 243]
 *
 * @param[out]   None
 * @param[in]    uchPacketMaxLen      packet max len
 *
 * @return  packet max len after config
 */
GU8 GH3X2X_UprotocolPacketMaxLenConfig(GU8 uchPacketMaxLen)
{
    GH3X2X_DEBUG_LOG_PARAM("%s\r\n", __FUNCTION__);

    if (uchPacketMaxLen < GH3X2X_UPROTOCOL_PACKET_LEN_MIN)
    {
        g_uchPacketMaxLenSupport = GH3X2X_UPROTOCOL_PACKET_LEN_MIN;
    }
    else if (uchPacketMaxLen > GH3X2X_UPROTOCOL_PACKET_LEN_MAX)
    {
        g_uchPacketMaxLenSupport = GH3X2X_UPROTOCOL_PACKET_LEN_MAX;
    }
    else
    {
        g_uchPacketMaxLenSupport = uchPacketMaxLen;
    }

    return g_uchPacketMaxLenSupport;
}

void GH3x2xSendFunctionInfo(const STGh3x2xFrameInfo * const pstFrameInfo)
{
    if ((*(pstFrameInfo->punFrameCnt)) == 0)
    {
        GU8 uchFunctionInfoBufferLen = 0;
        GU8 *puchUploadBuf = g_pstGh3x2xProtocolData->puchPacketPayloadArr + 4;
        GU8 uchFunctionIDForMaster = 0;

        for (int uchOffsetCnt = 0 ; uchOffsetCnt < GH3X2X_FUNC_OFFSET_MAX ; uchOffsetCnt ++)
        {
            if ((pstFrameInfo->unFunctionID >> uchOffsetCnt) & 0x1)
            {
                uchFunctionIDForMaster = uchOffsetCnt;
            }
        }
        puchUploadBuf[uchFunctionInfoBufferLen] = uchFunctionIDForMaster;
        uchFunctionInfoBufferLen += 1;

        GH3X2X_Memcpy(&puchUploadBuf[uchFunctionInfoBufferLen], pstFrameInfo->pstFunctionInfo, sizeof(STGh3x2xFunctionInfo));
        uchFunctionInfoBufferLen += sizeof(STGh3x2xFunctionInfo);

        GH3X2X_Memcpy(&puchUploadBuf[uchFunctionInfoBufferLen], pstFrameInfo->pchChnlMap, pstFrameInfo->pstFunctionInfo->uchChnlNum * sizeof(GU8));
        uchFunctionInfoBufferLen += pstFrameInfo->pstFunctionInfo->uchChnlNum * sizeof(GU8);
        
        GU16 usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_FUNCTION_INFO_UPDATE, puchUploadBuf - 4,
                            puchUploadBuf, uchFunctionInfoBufferLen);
        Gh3x2xDemoSendProtocolData(puchUploadBuf, usRespondLen);
    }
}

void Gh2x2xPackPakcageHeader(GU8 *puchNeedContinue, GU8 uchFunctionIDForMaster, GU8 *puchUploadBuf, GU8 *puchTagArray,
                                            GU8 uchGsEnable, GU8 uchAlgoResFlag, GU8 uchAgcEnable, GU8 uchAmbEnable, GU8 uchGsGyroEnable,GU8 uchCapEnable,GU8 uchTempEnable,
                                            GU8 uchZipEnableFlag, GU8 uchOddEvenChangeFlag)
{
    *puchNeedContinue = 0;
    g_uchGh3x2xUploadBufUse = 0;
    g_uchGh3x2xUploadStatus = GH3X2X_UPLOAD_STATUS_HAVE_STORE_DATA;

    //section DATA_TYPE
    puchUploadBuf[UPROTOCOL_FUNCTION_ID_INDEX] = uchFunctionIDForMaster;
    g_uchGh3x2xUploadBufUse ++;
    puchUploadBuf[UPROTOCOL_RAWDATA_TYPE_INDEX] = \
                                            GH3X2X_VAL_LEFT_SHIFT(uchGsEnable, UPROTOCOL_GS_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchAlgoResFlag, UPROTOCOL_ALGO_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchAgcEnable, UPROTOCOL_AGC_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchAmbEnable, UPROTOCOL_AMBIANCE_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchGsGyroEnable,UPROTOCOL_GS_GYRO_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchCapEnable,UPROTOCOL_CAP_ENABLE_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchTempEnable,UPROTOCOL_TEMP_ENABLE_ENABLE_FIELD);

    g_uchGh3x2xUploadBufUse ++;

    GU32 unCompeletMask = 0;
    for (GU32 unCnt = 0 ; unCnt < 32 ; unCnt ++)
    {
        if (puchTagArray[unCnt] != 0xFF)
        {
            unCompeletMask |= (1 << unCnt);
        }
    }

    //chnl num
    puchUploadBuf[UPROTOCOL_RAWDATA_CHNL_NUM] = (unCompeletMask >> 24) & 0xFF;
    puchUploadBuf[UPROTOCOL_RAWDATA_CHNL_NUM + 1] = (unCompeletMask >> 16) & 0xFF;
    puchUploadBuf[UPROTOCOL_RAWDATA_CHNL_NUM + 2] = (unCompeletMask >> 8) & 0xFF;
    puchUploadBuf[UPROTOCOL_RAWDATA_CHNL_NUM + 3] = (unCompeletMask) & 0xFF;
    g_uchGh3x2xUploadBufUse += 4;

    //pack type flag
    puchUploadBuf[UPROTOCOL_PACKAGE_TYPE_INDEX] = \
                                            GH3X2X_VAL_LEFT_SHIFT(uchZipEnableFlag, UPROTOCOL_ZIP_ENABLE_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(uchOddEvenChangeFlag, UPROTOCOL_ODDEVEN_FLAG_FIELD)
                                            | GH3X2X_VAL_LEFT_SHIFT(GH3X2X_GetFifoPackageMode(), UPROTOCOL_FUNCTION_MODE_FIELD);
    g_uchGh3x2xUploadBufUse ++;

    //section DATA_LEN
    puchUploadBuf[UPROTOCOL_RAWDATA_LEN_INDEX] = 0;
    g_uchGh3x2xUploadBufUse ++;
}

void Gh2x2xUploadDataToMaster(const STGh3x2xFrameInfo * const pstFrameInfo, GU16 usFrameCnt, GU16 usFrameNum, GU8* puchTagArray)
{
    GU8  uchChnlNum;
    GU32 unFunctionID;
    GU8  uchFunctionIDForMaster = 0xFF;
    GU8  uchGsEnable;
    GU8  uchGsGyroEnable;
    GU8  uchCapEnable;
    GU8  uchTempEnable;
    GU8  uchAgcEnable;
    GU8  uchAmbEnable;
    GU8  uchAlgoResFlag;
    GU16 uchOneFrameByteNum;
    GU8  uchReslutByteNum;
    GU8  uchNeedContinue;
    GU8  *puchUploadBuf = g_pstGh3x2xProtocolData->puchPacketPayloadArr + 4;
    GU16 usRespondLen;
    GU8 uchFramePosi;
    if(0 == pstFrameInfo)
    {
        return;
    }
    unFunctionID = pstFrameInfo->unFunctionID;
    uchChnlNum = pstFrameInfo->pstFunctionInfo->uchChnlNum;
    uchFramePosi = 0;
    if(0 == usFrameCnt)
    {
        uchFramePosi |= GH3X2X_FRAME_POSI_HEAD;
    }
    if((usFrameNum - 1) == usFrameCnt)
    {
        uchFramePosi |= GH3X2X_FRAME_POSI_TAIL;
    }
    if((GH3X2X_FUNCTION_SOFT_ADT_GREEN | GH3X2X_FUNCTION_SOFT_ADT_IR) & unFunctionID)
    {
        //return;
    }

    //function id transform
    for (int uchOffsetCnt = 0 ; uchOffsetCnt < GH3X2X_FUNC_OFFSET_MAX ; uchOffsetCnt ++)
    {
        if ((unFunctionID >> uchOffsetCnt) & 0x1)
        {
            uchFunctionIDForMaster = uchOffsetCnt;
        }
    }

    if(0xFF == uchFunctionIDForMaster)  //invalid function id
    {
        return;
    }
    uchGsEnable = g_uchGsensorEnable;
    uchGsGyroEnable = (g_uchGyroEnable & uchGsEnable);
    uchCapEnable =  GH3X2X_GetCapEnableFlag();
    uchTempEnable = GH3X2X_GetTempEnableFlag();
    uchAgcEnable = 1;
    if(unFunctionID == GH3X2X_FUNC_OFFSET_ECG)
    {
        uchAgcEnable = 0;
    }
    uchAmbEnable = 0;
    uchAlgoResFlag = 1;

    /********  calculate how many bytes in one frame  ******/
    uchOneFrameByteNum = 0;
    //section FRAME_ID
    uchOneFrameByteNum += 1;
    //section GSENSOR_DATA
    if(uchGsEnable)
    {
        uchOneFrameByteNum += 6;
    }
    if(uchGsEnable && uchGsGyroEnable)
    {
        uchOneFrameByteNum += 6;
    }

    //section Cap and temp
    if(uchCapEnable)
    {
        uchOneFrameByteNum += 12;
    }
    if(uchTempEnable)
    {
        uchOneFrameByteNum += 12;
    }

    //section RAWDATA
    if (g_uchFifoPackageMode)
    {
        uchOneFrameByteNum ++;
    }
    else
    {
        uchOneFrameByteNum += uchChnlNum*4;
    }
    //section AGC_INFO
    if(uchAgcEnable)
    {
        uchOneFrameByteNum += uchChnlNum*4;
    }
    //section RESLUT
    //result byte num
    uchReslutByteNum = 0;
    uchOneFrameByteNum += 1;
    uchReslutByteNum += 1;
    //FLAG 2
    uchOneFrameByteNum += 5;
    uchReslutByteNum += 5;
    //FLAG 3 (need upload on only first frame)
    if(uchFramePosi&GH3X2X_FRAME_POSI_HEAD)
    {
        uchOneFrameByteNum += 5;
        uchReslutByteNum += 5;
    }
    if (uchAlgoResFlag && 1 == pstFrameInfo->pstAlgoResult->uchUpdateFlag)
    {
        uchOneFrameByteNum += (5*pstFrameInfo->pstAlgoResult->uchResultNum);
        uchReslutByteNum += (5*pstFrameInfo->pstAlgoResult->uchResultNum);
    }

    #if GH3X2X_DIVIDE_ZERO_FLAG_UPLODE_EN
    //flag4
    uchOneFrameByteNum += 5;
    uchReslutByteNum += 5;
    #endif

    GH3x2xSendFunctionInfo(pstFrameInfo);  //send function info when first frame
    
    uchNeedContinue = 1;
    while(uchNeedContinue)
    {
        if(GH3X2X_UPLOAD_STATUS_NULL == g_uchGh3x2xUploadStatus)
        {
            Gh2x2xPackPakcageHeader(&uchNeedContinue, uchFunctionIDForMaster,\
                                    puchUploadBuf, puchTagArray, uchGsEnable, uchAlgoResFlag,\
                                    uchAgcEnable, uchAmbEnable, uchGsGyroEnable,uchCapEnable,uchTempEnable, 0, 0);
        }
        if(GH3X2X_UPLOAD_STATUS_HAVE_STORE_DATA == g_uchGh3x2xUploadStatus)
        {
            if(((GU16)g_uchGh3x2xUploadBufUse + uchOneFrameByteNum) >= GH3X2X_UPROTOCOL_GET_PAYLOAD_LEN_SUPPORT(g_uchPacketMaxLenSupport))  //can not store more data
            {
                //send data
                usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_NEW_STRUCT_RAWDATA, puchUploadBuf - 4,
                                    puchUploadBuf, g_uchGh3x2xUploadBufUse);
                Gh3x2xDemoSendProtocolData(puchUploadBuf - 4,usRespondLen);
                
                
                g_uchGh3x2xUploadStatus = GH3X2X_UPLOAD_STATUS_NULL;
                continue;
            }
            else
            {
                uchNeedContinue = 0;
                puchUploadBuf[UPROTOCOL_RAWDATA_LEN_INDEX] += uchOneFrameByteNum;
                    
                //section FRAME_ID
                puchUploadBuf[g_uchGh3x2xUploadBufUse] = ((*(pstFrameInfo->punFrameCnt))&0x000000FF);
                g_uchGh3x2xUploadBufUse ++;
                //section GSENSOR_DATA
                if(uchGsEnable)
                {
                    GS16 *psGsensorData;
                    psGsensorData = pstFrameInfo->pusFrameGsensordata;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[0]); //gsx
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[0]);
                    g_uchGh3x2xUploadBufUse += 1;
                    
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[1]); //gsy
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[1]);
                    g_uchGh3x2xUploadBufUse += 1;
                    
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[2]); //gsz
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[2]);
                    g_uchGh3x2xUploadBufUse += 1;
                    if(uchGsGyroEnable)
                    {
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[3]); //gyrox
                        g_uchGh3x2xUploadBufUse += 1;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[3]);
                        g_uchGh3x2xUploadBufUse += 1;
                        
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[4]); //gyroy
                        g_uchGh3x2xUploadBufUse += 1;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[4]);
                        g_uchGh3x2xUploadBufUse += 1;
                        
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(psGsensorData[5]); //gyroz
                        g_uchGh3x2xUploadBufUse += 1;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_LOW_BYTE_FROM_WORD(psGsensorData[5]);
                        g_uchGh3x2xUploadBufUse += 1;
                    }
                }
                //section Cap and Temp
                if(uchCapEnable)
                {

                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;

                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;

                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameCapdata->unCapData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;

                }
                if(uchTempEnable)
                {
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[0]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[1]); 
                    g_uchGh3x2xUploadBufUse += 1;

                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[2]); 
                    g_uchGh3x2xUploadBufUse += 1;

                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstFrameTempdata->unTempData[3]); 
                    g_uchGh3x2xUploadBufUse += 1;
              
                }


                //section RAWDATA
                //set flag ,copy rawdata, clear flag

                if (GH3X2X_GetFifoPackageMode() == 1)
                {
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = g_uchFifoPackageID;
                    g_uchGh3x2xUploadBufUse ++;
                }
                else
                {
                    for(GU8 uchChnlCnt = 0; uchChnlCnt < uchChnlNum; uchChnlCnt ++)
                    {
                        //led adj flag 0 
                        if(pstFrameInfo->punFrameFlag[0]&((((GU32)1)) << uchChnlCnt))
                        {
                            pstFrameInfo->punFrameRawdata[uchChnlCnt] |= ((((GU32)1)) << 26);
                        }
                        // fast recovery
                        if ((GH3X2X_FUNCTION_ECG == unFunctionID)||(GH3X2X_FUNCTION_PWTT == unFunctionID))
                        {
                            if(0 == uchChnlCnt)
                            {
                                if(pstFrameInfo->punFrameFlag[2]&((((GU32)1)) << 0))
                                {
                                    pstFrameInfo->punFrameRawdata[uchChnlCnt] |= ((((GU32)1)) << 26);
                                }
                            }
                        }
                        //led adj flag 1 
                        if(pstFrameInfo->punFrameFlag[1]&((((GU32)1)) << uchChnlCnt))
                        {
                            pstFrameInfo->punFrameRawdata[uchChnlCnt] |= ((((GU32)1)) << 25);
                        }

                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = (pstFrameInfo->punFrameRawdata[uchChnlCnt] >> 24)&0x000000FF;
                        g_uchGh3x2xUploadBufUse ++;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = (pstFrameInfo->punFrameRawdata[uchChnlCnt] >> 16)&0x000000FF;
                        g_uchGh3x2xUploadBufUse ++;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = (pstFrameInfo->punFrameRawdata[uchChnlCnt] >> 8)&0x000000FF;
                        g_uchGh3x2xUploadBufUse ++;
                        puchUploadBuf[g_uchGh3x2xUploadBufUse] = (pstFrameInfo->punFrameRawdata[uchChnlCnt] >> 0)&0x000000FF;
                        g_uchGh3x2xUploadBufUse ++;

                        pstFrameInfo->punFrameRawdata[uchChnlCnt] &= 0x00FFFFFF;
                    }
                }
                //section AGC_INFO
                if(uchAgcEnable)
                {
                    GH3X2X_Memcpy((GU8*)&(puchUploadBuf[g_uchGh3x2xUploadBufUse]), (GU8*)(pstFrameInfo->punFrameAgcInfo), uchChnlNum*4);
                    g_uchGh3x2xUploadBufUse += uchChnlNum*4;
                }
                //section AmbData
                if(uchAmbEnable)
                {
                    GH3X2X_Memcpy((GU8*)&(puchUploadBuf[g_uchGh3x2xUploadBufUse]), (GU8*)(pstFrameInfo->punFrameAgcInfo), uchChnlNum*3);
                    g_uchGh3x2xUploadBufUse += uchChnlNum*3;
                }
                
                //section RESLUT_DATA
                
                //result byte num
                puchUploadBuf[g_uchGh3x2xUploadBufUse] = uchReslutByteNum;
                g_uchGh3x2xUploadBufUse ++;
                
                //flag2
                puchUploadBuf[g_uchGh3x2xUploadBufUse] = 2;
                g_uchGh3x2xUploadBufUse ++;        
                GH3X2X_Memcpy((GU8*)&(puchUploadBuf[g_uchGh3x2xUploadBufUse]), (GU8*)(&(pstFrameInfo->punFrameFlag[2])), 4);
                g_uchGh3x2xUploadBufUse += 4;    
                //flag3
                if(uchFramePosi&GH3X2X_FRAME_POSI_HEAD)
                {
                    puchUploadBuf[g_uchGh3x2xUploadBufUse] = 3;
                    g_uchGh3x2xUploadBufUse ++;     
                    GH3X2X_Memcpy((GU8*)&(puchUploadBuf[g_uchGh3x2xUploadBufUse]), (GU8*)(&(pstFrameInfo->punFrameFlag[3])), 4);
                    g_uchGh3x2xUploadBufUse += 4;    
                }

                #if GH3X2X_DIVIDE_ZERO_FLAG_UPLODE_EN
                //flag4
                puchUploadBuf[g_uchGh3x2xUploadBufUse] = 4;
                g_uchGh3x2xUploadBufUse ++;     
                GH3X2X_Memcpy((GU8*)&(puchUploadBuf[g_uchGh3x2xUploadBufUse]), (GU8*)(&(pstFrameInfo->punFrameFlag[4])), 4);
                g_uchGh3x2xUploadBufUse += 4;   
                #endif

                //alg result
                if (1 == uchAlgoResFlag)
                {
                    //puchUploadBuf[g_uchGh3x2xUploadBufUse] = 0;
                    //g_uchGh3x2xUploadBufUse++;
                    if (1 == pstFrameInfo->pstAlgoResult->uchUpdateFlag)
                    {                                                                             
                        GU8 uchResTagData = GH3X2X_GET_LEFT_SHIFT_VAL(UPROTOCOL_RESULT_TYPE_BIT_FIELD);
                        GU8 uchResultNumTemp = 0;
                        for(GU8 uchResultCnt = 0;uchResultCnt < GH3X2X_ALGO_RESULT_MAX_NUM;uchResultCnt++)
                        {
                            if ((pstFrameInfo->pstAlgoResult->usResultBit >> uchResultCnt) & 0x1)
                            {
                                puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_SET_BIT(uchResTagData, uchResultCnt);
                                g_uchGh3x2xUploadBufUse ++ ;
                                puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE0_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[uchResultCnt]);
                                g_uchGh3x2xUploadBufUse ++ ;
                                puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE1_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[uchResultCnt]);
                                g_uchGh3x2xUploadBufUse ++ ;
                                puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE2_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[uchResultCnt]);
                                g_uchGh3x2xUploadBufUse ++ ;
                                puchUploadBuf[g_uchGh3x2xUploadBufUse] = GH3X2X_GET_BYTE3_FROM_DWORD(pstFrameInfo->pstAlgoResult->snResult[uchResultCnt]);
                                g_uchGh3x2xUploadBufUse ++ ;
                                uchResultNumTemp ++;
                                if (uchResultNumTemp >= pstFrameInfo->pstAlgoResult->uchResultNum)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    // end of if (1 == uchAlgoResFlag)
                }

                //modify uchReslutByteNum
                puchUploadBuf[g_uchGh3x2xUploadBufUse - uchReslutByteNum] = uchReslutByteNum - 1;

                if(uchFramePosi&GH3X2X_FRAME_POSI_TAIL)
                {
                    //send data
                    usRespondLen = GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_NEW_STRUCT_RAWDATA, puchUploadBuf - 4,
                                        puchUploadBuf, g_uchGh3x2xUploadBufUse);
                    Gh3x2xDemoSendProtocolData(puchUploadBuf - 4,usRespondLen);

                    g_uchGh3x2xUploadStatus = GH3X2X_UPLOAD_STATUS_NULL;
                }
            }
        }
    }
}
#else
void Gh2x2xUploadDataToMaster(const STGh3x2xFrameInfo * const pstFrameInfo, GU16 usFrameCnt, GU16 usFrameNum, GU8* puchTagArray)
{
    UNUSED_VAR(pstFrameInfo);
    UNUSED_VAR(usFrameCnt);
    UNUSED_VAR(usFrameNum);
    UNUSED_VAR(puchTagArray);
}
#endif

/**
 * @fn     GU32 GH3X2X_GetFuncModeAvailable(void)
 *
 * @brief  Get all function mode available
 *
 * @attention   None
 *
 * @param[out]  None
 * @param[in]   None
 * @return  function mode, @ref GH3X2X_NO_FUNCTION ... GH3X2X_FUNCTION_PWTT
 */
GU32 GH3X2X_GetFuncModeAvailable(void)
{
    return g_unAllFuncMode;
}

/**
 * @fn     GU32 GH3X2X_GetFuncModeOpened(void)
 *
 * @brief  Get function mode that already opened
 *
 * @attention   None
 *
 * @param[out]  None
 * @param[in]   None
 * @return  function mode, @ref GH3X2X_NO_FUNCTION ... GH3X2X_FUNCTION_PWTT
 */
GU32 GH3X2X_GetFuncModeOpened(void)
{
    return g_unFuncModeOpened;
}

/**
 * @fn     GU32 GH3X2X_GetTargetFuncMode(void)
 *
 * @brief  Get function mode that will be opened or closed
 *
 * @attention   None
 *
 * @param[out]  None
 * @param[in]   None
 * @return  function mode, @ref GH3X2X_NO_FUNCTION ... GH3X2X_FUNCTION_PWTT
 */
GU32 GH3X2X_GetTargetFuncMode(void)
{
    return g_unTargetFuncMode;
}

/**
 * @fn     void GH3X2X_ReportIrqStatus(GU8 *puchRespondBuffer, GU16 *pusRespondLen, GU16 usIrqStatus)
 *
 * @brief  report irq status of Gh3x2x
 *
 * @attention   none
 *
 * @param[out]  puchRespondBuffer       pointer to protocol pkg data.
 * @param[out]  pusRespondLen           pointer to protocol pkg data length
 * @param[in]   usIrqStatus             irq status need report
 * @return  none
 */
void GH3X2X_ReportIrqStatus(GU8 *puchRespondBuffer, GU16 *pusRespondLen, GU16 usIrqStatus)
{
    GU16 usIrqStatusTmp = usIrqStatus & g_usGh3x2xEventNeedReport;

    if (usIrqStatusTmp != 0)
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_EVENT_REPORT_INDEX]     = GH3X2X_GET_HIGH_BYTE_FROM_WORD(usIrqStatus);
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_EVENT_REPORT_INDEX + 1] = GH3X2X_GET_LOW_BYTE_FROM_WORD(usIrqStatus);

        g_pstGh3x2xProtocolData->puchPacketPayloadArr[UPROTOCOL_CHIP_EVENT_REPORT_ID_INDEX]  = g_uchEventReportId;
        g_uchEventReportId++;
        SET_VAL_VIA_PTR(pusRespondLen, GH3X2X_UprotocolPacketFormat(GH3X2X_UPROTOCOL_CMD_CHIP_EVENT_REPORT,
                            puchRespondBuffer, g_pstGh3x2xProtocolData->puchPacketPayloadArr, UPROTOCOL_CHIP_EVENT_REPORT_LEN));
    }
    else
    {
        SET_VAL_VIA_PTR(pusRespondLen, 0);
    }   
}

//__attribute__((weak)) void GH3X2X_PROTOCOL_FRAMID_CLEAN(GU8 uchFunctionID)
void GH3X2X_ProtocolFrameIdClean(GU8 uchFunctionID)
{
    g_pstGh3x2xProtocolData->uchFrameIdArr[uchFunctionID] = 0;
}

#if GH3X2X_DUMP_MODE_EN
/**
 * @fn       void GH3X2X_FillGsensorData(GU16* pusPayloadIndex, STGsensorRawdata* pstGsAxisValue, 
 *                                        )
 *d
 * @brief  fill g sensor data to protocol data
 *
 * @attention    None
 *
 * @param[in]    pstGsAxisValue       g sensor data
 * @param[out]   pusPayloadIndex      current index of payload
 *
 * @return    None
 */
void GH3X2X_FillGsensorData(GU16* pusPayloadIndex, STGsensorRawdata* pstGsAxisValue)
{
    //GS16 sGsensorRightShift = (1 << emGsSensitivity);
    STGsensorRawdata stGsensorData = *pstGsAxisValue;

    //stGsensorData.sXAxisVal = (GS16)(stGsensorData.sXAxisVal / sGsensorRightShift);
    //stGsensorData.sYAxisVal = (GS16)(stGsensorData.sYAxisVal / sGsensorRightShift);
    //stGsensorData.sZAxisVal = (GS16)(stGsensorData.sZAxisVal / sGsensorRightShift);
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(stGsensorData.sXAxisVal); //gs
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_LOW_BYTE_FROM_WORD(stGsensorData.sXAxisVal);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(stGsensorData.sYAxisVal);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_LOW_BYTE_FROM_WORD(stGsensorData.sYAxisVal);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_HIGH_BYTE_FROM_WORD(stGsensorData.sZAxisVal);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_LOW_BYTE_FROM_WORD(stGsensorData.sZAxisVal);
    *pusPayloadIndex += 1;
}

/**
 * @fn       void GH3X2X_FillAlgoData(GU16* pusPayloadIndex, GS32 nAlgoCalcResultArr[], GU8 uchAlgoResNum)
 *
 * @brief  fill algorithm data to protocol data
 *
 * @attention    None
 *
 * @param[in]    nAlgoCalcResultArr  pointer to algorithm result
 * @param[in]    uchAlgoResNum       algorithm data num
 * @param[out]   pusPayloadIndex     current index of payload
 *
 * @return    None
 */
void GH3X2X_FillAlgoData(GU16* pusPayloadIndex, GS32 nAlgoCalcResultArr[], GU8 uchAlgoResNum)
{
    GU8  uchAlgoCnt    = 0;  
    GU8  uchResTagData = 0;
    uchResTagData = GH3X2X_GET_LEFT_SHIFT_VAL(UPROTOCOL_RESULT_TYPE_BIT_FIELD);
    for (uchAlgoCnt = 0; uchAlgoCnt < uchAlgoResNum; uchAlgoCnt++)
    {
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_SET_BIT(uchResTagData, uchAlgoCnt);
        *pusPayloadIndex += 1;
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE0_FROM_DWORD(nAlgoCalcResultArr[uchAlgoCnt]);
        *pusPayloadIndex += 1;
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE1_FROM_DWORD(nAlgoCalcResultArr[uchAlgoCnt]);
        *pusPayloadIndex += 1;
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE2_FROM_DWORD(nAlgoCalcResultArr[uchAlgoCnt]);
        *pusPayloadIndex += 1;
        g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE3_FROM_DWORD(nAlgoCalcResultArr[uchAlgoCnt]);
        *pusPayloadIndex += 1;
    }
}
#endif

#if GH3X2X_ELECTRO_RRECER_OBSERVE_EN
/**
 * @fn       void GH3X2X_FillElectrodeWearRevertData(GU16* pusPayloadIndex)
 *
 * @brief  fill electrode wear revert data to protocol data buf
 *
 * @attention    None
 *
 * @param[in]    pusPayloadIndex      current index of payload
 * @param[out]   None
 *
 * @return    None
 */
void GH3X2X_FillElectrodeWearRevertData(GU16* pusPayloadIndex)
{
    GU8  uchTagData = 0;
    GU32 unWearRevertCnt = 0;
    GH3X2X_VAL_CLEAR_LEFT_BIT(uchTagData, (GU8)UPROTOCOL_RESULT_TYPE_BIT_FIELD);
    GH3X2X_VAL_SET_BIT(uchTagData, UPROTOCOL_UPLOAD_FLAG_INDEX3);
    unWearRevertCnt = GH3X2X_GetAndClearElectrodeWearRevCnt();
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = uchTagData;
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE0_FROM_DWORD(unWearRevertCnt);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE1_FROM_DWORD(unWearRevertCnt);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE2_FROM_DWORD(unWearRevertCnt);
    *pusPayloadIndex += 1;
    g_pstGh3x2xProtocolData->puchPacketPayloadArr[*pusPayloadIndex] = GH3X2X_GET_BYTE3_FROM_DWORD(unWearRevertCnt);
    *pusPayloadIndex += 1;
}
#endif

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
