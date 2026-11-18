/*
* HRVLib.h
*/

#ifndef _GOODIX_SYS_HRV_LIB_H_
#define _GOODIX_SYS_HRV_LIB_H_

#include <stdint.h>
#include "iot_sys_def.h"
#if DEBUG_HRV
#include "..\..\HRVTest\goodix_debug.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 返回错误码 */
typedef enum {
    GX_HRV_ALGO_OK              = 0x00000000, // 成功返回，或无出值刷新
    GX_HRV_ALGO_FAILED          = 0x10000001, // 失败返回
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // 外部传入的指针为空
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // 参数范围无效
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // 内存分配失败
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // 释放失败
    GX_HRV_ALGO_UPDATE          = 0x10000006, // 有出值刷新
}goodix_hrv_ret;

#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

#define HRV_INTERFACE_VERSION "pv_v1.1.0"
#define HRV_INTERFACE_VERSION_LEN_MAX (20)

typedef struct {
    int32_t need_ipl;                                   // 是否需要插值到1KHz
    uint32_t fs;                                         // 采样率
    int32_t acc_thr[ACC_THR_NUM];                       // acc 相关阈值，默认值为：20/10/3/3
}goodix_hrv_config;

typedef struct
{
	uint8_t hr;			  // 外部输入心率
}goodix_hrv_input_unique;

typedef struct
{
	uint32_t frame_id;			  // 帧序号
	uint8_t total_ch_num;         // 灌入算法的通道总数
	uint8_t *enable_flg;		  // ①指示ppg_rawdata中的每路ppg信号的有效性，从左到右，每个bit代表1路ppg信号；②需设置total_chl_num*3路bit，共占用ceil(total_chl_num*3/8)各字节
	int32_t *ppg_rawdata;		  // PPG原始数据，根据total_chl_num分组。存放顺序，依次green[ch1-total_chl_num]，ir[ch1-total_chl_num]，red[ch1-total_chl_num]
	uint8_t *ch_agc_gain;	      //  gain值,顺序与ppg保持一致
	uint16_t *ch_agc_drv;		  //  驱动电流,顺序与ppg保持一致
	int32_t acc_x;				  // 加速度计x轴
	int32_t acc_y;				  // 加速度计y轴
	int32_t acc_z;				  // 加速度计z轴
	int32_t groy_x;				  // 陀螺仪x轴
	int32_t groy_y;				  // 陀螺仪y轴
	int32_t groy_z;				  // 陀螺仪z轴

	uint8_t sleep_flg;            // 睡眠flg
	uint8_t bit_num;              // 数据位数
	uint8_t chip_type;            // 芯片类型
	uint8_t data_type;            // 数据类型

	void *pst_module_unique;      //心率特有输入
}goodix_hrv_input_rawdata;

typedef struct {
    int32_t rri[RRI_NUM];                               // RRI结果数组
    int32_t rri_confidence;                             // 结果置信度
	int32_t rri_valid_num;                              // RRI有效结果数量

	int32_t reserved1;					// 备用三个接口
	int32_t reserved2;
	int32_t reserved3;
}goodix_hrv_result;

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_version(uint8_t version[120]);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_calc(goodix_hrv_input_rawdata *raw, goodix_hrv_result *res);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_deinit(void);

/**
* @brief 获取hrv的默认配置参数
* @return     hrv的默认配置参数
*/
IOT_SYS_DLL_API const void *goodix_hrv_config_get_arr(void);

/**
* @brief 获取hrv的配置size
* @return     hrv的配置size
*/
IOT_SYS_DLL_API int32_t  goodix_hrv_config_get_size(void);

/**
* @brief 获取HRV配置文件的版本号
* @param[in/out]  ver       版本号保存数组
* @param[in] 	  ver_len   版本号长度，建议使用HRV_INTERFACE_VERSION_LEN_MAX
* @return     	  无
*/
IOT_SYS_DLL_API void goodix_hrv_config_get_version(char* ver, uint8_t ver_len);


/**
* @brief 用户调用打印HRV重要信息，需要提前初始化打印函数
* @param[in]  raw    心率输入数据，NULL == raw时，不打印数据
* @return     	  无
*/
IOT_SYS_DLL_API void goodix_hrv_essential_info_print(goodix_hrv_input_rawdata* raw);

#if DEBUG_HRV
IOT_SYS_DLL_API goodix_hrv_debug *copy_debug_addr(void);
#endif
#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_HRV_LIB_H_
