#ifndef __GOODIX_ECG_H
#define __GOODIX_ECG_H

#include "stdint.h"
#include "goodix_type.h"


#define ECG_INTERFACE_VERSION               "pv_v1.0.0"
#define ECG_INTERFACE_VERSION_LEN_MAX       (20)
#define ECG_CHANNEL_NUM                     (2)

/**
* @brief ECG 算法函数返回状态值
*/
typedef enum
{
    GX_ALGO_ECG_SUCCESS = 0x00000000,               /**< 成功     */	

    GX_ALGO_ECG_RWONG_INPUT = 0x00000001,           /**< 输入数据格式不合法     */

    GX_ALGO_ECG_NO_MEMORY = 0x00000002,             /**< 内存空间不够     */

}goodix_ecg_ret;


typedef struct 
{
    uint32_t fs;                                    // 采样率
    uint32_t valid_channel_num;                     // 有效通道数
    uint32_t qrs_flag;                              // QRS检测flag，0：不做房颤检测
    uint32_t mode_type;                             // ECG耦合方式，0: AC mode,1: DC mode
    uint32_t ic_type;                               // 芯片类型，0：gh3x2x，1：gh3300
}goodix_ecg_config;


typedef struct
{
    uint32_t out_delay;                             //滤波延迟
}goodix_ecg_init_result;


typedef struct
{
    uint32_t frameid;                               //帧序号
    int32_t *ecg_rawdata;                           //原始数据
    int32_t acc_x;                                  //加速度计x轴
    int32_t acc_y;                                  //加速度计y轴
    int32_t acc_z;                                  //加速度计z轴
    int32_t ecg_qc_flag;                            //快速恢复标记
    int32_t pace;                                   //pace数据
    uint8_t pace_en_flag;                           //pace有效标志位
    uint8_t data_type;                              //数据类型，0：ADC，1：电压
}goodix_ecg_input_rawdata;

typedef struct
{
    float32_t *ecg_out;                 //心电值
    goodix_stage *ecg_stage;            //P-QRS-J-T 点状态
    float32_t ecg_bpm;

    float32_t *ecg_snr;                  //信噪比
    float32_t *ecg_powerline_out;        // 滤除工频干扰 输出
    float32_t *ecg_blw_out;              // 滤除基线漂移 输出
    float32_t *ecg_bpm_ch;               //心率值
}goodix_ecg_result;


#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief ECG 处理算法版本号
* @param[in]  version     外部储存版本号空间，分配20字节
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_version(uint8_t version[100]);


/**
* @brief ECG 处理算法初始化
* @param[in]  cfg         配置信息
* @param[in]  cfg_size    配置信息大小
* @param[out] res         返回的结果
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_init(const void *cfg_instance, goodix_ecg_init_result *res, int32_t cfg_size, const char *interface_ver);


/**
* @brief ECG 算法更新
* @param[in] raw    ECG 输入的相关信息
* @param[out] res    ECG 返回的结果
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_update(goodix_ecg_input_rawdata* raw, goodix_ecg_result *res);

/**
*@brief ECG 病例分类
* @param[in] 无
* @param[out] res    ECG 分类结果
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_classification_update(goodix_classification_flag *res);

/**
* @brief ECG 算法销毁，释放内部的内存空间，调用其他模块算法时使用
* @param[out] 无 
* @return     函数执行情况
*/
DRVDLL_API goodix_ecg_ret goodix_ecg_deinit(void);

/**
* @brief ECG  配置参数获取，初始化前调用，导入配置结构体内存
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API const void *goodix_ecg_config_get_arr(void);

/**
* @brief ECG  配置参数大小获取，初始化前调用，用于算法内部进行校验
* @param[out] 无
* @return     导入的配置参数大小
*/
DRVDLL_API int32_t goodix_ecg_config_get_size(void);

/**
* @brief ECG  配置版本号获取，初始化前调用，用于算法内部进行校验
* param[in] 版本号长度
* @param[out] 用于存放config的版本号数组
* @return     无
*/
DRVDLL_API void goodix_ecg_config_get_version(char* ver, uint8_t ver_len);

#ifdef __cplusplus
}
#endif

#endif
