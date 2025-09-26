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

/* ���ش����� */
typedef enum {
    GX_HRV_ALGO_OK              = 0x00000000, // �ɹ����أ����޳�ֵˢ��
    GX_HRV_ALGO_FAILED          = 0x10000001, // ʧ�ܷ���
    GX_HRV_ALGO_NULL_PTR        = 0x10000002, // �ⲿ�����ָ��Ϊ��
    GX_HRV_ALGO_INVALID_PARAM   = 0x10000003, // ������Χ��Ч
    GX_HRV_ALGO_OUT_OF_MEM      = 0x10000004, // �ڴ����ʧ��
    GX_HRV_ALGO_DEINIT_ABORT    = 0x10000005, // �ͷ�ʧ��
    GX_HRV_ALGO_UPDATE          = 0x10000006, // �г�ֵˢ��
}goodix_hrv_ret;

#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

#define HRV_INTERFACE_VERSION "pv_v1.1.0"
#define HRV_INTERFACE_VERSION_LEN_MAX (20)

typedef struct {
    int32_t need_ipl;                                   // �Ƿ���Ҫ��ֵ��1KHz
    uint32_t fs;                                         // ������
    int32_t acc_thr[ACC_THR_NUM];                       // acc �����ֵ��Ĭ��ֵΪ��20/10/3/3
}goodix_hrv_config;

typedef struct
{
	uint8_t hr;			  // �ⲿ��������
}goodix_hrv_input_unique;

typedef struct
{
	uint32_t frame_id;			  // ֡���
	uint8_t total_ch_num;         // �����㷨��ͨ������
	uint8_t *enable_flg;		  // ��ָʾppg_rawdata�е�ÿ·ppg�źŵ���Ч�ԣ������ң�ÿ��bit����1·ppg�źţ���������total_chl_num*3·bit����ռ��ceil(total_chl_num*3/8)���ֽ�
	int32_t *ppg_rawdata;		  // PPGԭʼ���ݣ�����total_chl_num���顣���˳������green[ch1-total_chl_num]��ir[ch1-total_chl_num]��red[ch1-total_chl_num]
	uint8_t *ch_agc_gain;	      //  gainֵ,˳����ppg����һ��
	uint16_t *ch_agc_drv;		  //  ��������,˳����ppg����һ��
	int32_t acc_x;				  // ���ٶȼ�x��
	int32_t acc_y;				  // ���ٶȼ�y��
	int32_t acc_z;				  // ���ٶȼ�z��
	int32_t groy_x;				  // ������x��
	int32_t groy_y;				  // ������y��
	int32_t groy_z;				  // ������z��

	uint8_t sleep_flg;            // ˯��flg
	uint8_t bit_num;              // ����λ��
	uint8_t chip_type;            // оƬ����
	uint8_t data_type;            // ��������

	void *pst_module_unique;      //������������
}goodix_hrv_input_rawdata;

typedef struct {
    int32_t rri[RRI_NUM];                               // RRI�������
    int32_t rri_confidence;                             // ������Ŷ�
	int32_t rri_valid_num;                              // RRI��Ч�������

	int32_t reserved1;					// ���������ӿ�
	int32_t reserved2;
	int32_t reserved3;
}goodix_hrv_result;

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_version(uint8_t version[120]);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_init(const void *cfg_instance, uint32_t cfg_size, const char *interface_ver);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_calc(goodix_hrv_input_rawdata *raw, goodix_hrv_result *res);

IOT_SYS_DLL_API goodix_hrv_ret goodix_hrv_deinit(void);

/**
* @brief ��ȡhrv��Ĭ�����ò���
* @return     hrv��Ĭ�����ò���
*/
IOT_SYS_DLL_API const void *goodix_hrv_config_get_arr(void);

/**
* @brief ��ȡhrv������size
* @return     hrv������size
*/
IOT_SYS_DLL_API int32_t  goodix_hrv_config_get_size(void);

/**
* @brief ��ȡHRV�����ļ��İ汾��
* @param[in/out]  ver       �汾�ű�������
* @param[in] 	  ver_len   �汾�ų��ȣ�����ʹ��HRV_INTERFACE_VERSION_LEN_MAX
* @return     	  ��
*/
IOT_SYS_DLL_API void goodix_hrv_config_get_version(char* ver, uint8_t ver_len);


/**
* @brief �û����ô�ӡHRV��Ҫ��Ϣ����Ҫ��ǰ��ʼ����ӡ����
* @param[in]  raw    �����������ݣ�NULL == rawʱ������ӡ����
* @return     	  ��
*/
IOT_SYS_DLL_API void goodix_hrv_essential_info_print(goodix_hrv_input_rawdata* raw);

#if DEBUG_HRV
IOT_SYS_DLL_API goodix_hrv_debug *copy_debug_addr(void);
#endif
#ifdef __cplusplus
}
#endif

#endif // _GOODIX_SYS_HRV_LIB_H_
