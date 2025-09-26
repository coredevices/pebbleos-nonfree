/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_protocol.c
 * 
 * @brief   gh3x2x driver lib demo code for protocol
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh_demo_inner.h"
#include "gh_multi_sen_pro.h"

GU8 gubUseZipProtocol = 0;

GU32 Gh3x2xGetProtocolDataFunctionIntercept()
{
    return __GH3X2X_PROTOCOL_DATA_FUNCTION_INTERCEPT__ ;
}

#if (__SUPPORT_PROTOCOL_ANALYZE__)

#define GH3X2X_PROTOCOL_BUF_LEN             (255)
#define GH3X2X_DEMO_USE_SERIAL_UART         (1)
#define GH3X2X_DEMO_USE_SERIAL_BLE          (2)

/* use BLE */
#define RX_BYTE_STREAM_STATE_WAIT_HEADER    (0)
#define RX_BYTE_STREAM_STATE_WAIT_VERSION   (1)
#define RX_BYTE_STREAM_STATE_WAIT_CMD       (2)
#define RX_BYTE_STREAM_STATE_WAIT_LEN       (3)
#define RX_BYTE_STREAM_STATE_WAIT_DATA      (4)
#define RX_BYTE_STREAM_STATE_WAIT_CRC8      (5)

#define GH3X2X_PROTOCOL_HEADER              (0xAA)
#define GH3X2X_PROTOCOL_VERSION             (0x11)
#define GH3X2X_PROTOCOL_LEN_INDEX           (3)
#define GH3X2X_PROTOCOL_HEADER_LEN          (4)

/* use UART */
#define RX_STREAM_STATE_WAIT_UART_HEADER_47 (0)
#define RX_STREAM_STATE_WAIT_UART_HEADER_44 (1)
#define RX_STREAM_STATE_WAIT_UART_LEN       (2)
#define RX_STREAM_STATE_WAIT_UART_DATA      (3)
#define RX_STREAM_STATE_WAIT_TAIL           (4)

#define GH3X2X_PROTOCOL_BLE_UART_47         (0x47)
#define GH3X2X_PROTOCOL_BLE_UART_44         (0x44)
#define GH3X2X_PROTOCOL_BLE_TAIL            (0x0A)

GU8  g_uchRxByteStreamState  = 0;
GU16 g_usUartPayloadLen      = 0;
GU16 g_usRxProtocolDataIndex = 0;
GU8  g_puchProtocolBufferRecv[GH3X2X_PROTOCOL_BUF_LEN] = {0};

STGh3x2xProtocolData  g_stGh3x2xProtocolData ={0};
STGh3x2xProtocolData * const g_pstGh3x2xProtocolData = &g_stGh3x2xProtocolData;

#define GH3X2X_PROTOCOL_DATA_PKG_SIZE (255)
#define GH3X2X_PROTOCOL_EVENT_PKG_SIZE (4)
#define GH3X2X_ROTOCOL_TEMP_EVENT_BUF_SIZE  (20)

#define GH3X2X_SERIALSEND_TIMER_STATUS_STOP       0
#define GH3X2X_SERIALSEND_TIMER_STATUS_START      1

#define GH3X2X_PROTOCOL_EVENT_ACK_STATUS_NO_ACK   0
#define GH3X2X_PROTOCOL_EVENT_ACK_STATUS_ACK      1

volatile GU8 g_uchGh3x2xSerialSendTimerStatus;
GU8 g_puchGh3x2xProtocolDataSendBuf[__GH3X2X_PROTOCOL_DATA_FIFO_LEN__][GH3X2X_PROTOCOL_DATA_PKG_SIZE];
volatile GU8 g_uchGh3x2xProtocolDataSendFifoWp;
GU8 g_uchGh3x2xProtocolDataSendFifoRp;
GU8 g_puchGh3x2xProtocolEventSendBuf[__GH3X2X_PROTOCOL_EVENT_FIFO_LEN__][GH3X2X_PROTOCOL_EVENT_PKG_SIZE];
volatile GU8 g_puchGh3x2xProtocolEventSendFifoWp;
GU8 g_uchGh3x2xProtocolEventSendFifoRp;
GU8 g_uchGh3x2xProtocolEventReportId;
GU8 g_uchGh3x2xProtocolEventReportRetryCnt;
GU8 g_uchGh3x2xProtocolEventAckStatus;
GU16 g_usGh3x2xProtocolEventWaitingAckTime;
GU8 g_uchGh3x2xProtocolEventAckId;
GU8 g_uchGh3x2xProtocolIdleFlag;

