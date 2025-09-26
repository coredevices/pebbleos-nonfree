#ifndef GOODIX_ALGO_BT_H
#define GOODIX_ALGO_BT_H
#include <stdint.h>        
#include "goodix_algo.h"
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if defined(_WIN32)    || defined(_WIN64)
#define DLL_API_BODY_TEMPERATURE extern _declspec(dllexport)
#else
#define DLL_API_BODY_TEMPERATURE
#endif

/* **************************************const value define*********************************************** */
/**
 * @brief configuration virtual registers define
*/
#define BT_VIRTUAL_REG_UINIT             (sizeof(GU16))//length of virtual register
#define BT_CFG_HEADER_LENGTH                 (0x06)    //length of header registers
#define BT_CFG_HWINFO_LENGTH                 (0x2D)    //length of hardware information registers
#define BT_CFG_THRES_LENGTH                  (0x14)    //length of threshold registers
#define BT_CFG_ALGO_LENGTH                   (0x64)    //length of algorithm configuration registers
#define BT_CFG_TOTAL_LENGTH                  (0xAB)    //length of configuration

/**
 * @brief algorithm errorcode define
*/
// common errorcode
#define BT_ERRORCODE_OK                      (0x00)    //Errorcode: ok
#define BT_ERRORCODE_UNINIT                  (0x01)    //Errorcode: the initialization function was not called
//function errorcode
#define BT_ERRORCODE_NULL_PTR                (0x03)    //Errorcode: called empty pointer
#define BT_ERRORCODE_MALLOC_FAIL             (0x04)    //Errorcode: out of memory
#define BT_ERRORCODE_INVALID_PARAM           (0x05)    //Errorcode: illegal parameter
#define BT_ERRORCODE_FAIL                    (0x06)    //Errorcode: calculation failed
//module base value   
#define BT_ERRORCODE_BASE_VALUE_INIT         (0x10)    //Errorcode: preprocessing module
#define BT_ERRORCODE_BASE_VALUE_PRE          (0x20)    //Errorcode: preprocessing module
#define BT_ERRORCODE_BASE_VALUE_ALGO         (0x30)    //Errorcode: algorithm module

#define BT_MAX_CH_NUM                        (0x02)    //Channel number

/* ******************************************enum define************************************************** */

/**
 * @brief BT device type enum
*/
typedef enum
{
    CB62K928D = 0,      
    CB62K552D,             
    DT104D3950,           
}EM_BT_NTC_TYPE;
    
/**
 * @brief BT R_T convert flag
*/
typedef enum
{
    EM_CONVER_OK = 0,      
    EM_TEMP_OUT_RANGE_H,             
    EM_TEMP_OUT_RANGE_L,
    EM_TEMP_OUT_RANGE_WRONG_INPUT, 
}EM_BT_ALGO_OUT_RANGE;
    
    
/**
 * @brief BT device type enum
*/
typedef enum
{
    EM_BT_DEVICE_WATCH_BAND = 0x00,      //Device type: wearable watch or band
    EM_BT_DEVICE_INEAR_DEEP,             //Device type: in-depth earphones
    EM_BT_DEVICE_INEAR_NORMAL,           //Device type: shallow in-ear earphones
    EM_BT_DEVICE_TYPE_MAX,               //Device type: device type total number
}EM_BT_DEVICE_TYPE;

/**
 * @brief BT monitor mode
*/
typedef enum
{
    EM_MODE_ONCE = 0x00,                 //monitor mode: single measurement
    EM_MODE_CONTINUE,                    //monitor mode: continue measurement
    EM_MODE_TEST,                        //monitor mode: mass production testing
    EM_MODE_MAX,                         //monitor mode: monitor mode total number
}EM_BT_MONITOR_MODE;

/**
 * @brief BT monitor mode
*/
typedef enum
{
    EM_BT_TYPE_SHELL = 0x00,             //monitor body temperature type: single
    EM_BT_TYPE_CORE,                     //monitor body temperature type: continue
    EM_BT_TYPE_MAX,                      //monitor body temperature type: mode total number
}EM_BT_TYPE;

/**
 * @brief BT rawdate type
*/
typedef enum
{
    EM_RAWDATA_ADC = 0x00,               //rawdata type: normal ADC code
    EM_RAWDATA_RESIS,                    //rawdata type: NTC resistance  
    EM_RAWDATA_TEMP,                     //rawdata type: temperature 
    EM_RAWDATA_MAX,                      //rawdata type: type total
}EM_BT_RAWDATA_TYPE;

/**
 * @brief BT sensor type
*/
typedef enum
{
    EM_SENSOR_NTC = 0x00,               //sensor type: NTC
    EM_SENSOR_IC,                       //sensor type: IC  
    EM_SENSOR_MAX,                      //sensor type: type total
}EM_BT_SENSOR_TYPE;

