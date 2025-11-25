/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_mp_config.h
 *
 * @brief   header file of gh3x2x_mp_config.c
 *
 * @author  Goodix Product Tools Development Team
 *
 */
#ifndef _GH3X2X_MP_CONFIG_H_
#define _GH3X2X_MP_CONFIG_H_

#include <stdint.h>

/* communicate interface macro */
#define __GH3X2X_MP_INTERFACE_I2C__            			  (1)     							/**< interface use i2c >**/
#define __GH3X2X_MP_INTERFACE_SPI__            			  (2)     							/**< interface use spi >**/

/* code type type macro */
#define __GH3X2X_DRV_MODE__             		    	  (1)     							/**< include drv code >**/
#define __GH3X2X_MP_MODE_                 		    	  (0)     							/**< just mp code >**/

/* drv interface type macro */
#define __GH3X2X_DRV_LIB_INTERFACE__					  (1)     							/**< mp test use drv lib interface >**/
#define __GH3X2X_AFE_DRV_INTERFACE__					  (2)								/**< mp test use afe drv interface >**/

/* type redefine */
typedef uint8_t             GU8;    /**< 8bit unsigned integer type */
typedef int8_t              GS8;    /**< 8bit signed integer type */
typedef uint16_t            GU16;   /**< 16bit unsigned integer type */
typedef int16_t             GS16;   /**< 16bit signed integer type */
typedef int32_t             GS32;   /**< 32bit signed integer type */
typedef uint32_t            GU32;   /**< 32bit unsigned integer type */
typedef float               GF32;   /**< float type */
typedef double              GD64;   /**< double type */
typedef char                GCHAR;  /**< char type */
typedef unsigned char       GBOOL;  /**< bool type */
typedef long long           GS64;   /**< 64bit signed integer type */


//config
#define __GH3X2X_MP_USE_DRV_INTERFACE__				  	  (__GH3X2X_DRV_MODE__)								/**< use drv or mp iterface define, 0:use mp lib interface i2c/spi,  1:use drv interface i2c/spi >**/

#if(__GH3X2X_MP_USE_DRV_INTERFACE__)
#define __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__              (__GH3X2X_DRV_LIB_INTERFACE__)    /**< drv interface config __GH3X2X_DRV_LIB_INTERFACE__ /__GH3X2X_AFE_DRV_INTERFACE__ >**/           
#else
#define __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__              (0)    /**< drv interface config __GH3X2X_DRV_LIB_INTERFACE__ /__GH3X2X_AFE_DRV_INTERFACE__ >**/           
#endif

#define __GH3X2X_MP_INTERFACE__                			  (__GH3X2X_MP_INTERFACE_I2C__)     /**< interface config __GH3X2X_MP_INTERFACE_I2C__ /__GH3X2X_MP_INTERFACE_SPI__ >**/

#define __GH3X2X_MP_LOG_LVL__                          	  (2)                          		/**< gh3x2x mp log level, <==0>off, <==1>error log, <==2>info log >**/

#define __GH3X2X_MP_PRINT_RAWDATA__						  (0)								/**< 0:close print rawdata,  1:open print rawdata >**/
#define __GH3X2X_MP_PRINT_TEST_RESULT__					  (1)								/**< 0:close print test result,  1:open print test result >**/

#define GH3X2X_MP_LOG_MAX_LEN                             (256)                        		/**< gh3x2x mp log max len >**/

#define MAX_PPG_RAWDATA_SAMPLE_NUM						  (550) 							/**< ppg max rawdata sample, not more than MAX_RAWDATA_SAMPLE_NUM >**/
#define MAX_RAWDATA_SAMPLE_NUM							  (4096)							/**< ECG max rawdata sample >**/
#define MAX_FFT_LENGTH									  (4096) 							/**< ECG FFT process data max length, must(MAX_FFT_LENGTH*2*4) < (RAWDATA_BUF_SIZE / 2) >**/

/***********************************  DO NOT MODIFY FOLLOWING CODE *******************************/

#ifdef GOODIX_DEMO_PLATFORM
#undef __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__
#define __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ (__GH3X2X_DRV_LIB_INTERFACE__)
#endif

#ifdef GH3X2X_AFE_DRV_CODE
#undef __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__
#define __GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ (__GH3X2X_AFE_DRV_INTERFACE__)
#endif

#endif /* _GH3X2X_MP_CONFIG_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
