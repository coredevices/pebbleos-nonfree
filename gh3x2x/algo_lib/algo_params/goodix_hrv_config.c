#ifndef Goodix_DSP_EXPORTS
#include "string.h"
#include "goodix_hrv.h"

const goodix_hrv_config external_hrv_cfg =
{
	.need_ipl = 1,			// 是否需要插值到1KHz
	.fs = 100,			    // 采样率
	.acc_thr[0] = 200000,   // acc 相关阈值，默认值为：20/10/3/3
	.acc_thr[1] = 100000,
	.acc_thr[2] = 30000,
	.acc_thr[3] = 30000,
};

const void *goodix_hrv_config_get_arr(void)
{
	return &external_hrv_cfg;
}

int32_t goodix_hrv_config_get_size(void)
{
	return sizeof(external_hrv_cfg);
}

void goodix_hrv_config_get_version(char* ver, uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(HRV_INTERFACE_VERSION) + 1) ? ver_len : (strlen(HRV_INTERFACE_VERSION) + 1);
	memcpy(ver, HRV_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //如果NET_VERSION的长度大于size，需要添加结束符
}

#endif //Goodix_DSP_EXPORTS
