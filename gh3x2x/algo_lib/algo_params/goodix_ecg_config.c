#ifndef Goodix_DSP_EXPORTS
#include "goodix_ecg.h"
#include "gh3x2x_demo_algo_call.h"
#if 0
const goodix_ecg_config external_cfg =
{
	.fs = 500,
	.valid_channel_num = 1,
	.qrs_flag = 0,
	.mode_type = 0,
	.ic_type = 0
};

const void *goodix_ecg_config_get_arr()
{
	return &external_cfg;
}

int32_t goodix_ecg_config_get_size()
{
	return sizeof(goodix_ecg_config);
}
#endif

void goodix_ecg_config_get_version(char* ver, uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(ECG_INTERFACE_VERSION) + 1) ? ver_len : (strlen(ECG_INTERFACE_VERSION) + 1);
	GH3X2X_Memcpy(ver, ECG_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //如果ECG_CONFIG_VER的长度大于size，需要添加结束符
}
#endif
