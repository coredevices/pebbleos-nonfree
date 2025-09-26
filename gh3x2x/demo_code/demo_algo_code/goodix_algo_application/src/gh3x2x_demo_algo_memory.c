/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_algorithm.c
 * 
 * @brief   gh3x2x driver lib demo code for algorithm
 * 
 * @author  Gooidx Iot Team
 * 
 */
#include "gh3x2x_demo_algo_call.h"
#include "gh3x2x_demo_algo_memory.h"
#include "gh3x2x_demo_algo_config.h"
#include "gh3x2x_demo_algorithm_calc.h"
#include "goodix_algo.h"
#include "goodix_mem.h"

#if (__GOODIX_ALGO_CALL_MODE__)
#if __USER_DYNAMIC_ALGO_MEM_EN__
GU32 *g_punGh3x2xAlgoMemBufferPointer = GH3X2X_ALGORITHMS_MEMORY_SIZE_USERSET;
#else
GU32 g_punGh3x2xAlgoMemBuffer[GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL/4] = {0};
GU32 *g_punGh3x2xAlgoMemBufferPointer = g_punGh3x2xAlgoMemBuffer;
#endif
#else  //without algorithm
GU32 *g_punGh3x2xAlgoMemBufferPointer = 0;
#endif

void GH3X2X_MemFree(void* mem_addr)
{
    goodix_mem_free(mem_addr);
}

void* GH3X2X_MemMalloc(GS32 nSize)
{
    return goodix_mem_malloc(nSize);
}

void Gh3x2xPoolIsNotEnough(void)
{
    while(1)
    {
        GH3X2X_DEBUG_ALGO_LOG("MEMORY ERROR!!!PLEASE RESET HARDWARE!!!\n");
    }
}

#if (__GOODIX_ALGO_CALL_MODE__)
#if __GH3X2X_MEM_POOL_CHECK_EN__
const GU32 g_unGh3z2xMemCheckAreaStart[17] =
{
    0,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*1/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*2/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*3/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*4/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*5/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*6/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*7/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*8/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*9/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*10/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*11/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*12/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*13/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*14/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL*15/16,
    GH3X2X_ALGORITHMS_MEMORY_SIZE_FINAL,
};

GU32 g_unGh3x2xLastMemPoolCheckSumArea[16];

GU8  g_uchGh3x2xMemPollHaveGetCheckSum = 0;
GU16 g_usGh3x2xMemPollErrorFlag = 0;
GU32 Gh3x2xGetMemPoolCheckSum(GU8 *lunMemPool, GU32 lunByteSize)
{
    GU32 lunCheckSum = 0;
    for(GU32 lunByteCnt = 0; lunByteCnt < lunByteSize; lunByteCnt += 4)
    {
        lunCheckSum += *((GU32*)(lunMemPool+lunByteCnt));
    }
    
    return lunCheckSum;
}

void Gh3x2xMemPoolCheckSumWrite(GU8 *lunMemPool)
{
    for(GU8 uchAreaCnt = 0; uchAreaCnt < 16; uchAreaCnt ++)
    {
        g_unGh3x2xLastMemPoolCheckSumArea[uchAreaCnt] = Gh3x2xGetMemPoolCheckSum(lunMemPool + ((g_unGh3z2xMemCheckAreaStart[uchAreaCnt]/4)*4), (g_unGh3z2xMemCheckAreaStart[uchAreaCnt + 1]/4)*4 - (g_unGh3z2xMemCheckAreaStart[uchAreaCnt]/4)*4);
    }
    g_uchGh3x2xMemPollHaveGetCheckSum = 1;
}

GU16 Gh3x2xMemPoolCheckSumCheck(GU8 *lunMemPool)
{
    GU16 usErrorFlag = 0;

    if(0 == g_usGh3x2xMemPollErrorFlag)
    {
        for(GU8 uchAreaCnt = 0; uchAreaCnt < 16; uchAreaCnt ++)
        {
            if(g_unGh3x2xLastMemPoolCheckSumArea[uchAreaCnt] != Gh3x2xGetMemPoolCheckSum(lunMemPool + ((g_unGh3z2xMemCheckAreaStart[uchAreaCnt]/4)*4), (g_unGh3z2xMemCheckAreaStart[uchAreaCnt + 1]/4)*4 - (g_unGh3z2xMemCheckAreaStart[uchAreaCnt]/4)*4))
            {
                usErrorFlag |= (((GU16)1) << uchAreaCnt);
            }
        }
    }

    g_usGh3x2xMemPollErrorFlag = usErrorFlag;
    return usErrorFlag;
}

void Gh3x2xCheckMemPollBeforeAlgoCal(void)
{
    if(g_uchGh3x2xMemPollHaveGetCheckSum)
    {
        if(Gh3x2xMemPoolCheckSumCheck((GU8*)g_punGh3x2xAlgoMemBufferPointer))
        {
            GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check error!!!  error flag = 0x%x\r\n", g_usGh3x2xMemPollErrorFlag);
            while(1);  //error !!!  mem pool is be destroy
        }
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 0  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[0]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 1  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[1]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 2  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[2]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 3  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[3]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 4  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[4]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 5  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[5]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 6  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[6]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 7  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[7]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 8  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[8]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 9  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[9]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 10  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[10]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 11  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[11]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 12  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[12]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 13  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[13]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 14  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[14]);
        GH3X2X_DEBUG_ALGO_LOG("Gh3x2x mem check ok, check sum 15  = 0x%x\r\n",g_unGh3x2xLastMemPoolCheckSumArea[15]);
    }
}

