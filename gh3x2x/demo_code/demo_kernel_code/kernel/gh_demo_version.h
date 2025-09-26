/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_version.h
 *
 * @brief   gh3x2x demo code version
 *
 * @version ref gh3x2x_demo_version.h
 *
 */

#ifndef _GH3X2X_DEMO_VERSION_H_
#define _GH3X2X_DEMO_VERSION_H_

#include "gh_demo_config.h"

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#include "gh3x2x_demo_algo_call_version.h"
#endif

#define   GH3X2X_DEMO_MAJOR_VERSION_NUMBER        1      /**< major version number */
#define   GH3X2X_DEMO_MINOR_VERSION_NUMBER        6      /**< minor version number */

#define   GH3X2X_TO_STRING(x)       #x                      /**< number to char */
#define   GH3X2X_STR(x)             GH3X2X_TO_STRING(x)     /**< number to char */

#if (__DRIVER_LIB_MODE__ == __DRV_LIB_WITH_ALGO__)
#define   GH3X2X_ALGOCALL_EXTRA_VERSION_STRING      "_AC_"\
                                                    "v"GH3X2X_STR(GOODIX_DEMO_MAJOR_VERSION_NUMBER)\
                                                    "."GH3X2X_STR(GOODIX_DEMO_MINOR_VERSION_NUMBER)
#else
#define   GH3X2X_ALGOCALL_EXTRA_VERSION_STRING      "noAlgo"
#endif

/// makeup version string
#define   GH3X2X_DEMO_VERSION_STRING    "GH(M)3X2X_DEMO_"\
                                        "v"GH3X2X_STR(GH3X2X_DEMO_MAJOR_VERSION_NUMBER)\
                                        "."GH3X2X_STR(GH3X2X_DEMO_MINOR_VERSION_NUMBER)\
                                        GH3X2X_ALGOCALL_EXTRA_VERSION_STRING\
                                        "(build:"__DATE__"_"__TIME__")"
#endif