/* ******************************************struct define************************************************ */

/**
 * @brief data min max average
*/
typedef struct
{
    GS16 nMin;
    GS16 nMax;
    GS32 nSum;
}STBtPreprocRwatemp;

/**
 * @brief final output struct
*/
typedef struct
{
    GU8 chFrameIndex;                                  // frame count
    STBtPreprocRwatemp stRawTemp[BT_MAX_CH_NUM];
    /* Body Temperature */
    GS16 nFrameTemperature[BT_MAX_CH_NUM];                         //finaloutput: (included) shell body temperature
    GS16 nSecTemperature[BT_MAX_CH_NUM];                           //finaloutput: (included) core body temperature
    GU8 chOutRangeFlag;                                 //
}STGH3x2xDrvBtOutputResult;


/**
 * @brief initialize configuration
*/
typedef struct
{
    /*********************** configuration header 6 words *****************************/
    //config length & config version (up to the tool)
    GU16 unCfgLen;                       //initialize configuration: size of struct
    GU16 unCfgVer;                       //initialize configuration: version of configuration
    GU16 unCfgTimL;                      //initialize configuration: configuration compiler time
    GU16 unCfgTimH;                      //initialize configuration: configuration compiler time
    GU16 unCfgProjCode;                  //initialize configuration: customer code
    GU16 unCheckSum;                     //initialize configuration: configuration checksum

    /*********************** hardware information 45 words ****************************/
    //device information (up to the device hardware)
    GU16 unDeviceType;                   //initialize configuration: type of device
    GU16 unChannelNum;                   //initialize configuration: number of channels
    GU16 unMonitorMode;                  //initialize configuration: monitor mode
    GU16 unMonitorType;                  //initialize configuration: monitor type
    GU16 unTempRangeUpper;               //initialize configuration: temperature upper limit
    GU16 unTempRangeLower;               //initialize configuration: lower temperature limit
    //rawdata information (up to the application)
    GU16 unSampleFrequency;              //initialize configuration: sampling frequency 
    GU16 unRawdataType;                  //initialize configuration: rawdata meaning 
    //inherent deviation (up to the device)
    GS16 nTempConstOffset0;              //initialize configuration: inherent deviation of device //GS16 force to GU16
    GS16 nTempConstOffset1;              //initialize configuration: inherent deviation of device //GS16 force to GU16
    //reserved
    GU16 unDeviceReserved[2];            //initialize configuration: device info reserved

    // temperature sensor information
    GU16 unSensorType;                   //initialize configuration: sensor type
    GU16 unSensorNtcRef;                 //initialize configuration: NTC ref resistance: unit:10ohm
    GU16 unSensorNtcRT_Len;              //initialize configuration: NTC R-T table length 
    GU16 unSensorParameter[30];          //initialize configuration: sensor parameters buffer NTC_R:Order: unTempRangeLower to unTempRangeUpper 
                                                                     //unTempRangeLower step 2 celsius until 25 celsius, 25 celsius step 1 celsius to unTempRangeUpper
    /*********************** threshold information 20 words ****************************/
    // threshold 
    GU16 unBufferMax;                    //initialize configuration: data buffer length, unit is senconds, default:300s

    GU16 unTimeWindow;                   //initialize configuration: data buffer window, unit is senconds, default: 20s
    GU16 unTimeStep;                     //initialize configuration: step size of sliding window, unit is senconds, default: 5s
    GU16 unTimeFast;                     //initialize configuration: fastest output time, unit is senconds, at least 60s 
    GU16 unTimeFastSlidWindow;           //initialize configuration: fastest output slid window size
    GU16 unTimeFastSlidStep;             //initialize configuration: fastest output slid window step

    GU16 unCntStable;                    //initialize configuration: stable time
    GU16 chTempFluctuate;                //initialize configuration: fluctuate temperature
    GU16 unCntFluctRecover;              //initialize configuration: recver time frome fluctuate
    GU16 unTimeStable;                   //initialize configuration: minimum stable time requirement, unit is senconds, at least: 30s

    GU16 unTimeRecover;                  //initialize configuration: minimum recover time requirement, unit is senconds, at least: 30s 

    GU16 unTempStable;                   //initialize configuration: minimum stable time requirement, unit is senconds, at least: 30s
    GU16 unTempStableUpper;              //initialize configuration: minimum stable time requirement, unit is senconds, at least: 30s
    GU16 unTempStableLower;              //initialize configuration: minimum stable time requirement, unit is senconds, at least: 30s
    GU16 unThresReserved[6];             //initialize configuration: threshold reserved

    /*********************** algorithm information 100 words ****************************/
    // Please contact GOODIX
    GU16 unConfig[100];                  //initialize configuration: algorithm   

}STGoodixAlgorithmBtControlConfigParametersBuffer;

