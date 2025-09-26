/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_reg_array.c
 * 
 * @brief   gh3x2x driver lib reg config array
 * 
 * @author  Gooidx Iot Team
 * 
 */

#include "gh3x2x_demo_algo_config.h"
#include "gh_drv.h"

/// HR
const STGh3x2xReg gh3x2x_algo_reg_list0[] =
{
    {0x33C0, 0x0000}, {0x33C2, 0x0000}, {0x33C4, 0x0000}, {0x33C6, 0x0000}, {0x33C8, 0x0000}, {0x33CA, 0x0000},
    {0x33CC, 0x0000}, {0x33CE, 0x0000}, {0x33D0, 0x0000}, {0x33D2, 0x0000}, {0x36C0, 0x0000}, {0x36C2, 0x0000},
    {0x36C4, 0x0014}, {0x36C6, 0x0000}, {0x36C8, 0x000A}, {0x36CA, 0x0000}, {0x36CC, 0x0003}, {0x36CE, 0x0000},
    {0x36D0, 0x0003}, {0x36D2, 0x0000}, {0x42C0, 0x3D8D}, {0x42C2, 0x0000}, {0x42C4, 0x92D6}, {0x42C6, 0xFFFB},
    {0x42C8, 0x0FC6}, {0x42CA, 0x0011}, {0x42CC, 0x0001}, {0x42CE, 0x0000}, {0x42D0, 0x0000}, {0x42D2, 0x0000},
    {0x42D4, 0x001E}, {0x42D6, 0x0000}, {0x42D8, 0x000F}, {0x42DA, 0x0000}, {0x42DC, 0x0004}, {0x42DE, 0x0000},
    {0x42E0, 0x0003}, {0x42E2, 0x0000}, {0x42E4, 0x0000}, {0x42E6, 0x0000}, {0x42E8, 0x0000}, {0x42EA, 0x0000},
    {0xFFFF, 0x0001}
};

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 1)
const STGh3x2xReg gh3x2x_algo_reg_list1[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 1) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 2)
const STGh3x2xReg gh3x2x_algo_reg_list2[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 2) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 3)
const STGh3x2xReg gh3x2x_algo_reg_list3[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 3) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 4)
const STGh3x2xReg gh3x2x_algo_reg_list4[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 4) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 5)
const STGh3x2xReg gh3x2x_algo_reg_list5[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 5) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 6)
const STGh3x2xReg gh3x2x_algo_reg_list6[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 6) */

#if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 7)
const STGh3x2xReg gh3x2x_algo_reg_list7[] =
{
    {0,0},
};
#endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 7) */

const STGh3x2xInitConfig g_stGoodixAlgoCfgListArr[__GOODIX_ALGO_CFG_LIST_MAX_NUM__] = 
{
    {gh3x2x_algo_reg_list0,sizeof(gh3x2x_algo_reg_list0) / sizeof(gh3x2x_algo_reg_list0[0])},
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 1)
    {gh3x2x_algo_reg_list1,sizeof(gh3x2x_algo_reg_list1) / sizeof(gh3x2x_algo_reg_list1[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 1) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 2)
    {gh3x2x_algo_reg_list2,sizeof(gh3x2x_algo_reg_list2) / sizeof(gh3x2x_algo_reg_list2[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 2) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 3)
    {gh3x2x_algo_reg_list3,sizeof(gh3x2x_algo_reg_list3) / sizeof(gh3x2x_algo_reg_list3[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 3) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 4)
    {gh3x2x_algo_reg_list4,sizeof(gh3x2x_algo_reg_list4) / sizeof(gh3x2x_algo_reg_list4[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 4) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 5)
    {gh3x2x_algo_reg_list5,sizeof(gh3x2x_algo_reg_list5) / sizeof(gh3x2x_algo_reg_list5[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 5) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 6)
    {gh3x2x_algo_reg_list6,sizeof(gh3x2x_algo_reg_list6) / sizeof(gh3x2x_algo_reg_list6[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 6) */
    #if (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 7)
    {gh3x2x_algo_reg_list7,sizeof(gh3x2x_algo_reg_list7) / sizeof(gh3x2x_algo_reg_list7[0])},
    #endif /* (__GOODIX_ALGO_CFG_LIST_MAX_NUM__ > 7) */
};

