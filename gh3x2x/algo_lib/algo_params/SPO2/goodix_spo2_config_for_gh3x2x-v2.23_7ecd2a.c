#ifndef Goodix_DSP_EXPORTS

#include "goodix_spo2.h"  
const goodix_spo2_config external_cfg =
{
	//raw 配置信息
	.valid_chl_num = 1,			// 有效通道数
	.raw_fs = 25,			    // 原始采样率
	//校准参数
	.cali_coef_a4 = 0,			//校准参数4次项
	.cali_coef_a3 = 0,			//校准参数3次项
	.cali_coef_a2 = -124026,		    //校准参数2次项
	.cali_coef_a1 = -59505,    //校准参数1次项
	.cali_coef_a0 = 1043941,	//校准参数常数项
	//hb使能标志
	.hb_en_flg = 1,			    //hb使能标志
	//佩戴状态
	.wear_mode = 0,			    //佩戴状态 0:手环  1：手指  2：耳机
	// acc move thr
	// PebbleOS deviation: the stock gate (max=50/min=25 in 512-LSB/g units, ~98/49mg) rejected
	// almost any wrist movement. Widen it so the algorithm's motion compensation attempts a
	// (lower-confidence) reading through moderate arm motion instead of discarding it. max 250 ~0.49g.
	.acc_thr_max = 250,
	.acc_thr_min = 80,
	.acc_thr_scale = 3,
	.acc_thr_num = 3,
	.acc_thr_angle = 90,
	//CTR设置
	.ctr_en_flg = 0,
	.ctr_red_thr = 160,
	//信号质量评估相关阈值
	// PebbleOS deviation: loosen the signal-quality gates (stock jitter 35 / noise 25 / coeff 85).
	// Motion inflates jitter/noise and lowers PPG correlation, so the tight values reject
	// recoverable beats. Trades some accuracy for getting a value out while moving.
	.ppg_jitter_thr = 60,
	.ppg_noise_thr = 45,
	.ppg_coeff_thr = 60,
	.quality_module_key = 253,
	//出值策略
	.low_spo2_thr = 95,
	// PebbleOS deviation: shorten the fast-path latency (stock 12s) so the first SpO2 value
	// lands sooner. slow_out_time (stable value) is unchanged, so steady-state accuracy holds.
	.fast_out_time = 8,
	.slow_out_time = 30,
	.min_stable_time_high = 2,
	.min_stable_time_low = 4,
	.max_spo2_variation_high = 10,
	.max_spo2_variation_low = 5,
	.ouput_module_key = 3
};


const void *goodix_spo2_config_get_instance()
{
	return &external_cfg;
}

int32_t  goodix_spo2_config_get_size()
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