void Gh3x2x_HalSerialFifoInit(void)
{
    g_uchGh3x2xSerialSendTimerStatus = GH3X2X_SERIALSEND_TIMER_STATUS_STOP;
    g_uchGh3x2xProtocolDataSendFifoWp = 0;
    g_uchGh3x2xProtocolDataSendFifoRp = 0;
    g_uchGh3x2xProtocolEventReportId = 0;
    g_uchGh3x2xProtocolEventReportRetryCnt = 0;
    g_uchGh3x2xProtocolEventAckStatus = GH3X2X_PROTOCOL_EVENT_ACK_STATUS_NO_ACK;
    g_usGh3x2xProtocolEventWaitingAckTime = 0;
    g_uchGh3x2xProtocolIdleFlag = 0;
    Gh3x2xSerialSendTimerInit(__GH3X2X_PROTOCOL_SEND_TIMER_PERIOD__);
}

#ifdef GOODIX_DEMO_PLATFORM
extern SemaphoreHandle_t UartTxFifoWritePower;
extern void UartSendIdlePkgGoodixPlanform(void);
#endif

void Gh3x2x_HalSerialWriteDataToFifo(GU8 * lpubSource, GU8 lubLen)
{
    GU8 uchBufCnt;
    if(0 == lubLen)
    {
        return ;
    }

    if(g_uchGh3x2xProtocolDataSendFifoWp >= g_uchGh3x2xProtocolDataSendFifoRp)
    {
        uchBufCnt = g_uchGh3x2xProtocolDataSendFifoWp - g_uchGh3x2xProtocolDataSendFifoRp;
    }
    else
    {
        uchBufCnt = __GH3X2X_PROTOCOL_DATA_FIFO_LEN__ - (g_uchGh3x2xProtocolDataSendFifoRp - g_uchGh3x2xProtocolDataSendFifoWp);
    }

#ifdef GOODIX_DEMO_PLATFORM
    if((uchBufCnt >= (__GH3X2X_PROTOCOL_DATA_FIFO_LEN__ - 1)) || (LP_MODE_DSLEEP == LP_GetLowPwrMode()))
#else
    if((uchBufCnt >= (__GH3X2X_PROTOCOL_DATA_FIFO_LEN__ - 1)) )
#endif
    {
        //GH3X2X_INFO_LOG("Warnning: Protocol Data fifo is overflow !!!\r\n");
        return ;
    }

#ifdef GOODIX_DEMO_PLATFORM
    xSemaphoreTake(UartTxFifoWritePower,portMAX_DELAY);
#endif

    if(lubLen > (GH3X2X_PROTOCOL_DATA_PKG_SIZE - 4))
    {
        lubLen = (GH3X2X_PROTOCOL_DATA_PKG_SIZE - 4);
    }

#ifdef GOODIX_DEMO_PLATFORM
    #if (!defined(GR5515_SK)) && (__PROTOCOL_SERIAL_TYPE__ != __PROTOCOL_SERIAL_USE_UART__)
    GH3X2X_Memset(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp])), 0xA0,GH3X2X_PROTOCOL_DATA_PKG_SIZE);
    GH3X2X_Memcpy(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp])) + 3,lpubSource,lubLen);
    g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp][0] = 0x47;
    g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp][1] = 0x44;
    g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp][2] = GH3X2X_PROTOCOL_DATA_PKG_SIZE - 4;
    g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp][254] = 0x0A;
    #else
    GH3X2X_Memset(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp])), 0xA0,GH3X2X_PROTOCOL_DATA_PKG_SIZE);
    GH3X2X_Memcpy(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp])),lpubSource,lubLen);
    #endif