void Gh3x2xUpdataMemPollChkSumAfterAlgoCal(void)
{
    Gh3x2xMemPoolCheckSumWrite((GU8*)g_punGh3x2xAlgoMemBufferPointer);
}
#endif

#if __USER_DYNAMIC_ALGO_MEM_EN__
void* Gh3x2xAlgoMemApply(GU32 unMemSize)
{
    return Gh3x2xMallocUser(unMemSize)
}

void Gh3x2xAlgoMemFree(void* MemAddr)
{
    Gh3x2xFreeUser(MemAddr);
}
#else
void* Gh3x2xAlgoMemApply(GU32 unMemSize)
{
    (void)(unMemSize);
    return &g_punGh3x2xAlgoMemBuffer[0];
}

void Gh3x2xAlgoMemFree(void* MemAddr)
{
    (void)(MemAddr);
}
#endif

/// algorithm memory is inited or not
GU8 g_uchAlgoMemStatus = ALGO_MEM_NO_INIT;

/// buffer for algorithm, this addr is import from outer 
//void* g_unAlgorithmMemBuffer = 0;

/// buffer size(byte) for algorithm, this size is import from outer
GU32 g_unAlgorithmMemBufferSize = 0;

/**
 * @fn     GS8 GH3X2X_AlgoMemConfig( GU32 unMemSize)
 *
 * @brief  Config algorithm memory.
 *
 * @attention   Mem can only be static,such as global array
 *
 * @param[in]  unMemSize       memory size(unit:byte)
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_RESOURCE_ERROR          return algorithm mem config error
 */
GS8 GH3X2X_AlgoMemConfig( GU32 unMemSize)
{
    GS8 chRet = GH3X2X_RET_OK;
    if (unMemSize > 0)
    {
        GH3X2X_AlgoDeinit(0xFFFFFFFF);
        g_unAlgorithmMemBufferSize = unMemSize;
        chRet = GH3X2X_RET_OK;
        GH3X2X_DEBUG_ALGO_LOG("Algorithm mem Config complete!Size:%d\r\n", (int)unMemSize);
        GH3X2X_DEBUG_ALGO_LOG("Algorithm mem init start...\r\n");
        chRet = GH3X2X_AlgoMemInit();
    }
    else
    {
        GH3X2X_DEBUG_ALGO_LOG("Algorithm mem config fail!\r\n");
        chRet = GH3X2X_RET_RESOURCE_ERROR;
    }
    return chRet;
}

/**
 * @fn     GS8 GH3X2X_AlgoMemInit(void)
 *
 * @brief  algorithm memory init
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  error code
 * @retval  #GH3X2X_RET_OK                      return successfully
 * @retval  #GH3X2X_RET_RESOURCE_ERROR          return algorithm mem init error
 */
GS8 GH3X2X_AlgoMemInit(void)
{
    GS8 chRet = GH3X2X_RET_OK;
    GH3X2X_DEBUG_ALGO_LOG("Algorithm mem init start...\r\n");
    if (ALGO_MEM_NO_INIT == g_uchAlgoMemStatus)
    {
        g_punGh3x2xAlgoMemBufferPointer = (GU32*)Gh3x2xAlgoMemApply(g_unAlgorithmMemBufferSize);
        if ((g_punGh3x2xAlgoMemBufferPointer != 0) && (g_unAlgorithmMemBufferSize > 0))
        {
            if(MEMPOOL_IS_OK_E != goodix_mem_init((void*)g_punGh3x2xAlgoMemBufferPointer, g_unAlgorithmMemBufferSize))
            {
                chRet = GH3X2X_RET_RESOURCE_ERROR;
                GH3X2X_DEBUG_ALGO_LOG("Algorithm mem init error!\r\n");
            }
            else
            {
                g_uchAlgoMemStatus = ALGO_MEM_INIT_OK;
                chRet = GH3X2X_RET_OK;
                GH3X2X_DEBUG_ALGO_LOG("Algorithm mem init success.Addr:0x%x,Size:%d\r\n", (unsigned int)g_punGh3x2xAlgoMemBufferPointer, \
                                                                                        (int)g_unAlgorithmMemBufferSize);
            }
        }
        else
        {
            GH3X2X_DEBUG_ALGO_LOG("Algorithm mem para error!\r\n");
            chRet = GH3X2X_RET_RESOURCE_ERROR;
        }
    }
    return chRet;
}

/**
* @fn     void GH3X2X_AlgoMemDeinit(void)
*
* @brief  algorithm memory deinit
*
* @attention   None
*
* @param[in]   None
* @param[out]  None
*
* @return  None
*/
void GH3X2X_AlgoMemDeinit(void)
{
    if (ALGO_MEM_INIT_OK == g_uchAlgoMemStatus)
    {
        goodix_mem_deinit();  //destroy all memory
        if(g_punGh3x2xAlgoMemBufferPointer)
        {
            Gh3x2xAlgoMemFree((void*)g_punGh3x2xAlgoMemBufferPointer);
            g_punGh3x2xAlgoMemBufferPointer = 0;
        }
        g_uchAlgoMemStatus = ALGO_MEM_NO_INIT;
        GH3X2X_DEBUG_ALGO_LOG("Algorithm mem deinit!\r\n");
    }
}

GU8 GH3X2X_GetAlgoMemStatus(void)
{
    return g_uchAlgoMemStatus;
}

GU8 Gh3x2xGetHrAlgoSupportChnl(void)
{
    return __HR_ALGORITHM_SUPPORT_CHNL_NUM__;
}

GU8 Gh3x2xGetSpo2AlgoSupportChnl(void)
{
    return __SPO2_ALGORITHM_SUPPORT_CHNL_NUM__;
}

#endif
