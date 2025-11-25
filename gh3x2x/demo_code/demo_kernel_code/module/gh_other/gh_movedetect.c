/**
* @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
*
* @file    gh_movedetect.c
*
* @brief   gh move detect functions
*
* @version ref gh3x2x_drv_version.h
*
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gh_demo_config.h"
#include "gh_demo_inner.h"
#include "gh_drv.h"
#include "gh_movedetect.h"


//-----------------------------------------------------
typedef struct
{
    GS16 sXAxisVal;     /**< x-axis value */
    GS16 sYAxisVal;     /**< y-axis value */
    GS16 sZAxisVal;     /**< z-axis value */
} STGsAccRawdata;

STGsAccRawdata g_LastPointAdtGsenorRawdata;

typedef struct
{
    GS16 sXAxisVal;     /**< x-axis value */
    GS16 sYAxisVal;     /**< y-axis value */
    GS16 sZAxisVal;     /**< z-axis value */
    GS16 sAccVal;       /**< acc-axis value */
    GS16 sLastAccVal;       /**< acc-axis value */
    GS16 sAccDifBuf[125];
    GU16  sAccThrNum;       /**< acc-axis value */
    GU16  sAccThrMax;       /**< acc-axis value */
    GU16  sAccThrMin;       /**< acc-axis value */
    GU16  sAccThrScale;       /**< acc-axis value */
    GU8  uchGsensorDifBufIndex;
} STGsAccMoverEventCheck;

STGsAccMoverEventCheck g_MoveEventCheckStruct;
GU16 g_bufferlen;
GS32 g_siLastDirectionAngle = 0;

/**
* @fn      GetGsensorMoveEventCheckIni(GU8 usAccThrMax, GU8 usAccThrMin,GU8 usAccThrScale,GU8 usAccThrNum)
*
* @brief  initialzation of GetGsensorMoveEventCheckExe.
*
* @attention   None
*
* @param[in]   Thr param
* @param[out]   none
*
* @return  none
*/

void GetGsensorMoveEventCheckIni(GU16 usAccThrMax, GU16 usAccThrMin, GU16 usAccThrScale, GU16 usAccThrNum)
{
    GH3X2X_Memset(g_MoveEventCheckStruct.sAccDifBuf, 0, 125 * sizeof(GS16));
    g_MoveEventCheckStruct.sAccThrMax = usAccThrMax;
    g_MoveEventCheckStruct.sAccThrMin = usAccThrMin;
    g_MoveEventCheckStruct.sAccThrNum = usAccThrNum;
    g_MoveEventCheckStruct.sAccThrScale = usAccThrScale;
    g_MoveEventCheckStruct.sAccVal = 0;
    g_MoveEventCheckStruct.sLastAccVal = 0;
    g_MoveEventCheckStruct.sXAxisVal = 0;
    g_MoveEventCheckStruct.sYAxisVal = 0;
    g_MoveEventCheckStruct.sZAxisVal = 0;
    g_MoveEventCheckStruct.uchGsensorDifBufIndex = 0;
    g_bufferlen = 125;
    g_siLastDirectionAngle = 0;
}