#else
    GH3X2X_Memcpy(((GU8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoWp])) + 0,lpubSource,lubLen);
#endif

    g_uchGh3x2xProtocolDataSendFifoWp ++;
    if(g_uchGh3x2xProtocolDataSendFifoWp >= __GH3X2X_PROTOCOL_DATA_FIFO_LEN__)
    {
        g_uchGh3x2xProtocolDataSendFifoWp = 0;
    }
    if(GH3X2X_SERIALSEND_TIMER_STATUS_STOP == g_uchGh3x2xSerialSendTimerStatus)
    {
        Gh3x2xSerialSendTimerStart();
        g_uchGh3x2xSerialSendTimerStatus = GH3X2X_SERIALSEND_TIMER_STATUS_START;
    }

#ifdef GOODIX_DEMO_PLATFORM
    xSemaphoreGive(UartTxFifoWritePower);
#endif

    return ;
}

void Gh3x2xSetProtocolEventAck(void)
{
    g_uchGh3x2xProtocolEventAckStatus = GH3X2X_PROTOCOL_EVENT_ACK_STATUS_ACK;
}

GU8 Gh3x2xGetProtocolEventReportId(void)
{
    return g_puchGh3x2xProtocolEventSendBuf[g_uchGh3x2xProtocolEventSendFifoRp][2];
}

void Gh3x2x_HalSerialWriteEventToFifo(GU16 luwEvent,GU8 uchEventEx)
{
    GU8 uchBufCnt;

    if(g_puchGh3x2xProtocolEventSendFifoWp >= g_uchGh3x2xProtocolEventSendFifoRp)
    {
        uchBufCnt = g_puchGh3x2xProtocolEventSendFifoWp - g_uchGh3x2xProtocolEventSendFifoRp;
    }
    else
    {
        uchBufCnt = __GH3X2X_PROTOCOL_EVENT_FIFO_LEN__ - (g_uchGh3x2xProtocolEventSendFifoRp - g_puchGh3x2xProtocolEventSendFifoWp);
    }

#ifdef GOODIX_DEMO_PLATFORM
    if((uchBufCnt >= (__GH3X2X_PROTOCOL_EVENT_FIFO_LEN__ - 1)) || (LP_MODE_DSLEEP == LP_GetLowPwrMode()))
#else
    if((uchBufCnt >= (__GH3X2X_PROTOCOL_EVENT_FIFO_LEN__ - 1)))
#endif
    {
        GH3X2X_INFO_LOG("Warnning: Protocol event fifo is overflow !!!\r\n");
        return ;
    }

    g_uchGh3x2xProtocolEventReportId ++;
    g_puchGh3x2xProtocolEventSendBuf[g_puchGh3x2xProtocolEventSendFifoWp][0] = ((GU8*)(&luwEvent))[1];
    g_puchGh3x2xProtocolEventSendBuf[g_puchGh3x2xProtocolEventSendFifoWp][1] = ((GU8*)(&luwEvent))[0];
    g_puchGh3x2xProtocolEventSendBuf[g_puchGh3x2xProtocolEventSendFifoWp][2] = g_uchGh3x2xProtocolEventReportId;
    g_puchGh3x2xProtocolEventSendBuf[g_puchGh3x2xProtocolEventSendFifoWp][3] = uchEventEx;
    g_puchGh3x2xProtocolEventSendFifoWp ++;

    if(g_puchGh3x2xProtocolEventSendFifoWp >= __GH3X2X_PROTOCOL_EVENT_FIFO_LEN__)
    {
        g_puchGh3x2xProtocolEventSendFifoWp = 0;
    }
    if(GH3X2X_SERIALSEND_TIMER_STATUS_STOP == g_uchGh3x2xSerialSendTimerStatus)
    {
        Gh3x2xSerialSendTimerStart();
        g_uchGh3x2xSerialSendTimerStatus = GH3X2X_SERIALSEND_TIMER_STATUS_START;
    }

    return ;
}

