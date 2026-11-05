#ifndef _WWD_MAIN_H_
#define _WWD_MAIN_H_
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "goodix_type.h"

#define NADT_INTERFACE_VERSION "pv_v1.0.0"
#define NADT_INTERFACE_VERSION_LEN_MAX (20)

//#define BYTE_TRUE       (uint8_t)1
//#define BYTE_FALSE      (uint8_t)0

#define PPG_CHANNEL_NUM 4

#define IOT_NADT_MAX(a,b)                ((a)<(b)?(b):(a))
#define IOT_NADT_MIN(a,b)                ((a)>(b)?(b):(a))
#define IOT_NADT_ABS(x)                  abs(x)
#define IOT_NADT_ABS_GS32(x)             abs((int32_t)(x))
//#define IOT_NADT_ABS_GF32(x)             fabsf((float32_t)(x))
//#define IOT_NADT_ABS_GF64(x)             fabs((GF64)(x))
//#define IOT_NADT_SQRT(x)                 sqrt(x)
#define IOT_NADT_SQRTF(x)                sqrtf(x)
#define IOT_NADT_ROUND(x)                round(x)
#define NADT_PTR_NULL                   ((void *)0)

#define DEBUG_NADT_LOG 0 //1
#if DEBUG_NADT_LOG
#define NADT_LOG_PRINT	 GH3X2X_SAMPLE_LOG_PARAM
#else
#define NADT_LOG_PRINT	
#endif //#if DEBUG_NADT_LOG

typedef enum
{
	GX_ALGO_NADT_SUCCESS = 0x00000000,				/**< 成功     */
	GX_ALGO_NADT_NO_MEMORY = 0x00000001,			/**< 内存空间不够     */
	GX_ALGO_NADT_UNINIT = 0x00000002,
	GX_ALGO_NADT_RWONG_INPUT = 0x00000003,			/**< 输入数据格式不合法     */
}goodix_nadt_ret;

typedef struct
{
	uint16_t sampleRateNADT;				//采样率
	uint8_t unwearDetectLevel;				//佩戴等级
	//uint8_t sleepStatus;					//睡眠状态
	//
	uint16_t checkTimeoutThr;				//活体检测超时时间
	uint8_t unwearCheckCntThr;				//脱落确认时间
	uint8_t wearCheckCntThr;				//活体确认时间
	//
	//ADT判断
	int32_t adtWearOnThr;					//ADT wear on阈值
	int32_t adtWearOffThr;					//ADT wear off阈值
	//
	//运动判断
	int32_t accStdThr;						//ACC标准差
	//
	//活体检测
	uint8_t liveDetectEnable;				//活体检测使能
	int32_t rawPVLenThr;					//高次谐波阈值
	uint16_t heartBeatThrLow;				//心率下限
	uint16_t heartBeatThrHigh;				//心率上限
	int32_t difRrInterThr;					//心率变化阈值
	uint8_t wearAcfScoreThr;				//自相关分数阈值
	uint16_t baseLinePosRatioThr;			//基线单调率阈值
	int32_t baseLineDiffThr;			    //基线平坦性阈值
	int32_t sigScaleThr;
	//
	//高频背景光检测
	uint8_t bgJumpDetectEnable;				//背景光突变检测使能
	int16_t jumpBgDiffThr;					//背景光变化量阈值
	uint8_t bgPeakValleyNumThr;				//背景光峰谷个数阈值
	//
	//背景光周期检测
	uint8_t bgPeriodDetectEnable;			//背景光周期检测使能
	int16_t periodBgDiffThr;				//背景光变化量阈值
	uint16_t bgPeriodThr;					//背景光周期阈值
	uint16_t bgPeriodDiffThr;				//背景光周期差异阈值
}goodix_nadt_config;


typedef struct{
	uint32_t frame_id;			  // 帧序号
	uint8_t total_ch_num;		  // 灌入算法的通道总数=有效通道+无效通道，一个通道对应一组数据，每组数据包括ppg，adt，amb
	uint8_t *enable_flg;		  // PPG通道使能标志位，从左到右，每个bit代表1路ppg信号
	int32_t *ppg_rawdata;		  // PPG原始数据，依次绿光(或红外）PPG-红外ADT-背景光
	uint8_t *ch_agc_gain;	      //  gain值，顺序与ppg_rawdata一致
	uint16_t *ch_agc_drv;		  //  驱动电流，顺序与ppg_rawdata一致

	int32_t acc_x;				  // 加速度计x轴
	int32_t acc_y;				  // 加速度计y轴
	int32_t acc_z;				  // 加速度计z轴
	int32_t gyro_x;               // 陀螺仪x轴
	int32_t gyro_y;               // 陀螺仪y轴
	int32_t gyro_z;               // 陀螺仪z轴

	uint8_t sleep_flg;           // 睡眠标记，0-未睡眠，1-睡眠中
	uint8_t bit_num;              // 数据位数
	uint8_t chip_type;           // 芯片类型，0-roma，1-cardiffA，2-CardiffB
	uint8_t data_type;           // 数据类型，0-adc，1-adc&ipd

	void *pst_module_unique;     //NADT特有输入参数
}goodix_nadt_input_rawdata;

typedef struct{
	uint32_t ppg_colour_flg;      // PPG颜色，0-绿光，1-红外
	uint32_t pre_status;          // 上一时刻的佩戴状态，0 脱落(adt wear on)，1 活体，2 超时30s，但超时状态不超过2次，3 超时30s，且超时状态超过2次

	uint32_t cap_channel_num;     // 电容通道总数(PPG Only默认传0）
	int32_t *cap_rawdata;		  // 电容原始数据，依次电容12-ref12(PPG Only默认传0）
	int32_t *cap_enable_flg;      // 电容通道使能标志位(PPG Only默认传0）

	int32_t *ch_ipd;		      //  芯片电流，顺序与ppg_rawdata一致
}goodix_nadt_input_unique;


typedef struct{
	int32_t nadt_out_flag;
	int32_t nadt_out;				// bit0-bit1：佩戴状态，0-默认，1-佩戴，2-脱落，3-非活体；bit2：疑似脱落标记，0-正常，1-疑似脱落
	int32_t nadt_confi;				// 置信度
	int32_t reserved1;
	int32_t reserved2;
	int32_t reserved3;
}goodix_nadt_result;


#ifdef __cplusplus
extern "C" {
#endif

	DRVDLL_API goodix_nadt_ret goodix_nadt_init(const void *cfg_instance, int32_t cfg_size, const char *interface_ver);
	DRVDLL_API goodix_nadt_ret goodix_nadt_deinit(void);
	DRVDLL_API goodix_nadt_ret goodix_nadt_calc(goodix_nadt_input_rawdata* raw, goodix_nadt_result *res);
	DRVDLL_API goodix_nadt_ret goodix_nadt_version(uint8_t version[120]);
	DRVDLL_API goodix_nadt_ret goodix_nadt_calc_by_cap(goodix_nadt_input_rawdata* raw, goodix_nadt_result *res);

	DRVDLL_API const void *goodix_nadt_config_get_arr(void);
	DRVDLL_API int32_t goodix_nadt_config_get_size(void);
	DRVDLL_API void goodix_nadt_config_get_version(char* ver, uint8_t ver_len);

	DRVDLL_API void goodix_nadt_essential_info_print(goodix_nadt_input_rawdata* raw);

#ifdef __cplusplus
}
#endif

#endif
