#ifndef __GOODIX_MEM_H
#define __GOODIX_MEM_H

#include "stdint.h"
#include "goodix_type.h"


typedef enum
{
    MEMPOOL_IS_OK_E =                       0, /*< MEM POOL is ok >*/
    ERR_MEMPOOL_NOT_EXIST_E =               -1, /*< mempool not exist >*/
    ERR_PARAMS_MEMPOOL_SIZE_E =             -2, /*< Input parameter is invalid: mempool size is invalid >*/
    ERR_PARAMS_MALLOC_SIZE_E =              -3, /*< Input parameter is invalid: malloc size is invalid >*/
    ERR_MEMPOOL_NOT_ENOUGH_MEM_E =          -4, /*< Memory size does not meet the required memory >*/
    ERR_PARAMS_ADDR_WRONG_E =               -5, /*< Input parameter is invalid: free addr is invalid >*/
    ERR_OP_CHUNK_TREAD_E =                  -6, /*< Occur memory tread >*/
    ERR_OP_CHUNK_LEAKY_E =                  -7, /*< Occur memory leaky >*/
}goodix_mem_ret;

#ifdef __cplusplus
extern "C" {
#endif

//内存不足时的接口，只支持内存分配时候调用，不支持内存越界时调用
#if !(defined(_WIN32) || defined(_WIN64))
extern void Gh3x2xPoolIsNotEnough(void);
#endif

/**
* @brief        内存池初始化操作
* @param[in]    mem_addr 内存地址
                size 内存大小
* @param[out]    无
* @return        初始化情况  0   < Operation success.
                            -1  < Input parameter is invalid
                            -2  < Memory size does not meet the minimum required memory
                            -3  < Memory manager has been initialized.
*/
DRVDLL_API int32_t goodix_mem_init(void* mem_addr, int32_t size);

/**
* @brief        内存池销毁操作
* @param[in]    无
* @param[out]    无
* @return        无
*/
DRVDLL_API void goodix_mem_deinit(void);

/**
* @brief        内存申请操作
* @param[in]    申请内存大小(字节)
* @param[out]    无
* @return        申请到的内存段首地址（在函数调用外做类型转换）
*/
DRVDLL_API void* goodix_mem_malloc(int32_t size);

/**
* @brief        释放内存操作
* @param[in]    释放内存段的首地址
* @param[out]    无
* @return        无
*/
DRVDLL_API void goodix_mem_free(void* mem_addr);

/**
* @brief        获取内存池当前空闲内存容量
* @param[in]    无
* @param[out]    无
* @return        内存池当前空闲容量(byte)
*/
DRVDLL_API uint32_t goodix_mem_get_free_size(void);

#ifdef __cplusplus
}
#endif


#endif