/**
* @fn      GU8 GetGsensorMoveEventCheckExe(GS16* pusGsData)

*
* @brief  Get the movement event by software.
*
* @attention   None
*
* @param[in]   pusGsData         gsensor data buffer
* @param[out]  mN >= g_MoveEventCheckStruct.sAccThrNum
*
* @return  //bit0 move flag  bit1 direct abnormal flag
*/
GU8 GetGsensorMoveEventCheckExe(GS16* pusGsData)
{
    GS32 sRmsAcc = 0.0;
    GF32 fMeanAcc = 0.0;
    GF32 fStdAcc = 0.0;
    GF32 fSumX = 0.0;
    GF32 fSumX2 = 0.0;
    GU8 mN = 0;
    GU8 lubMoveFlag = 0; //bit0 move flag  bit1 direct abnormal flag
    GS32 lsiDirectionAngle = 0;
    GU8 uchFirstFrameFlag = 0;
    g_MoveEventCheckStruct.sXAxisVal = pusGsData[0];
    g_MoveEventCheckStruct.sYAxisVal = pusGsData[0 + 1];
    g_MoveEventCheckStruct.sZAxisVal = pusGsData[0 + 2];

    sRmsAcc = sqrt((GS32)g_MoveEventCheckStruct.sXAxisVal*g_MoveEventCheckStruct.sXAxisVal + (GS32)g_MoveEventCheckStruct.sYAxisVal*g_MoveEventCheckStruct.sYAxisVal + (GS32)g_MoveEventCheckStruct.sZAxisVal*g_MoveEventCheckStruct.sZAxisVal) / 3;

    if (0 == g_MoveEventCheckStruct.uchGsensorDifBufIndex)
    {
        uchFirstFrameFlag = 1;
        g_MoveEventCheckStruct.sAccDifBuf[g_MoveEventCheckStruct.uchGsensorDifBufIndex] = 0;
        ++g_MoveEventCheckStruct.uchGsensorDifBufIndex;
    }
    else
    {
        uchFirstFrameFlag = 0;
        if (g_MoveEventCheckStruct.uchGsensorDifBufIndex < g_bufferlen)
        {
            g_MoveEventCheckStruct.sAccDifBuf[g_MoveEventCheckStruct.uchGsensorDifBufIndex] = (GS16)sRmsAcc - g_MoveEventCheckStruct.sLastAccVal;
            ++g_MoveEventCheckStruct.uchGsensorDifBufIndex;
        }
        else
        {
            memmove(g_MoveEventCheckStruct.sAccDifBuf, g_MoveEventCheckStruct.sAccDifBuf + 1, (g_bufferlen - 1)*sizeof(GS16));
            g_MoveEventCheckStruct.sAccDifBuf[g_bufferlen - 1] = (GS16)sRmsAcc - g_MoveEventCheckStruct.sLastAccVal;
        }
    }
    g_MoveEventCheckStruct.sLastAccVal = sRmsAcc;

    for (GU8 i = 0; i < g_MoveEventCheckStruct.uchGsensorDifBufIndex; i++)
    {
        fSumX = fSumX + g_MoveEventCheckStruct.sAccDifBuf[i];
        fSumX2 = fSumX2 + (GF32)g_MoveEventCheckStruct.sAccDifBuf[i] * g_MoveEventCheckStruct.sAccDifBuf[i];
    }
    fMeanAcc = fSumX / g_MoveEventCheckStruct.uchGsensorDifBufIndex;
    fStdAcc = sqrtf(((fSumX2 / g_MoveEventCheckStruct.uchGsensorDifBufIndex - fMeanAcc*fMeanAcc) >(0) ? (fSumX2 / g_MoveEventCheckStruct.uchGsensorDifBufIndex - fMeanAcc*fMeanAcc) : (0)));

    GF32 ftemp = ((g_MoveEventCheckStruct.sAccThrScale*fStdAcc) > (g_MoveEventCheckStruct.sAccThrMin) ? (g_MoveEventCheckStruct.sAccThrScale*fStdAcc) : (g_MoveEventCheckStruct.sAccThrMin));
    GF32 fThr = ((g_MoveEventCheckStruct.sAccThrMax) < (ftemp) ? (g_MoveEventCheckStruct.sAccThrMax) : (ftemp));

    for (GU8 i = 0; i < g_MoveEventCheckStruct.uchGsensorDifBufIndex; i++)
    {
        if ((GF32)g_MoveEventCheckStruct.sAccDifBuf[i] - fMeanAcc > fThr || (GF32)fMeanAcc - g_MoveEventCheckStruct.sAccDifBuf[i] > fThr)
        {
            mN++;
        }
    }
    //return (mN >= g_MoveEventCheckStruct.sAccThrNum);
    if (mN >= g_MoveEventCheckStruct.sAccThrNum)
    {
        lubMoveFlag |= (((GU8)1) << 0);   //set bit0 : move flag
    }


    //checak calculate direct is abnormal or not
    lsiDirectionAngle = atan(sqrt(((GS32)g_MoveEventCheckStruct.sXAxisVal*g_MoveEventCheckStruct.sXAxisVal + (GS32)g_MoveEventCheckStruct.sYAxisVal*g_MoveEventCheckStruct.sYAxisVal)) / g_MoveEventCheckStruct.sZAxisVal) * 180 / 3.1416;
    if (uchFirstFrameFlag)  // first frame
    {
        g_siLastDirectionAngle = lsiDirectionAngle; //use the rawdata
    }
    else  // do IIR filter
    {
        g_siLastDirectionAngle = (lsiDirectionAngle * 1 + g_siLastDirectionAngle * 3) / 4;  // current data weight is 0.25
    }
    if ((g_siLastDirectionAngle > 40) || (g_siLastDirectionAngle < 0)) // angle is too big or  watch screen is downward
    {
        lubMoveFlag |= (((GU8)1) << 1);   //set bit1 : abnormale angel
    }

    return lubMoveFlag;
}


//source code
#if 0
void main()
{
    GU16 usAccThrMax=30;
    GU16 usAccThrMin=15;
    GU16 usAccThrScale=4;
    GU16 usAccThrNum=3;
    GetGsensorMoveEventCheckIni(usAccThrMax, usAccThrMin, usAccThrScale, usAccThrNum);

    GS16 pusGsData[3] = {0};
    pusGsData[0] = 1; //x
    pusGsData[1] = 2; //y
    pusGsData[2] = 512; //z
    GU8 result = GetGsensorMoveEventCheckExe(pusGsData);

}
#endif
