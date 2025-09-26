#ifndef Goodix_DSP_EXPORTS

#include "goodix_spo2.h"  
const goodix_spo2_config external_cfg =
{
	//raw 配置信息
	.valid_chl_num = 1,			// 有效通道数
	.raw_fs = 100,			    // 原始采样率
	//校准参数
	.cali_coef_a4 = 0,			//校准参数4次项
	.cali_coef_a3 = 0,			//校准参数3次项
	.cali_coef_a2 = 0,		    //校准参数2次项
	.cali_coef_a1 = -239878,    //校准参数1次项
	.cali_coef_a0 = 1111827,	//校准参数常数项
	//hb使能标志
	.hb_en_flg = 1,			    //hb使能标志
	//佩戴状态
	.wear_mode = 0,			    //佩戴状态 0:手环  1：手指  2：耳机
	// acc move thr
	.acc_thr_max = 50,
	.acc_thr_min = 25,
	.acc_thr_scale = 3,
	.acc_thr_num = 3,
	.acc_thr_angle = 90,
	//CTR设置
	.ctr_en_flg = 0,
	.ctr_red_thr = 160,
	//信号质量评估相关阈值
	.ppg_jitter_thr = 35,
	.ppg_noise_thr = 25,
	.ppg_coeff_thr = 85,
	.quality_module_key = 253,
	//出值策略
	.low_spo2_thr = 95,
	.fast_out_time = 12,
	.slow_out_time = 30,
	.min_stable_time_high = 2,
	.min_stable_time_low = 7,
	.max_spo2_variation_high = 10,
	.max_spo2_variation_low = 5,
	.ouput_module_key = 3
};


const void *goodix_spo2_config_get_instance(void)
{
	return &external_cfg;
}

int32_t  goodix_spo2_config_get_size(void)
{
	return sizeof(goodix_spo2_config);
}

void goodix_spo2_config_get_version(char* ver,uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(SPO2_INTERFACE_VERSION) + 1) ? ver_len : (strlen(SPO2_INTERFACE_VERSION) + 1);
	memcpy(ver, SPO2_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //如果NET_VERSION的长度大于size，需要添加结束符
}
#endif
