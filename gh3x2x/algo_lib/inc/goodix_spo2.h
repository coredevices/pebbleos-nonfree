#ifndef __GOODIX_SPO2_INTERFACE_H
#define __GOODIX_SPO2_INTERFACE_H

#include <stdint.h>
#include "goodix_type.h"

#define SPO2_INTERFACE_VERSION   "pre_pv_v1.1.0"
#define SPO2_INTERFACE_VERSION_LEN_MAX  (20)
/**
* @brief SPO2 算法函数返回状态值
*/
typedef enum
{
	GX_ALGO_SPO2_SUCCESS = 0x00000000,			/**< 成功     */
	GX_ALGO_SPO2_RWONG_INPUT = 0x00000001,		/**< 输入数据格式不合法     */
	GX_ALGO_SPO2_NO_MEMORY = 0x00000002,        /**< 内存空间不够     */
	GX_ALGO_SPO2_FRAME_UNCOMPLETE = 0x00000003, /**< 处于降采样平均过程中*/
	GX_ALGO_SPO2_WIN_UNCOMPLETE = 0x00000004,   /**< 未到完整滑窗帧位置（整数s）     */
	GX_ALGO_SPO2_UNEXPECTED = 0x00000005,       /*不合设计逻辑异常 */
	GX_ALGO_SPO2_CFG_ERROR = 0x00000006,        /*配置参数校验不通过*/
	GX_ALGO_SPO2_INIT_FAILURE = 0x00000007,     /*初始化失败*/
}goodix_spo2_ret;



typedef struct {
	//raw 配置信息
	uint32_t valid_chl_num;			// 有效通道数
	uint32_t raw_fs;			    // 原始采样率

	int32_t cali_coef_a4;							//校准参数4次项
	int32_t cali_coef_a3;							//校准参数3次项
	int32_t cali_coef_a2;							//校准参数2次项
	int32_t cali_coef_a1;							//校准参数1次项
	int32_t cali_coef_a0;							//校准参数常数项
	//hb使能标志
	uint32_t hb_en_flg;					//hb使能标志
	//佩戴状态
	uint32_t wear_mode;					//佩戴状态 0:手环  1：手指  2：耳机
	// acc move thr
	uint32_t acc_thr_max;
	uint32_t acc_thr_min;
	uint32_t acc_thr_scale;
	uint32_t acc_thr_num;
	uint32_t acc_thr_angle;
	//CTR设置
	uint32_t ctr_en_flg;
	uint32_t ctr_red_thr;

	//信号质量评估相关阈值
	uint8_t ppg_jitter_thr;
	uint8_t ppg_noise_thr;
	uint8_t ppg_coeff_thr;
	uint8_t quality_module_key;

	//出值策略
	uint8_t low_spo2_thr;
	uint8_t fast_out_time;
	uint8_t slow_out_time;
	uint8_t min_stable_time_high;
	uint8_t min_stable_time_low;
	uint8_t max_spo2_variation_high;
	uint8_t max_spo2_variation_low;
	uint8_t ouput_module_key;
}goodix_spo2_config;



typedef struct
{
	uint32_t frame_id;	        // 帧序号
	uint8_t  total_ch_num;             //灌入数据的通道数（1个通道同时包含绿光、红外、红光数据）
	uint8_t *enable_flg;		   // 通道使能标志位， 依次绿光1234-红外1234-红光1234
	int32_t *ppg_rawdata;		 // PPG原始数据，依次绿光1234-红外1234-红光1234
	uint8_t *ch_agc_gain;	    //  gain值1-依次绿光1234-红外1234-红光1234
	uint16_t *ch_agc_drv;		//  驱动电流0-依次绿光1234-红外1234-红光1234

	int32_t acc_x;				//加速度计x轴
	int32_t acc_y;				//加速度计y轴
	int32_t acc_z;				//加速度计z轴
	int32_t groy_x;
	int32_t groy_y;
	int32_t groy_z;

	uint8_t sleep_flg;
	uint8_t bit_num;            //数据位数
	uint8_t chip_type;
	uint8_t data_type;
	void *pst_module_unique;
}goodix_spo2_input_rawdata;