void Gh3x2xSerialSendTimerHandle(void)
{
    GU8 uchHaveSendPkg = 0;
#ifdef GOODIX_DEMO_PLATFORM
    GU8 puchTempEventSendBuf[GH3X2X_ROTOCOL_TEMP_EVENT_BUF_SIZE];
#endif
    GU8 uchStopTimerFlag = 0;
    GU8 puchTempBuf2[GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5];

    //check event fifo is empyty or not
    if(g_puchGh3x2xProtocolEventSendFifoWp != g_uchGh3x2xProtocolEventSendFifoRp)
    {
        if(GH3X2X_PROTOCOL_EVENT_ACK_STATUS_ACK == g_uchGh3x2xProtocolEventAckStatus)  //got ack
        {
            g_uchGh3x2xProtocolEventSendFifoRp ++;
            if(g_uchGh3x2xProtocolEventSendFifoRp >= __GH3X2X_PROTOCOL_EVENT_FIFO_LEN__)
            {
                g_uchGh3x2xProtocolEventSendFifoRp = 0;
            }
            g_uchGh3x2xProtocolEventAckStatus = GH3X2X_PROTOCOL_EVENT_ACK_STATUS_NO_ACK;
            g_usGh3x2xProtocolEventWaitingAckTime = 0;
        }
        else   //have not got ack
        {
            if(g_usGh3x2xProtocolEventWaitingAckTime > __GH3X2X_PROTOCOL_EVENT_WAITING_ACK_TIME__)
            {
                if(g_uchGh3x2xProtocolEventReportRetryCnt < 255)
                {
                    g_uchGh3x2xProtocolEventReportRetryCnt ++;
                }
                g_usGh3x2xProtocolEventWaitingAckTime = 0;
            }
            if(g_uchGh3x2xProtocolEventReportRetryCnt > __GH3X2X_PROTOCOL_EVENT_RESEND_NUM__)  //retry cnt is enough
            {
                g_uchGh3x2xProtocolEventSendFifoRp ++;
                if(g_uchGh3x2xProtocolEventSendFifoRp >= __GH3X2X_PROTOCOL_EVENT_FIFO_LEN__)
                {
                    g_uchGh3x2xProtocolEventSendFifoRp = 0;
                }
                g_uchGh3x2xProtocolEventReportRetryCnt = 0;
            }
        }
    }

    if(g_puchGh3x2xProtocolEventSendFifoWp != g_uchGh3x2xProtocolEventSendFifoRp)
    {
        if(0 == g_usGh3x2xProtocolEventWaitingAckTime)
        {
            if(0 == g_uchGh3x2xProtocolEventReportRetryCnt)
            {
                g_uchGh3x2xProtocolEventAckStatus = GH3X2X_PROTOCOL_EVENT_ACK_STATUS_NO_ACK;
            }

            //send next event or no-ack event
            puchTempBuf2[0] = 0xAA;
            puchTempBuf2[1] = 0x11;
            puchTempBuf2[2] = 0x16;
            puchTempBuf2[3] = GH3X2X_PROTOCOL_EVENT_PKG_SIZE;            
            GH3X2X_Memcpy(puchTempBuf2 + 4,g_puchGh3x2xProtocolEventSendBuf[g_uchGh3x2xProtocolEventSendFifoRp],GH3X2X_PROTOCOL_EVENT_PKG_SIZE);
            puchTempBuf2[GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5 - 1] = GH3X2X_CalcArrayCrc8Val(&(puchTempBuf2[0]),0,GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 4);
            
#ifdef GOODIX_DEMO_PLATFORM
    #if (!defined(GR5515_SK)) && (__PROTOCOL_SERIAL_TYPE__ != __PROTOCOL_SERIAL_USE_UART__)
            GH3X2X_Memcpy(puchTempEventSendBuf + 3,puchTempBuf2,GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5);
            puchTempEventSendBuf[0] = 0x47;
            puchTempEventSendBuf[1] = 0x44;
            puchTempEventSendBuf[2] = GH3X2X_ROTOCOL_TEMP_EVENT_BUF_SIZE - 4;
            puchTempEventSendBuf[GH3X2X_ROTOCOL_TEMP_EVENT_BUF_SIZE - 1] = 0x0A;
            Gh3x2x_HalSerialSendData((u8*)puchTempEventSendBuf,GH3X2X_ROTOCOL_TEMP_EVENT_BUF_SIZE);
            g_uchGh3x2xProtocolIdleFlag = 1;
    #else
            GH3X2X_Memcpy(puchTempEventSendBuf,puchTempBuf2,GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5);
            Gh3x2x_HalSerialSendData((GU8*)puchTempBuf2,GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5);
            g_uchGh3x2xProtocolIdleFlag = 1;
    #endif
#else
            Gh3x2x_HalSerialSendData((GU8*)puchTempBuf2,GH3X2X_PROTOCOL_EVENT_PKG_SIZE + 5);
#endif
            uchHaveSendPkg = 1;
        }
        g_usGh3x2xProtocolEventWaitingAckTime += __GH3X2X_PROTOCOL_SEND_TIMER_PERIOD__;
    }

    if(0 == uchHaveSendPkg)
    {
        if(g_uchGh3x2xProtocolDataSendFifoWp != g_uchGh3x2xProtocolDataSendFifoRp)
        {
            /**************  send to  master ************/
#ifdef GOODIX_DEMO_PLATFORM
    #if (!defined(GR5515_SK)) && (__PROTOCOL_SERIAL_TYPE__ != __PROTOCOL_SERIAL_USE_UART__)
            Gh3x2x_HalSerialSendData(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoRp])),GH3X2X_PROTOCOL_DATA_PKG_SIZE);
    #else
            Gh3x2x_HalSerialSendData(((u8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoRp])),5 + g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoRp][3]);
    #endif
            g_uchGh3x2xProtocolIdleFlag = 1;
