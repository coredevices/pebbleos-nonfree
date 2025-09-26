/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh_movedetect.h
 *
 * @brief   move detect
 *
 * @version ref gh_drv_version.h
 *
 */


#ifndef _GH_MOVEDETECT_H_
#define _GH_MOVEDETECT_H_

#include "gh_drv.h"

void GetGsensorMoveEventCheckIni(GU16 usAccThrMax, GU16 usAccThrMin,GU16 usAccThrScale,GU16 usAccThrNum);
GU8 GetGsensorMoveEventCheckExe(GS16* pusGsData);

#endif /* _GH_MOVEDETECT_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/

