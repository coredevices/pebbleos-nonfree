/*
* hrv_type_def.h
*/

#ifndef _GOODIX_IOT_SYS_TYPE_DEF_H_
#define _GOODIX_IOT_SYS_TYPE_DEF_H_

#include <stdint.h>
#include <stddef.h>

typedef unsigned char            GBOOL;
typedef          char            GCHAR;
typedef unsigned char            GUCHAR;
typedef signed   char            GS8;
typedef unsigned char            GU8;

typedef signed   short           GS16;
typedef unsigned short           GU16;

typedef signed   int             GS32;
typedef unsigned int             GU32;

typedef signed   long long       GS64;
typedef unsigned long long       GU64;

typedef          float           GF32;
typedef          double          GF64;
typedef long     double          GF128;

#if defined(_WIN32) || defined(_WIN64)
#define DEBUG_HRV  1
#endif

/* 打印开关 */
#define IOT_SYS_DEBUG 1

#if defined(_WIN32) || defined(_WIN64)
#define USE_MEMPOOL 1
#else
#define USE_MEMPOOL 0
#endif
#define UPDATEMEMINFO()
#if defined(_WIN32) || defined(_WIN64)

#if USE_MEMPOOL
#ifdef UPDATEMEMINFO()
#undef UPDATEMEMINFO()
#endif
#include "goodix_memusage.h"
#if DEBUG_MEMUSAGE
#define IOT_SYS_REALLOC(p,x)    MEMSTAT_REALLOC(p,x)
#define IOT_SYS_MALLOC(x)       MEMSTAT_MALLOC(x)
#define IOT_SYS_FREE(x)         MEMSTAT_FREE(x)
#define IOT_SYS_MEM_INIT(p,x)   GD_MEM_INIT((p),(x)); MEMSTAT_INIT(p);
#define IOT_SYS_MEM_DESTORY()   GD_MEM_DESTORY();     MEMSTAT_COMP();
#else
#define IOT_SYS_MEM_INIT(p,x)   GD_MEM_INIT((p),(x));
#define IOT_SYS_MEM_DESTORY()   GD_MEM_DESTORY();
#define IOT_SYS_MALLOC(x)       GD_MEM_MALLOC(x)
#define IOT_SYS_FREE(x)         GD_MEM_FREE(x)
#define IOT_SYS_REALLOC(p,x)    GD_MEM_REALLOC(p,x)
#endif

#else  //USE_MEMPOOL
#include <malloc.h>
#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC          malloc
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE            free
#endif
#endif //!USE_MEMPOOL

#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API         extern _declspec(dllexport)
#endif

#if IOT_SYS_DEBUG
#include <stdio.h>
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT           printf
#endif
#endif

#elif defined(__linux__) || defined(ANDROID)
#include <malloc.h>
#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC          malloc
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE            free
#endif
#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API
#endif

#ifndef __ANDROID_NATIVE__ // for app
#include <android/log.h>
#ifndef IOT_SYS_LOG_TAG
#define IOT_SYS_LOG_TAG "IoTSys"
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT(...)       __android_log_print(ANDROID_LOG_DEBUG, IOT_SYS_LOG_TAG, __VA_ARGS__)
#endif
#endif
#else // for exe on cell phone
#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT            printf
#endif
#endif

#else
/*
* Roma-II
*/
//extern void *GmAlgMemMalloc(GU32 unSize);
//extern void GmAlgMemFree(void *pmemaddr);
//
//#define IOT_SYS_MALLOC(x)        GmAlgMemMalloc((x))
//#define IOT_SYS_FREE(p)          GmAlgMemFree((p))
//#define IOT_SYS_DLL_API

/*
* Cardiff
*/
extern void* goodix_mem_malloc(int32_t size);
extern void goodix_mem_free(void* mem_addr);

#ifndef IOT_SYS_MALLOC
#define IOT_SYS_MALLOC(x)        goodix_mem_malloc((x))
#endif
#ifndef IOT_SYS_FREE
#define IOT_SYS_FREE(p)          goodix_mem_free((p))
#endif
#ifndef IOT_SYS_DLL_API
#define IOT_SYS_DLL_API
#endif

#if IOT_SYS_DEBUG
/* extern log function */
extern void GH3X2X_Log(char *chLogString);

/// log support len
#define GH3X2X_LOG_DEBUG_SUP_LEN     (128)

/// log string with param
#define   GH3X2X_DEBUG_LOG_PARAM(...)       do {\
                                                char chDebugStr[GH3X2X_LOG_DEBUG_SUP_LEN] = {0};\
                                                snprintf(chDebugStr, GH3X2X_LOG_DEBUG_SUP_LEN, \
                                                        "[gh3x2x_drv]: "__VA_ARGS__);\
                                                GH3X2X_Log(chDebugStr);\
                                            } while (0)

#ifndef IOT_SYS_PRINT
#define IOT_SYS_PRINT        GH3X2X_DEBUG_LOG_PARAM
#endif
#endif

#endif

// 编译宏定义
#define SINGLE_PRECISION_SUPPORT    // 编译成库时使能，在VS工程中，影响 LMS_FILTER_STEP_FACTOR，导致VS和MCU对不齐

/********END OF FILE********* (C) COPYRIGHT 2015-2025 .********/
#endif  //_GOODIX_IOT_SYS_TYPE_DEF_H_
