#ifndef Goodix_DSP_EXPORTS
#include "goodix_hba.h"

const goodix_hba_config external_hba_cfg =
{
	.mode = HBA_TEST_DYNAMIC,			// ��Чͨ����
	.fs = 25,			    // ԭʼ����
	.valid_ch_num = 4,
	.hba_earliest_output_time = 9,
	.hba_latest_output_time = 20,
	.sigma = 1,
	.raw_ppg_scale = 202,
	.delay_time = 5,
	.valid_score_scale = 1,
};

const void *goodix_hba_config_get_arr()
{
	return &external_hba_cfg;
}

int32_t  goodix_hba_config_get_size()
{
	return sizeof(external_hba_cfg);
}

void goodix_hba_config_get_version(char* ver, uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(HBA_INTERFACE_VERSION) + 1) ? ver_len : (strlen(HBA_INTERFACE_VERSION) + 1);
	memcpy(ver, HBA_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //���NET_VERSION�ĳ��ȴ���size����Ҫ��ӽ�����
}

#endif //Goodix_DSP_EXPORTS