#else 
            Gh3x2x_HalSerialSendData(((GU8*)(g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoRp])),5 + g_puchGh3x2xProtocolDataSendBuf[g_uchGh3x2xProtocolDataSendFifoRp][3]);
#endif
            g_uchGh3x2xProtocolDataSendFifoRp ++;
            if(g_uchGh3x2xProtocolDataSendFifoRp >= __GH3X2X_PROTOCOL_DATA_FIFO_LEN__)
            {
                g_uchGh3x2xProtocolDataSendFifoRp = 0;
            }
            uchHaveSendPkg = 1;
        }
    }

#ifdef GOODIX_DEMO_PLATFORM
    if(0 == uchHaveSendPkg)
    {
        if(g_uchGh3x2xProtocolIdleFlag)
        {
            g_uchGh3x2xProtocolIdleFlag = 0;
            UartSendIdlePkgGoodixPlanform();
        }
    }
#endif
    if((g_puchGh3x2xProtocolEventSendFifoWp == g_uchGh3x2xProtocolEventSendFifoRp)&&(g_uchGh3x2xProtocolDataSendFifoWp == g_uchGh3x2xProtocolDataSendFifoRp))  //event fifo and data fifo is empyt
    {
        //close timer
#ifdef GOODIX_DEMO_PLATFORM
        if(0 == g_uchGh3x2xProtocolIdleFlag)
#endif
        {
            if(GH3X2X_SERIALSEND_TIMER_STATUS_START == g_uchGh3x2xSerialSendTimerStatus)
            {
                Gh3x2xSerialSendTimerStop();
                g_uchGh3x2xSerialSendTimerStatus = GH3X2X_SERIALSEND_TIMER_STATUS_STOP;
                uchStopTimerFlag = 1;
            }
        }
    }
    //check again for mulit-thread confict
    if(uchStopTimerFlag)
    {
        if((g_puchGh3x2xProtocolEventSendFifoWp != g_uchGh3x2xProtocolEventSendFifoRp)||(g_uchGh3x2xProtocolDataSendFifoWp != g_uchGh3x2xProtocolDataSendFifoRp))  //event fifo and data fifo is empyt
        {
            Gh3x2xSerialSendTimerStart();
            g_uchGh3x2xSerialSendTimerStatus = GH3X2X_SERIALSEND_TIMER_STATUS_START;
        }
    }
}

