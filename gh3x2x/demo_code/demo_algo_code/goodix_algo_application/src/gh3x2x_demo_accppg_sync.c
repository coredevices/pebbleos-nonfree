#include <stdio.h>
//#include "gh3x2x_demo_inner.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_accppg_sync.h"
#include "gh3x2x_demo_common.h"

GU8 GH3X2X_TimestampSyncGetFrameDataFlag(void)
{
    return 1;
}

void GH3X2X_TimestampSyncSetPpgIntFlag(GU8 uchPpgIntFlag)
{
}

void GH3X2X_TimestampSyncAccInit(void)
{
}

void GH3X2X_TimestampSyncPpgInit(GU32 unFunctionID)
{
}

void GH3X2X_TimestampSyncSend2AlgoCall(GU8 uchFuncOffsetID)
{
}

void GH3X2X_TimestampSyncFillAccSyncBuffer(GU32 unTimeStamp,
                                           GS16 usAccX,
                                           GS16 usAccY,
                                           GS16 usAccZ)
{
}

void GH3X2X_TimestampSyncFillPpgSyncBuffer(GU32 unTimeStamp,
                                           const STGh3x2xFrameInfo * const pstFrameInfo)
{
}

