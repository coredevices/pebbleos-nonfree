#ifndef __GOODIX_SPO2_INTERFACE_H
#define __GOODIX_SPO2_INTERFACE_H

#include <stdint.h>
#include "goodix_type.h"

#define SPO2_INTERFACE_VERSION   "pre_pv_v1.1.0"
#define SPO2_INTERFACE_VERSION_LEN_MAX  (20)
/**
* @brief SPO2 �㷨��������״ֵ̬
*/
typedef enum
{
	GX_ALGO_SPO2_SUCCESS = 0x00000000,			/**< �ɹ�     */
	GX_ALGO_SPO2_RWONG_INPUT = 0x00000001,		/**< �������ݸ�ʽ���Ϸ�     */
	GX_ALGO_SPO2_NO_MEMORY = 0x00000002,        /**< �ڴ�ռ䲻��     */
	GX_ALGO_SPO2_FRAME_UNCOMPLETE = 0x00000003, /**< ���ڽ�����ƽ��������*/
	GX_ALGO_SPO2_WIN_UNCOMPLETE = 0x00000004,   /**< δ����������֡λ�ã�����s��     */
	GX_ALGO_SPO2_UNEXPECTED = 0x00000005,       /*��������߼��쳣 */
	GX_ALGO_SPO2_CFG_ERROR = 0x00000006,        /*���ò���У�鲻ͨ��*/
	GX_ALGO_SPO2_INIT_FAILURE = 0x00000007,     /*��ʼ��ʧ��*/
}goodix_spo2_ret;



typedef struct {
	//raw ������Ϣ
	uint32_t valid_chl_num;			// ��Чͨ����
	uint32_t raw_fs;			    // ԭʼ������

	int32_t cali_coef_a4;							//У׼����4����
	int32_t cali_coef_a3;							//У׼����3����
	int32_t cali_coef_a2;							//У׼����2����
	int32_t cali_coef_a1;							//У׼����1����
	int32_t cali_coef_a0;							//У׼����������
	//hbʹ�ܱ�־
	uint32_t hb_en_flg;					//hbʹ�ܱ�־
	//���״̬
	uint32_t wear_mode;					//���״̬ 0:�ֻ�  1����ָ  2������
	// acc move thr
	uint32_t acc_thr_max;
	uint32_t acc_thr_min;
	uint32_t acc_thr_scale;
	uint32_t acc_thr_num;
	uint32_t acc_thr_angle;
	//CTR����
	uint32_t ctr_en_flg;
	uint32_t ctr_red_thr;

	//�ź��������������ֵ
	uint8_t ppg_jitter_thr;
	uint8_t ppg_noise_thr;
	uint8_t ppg_coeff_thr;
	uint8_t quality_module_key;

	//��ֵ����
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
	uint32_t frame_id;	        // ֡���
	uint8_t  total_ch_num;             //�������ݵ�ͨ������1��ͨ��ͬʱ�����̹⡢���⡢������ݣ�
	uint8_t *enable_flg;		   // ͨ��ʹ�ܱ�־λ�� �����̹�1234-����1234-���1234
	int32_t *ppg_rawdata;		 // PPGԭʼ���ݣ������̹�1234-����1234-���1234
	uint8_t *ch_agc_gain;	    //  gainֵ1-�����̹�1234-����1234-���1234
	uint16_t *ch_agc_drv;		//  ��������0-�����̹�1234-����1234-���1234

	int32_t acc_x;				//���ٶȼ�x��
	int32_t acc_y;				//���ٶȼ�y��
	int32_t acc_z;				//���ٶȼ�z��
	int32_t groy_x;
	int32_t groy_y;
	int32_t groy_z;

	uint8_t sleep_flg;
	uint8_t bit_num;            //����λ��
	uint8_t chip_type;
	uint8_t data_type;
	void *pst_module_unique;
}goodix_spo2_input_rawdata;