/**
 * @brief Gsensor information
*/
typedef struct
{
    GS16 nGSensorX;                       //refer: acc_x_axis, normalized to 512/g(9.80m/s^2)
    GS16 nGSensorY;                       //refer: acc_y_axis, normalized to 512/g(9.80m/s^2)
    GS16 nGSensorZ;                       //refer: acc_z_axis, normalized to 512/g(9.80m/s^2)
}STGoodixAlgorithmBtCalculateInputAccdata;

/**
 * @brief external input reference
*/
typedef struct
{
    /* Personal information */
    GU8 uchAge;                           //refer: (recommend) age (0-100)
    GU8 uchGender;                        //refer: (recommend) gender (0- male, 1- female)
    GU16 uchHeight;                       //refer: (recommend) height (unit: cm)
    GU16 uchWeight;                       //refer: (recommend) weight (unit: kg)
    /* physiological signal */
    GU16 unHR;                            //refer: (required)  heart rate (unit: bpm)
    GU16 unRRI;                           //refer: (recommend) RR interval (unit: ms)
    GU16 unSPO2;                          //refer: (optional)  Oxygen saturation (unit: normalized to 0-100)
    GS16 unET;                            //refer: (optional)  enviroment temperature (unit: 0.01 Celsius)
    GU16 unBT;                            //refer: (optional)  core body temperature (unit: 0.01 Celsius)
    GU16 unPI;                            //refer: (optional)  perfusion index (unit: normalized to 0-100)
    /* wearing state */
    GU16 unWear;                          //refer: (recommend) wearing quality (unit: normalized to 0-100)
    /* device info */
    GU16 unChargeFlag;                    //refer: (required)  the device was charged ten minutes ago,0- no  1- charged 
    GU16 unReserves[4];                   //refer: (reserved)  reserved
}STGoodixAlgorithmBtInputRef;

/**
 * @brief output struct
*/
typedef struct
{
    /* flag of Body Temperature */
    GU8 uchRefresh;                       //output: (included) refresh flag
    GU8 uchCBTUpate;
    GU8 uchConfidence;                    //output: (included) confidence level, normalized to 0-100
    GU8 uchState;                         //output: (included) 0-default 1-rise 2-stable 3-under-stable 4-fluctuate
    GU8 uchReserved[5];                   //output: (Reserved) reserved
    /* Body Temperature */
    GS16 nBT;                             //output: (included) shell body temperature
    GS16 nCBT;                            //output: (included) core body temperature
    GS16 nFPCBT;                          //output: (included) core body temperature of quick forecast result
    GS16 nReserved[2];                    //output: (Reserved) reserved
}STGoodixAlgorithmBtCalculateOutputResult;

/**
 * @brief final output struct
*/
typedef struct
{
    /* flag of Body Temperature */
    GU8 uchRefresh;                       //finaloutput: (included) refresh flag
    GU8 uchConfidence;                    //finaloutput: (included) confidence level, normalized to 0-100
    GU8 uchState;                         //finaloutput: (included) 0-default 1-rise 2-stable 3-under-stable 4-fluctuate
    GU8 uchReserved[5];                   //finaloutput: (Reserved) Reserved
    /* Body Temperature */
    GS16 nBT;                             //finaloutput: (included) shell body temperature
    GU16 unCBT;                           //finaloutput: (included) core body temperature
    GS16 nReserved[2];                    //finaloutput: (Reserved) reserved
}STGoodixAlgorithmBtFinalOutputResult;

/* *************************************Implemented by customer******************************************* */
/**
 * @brief memory malloc function
 * @param size
 * @return pointer of memory
*/
extern void* GoodixSensorAlgoMalloc(GS32 size);

/**
 * @brief memory free function
 * @param addr
*/
extern void GoodixSensorAlgoFree(void* addr);

/**
 * @brief algorithm log function
 * @param addr
*/
extern void GoodixSensorAlgoLog(GCHAR* pLogStr);

/* ******************************************API INTERFACE************************************************ */
DLL_API_BODY_TEMPERATURE GU8* GoodixBtAlgoVersion(void);
DLL_API_BODY_TEMPERATURE GS32 GoodixBtAlgoInit(STGoodixAlgorithmInfo* pstGoodixAlgorithmInfo);
DLL_API_BODY_TEMPERATURE GS32 GoodixBtAlgoCalculate(STGoodixAlgorithmInfo* pstGoodixAlgorithmInfo);
DLL_API_BODY_TEMPERATURE GS32 GoodixBtAlgoFinalCalculate(STGoodixAlgorithmInfo* pstGoodixAlgorithmInfo);
DLL_API_BODY_TEMPERATURE GS32 GoodixBtAlgoDeinit(STGoodixAlgorithmInfo* pstGoodixAlgorithmInfo);

/* ******************************************************************************************************* */

#ifdef __cplusplus
}
#endif
#endif //GOODIX_ALGO_BT_H
