#ifndef Goodix_DSP_EXPORTS
#include "goodix_nadt.h"

const goodix_nadt_config nadt_external_cfg =
{
	.sampleRateNADT = 25,				//采样率
	.unwearDetectLevel = 3,				//佩戴等级
	//.sleepStatus = 0,					//睡眠状态
	//
	.checkTimeoutThr = 1200,			//活体检测超时时间
	.unwearCheckCntThr = 4,				//脱落确认时间
	.wearCheckCntThr = 2,				//活体确认时间
	//
	//ADT判断
	.adtWearOnThr = 8949996, //8638608,			//ADT wear on阈值
	.adtWearOffThr = 8793588, //8588608,			//ADT wear off阈值
	//
	//运动判断
	.accStdThr = 25,					//ACC标准差
	//
	//活体检测
	.liveDetectEnable = 1,				//活体检测使能
	.rawPVLenThr = 300,					//高次谐波阈值
	.heartBeatThrLow = 50,				//心率下限
	.heartBeatThrHigh = 140,			//心率上限
	.difRrInterThr = 8,					//心率变化阈值
	.wearAcfScoreThr = 50,				//自相关分数阈值
	.baseLinePosRatioThr = 30,			//基线单调率阈值
	.baseLineDiffThr = 15,			    //基线平坦性阈值
	.sigScaleThr = 1000,
	//
	//高频背景光检测
	.bgJumpDetectEnable = 1,			//背景光突变检测使能
	.jumpBgDiffThr = 100,				//背景光变化量阈值
	.bgPeakValleyNumThr = 12,			//背景光峰谷个数阈值
	//
	//背景光周期检测
	.bgPeriodDetectEnable = 1,			//背景光周期检测使能
	.periodBgDiffThr = 10,				//背景光变化量阈值
	.bgPeriodThr = 10,					//背景光周期阈值
	.bgPeriodDiffThr = 5				//背景光周期差异阈值
};

const void *goodix_nadt_config_get_arr(void)
{
	return &nadt_external_cfg;
}

int32_t goodix_nadt_config_get_size(void)
{
	return sizeof(goodix_nadt_config);
}

void goodix_nadt_config_get_version(char* ver, uint8_t ver_len)
{
	uint32_t copy_num = ver_len < (strlen(NADT_INTERFACE_VERSION) + 1) ? ver_len : (strlen(NADT_INTERFACE_VERSION) + 1);
	memcpy(ver, NADT_INTERFACE_VERSION, copy_num);
	ver[copy_num - 1] = '\0'; //如果NADT_CONFIG_VER的长度大于size，需要添加结束符
}

#endif
