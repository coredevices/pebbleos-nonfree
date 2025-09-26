/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_demo_mp.h
 *
 * @brief   header file of gh3x2x demo mp test
 *
 * @author  Goodix Product Tools Development Team
 *
 */
 
#ifndef _GH3X2X_DEMO_MP_H_
#define _GH3X2X_DEMO_MP_H_

#include "gh3x2x_mp_config.h"
#include "gh3x2x_mp_define.h"

#ifndef GH3X2X_MP_UNUSED_VAR
#define GH3X2X_MP_UNUSED_VAR(X) ((void)(X))
#endif  // GH3X2X_UNUSED_VAR

/**
 * @fn     void Gh3x2xDemoMp_Init()
 *
 * @brief  GH3x2x mp init
 *
 * @attention   None
 *
 * @param[in]   none
 * @param[out]  None
 *
 * @return  none
 */
void Gh3x2xDemoMp_Init(void);

/**
 * @fn     GU8 Gh3x2xDemoMp_StartTest()
 *
 * @brief  GH3x2x mp start test
 *
 * @attention   None
 *
 * @param[in]   none
 * @param[out]  None
 *
 * @return  0:test ok, other:error code
 */
GU8 Gh3x2xDemoMp_StartTest(void);

/**
 * @fn     void Gh3x2xDemoMp_SetTestParam_PPG(stGh3x2xMp_TestParam *stTestParam)
 *
 * @brief  GH3x2x MP set PPG test items param
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *stPPG_TestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam_PPG(stGh3x2xMp_PPGTestParam *stPPG_TestParam);

/**
 * @fn     void Gh3x2xDemoMp_SetTestParam_ECG(stGh3x2xMp_ECGTestParam *stECG_TestParam)
 *
 * @brief  GH3x2x MP set ECG test items param
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *stECG_TestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam_ECG(stGh3x2xMp_ECGTestParam *stECG_TestParam);

/**
 * @fn     void Gh3x2xDemoMp_SetTestParam(stGh3x2xMp_TestParam *stTestParam)
 *
 * @brief  GH3x2x MP set all test items param.
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_TestParam *stTestParam
 * @param[out]  None
 *
 * @return  None
 */
void Gh3x2xDemoMp_SetTestParam(stGh3x2xMp_TestParam *stTestParam);

/**
 * @fn     void Gh3x2xDemoMp_PrintTestResult(stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult)
 *
 * @brief  GH3x2x mp print all test result
 *
 * @attention   It must be called after the test ends
 *
 * @param[in]   stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult
 * @param[out]  None
 *
 * @return  none
 */
void Gh3x2xDemoMp_PrintTestResult(stGh3x2xMp_TestParam *stTestParam, stGh3x2xMp_TestResult *stTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_CommunicationTest(GCHAR *para, GCHAR *TestResult)
 *
 * @brief  GH3x2x MP Communication Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_CommunicationTest(GU8 *puchPara, GU8 *pstTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_CheckChipIDTest(GU8 *puchPara, GU8 *puchTestResult)
 *
 * @brief  GH3x2x MP Check Chip ID Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_CheckChipIDTest(GU8 *puchPara, GU8 *puchTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_RstIntTest(GCHAR *para, GCHAR *TestResult)
 *
 * @brief  GH3x2x MP Communication Test.
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, GU8 *pushTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_RstIntTest(GU8 *puchPara, GU8 *pstTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_EfuseTest(GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP efuse Test, check FT/CP and crc, and get chipUid
 *
 * @attention   None
 *
 * @param[in]   GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_EfuseTest(GU8 *puchPara, stGh3x2xMp_EfuseTestResult *pstTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_PPGTest(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP PPG Test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_PPGTest(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult);

/**
 * @fn     GU8 Gh3x2xDemoMp_ECGTest(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  GH3x2x MP ECG Test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  None
 *
 * @return  error code
 */
GU8 Gh3x2xDemoMp_ECGTest(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult);


#endif /* _GH3X2X_DEMO_MP_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