typedef struct
{
	/*
	int32_t spo2[CHIP_PPG_CHL_NUM];				//血氧值*10000
	int32_t valid_level[CHIP_PPG_CHL_NUM];		//置信等级
	int32_t confi_coeff[CHIP_PPG_CHL_NUM];		//置信度
	int32_t WeightR[CHIP_PPG_CHL_NUM];		//置信度

	//HB
	int32_t hb_mean[CHIP_PPG_CHL_NUM];
	int32_t hb_confi_lvl[CHIP_PPG_CHL_NUM];

	int32_t invalidFlg[CHIP_PPG_CHL_NUM];
	int32_t piIR[CHIP_PPG_CHL_NUM];
	int32_t piRed[CHIP_PPG_CHL_NUM];
	int32_t snrIR[CHIP_PPG_CHL_NUM];
	int32_t snrRed[CHIP_PPG_CHL_NUM];
	int32_t r_val[CHIP_PPG_CHL_NUM];				//r值（*10000）
	int32_t calc_flg[CHIP_PPG_CHL_NUM];
	*/

	//**********************四通道输出最终值***************************//
	int32_t final_spo2;
	int32_t final_valid_level;
	int32_t final_confi_coeff;
	int32_t final_WeightR;

	int32_t final_hb_mean;
	int32_t final_hb_confi_lvl;

	int32_t final_invalidFlg;
	int32_t final_piIR;
	int32_t final_piRed;
	int32_t final_snrIR;
	int32_t final_snrRed;
	int32_t final_r_val;
	int32_t final_calc_flg;
	int32_t reserved1;
	int32_t reserved2;
	int32_t reserved3;
}goodix_spo2_result;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief SPO2 处理算法版本号
	* @param[in]  version     外部储存版本号空间，分配20字节
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_version(uint8_t version[150]);

	/**
	* @brief SPO2 处理算法初始化
	* @param[in]  cfg_instance   外部传入的配置参数结构体
	* @param[in]  cfg_size       配置结构体大小，用于校验外部接口文件是否对齐
	* @param[in]  interface_ver	 接口版本号，用于校验外部接口文件是否对齐
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_init(const void *cfg_instance, int32_t cfg_size, char *interface_ver);

	/**
	* @brief SPO2 血氧计算
	* @param[in]  raw		输入原始数据结构体
	* @param[out] res		输出结果结构体
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_calc(goodix_spo2_input_rawdata* raw, goodix_spo2_result* res);


	/**
	* @brief 供客户调试对齐时调用，打印算法启动运行后的一些关键变量信息（在goodix_spo2_calc后调用）
	* @param[in]  raw		输入原始数据结构体，NULL==rawdata时，不打印数据信息
	* @param[out] 		    无
	* @return               无
	*/
	DRVDLL_API void goodix_spo2_essential_info_print(goodix_spo2_input_rawdata* raw);
	/**
	* @brief SPO2 算法销毁，释放内部的内存空间，调用其他模块算法时使用
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_deinit(void);


	/**
	* @brief SPO2 配置参数获取，初始化前调用，导入存放配置结构体内容的内存
	* @param[out] 无
	* @return     指向配置结构体实例的指针
	*/
	const void *goodix_spo2_config_get_instance(void);

	/**
	* @brief SPO2 获取配置参数结构体的内存空间大小，初始化前调用，用于算法内部进行检验
	* @param[out] 无
	* @return     配置参数占用的内存空间大小
	*/
	int32_t goodix_spo2_config_get_size(void);

	/**
	* @brief SPO2 获取接口文件的版本，初始化前调用，用于算法内部进行检验（防止外部编译时接口文件定义未对齐）
	* @param[out] 无
	* @return     无
	*/
	void goodix_spo2_config_get_version(char *ver, uint8_t ver_len);

#ifdef __cplusplus
}
#endif

#endif