/**
 * @fn     void Gh3x2xDemoSendProtocolData(GU8* puchProtocolDataBuffer, GU16 usProtocolDataLen)
 *
 * @brief  Copy protocol data to buffer that will be sent out to APP/EVK.
 *
 * @attention   None
 *
 * @param[in]   puchProtocolDataBuffer      point to protocol data
 * @param[in]   usProtocolDataLen           protocol data length
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoSendProtocolData(GU8* puchProtocolDataBuffer, GU16 usProtocolDataLen)
{
    if ((usProtocolDataLen > GH3X2X_UPROTOCOL_PAYLOAD_LEN_MAX) || (0 == usProtocolDataLen))
    {
        return;
    }
#if __SUPPORT_PROTOCOL_ANALYZE__
    Gh3x2x_HalSerialWriteDataToFifo(puchProtocolDataBuffer, usProtocolDataLen);
#endif
}

/**
 * @fn     void Gh3x2xDemoOneFrameDataProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen)
 *
 * @brief  Analyze protocol about GH3x2x,and pack protocol data to reply
 *
 * @attention   None
 *
 * @param[out]  puchProtocolDataBuffer      pointer to protocol data buffer
 * @param[out]  usRecvLen                   protocol data buffer len
 *
 * @return  None
 */
void Gh3x2xDemoOneFrameDataProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen)
{
    EMUprotocolParseCmdType emCmdType = UPROTOCOL_CMD_IGNORE;
    GU8  puchRespondBuffer[GH3X2X_UPROTOCOL_PAYLOAD_LEN_MAX] = {0};
    GU16 usRespondLen = 0;
    GU32 unFuncMode   = 0;
    emCmdType = GH3X2X_UprotocolParseHandler(puchRespondBuffer, &usRespondLen, puchProtocolDataBuffer, usRecvLen);
    /* if respond buffer len is 0, and return value is UPROTOCOL_CMD_IGNORE,
        it means driver lib can't analyze this protocol data */
    if ((0 == puchRespondBuffer[3]) && (UPROTOCOL_CMD_IGNORE == emCmdType))
    {
#ifdef GOODIX_DEMO_PLATFORM
        GOODIX_PLATFORM_PROTOCOL_ANALYZE_ENTITY();
#else
        GH3X2X_INFO_LOG("Driver lib can't analyze this protocol,skip it,or you can add code to process it.\r\n");
        return;
#endif
    }
    else
    {
#ifdef GOODIX_DEMO_PLATFORM
        GOODIX_PLATFORM_CMD_PROCESS_ENTITY();
#else
        if ((UPROTOCOL_CMD_START == emCmdType) || (UPROTOCOL_CMD_STOP == emCmdType))
        {
            unFuncMode = GH3X2X_GetTargetFuncMode();
            Gh3x2xDemoSamplingControl(unFuncMode, emCmdType);
            GH3X2X_INFO_LOG("[%s]:function = 0x%08x, ctrl = %d\r\n", __FUNCTION__, unFuncMode, emCmdType);
        }
#endif
        if (UPROTOCOL_CMD_MPT_MODE == emCmdType)
        {
            g_uchDemoWorkMode = GH3X2X_DEMO_WORK_MODE_MPT;
            GH3X2X_SetSingleChipModeEnableFlag(1);
        }
        if(g_uchDemoWorkMode != GH3X2X_DEMO_WORK_MODE_MPT)
        {
            GH3X2X_SetSingleChipModeEnableFlag(0);
        }
        Gh3x2xDemoSendProtocolData(puchRespondBuffer, usRespondLen);
    }
}

