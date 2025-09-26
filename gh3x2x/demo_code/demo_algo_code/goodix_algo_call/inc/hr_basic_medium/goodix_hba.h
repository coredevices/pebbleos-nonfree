#ifndef __GOODIX_HBA_H
#define __GOODIX_HBA_H

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif
// HBA 采样率
#ifndef HBA_FS
#define HBA_FS 25
#endif
#define PPG_DEAL_CH_NUM  4

#define HBA_INTERFACE_VERSION "pv_v1.1.0"
#define HBA_INTERFACE_VERSION_LEN_MAX (20)

/**
* @brief HBA 算法函数返回状态值
*/
typedef enum
{
	GX_ALGO_HBA_SUCCESS = 0x00000000,			/**< 成功     */
	GX_ALGO_HBA_RWONG_INPUT = 0x00000001,		/**< 输入数据格式不合法     */
	GX_ALGO_HBA_NO_MEMORY = 0x00008000,			/**< 内存空间不够     */
}goodix_hba_ret;

typedef enum {
	HBA_SCENES_DEFAULT = 0,				// 默认：由算法内部识别处理

	HBA_SCENES_DAILY_LIFE = 1,			// 日常生活
	HBA_SCENES_RUNNING_INSIDE = 2,		// 室内跑步机
	HBA_SCENES_WALKING_INSIDE = 3,		// 室内步行
	HBA_SCENES_STAIRS = 4,				// 上下楼梯

	HBA_SCENES_RUNNING_OUTSIDE = 5,		// 室外跑步
	HBA_SCENES_WALKING_OUTSIDE = 6,		// 室外步行

	HBA_SCENES_STILL_REST = 7,			// 静息
	HBA_SCENES_REST = 8,				// 休息
	HBA_SCENES_STILLRADON = 9,			// 憋气

	HBA_SCENES_BIKING_INSIDE = 10,		//室内自行车
	HBA_SCENES_BIKING_OUTSIDE = 11,		//室外自行车
	HBA_SCENES_BIKING_MOUNTAIN= 12,		//室外自行车越野
	HBA_SCENES_RUNNING_HIGH_HR = 13,	//高心率跑步

	HBA_SCENES_RUNNING_TREADMILL_CCOMBINE= 14,		// 跑步机组合跑

	HBA_SCENES_HIGH_INTENSITY_COMBINE = 15,		// 高强度运动组合
	HBA_SCENES_TRADITIONAL_STRENGTH_COMBINE = 16,		// 传统力量训练组合
	HBA_SCENES_STEP_TEST = 17,		    // 台阶测试

	HBA_SCENES_BALL_SPORTS = 18,		// 球类运动
	HBA_SCENES_AEROBICS = 19,		    // 健身操

    HBA_SCENES_SLEEP = 20,              // 睡眠场景
    HBA_SCENES_JUMP = 21,               //手腕跳绳（LS ROMA）
    HBA_SCENES_CORDLESS_JUMP = 22,	    // 万机无绳跳绳测试
	HBA_SCENES_SWIMMING = 23,           // 游泳场景
    HBA_SCENES_SIZE = 24,               // 场景数目
}hba_scenes_e;

typedef enum {
    HBA_TEST_DYNAMIC = 0,		// 默认：动态测试
    HBA_TEST_DOT = 1,			// 点测
    HBA_TEST_SENSELESS = 2,		// 无感模式
}hba_test_mode;

typedef struct {
	uint32_t mode;                             // 测试模式
	uint32_t fs;							   // 采样率
	int32_t valid_ch_num;				   // 有效通道数
	// 用于外部控制算法倾向性的参数
	int32_t hba_earliest_output_time;				// 最早出值时间
	int32_t hba_latest_output_time;					// 最晚出值时间
	uint32_t raw_ppg_scale;                        // rawdata滤波补偿因子
}goodix_hba_config;

typedef struct
{
	uint8_t scence;			  // 场景ID
}goodix_hba_input_unique;

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
}goodix_hba_input_rawdata;

typedef struct
{
	uint32_t hba_out_flag;				// 出值标记：为 1 有效
	uint32_t hba_out;					// 心率值
	uint32_t hba_snr;					// 信噪比   : 滤波去除干扰后，频谱主峰能量处于总能量
    uint32_t valid_level;			    // 置信等级 : 0 -> 1，越大越可靠
    uint32_t valid_score;				// 置信分数 : 0->100 越大越可靠
	uint32_t hba_acc_info;              // 运动状态 : 0-静息，小运动；1-步行-中运动；2-跑步-大运动；
	uint32_t hba_reg_scence;            // 运动场景 : 参考 hba_scenes_e 枚举类型

	int32_t reserved1;					// 备用三个接口
	int32_t reserved2;					
	int32_t reserved3;				
}goodix_hba_result;

#ifdef __cplusplus
extern "C" {
#endif


/**
* @brief HBA 处理算法版本号
* @param[in]  version     外部储存版本号空间，分配20字节
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_version(uint8_t version[120]);


/**
* @brief HBA 处理算法初始化
* @param[in]  scence     使用场景
* @param[in]  fs         HBA 和 ACC 的采样率【注：两者需要同频】
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret  goodix_hba_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);


/**
* @brief HBA 算法更新
* @param[in] raw    心率数据
* @param[out] res   心率输出结果
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_calc(goodix_hba_input_rawdata* raw, goodix_hba_result *res);

/**
* @brief HBA 算法销毁，释放内部的内存空间，调用其他模块算法时使用
* @param[out] 无
* @return     函数执行情况
*/
DRVDLL_API goodix_hba_ret goodix_hba_deinit(void);

/**
* @brief 获取hba的默认配置参数
* @return     hba的默认配置参数
*/
DRVDLL_API const void *goodix_hba_config_get_arr(void);

/**
* @brief 获取hba的配置size
* @return     hba的配置size
*/
DRVDLL_API int32_t  goodix_hba_config_get_size(void);

/**
* @brief 获取HBA配置文件的版本号
* @param[in/out]  ver       版本号保存数组
* @param[in] 	  ver_len   版本号长度，建议使用HBA_INTERFACE_VERSION_LEN_MAX
* @return     	  无
*/
DRVDLL_API void goodix_hba_config_get_version(char* ver, uint8_t ver_len);


/**
* @brief 用户调用打印HBA重要信息，需要提前初始化打印函数
* @param[in]  raw    心率输入数据，NULL == raw时，不打印数据
* @return     	  无
*/
DRVDLL_API void goodix_hba_essential_info_print(goodix_hba_input_rawdata* raw);

#ifdef __cplusplus
}
#endif

#endif