typedef struct
{
	/*
	int32_t spo2[CHIP_PPG_CHL_NUM];				//Ѫ��ֵ*10000
	int32_t valid_level[CHIP_PPG_CHL_NUM];		//���ŵȼ�
	int32_t confi_coeff[CHIP_PPG_CHL_NUM];		//���Ŷ�
	int32_t WeightR[CHIP_PPG_CHL_NUM];		//���Ŷ�

	//HB
	int32_t hb_mean[CHIP_PPG_CHL_NUM];
	int32_t hb_confi_lvl[CHIP_PPG_CHL_NUM];

	int32_t invalidFlg[CHIP_PPG_CHL_NUM];
	int32_t piIR[CHIP_PPG_CHL_NUM];
	int32_t piRed[CHIP_PPG_CHL_NUM];
	int32_t snrIR[CHIP_PPG_CHL_NUM];
	int32_t snrRed[CHIP_PPG_CHL_NUM];
	int32_t r_val[CHIP_PPG_CHL_NUM];				//rֵ��*10000��
	int32_t calc_flg[CHIP_PPG_CHL_NUM];
	*/

	//**********************��ͨ���������ֵ***************************//
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
	* @brief SPO2 �����㷨�汾��
	* @param[in]  version     �ⲿ����汾�ſռ䣬����20�ֽ�
	* @param[out] ��
	* @return     ����ִ�����
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_version(uint8_t version[150]);

	/**
	* @brief SPO2 �����㷨��ʼ��
	* @param[in]  cfg_instance   �ⲿ��������ò����ṹ��
	* @param[in]  cfg_size       ���ýṹ���С������У���ⲿ�ӿ��ļ��Ƿ����
	* @param[in]  interface_ver	 �ӿڰ汾�ţ�����У���ⲿ�ӿ��ļ��Ƿ����
	* @param[out] ��
	* @return     ����ִ�����
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_init(const void *cfg_instance, int32_t cfg_size, char *interface_ver);

	/**
	* @brief SPO2 Ѫ������
	* @param[in]  raw		����ԭʼ���ݽṹ��
	* @param[out] res		�������ṹ��
	* @return     ����ִ�����
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_calc(goodix_spo2_input_rawdata* raw, goodix_spo2_result* res);


	/**
	* @brief ���ͻ����Զ���ʱ���ã���ӡ�㷨�������к��һЩ�ؼ�������Ϣ����goodix_spo2_calc����ã�
	* @param[in]  raw		����ԭʼ���ݽṹ�壬NULL==rawdataʱ������ӡ������Ϣ
	* @param[out] 		    ��
	* @return               ��
	*/
	DRVDLL_API void goodix_spo2_essential_info_print(goodix_spo2_input_rawdata* raw);
	/**
	* @brief SPO2 �㷨���٣��ͷ��ڲ����ڴ�ռ䣬��������ģ���㷨ʱʹ��
	* @param[out] ��
	* @return     ����ִ�����
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_deinit(void);


	/**
	* @brief SPO2 ���ò�����ȡ����ʼ��ǰ���ã����������ýṹ�����ݵ��ڴ�
	* @param[out] ��
	* @return     ָ�����ýṹ��ʵ����ָ��
	*/
	const void *goodix_spo2_config_get_instance(void);

	/**
	* @brief SPO2 ��ȡ���ò����ṹ����ڴ�ռ��С����ʼ��ǰ���ã������㷨�ڲ����м���
	* @param[out] ��
	* @return     ���ò���ռ�õ��ڴ�ռ��С
	*/
	int32_t goodix_spo2_config_get_size(void);

	/**
	* @brief SPO2 ��ȡ�ӿ��ļ��İ汾����ʼ��ǰ���ã������㷨�ڲ����м��飨��ֹ�ⲿ����ʱ�ӿ��ļ�����δ���룩
	* @param[out] ��
	* @return     ��
	*/
	void goodix_spo2_config_get_version(char *ver, uint8_t ver_len);

#ifdef __cplusplus
}
#endif

#endif
