/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_version.h
 *
 * @brief   gh3x2x algo call demo code version
 *
 * @version ref gh3x2x_demo_algo_call_version.h
 *
 */

#ifndef _GH3X2X_DEMO_ALGO_CALL_VERSION_H_
#define _GH3X2X_DEMO_ALGO_CALL_VERSION_H_

#define   GOODIX_DEMO_MAJOR_VERSION_NUMBER        0      /**< major version number */
#define   GOODIX_DEMO_MINOR_VERSION_NUMBER        5      /**< minor version number */

#define   GOODIX_TO_STRING(x)       #x                      /**< number to char */
#define   GOODIX_STR(x)             GOODIX_TO_STRING(x)     /**< number to char */

/// makeup version string
#define   GOODIX_ALGO_CALL_VERSION_STRING     "Goodix_Algocall_"\
                                              "v"GOODIX_STR(GOODIX_DEMO_MAJOR_VERSION_NUMBER)\
                                              "."GOODIX_STR(GOODIX_DEMO_MINOR_VERSION_NUMBER)
#endif