void Gh3x2xDemoHandleRecvUartData(GU8 uchRecvByte)
{
    switch (g_uchRxByteStreamState)
    {
    case RX_STREAM_STATE_WAIT_UART_HEADER_47:
        if (GH3X2X_PROTOCOL_BLE_UART_47 == uchRecvByte)
        {
            g_uchRxByteStreamState++;
        }
        else
        {
            g_uchRxByteStreamState = RX_STREAM_STATE_WAIT_UART_HEADER_47;
        }
        break;
    case RX_STREAM_STATE_WAIT_UART_HEADER_44:
        if (GH3X2X_PROTOCOL_BLE_UART_44 == uchRecvByte)
        {
            g_uchRxByteStreamState++;
        }
        else
        {
            g_uchRxByteStreamState = RX_STREAM_STATE_WAIT_UART_HEADER_47;
        }
        /* code */
        break;
    case RX_STREAM_STATE_WAIT_UART_LEN:
        if (uchRecvByte > 0)
        {
            g_usUartPayloadLen = uchRecvByte;
            g_usRxProtocolDataIndex = 0;
            g_uchRxByteStreamState++;
        }
        else
        {
            g_uchRxByteStreamState = RX_STREAM_STATE_WAIT_UART_HEADER_47;
        }        
        break;
    case RX_STREAM_STATE_WAIT_UART_DATA:
        g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
        g_usRxProtocolDataIndex++;
        if (g_usRxProtocolDataIndex >= g_usUartPayloadLen)
        {
            g_uchRxByteStreamState++;
        }        
        break;
    case RX_STREAM_STATE_WAIT_TAIL:
        if (GH3X2X_PROTOCOL_BLE_TAIL == uchRecvByte)
        {
            Gh3x2xDemoOneFrameDataProcess(g_puchProtocolBufferRecv, g_usRxProtocolDataIndex);
        }
        g_uchRxByteStreamState  = RX_STREAM_STATE_WAIT_UART_HEADER_47;       
        break;
    default:
        g_usRxProtocolDataIndex = 0;
        g_usUartPayloadLen       = 0;
        g_uchRxByteStreamState  = RX_STREAM_STATE_WAIT_UART_HEADER_47;
        break;
    }
}

void Gh3x2xDemoHandleRecvBleData(GU8 uchRecvByte)
{
    switch (g_uchRxByteStreamState)
    {
    case RX_BYTE_STREAM_STATE_WAIT_HEADER:
       if (GH3X2X_PROTOCOL_HEADER == uchRecvByte)
       {
           g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
           g_uchRxByteStreamState++;
           g_usRxProtocolDataIndex++;
       }
       else
       {
           g_usRxProtocolDataIndex = 0; 
           g_uchRxByteStreamState  = RX_BYTE_STREAM_STATE_WAIT_HEADER;
       }
       break;
    case RX_BYTE_STREAM_STATE_WAIT_VERSION:
       if (GH3X2X_PROTOCOL_VERSION == uchRecvByte)
       {
           g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
           g_uchRxByteStreamState++;
           g_usRxProtocolDataIndex++;
       }
       else
       {
           g_usRxProtocolDataIndex = 0; 
           g_uchRxByteStreamState  = RX_BYTE_STREAM_STATE_WAIT_HEADER;
       }
       break;
    case RX_BYTE_STREAM_STATE_WAIT_CMD:
        g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
        g_uchRxByteStreamState++;
        g_usRxProtocolDataIndex++;
        break;
    case RX_BYTE_STREAM_STATE_WAIT_LEN:
        if (uchRecvByte > 0)
        {
            g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
            g_uchRxByteStreamState++;
            g_usRxProtocolDataIndex++;
        }
        else
        {
            g_usRxProtocolDataIndex = 0;
            g_uchRxByteStreamState  = RX_BYTE_STREAM_STATE_WAIT_HEADER;
        }        
        break;       
    case RX_BYTE_STREAM_STATE_WAIT_DATA:
        g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
        g_usRxProtocolDataIndex++;
        if (g_usRxProtocolDataIndex >= g_puchProtocolBufferRecv[GH3X2X_PROTOCOL_LEN_INDEX] + GH3X2X_PROTOCOL_HEADER_LEN)
        {
            g_uchRxByteStreamState++;
        }
        break;
    case RX_BYTE_STREAM_STATE_WAIT_CRC8:
        g_puchProtocolBufferRecv[g_usRxProtocolDataIndex] = uchRecvByte;
        g_usRxProtocolDataIndex++;
        Gh3x2xDemoOneFrameDataProcess(g_puchProtocolBufferRecv, g_usRxProtocolDataIndex);
        g_usRxProtocolDataIndex = 0;
        g_uchRxByteStreamState  = RX_BYTE_STREAM_STATE_WAIT_HEADER;
        break;
    default:
        g_usRxProtocolDataIndex = 0;
        g_uchRxByteStreamState  = RX_BYTE_STREAM_STATE_WAIT_HEADER;
        break;
    }
}

