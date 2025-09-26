#ifndef __GOODIX_TYPE
#define __GOODIX_TYPE

// 与零比较的公共阈值
#ifndef EPS
#define EPS            0.000001f
#endif

// 输出 lib 用
#if defined(_WIN32) || defined(_WIN64)
#define  DRVDLL_API    extern  _declspec(dllexport)
#else
#define  DRVDLL_API
#endif

// 类型重定义
#if defined(_WIN32) || defined(_WIN64)
typedef signed char          int8_t;
typedef short                int16_t;
typedef int                  int32_t;
typedef long long            int64_t;
typedef unsigned char        uint8_t;
typedef unsigned short       uint16_t;
typedef unsigned int         uint32_t;
typedef unsigned long long   uint64_t;
#else
#include <stdint.h>
#endif
#ifndef _ARM_MATH_H
typedef float                float32_t;
typedef double               float64_t;
#endif

typedef unsigned char   GBOOL;
typedef          char   GCHAR;
typedef unsigned char   GUCHAR;
typedef signed   char   GS8;
typedef unsigned char   GU8;

typedef signed   short  GS16;
typedef unsigned short  GU16;

typedef signed   int        GS32;
typedef unsigned int        GU32;

typedef signed   long long  GS64;
typedef unsigned long long  GU64;

typedef          float      GF32;
typedef          double     GF64;
typedef long     double     GF128;

#include <stddef.h>
#include <string.h>

typedef enum {
    False = 0,
    True = 1,
}goodix_flag_e;

// 当前点所处的状态：算法内部检测【目前输出 R 点】
typedef enum {
    // 输出值是否有效
    stage_noneffect_out = 0,

    // 默认值
    stage_default,
    

    // 快速恢复相关
    //stage_former_begin,
    //stage_former_top,
    //stage_former_downing,

    // 波形特征点
    stage_pqrst_p,
    stage_pqrst_q,
    stage_pqrst_r,
    stage_pqrst_j,
    stage_pqrst_j60,
    stage_pqrst_s,
    stage_pqrst_t_onset,
    stage_pqrst_t,
    stage_pqrst_t_offset,
    stage_pqrst_u,
}goodix_stage;

// ECG信号病例分类标志
typedef enum {
    FLAG_NORMAL,                // ECG信号检测为正常
    FLAG_ATRIAL_FIBRILLATION,   // ECG信号检测为房颤
    FLAG_OTHER,                 // ECG信号检测为其它
    FLAG_NOISE,                 // ECG信号检测为噪声
}goodix_classification_flag;

#endif // !__GOODIX_TYPE
