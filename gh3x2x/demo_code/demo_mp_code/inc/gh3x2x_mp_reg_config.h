/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_reg_config.h
 *
 * @brief   header file of gh3x2x_mp_reg_config.c
 *
 * @author  Goodix Product Tools Development Team
 *
 */
#ifndef _GH3X2X_MP_REG_CONFIG_H_
#define _GH3X2X_MP_REG_CONFIG_H_

#include "gh3x2x_mp_config.h"

typedef struct
{
    GU16 usRegAddr;
    GU16 usRegValue;
	
}stGh3x2xReg;


extern const stGh3x2xReg stGh3x2xMpRegConfigDefault[24];
extern const stGh3x2xReg stGh3x2xMpRegPPGConfig[15];
extern const stGh3x2xReg stGh3x2xMpRegECGLeadConfg[41];
extern const stGh3x2xReg stGh3x2xMpRegECGConfgDefault[23];

#endif /* _GH3X2X_MP_REG_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