/**
 * @fn     void Gh3x2xDemoProtocolProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen)
 *
 * @brief  Analyze protocol about GH3x2x,and pack protocol data to reply
 *
 * @attention   None
 *
 * @param[in]  puchProtocolDataBuffer   pointer to received protocol data buffer
 * @param[in]  usRecvLen                protocol data buffer length
 *
 * @return  None
 */
void Gh3x2xDemoProtocolProcess(GU8* puchProtocolDataBuffer, GU16 usRecvLen)
{
    GU16 uchIndex = 0;
    for (uchIndex = 0; uchIndex < usRecvLen; uchIndex++)
    {
#if (__PROTOCOL_SERIAL_TYPE__ == __PROTOCOL_SERIAL_USE_UART__)
        Gh3x2xDemoHandleRecvBleData(puchProtocolDataBuffer[uchIndex]);
#else
        Gh3x2xDemoHandleRecvBleData(puchProtocolDataBuffer[uchIndex]);
#endif
    }
}

/**
 * @fn     void Gh3x2xDemoReportEvent(GU16 usEvent,GU8 uchWearType)
 *
 * @brief  Report event to APP/EVK
 *
 * @attention   None
 *
 * @param[in]   usEvent      GH3x2x event
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoReportEvent(GU16 usEvent,GU8 uchEventEx)
{
    //GOODIX_PLATFORM_REPORT_EVRENT_ENTITY();
    //u16 usEventTemp = usEvent & (~GH3X2X_IRQ_MSK_FIFO_FULL_BIT);  // fifo full event no need to report

    if(GhGsMoveDetecterIsEnable())   //is in multi-sensor wear det mode
    {
        usEvent &= ~(GH3X2X_IRQ_MSK_WEAR_ON_BIT);   //ignore wear on event
    }
    usEvent &= (GH3X2X_IRQ_MSK_LEAD_ON_DET_BIT|GH3X2X_IRQ_MSK_LEAD_OFF_DET_BIT|GH3X2X_IRQ_MSK_WEAR_ON_BIT|GH3X2X_IRQ_MSK_WEAR_OFF_BIT|GH3X2X_IRQ_MSK_CHIP_RESET_BIT|GH3X2X_IRQ_MSK_FIFO_FULL_BIT);

		if((usEvent != 0)||(uchEventEx & (GH3X2X_EVENT_EX_BIT_CHIP0_LEAD_ON|GH3X2X_EVENT_EX_BIT_CHIP0_LEAD_OFF|GH3X2X_EVENT_EX_BIT_CHIP1_LEAD_ON|GH3X2X_EVENT_EX_BIT_CHIP1_LEAD_OFF)))
    {
        Gh3x2x_HalSerialWriteEventToFifo(usEvent,uchEventEx);
    }
}
#else
STGh3x2xProtocolData * const g_pstGh3x2xProtocolData = 0;

void Gh3x2xDemoSendProtocolData(GU8* puchProtocolDataBuffer, GU16 usProtocolDataLen)
{
    UNUSED_VAR(puchProtocolDataBuffer);
    UNUSED_VAR(usProtocolDataLen);
}

void Gh3x2x_HalSerialWriteDataToFifo(GU8 * lpubSource, GU8 lubLen)
{
    UNUSED_VAR(lpubSource);
    UNUSED_VAR(lubLen);
}
#endif


