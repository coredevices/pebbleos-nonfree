/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 * 
 * @file    gh3x2x_demo_mp_process.c
 * 
 * @brief   process function of mp test
 * 
 * @author  Goodix Product Tools Development Team
 * 
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "gh3x2x_demo_mp_process.h"
#include "gh3x2x_mp_common.h"
#include "gh3x2x_mp_algorithm.h"
#include "gh3x2x_mp_param_config.h"

#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
	#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
		#include "gh_drv.h"
	#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
		#include "gh3x2x_afe_drv.h"
	#endif
#endif


GU8 GH3x2xMp_IntFlag = 0;  //int event flag

GS32 GH3x2xMp_RawdataHandleArr[MAX_RAWDATA_SAMPLE_NUM] = {0};
GU8 GH3x2xMp_RawdataBuf[RAWDATA_BUF_SIZE] = {0};
GF32 *fArrHandle_Temp = NULL;

GU32 Tia_Switch(GU32 tia)
{
	switch (tia)
	{
	case 0:
		return 10 ;
	case 1:
		return 25 ;
	case 2:
		return 50 ;
	case 3:
		return 75 ;
	case 4:
		return 100 ;
	case 5:
		return 250 ;
	case 6:
		return 500 ;
	case 7:
		return 750 ;
	case 8:
		return 1000;
	case 9:
		return 1250 ;
	case 10:
		return 1500 ;
	case 11:
		return 1750 ;
	case 12:
		return 2000 ;

	default:
		return 1;
	}
}


/**
 * @fn     gh3x2x_demo_mp_write_reg(GU16 usReg_Addr, GU16 usReg_Value)
 *
 * @brief  GH3x2x mp write reg
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr, GU16 usReg_Value
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_write_reg(GU16 usReg_Addr, GU16 usReg_Value)
{
	GU8 ret = 1;

	
	#if(__GH3X2X_MP_USE_DRV_INTERFACE__)
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			GH3X2X_WriteReg(usReg_Addr, usReg_Value);
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
			GH3X2X_AFE_WRITE_REG(usReg_Addr, usReg_Value);
		#endif
		
	#else //__GH3X2X_MP_USE_DRV_INTERFACE__ == 0, use mp lib interface
		GU8 uchBuffArr[4] = {0};
		uchBuffArr[0] = usReg_Addr >> 8;
		uchBuffArr[1] = usReg_Addr & 0xFF;
		uchBuffArr[2] = usReg_Value & 0xFF;
		uchBuffArr[3] = usReg_Value >> 8;
		#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
			gh3x2x_mp_i2c_write(device_id, uchBuffArr, 4);
		#else //__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
			gh3x2x_mp_spi_write(uchBuffArr, 4);
		#endif
	#endif
	
	return ret;
}

/**
 * @fn     GU16 gh3x2x_demo_mp_read_reg(GU16 usReg_Addr)
 *
 * @brief  GH3x2x mp read reg
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr
 * @param[out]  None
 *
 * @return  read reg value
 */
GU16 gh3x2x_demo_mp_read_reg(GU16 usReg_Addr)
{
	GU16 usRegData = 0;
	
	#if(__GH3X2X_MP_USE_DRV_INTERFACE__)
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			usRegData = GH3X2X_ReadReg(usReg_Addr);
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
			usRegData = GH3X2X_AFE_READ_REG(usReg_Addr);
		#endif
	#else //__GH3X2X_MP_USE_DRV_INTERFACE__ == 0, use mp lib interface
		GU8 uchCmdBuffArr[2] = {0};
		GU8 uchReadBuffArr[2] = {0};
		
		uchCmdBuffArr[0] = usReg_Addr >> 8;
		uchCmdBuffArr[1] = usReg_Addr & 0xFF;
	
		#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
			gh3x2x_mp_i2c_read(device_id, uchCmdBuffArr, 2,  uchReadBuffArr, 2);
		#else //__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
			gh3x2x_mp_spi_read(uchCmdBuffArr, 2,  uchReadBuffArr, 2);
		#endif
		usRegData = (uchReadBuffArr[0] << 8) + uchReadBuffArr[1];
	#endif
	
	return usRegData;
}

/**
 * @fn    GU8 gh3x2x_demo_mp_read_regs(GU16 usReg_Addr, GU8 uchRegs_Read[], GU16 usLength)
 *
 * @brief  GH3x2x mp read regs
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr, GU16 usLength
 * @param[out]  GU8 uchRegs_Read[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_read_regs(GU16 usReg_Addr, GU8 uchRegs_Read[], GU16 usLength)
{
	GU8 ret = 1;
	
	#if(__GH3X2X_MP_USE_DRV_INTERFACE__)
		GU16 length_temp = usLength / 2;
		
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			GH3X2X_ReadRegs(usReg_Addr, (GU16 *)uchRegs_Read, length_temp);
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
			GH3X2X_AFE_READ_REGS(usReg_Addr, (GU16 *)uchRegs_Read, length_temp);
		#endif
		for(GU16 i = 0; i < length_temp; i++)
		{
			GU8 swap_val = uchRegs_Read[2*i];
			uchRegs_Read[2*i] = uchRegs_Read[2*i + 1];
			uchRegs_Read[2*i + 1] = swap_val;
		}
	#else //__GH3X2X_MP_USE_DRV_INTERFACE__ == 0, use mp lib interface
		GU8 uchCmdBuffArr[2] = {0};
	
		uchCmdBuffArr[0] = usReg_Addr >> 8;
		uchCmdBuffArr[1] = usReg_Addr & 0xFF;
		#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
			gh3x2x_mp_i2c_read(uchCmdBuffArr, 2,  uchReadBuffArr, 2);
		#else //__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
			gh3x2x_mp_spi_read(uchCmdBuffArr, 2,  uchRegs_Read, usLength);
		#endif
	#endif
	
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_write_reg_mask(GU16 usRegAddr, GU16 usMask, GU16 usRegVal)
 *
 * @brief  GH3x2x mp write reg mask
 *
 * @attention   None
 *
 * @param[in]   GU16 usReg_Addr
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_write_reg_mask(GU16 usRegAddr, GU16 usMask, GU16 usRegVal)
{
	GU8 ret = 1;
    
	GU16 temp_val = gh3x2x_demo_mp_read_reg(usRegAddr);
	
	//temp_val |= 0x0602;//no sleep
	temp_val &= ~usMask;
	temp_val |= usRegVal & usMask;
	
	gh3x2x_demo_mp_write_reg(usRegAddr, temp_val);

    return ret;
}

/**
 * @fn     void gh3x2x_demo_mp_send_cmd(GU8 uchCmd)
 *
 * @brief  send cmd to gh3x2x chip
 *
 * @attention   None
 *
 * @param[in]   GU8 uchCmd
 * @param[out]  None
 *
 * @return  ret
 */
void gh3x2x_demo_mp_send_cmd(GU8 uchCmd)
{
	#if(__GH3X2X_MP_USE_DRV_INTERFACE__)
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			GH3X2X_SendCmd(uchCmd);
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
			GH3X2X_AFE_SEND_CMD(uchCmd);
		#endif
	#else //__GH3X2X_MP_USE_DRV_INTERFACE__ == 0, use mp lib interface
		#if ( __GH3X2X_MP_INTERFACE__ == __GH3X2X_INTERFACE_I2C__ )
			gh3x2x_mp_i2c_send_cmd(device_id, uchCmd);
		#else //__GH3X2X_INTERFACE__ == __GH3X2X_INTERFACE_SPI__
			gh3x2x_mp_spi_send_cmd(uchCmd);
		#endif
	#endif
}

/**
 * @fn     void gh3x2x_demo_mp_reset_operation(void)
 *
 * @brief  reset gh3x2x chip
 *
 * @attention   None
 *
 * @param[in]   None
 * @param[out]  None
 *
 * @return  None
 */
void gh3x2x_demo_mp_reset_operation(void)
{
	//rst gh3x2x,set low and set high later
	#if ( __GH3X2X_MP_USE_DRV_INTERFACE__ == __GH3X2X_DRV_MODE__ )
		#if (__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_DRV_LIB_INTERFACE__)
			#if __SUPPORT_HARD_RESET_CONFIG__
				GH3X2X_HardReset();
			#else
				GH3X2X_SoftReset();
			#endif
		#else //__GH3X2X_MP_USE_DRV_INTERFACE_TYPE__ == __GH3X2X_AFE_DRV_INTERFACE__
			GH3X2X_AFE_Reset();
		#endif
	
	#else 	
		gh3x2x_mp_reset_pin_ctrl(0);
		gh3x2x_mp_delay_ms(5);
		gh3x2x_mp_reset_pin_ctrl(1);
	#endif
	
	gh3x2x_mp_delay_ms(10); //must delay
}

/**
 * @fn    GU8 gh3x2x_demo_mp_wait_for_int(GU16 usDutFlag, GU16 usTimeOut_ms)
 *
 * @brief  wait for int 
 *
 * @attention   None
 *
 * @param[in]   GU16 usDutFlag, GU16 usTimeOut
 * @param[out]  none
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_wait_for_int(GU16 usDutFlag, GU16 usTimeOut_ms)
{
	GU8 ret = 1;
	GU32 time_count = 0;
	usTimeOut_ms *= 100;
	GH3x2xMp_IntFlag = 0;
	
	while(time_count++ < usTimeOut_ms)
	{
		gh3x2x_mp_delay_us(10);
		if(GH3x2xMp_IntFlag)
		{
			GH3x2xMp_IntFlag = 0;
			//clear int
			GU16 temp_read_val = gh3x2x_demo_mp_read_reg(0x508);
			gh3x2x_demo_mp_write_reg(0x508, temp_read_val);
			
			if(usDutFlag & temp_read_val)
			{
				return ret;
			}
		}
	}
	
	ret = 0;
	GH3X2X_MP_LOG_L1("Gh3x2x mp no interruption!\r\n ");
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_set_chip_config(const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen)
 *
 * @brief  GH3x2x mp write chip reg config
 *
 * @attention   None
 *
 * @param[in]   const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_chip_config(const stGh3x2xReg stGh3x2xMpRegConfig[], GU16 usConfigLen)
{
	GU8 ret = 1;
	GU8 retry_times = 3;
	GU8 i = 0;
	GU8 isWriteNormal = 1;
	
	for(i = 0; i < retry_times; i++)
	{
		isWriteNormal = 1;
		for(GU32 j = 0; j < usConfigLen; j++)
		{
			if((0 == stGh3x2xMpRegConfig[j].usRegAddr) && (0 == stGh3x2xMpRegConfig[j].usRegValue))
				continue;
			
			gh3x2x_demo_mp_write_reg(stGh3x2xMpRegConfig[j].usRegAddr, stGh3x2xMpRegConfig[j].usRegValue);
			
			if(stGh3x2xMpRegConfig[j].usRegValue != gh3x2x_demo_mp_read_reg(stGh3x2xMpRegConfig[j].usRegAddr))
			{
				isWriteNormal = 0;
				break;
			}
		}
		if(isWriteNormal)
		{
			break;
		}
	}
	if(i == retry_times)
	{
		GH3X2X_MP_LOG_L1("Gh3x2x mp set chip reg config fail! \r\n ");
		ret = 0;
	}
	
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_set_slot_config(stGh3x2xMp_SlotConfig stSlotConfig)
 *
 * @brief  GH3x2x mp set slot config
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_SlotConfig stSlotConfig
 * @param[out]  None
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_slot_config(stGh3x2xMp_SlotConfig stSlotConfig)
{
	GU8 ret = 1;
	GU8 index = 0;
	GU16 i = 0;
	stGh3x2xReg stCofigRegArr[20] = {0};
	
	if(SLOT_ONECH_MODE == stSlotConfig.Slot_Mode)
	{
		//Slot_cfgN refresh cycle is the system (sr_multiplier+1) times, RxN (ADCN) channel is enabled, data is written into FIFO
		stCofigRegArr[index].usRegAddr = 0x010A + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = 0x0040 | (stSlotConfig.Slot_Mutiple << 8) | (1 << (stSlotConfig.ADC_N + 2));
		
		//Clear all ADC channel connections
		stCofigRegArr[index].usRegAddr = 0x010C + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
        stCofigRegArr[index++].usRegValue = 0x0000;
		
		stCofigRegArr[index].usRegAddr = 0x010E + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
        stCofigRegArr[index++].usRegValue = 0x0000;
		
		//ADC_N is mapped to PD_N, only one PD channel can be opened at a time
		stCofigRegArr[index].usRegAddr = 0x010C + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N + (stSlotConfig.ADC_N / 2) * 2;
        stCofigRegArr[index++].usRegValue = stSlotConfig.PD_N << ((stSlotConfig.ADC_N % 2) * 8);
        
		//slot_cfgN ADCN TIA gain is TIA_RF
		stCofigRegArr[index].usRegAddr = 0x0112 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = (stSlotConfig.TIA_RF & 0x0F) << (stSlotConfig.ADC_N * 4);
        
		//slot_cfgN ADCN TIA capacitance is TIA_CF
		stCofigRegArr[index].usRegAddr = 0x0114 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = (stSlotConfig.TIA_CF & 0x0F) << (stSlotConfig.ADC_N * 4);
	}
	else //SLOT_MULTICH_MODE
	{
		//Slot_cfgN refresh cycle is the system (sr_multiplier+1) times, PDN (RxN) channel is enabled, data is written into FIFO
		stCofigRegArr[index].usRegAddr = 0x010A + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = 0x0040 | (stSlotConfig.Slot_Mutiple << 8) | (stSlotConfig.PD_N << 2);
        
		//Set ADC and PD one-to-one correspondence connection, ADCx is connected to PDx
		stCofigRegArr[index].usRegAddr = 0x010C + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = 0x0201;
		
		stCofigRegArr[index].usRegAddr = 0x010E + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
        stCofigRegArr[index++].usRegValue = 0x0804;
		
		GU16 temp = 0;
        for (temp = 0, i = 0; i < MAX_ADC_NUM; i++) 
			temp |= stSlotConfig.TIA_RF << (4 * i);
		//slot_cfgN ADCN TIA gain is TIA_RF
		stCofigRegArr[index].usRegAddr = 0x0112 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = temp;
		
        for (temp = 0, i = 0; i < MAX_ADC_NUM; i++) 
			temp |= stSlotConfig.TIA_CF << (4 * i);
		//slot_cfgN ADCN TIA capacitance is TIA_CF
		stCofigRegArr[index].usRegAddr = 0x0114 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
		stCofigRegArr[index++].usRegValue = temp;
	}
	
	//ADC integration times is ADC_int, bg suppression is bg_level, automatically dimming LED_ACG, turning off DC cancel and BG cancel
	stCofigRegArr[index].usRegAddr = 0x0110 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
	stCofigRegArr[index++].usRegValue = (stSlotConfig.LED_ACG << 8) | (stSlotConfig.ADC_N << 6) | (stSlotConfig.BG_Level << 4) | stSlotConfig.ADC_IntTimes;
    
	//LED DRV0 current is 0, not mapped to led
	stCofigRegArr[index].usRegAddr = 0x011E + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
	stCofigRegArr[index++].usRegValue = 0x0400;
	
	//LED DRV1 current is 0, not mapped to led
	stCofigRegArr[index].usRegAddr = 0x0120 + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N;
    stCofigRegArr[index++].usRegValue = 0x0400; 
	
	//LEDN current is LED_DRV
	stCofigRegArr[index].usRegAddr = 0x011E + SLOT_CFG_INDEX*stSlotConfig.Slot_Cfg_N + (stSlotConfig.LED_N / 4) * 2;
    stCofigRegArr[index++].usRegValue = ((stSlotConfig.LED_N % 4) << 8) | stSlotConfig.LED_Drv;
    
	//slot running time
	stCofigRegArr[index].usRegAddr = 0x01EC + 2 * stSlotConfig.Slot_Cfg_N;
	stCofigRegArr[index++].usRegValue = stSlotConfig.Slot_TMR;
	
	ret = gh3x2x_demo_mp_set_chip_config(stCofigRegArr, index);
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_set_slot_enable(GU8 uchSlotConfigEnable)
 *
 * @brief  Calculate which slots need to be enabled according to SLOT_CFG_EN, 
 * @brief  enable the corresponding slot, and output the number of enabled slots
 *
 * @attention   None
 *
 * @param[in]   GU8 uchSlotConfigEnable
 * @param[out]  GU8 uchSlotEnableNum
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_slot_enable(GU8 uchSlotConfigEnable, GU8 *uchSlotEnableNum)
{
	GU8 ret = 1;
	GU16 i = 0;
	GU8 index = 0;
	GU8 SlotMap[MAX_SLOT_NUM];
	stGh3x2xReg stCofigRegArr[20] = {0};
	
	memset(SlotMap, 0x08, MAX_SLOT_NUM); //init close all slot
	
	for (i = 0; i < MAX_SLOT_NUM; i++)
    {
		//Enable the corresponding slot according to the value of SLOT_CFG_EN
        if (uchSlotConfigEnable & (1 << i)) 
			SlotMap[(*uchSlotEnableNum)++] = i;
    }
	
	for (i = 0; i < MAX_SLOT_NUM / 2; i++)
	{
		//set slot_index
		stCofigRegArr[index].usRegAddr = 0x0100 + i * 2;
		stCofigRegArr[index++].usRegValue = ((SlotMap[1 + 2 * i] & 0x0F) << 8) | (SlotMap[0 + 2 * i] & 0x0F);
	}
	//slot_cfg enable
	stCofigRegArr[index].usRegAddr = 0x0108;
    stCofigRegArr[index++].usRegValue = uchSlotConfigEnable;
	
	if(!gh3x2x_demo_mp_set_chip_config(stCofigRegArr, index))
	{
		ret = 0;
		GH3X2X_MP_LOG_L1("Gh3x2x mp set chip reg config fail! \r\n ");
	}
	
	return ret;
	
}

/**
 * @fn     GU8 gh3x2x_demo_mp_set_LED_ACG_cfg(GU8 uchLED_N, GU8 uchLED_ACG_FS, stGh3x2xMp_PPGTestParam *pstTestPara)
 *
 * @brief  The automatic dimming test needs to adjust the LED driver gear to avoid the failure of the automatic dimming step by too large
 *
 * @attention   None
 *
 * @param[in]   GU8 uchSlotConfigEnable
 * @param[out]  GU8 *uchSlotEnableNum
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_set_LED_ACG_cfg(GU8 uchLED_N, GU8 uchLED_ACG_FS, stGh3x2xMp_PPGTestParam *pstTestPara)
{
	GU8 ret = 1;
	GU8 fs_temp = uchLED_ACG_FS & 0x03;//LED_ACG_FS range is 0~3
    stGh3x2xReg stCofigRegArr[20] = { 0 };
    int index = 0;
	
    if (uchLED_N < 4)
	{
		stCofigRegArr[index].usRegAddr = 0x0692;
        stCofigRegArr[index++].usRegValue = 0x0202 | (fs_temp << 4);
	}
    else
	{
		stCofigRegArr[index].usRegAddr = 0x0692;
        stCofigRegArr[index++].usRegValue = 0x0220 | fs_temp ;
	}
	
	//mixdata trigger down dimming threshold
	stCofigRegArr[index].usRegAddr = 0x0282;
    stCofigRegArr[index++].usRegValue = stGh3x2xPPG_AutoDimming.LED_Max_Rawdata;
	
	//rawdata triggers the upward dimming threshold
	stCofigRegArr[index].usRegAddr = 0x0284;
    stCofigRegArr[index++].usRegValue = stGh3x2xPPG_AutoDimming.LED_Min_Rawdata;
	
	//Dimming end upper threshold
	stCofigRegArr[index].usRegAddr = 0x0286;
    stCofigRegArr[index++].usRegValue = stGh3x2xPPG_AutoDimming.LED_Ref_Up;
	
	//Lower threshold of dimming end
	stCofigRegArr[index].usRegAddr = 0x0288;
    stCofigRegArr[index++].usRegValue = stGh3x2xPPG_AutoDimming.LED_Ref_Down;
	
	ret = gh3x2x_demo_mp_set_chip_config(stCofigRegArr, index);
	
	return ret;
}

/**
 * @fn     void gh3x2x_demo_mp_sampling_mode(GU8 uchMode)
 *
 * @brief  rawdata sampling start / stop
 *
 * @attention   None
 *
 * @param[in]   GU8 uchMode
 * @param[out] 
 *
 * @return  None
 */
void gh3x2x_demo_mp_sampling_mode(GU8 uchMode)
{
	if(START_SAMPLING == uchMode) //start
	{
		gh3x2x_demo_mp_write_reg(0x0508, 0x7FFF); //clear int
		gh3x2x_mp_delay_ms(10);
		GH3x2xMp_IntFlag = 0;
		gh3x2x_demo_mp_write_reg_mask(0x00, 0x01, 0x01); //Trigger the chip to start collecting rawdata
	}
	else //stop
	{
		gh3x2x_demo_mp_write_reg_mask(0x00, 0x01, 0x00); //stop get rawdata
	}
}

/**
 * @fn     GU8 gh3x2x_demo_mp_get_rawdata(GU16 uchRegAddr, GU8 uchRawdataArr[], GU32 usLength)
 *
 * @brief  get rawdata
 *
 * @attention   None
 *
 * @param[in]   GU16 uchRegAddr, GU32 usLength
 * @param[out]  GU8 uchRawdataArr[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_get_rawdata(GU16 uchRegAddr, GU8 uchRawdataArr[], GU32 usLength)
{
	GU8 ret = 1;
	GU32 i = 0;
	GU16 read_len = 0;
	GU16 read_byte_num = 0;
	
	if(usLength > RAWDATA_BUF_SIZE)
	{
		//error, over max read_byte_num
		GH3X2X_MP_LOG_L1("Gh3x2x mp get rawdata error, over read rawdata max num! \r\n ");
		ret = 0;
		return ret;
	}
		
	
	//get rawdata
	for(i = 0; i < usLength; i += read_byte_num)
	{
		if(!gh3x2x_demo_mp_wait_for_int(0x30, 200))
		{
			//error, wait for no int, exit the loop 
			GH3X2X_MP_LOG_L1("Gh3x2x mp get rawdata error, wait int over time! \r\n ");
			ret = 0;
			break;
		}
		read_len = gh3x2x_demo_mp_read_reg(0x050A); //read rawdata len from fifo cache	
		read_byte_num = read_len*4; //One sample rawdata has 4 bytes		
		gh3x2x_demo_mp_read_regs(uchRegAddr, uchRawdataArr + i, read_byte_num);

		if(0 == read_byte_num)
		{
			//error, read num ==0
			GH3X2X_MP_LOG_L1("Gh3x2x mp get rawdata error, read rawdata num equal 0! \r\n ");
			ret = 0;
			break;
		}
		if(read_byte_num > READ_RAWDATA_FIFO_MAX_BYTE_NUM)
		{
			//error, over max read_byte_num
			GH3X2X_MP_LOG_L1("Gh3x2x mp get rawdata error, over max read rawdata fifo byte num! \r\n ");
			ret = 0;
			break;
		}
	}
	
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_handle_rawdata(GU8 uchMode, GS32 nHandleRawdataArr[], GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, \
										GU8 uchRawdataArr[], GU32 unRawdataArrLength, GU8 uchSampleStep)
 *
 * @brief  handle rawdata
 *
 * @attention   None
 *
 * @param[in]  GU8 uchMode, GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, GU8 uchRawdataArr[], GU32 unRawdataArrLength,GU8 uchSampleStep
 * @param[out]  GS32 nHandleRawdataArr[]
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_handle_rawdata(GU8 uchMode, GS32 nHandleRawdataArr[], GU32 unHandleRawdataSkipLength, GU8 uchSlot_N, GU8 uchADC_N, \
										GU8 uchRawdataArr[], GU32 unRawdataArrLength, GU8 uchSampleStep)
{
	GU8 ret = 1;
	GU32 i = 0;
	GU8 cfg_temp = 0;
	GU8 ADC_temp = 0;
	GU8 ACG_temp = 0;
	GU32 rawdata_buf_pt[MAX_SLOT_NUM][MAX_ADC_NUM] = {0};
	
	
	for(i = 0; i < unRawdataArrLength; i++)
	{
		cfg_temp = (uchRawdataArr[4 * i] >> 5) & 0x07;
		ADC_temp = (uchRawdataArr[4 * i] >> 3) & 0x03;
		ACG_temp = (uchRawdataArr[4 * i] >> 1) & 0x03;
		
		if((cfg_temp == uchSlot_N) && (ADC_temp == uchADC_N))
		{
			if (cfg_temp >= MAX_SLOT_NUM || ADC_temp >= MAX_ADC_NUM)//Check if the rawdata format read is correct
			{
				ret = 0;
				break;
			}
			if(__GH3X2X_MP_PRINT_RAWDATA__)
				GH3X2X_MP_LOG("Gh3x2x mp handle rawdata, cfg: %d, adc: %d, acg: %d, rawdata number %d is: %d \r\n", cfg_temp, ADC_temp, ACG_temp, rawdata_buf_pt[cfg_temp][ADC_temp], (uchRawdataArr[4 * i + 1] << 16) | (uchRawdataArr[4 * i + 2] << 8) | (uchRawdataArr[4 * i + 3]));
			
			if (rawdata_buf_pt[cfg_temp][ADC_temp] >= unHandleRawdataSkipLength)
			{
				if(uchMode == SPEC_MODE_PPG)
				{
					if((rawdata_buf_pt[cfg_temp][ADC_temp] - unHandleRawdataSkipLength) < MAX_PPG_RAWDATA_SAMPLE_NUM)
						nHandleRawdataArr[rawdata_buf_pt[cfg_temp][ADC_temp] - unHandleRawdataSkipLength] = (uchRawdataArr[4 * i + 1] << 16) | (uchRawdataArr[4 * i + 2] << 8) | (uchRawdataArr[4 * i + 3]);
					else
					{
						GH3X2X_MP_LOG_L1("Gh3x2x mp handle ppg rawdata error, over max number of the rawdata sample! \r\n");
						ret = 0;
						break;
					}
				}
				else
				{
					if((rawdata_buf_pt[cfg_temp][ADC_temp] - unHandleRawdataSkipLength) < MAX_RAWDATA_SAMPLE_NUM)
						nHandleRawdataArr[rawdata_buf_pt[cfg_temp][ADC_temp] - unHandleRawdataSkipLength] = (uchRawdataArr[4 * i + 1] << 16) | (uchRawdataArr[4 * i + 2] << 8) | (uchRawdataArr[4 * i + 3]);
					else
					{
						GH3X2X_MP_LOG_L1("Gh3x2x mp handle ecg rawdata error, over max number of the rawdata sample! \r\n");
						ret = 0;
						break;
					}
					i += (uchSampleStep - 1);
				}
			}
			
			(rawdata_buf_pt[cfg_temp][ADC_temp])++;
		}
		
	}
	
	return ret;
}

/**
 * @fn     gh3x2x_demo_mp_ppg_get_rawdata(GS32 unRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM], GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num)
 *
 * @brief  ppg test get rawdata
 *
 * @attention   None
 *
 * @param[in]   GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num
 * @param[out]  GU32 unRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM]
 *
 * @return  ret
 */
//GU8 cfg_temp[1200] = {0};
//GU8 ADC_temp[1200] = {0};
//GU8 ACG_temp[1200] = {0};
//GU8 gh3x2x_demo_mp_ppg_get_rawdata(GS32 unRawdataArr[][MAX_ADC_NUM][MAX_PPG_RAWDATA_SAMPLE_NUM], GU16 usRawdataLen, GU16 usRawdataSkipLen, GU16 usLED_PD_Num)
//{
//	GU8 ret = 1;
//	GU32 i = 0;
//	GU32 rawdata_buf_pt[MAX_SLOT_NUM][MAX_ADC_NUM] = {0};
//	GU8 rawdata_buf[RAWDATA_BUF_SIZE] = {0};
//	if(usRawdataLen > MAX_PPG_RAWDATA_SAMPLE_NUM)
//		usRawdataLen = MAX_PPG_RAWDATA_SAMPLE_NUM; //max is 500
//	GU32 rawdata_total_len = (usRawdataLen + usRawdataSkipLen)*usLED_PD_Num;

//	do
//	{
//		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
//		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, rawdata_buf, rawdata_total_len*4)) //every rawdata have 4 bytes
//		{
//			ret = 0;
//			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
//			GH3X2X_MP_LOG_L1("Gh3x2x mp ppg get rawdata error! \r\n ");
//			break;
//		}
//		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
//		
//		//handle rawdata_buf
//		GU8 cfg_temp = 0;
//		GU8 ADC_temp = 0;
//		GU8 ACG_temp = 0;
//		
//		for(i = 0; i < rawdata_total_len; i++)
//		{
//			cfg_temp = (rawdata_buf[4 * i] >> 5) & 0x07;
//			ADC_temp = (rawdata_buf[4 * i] >> 3) & 0x03;
//			ACG_temp = (rawdata_buf[4 * i] >> 1) & 0x03;
//			
//			if (cfg_temp >= MAX_SLOT_NUM || ADC_temp >= MAX_ADC_NUM)//Check if the rawdata format read is correct
//			{
//				ret = 0;
//				break;
//			}
//			if(__GH3X2X_MP_PRINT_RAWDATA__)
//				GH3X2X_MP_LOG("Gh3x2x mp rawdata cfg: %d, adc: %d, acg: %d, rawdata: %d \r\n", cfg_temp, ADC_temp, ACG_temp, (rawdata_buf[4 * i + 1] << 16) | (rawdata_buf[4 * i + 2] << 8) | (rawdata_buf[4 * i + 3]));
//			
//			if (rawdata_buf_pt[cfg_temp][ADC_temp] >= usRawdataSkipLen)
//				unRawdataArr[cfg_temp][ADC_temp][rawdata_buf_pt[cfg_temp][ADC_temp] - usRawdataSkipLen] = (rawdata_buf[4 * i + 1] << 16) | (rawdata_buf[4 * i + 2] << 8) | (rawdata_buf[4 * i + 3]);
//			
//			(rawdata_buf_pt[cfg_temp][ADC_temp])++;
//		}
//		
//	}while(0);
//	
//	return ret;
//}

/**
 * @fn     void gh3x2x_demo_mp_calc_noise(GS32 nRawdataArr[], GU16 usLength, GF32 *fAvergeRawdata, GF32 *fNoise)
 *
 * @brief  cal noise
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength
 * @param[out]  GF32 *fAvergeRawdata, GF32 *fNoise
 *
 * @return  none
 */
void gh3x2x_demo_mp_calc_noise(GS32 nRawdataArr[], GU16 usLength, GF32 *fAvergeRawdata, GF32 *fNoise)
{
	GU32 i = 0;
	
	GD64 rawdata_sum = 0;
	GD64 tmp_sum = 0.0;
	
	if(0 == usLength)
	{
		GH3X2X_MP_LOG_L1("Gh3x2x mp cal noise error, rawdata length == 0! \r\n");
		*fNoise = 0x7FFFFFFF;
		return;
	}
	
	for(i = 0; i < usLength; i++)
	{
		rawdata_sum += nRawdataArr[i];
	}
	*fAvergeRawdata = 1.0 * rawdata_sum / usLength;
	
	for(i = 0; i < usLength; i++)
	{
		tmp_sum += pow(nRawdataArr[i] - (*fAvergeRawdata), 2);
	}
	*fNoise = 6.0 * sqrt(tmp_sum / usLength);
	
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_signal_filter(GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType)
 *
 * @brief  ECG signal filter
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType
				uchLpType:Low pass filter cut-off frequency
				uchHpType:High pass filter cut-off frequency
				uchFsType:Sampling rate of rawdata,0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
 * @param[out]  GS32 nRawdataArr[]: filter data
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_signal_filter(GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType)
{
	GU32 i = 0;
	GU8 ret = 1;
	
	GD64 filter_temp = nRawdataArr[0];
	for(i = 0; i < usLength; i++)
	{
		nRawdataArr[i] = nRawdataArr[i] - filter_temp; 
	}

	//filter
	gh3x2x_mp_signal_filter(nRawdataArr, usLength, uchHpType, uchLpType, uchFsType);
	
	return ret;
}

/**
 * @fn     GF32 gh3x2x_demo_mp_calc_LPCTR(GF32 fAverageRawdata, GU32 unTIA, GU32 unILed)
 *
 * @brief  cal LP CTR
 *
 * @attention   None
 *
 * @param[in]   GF32 fAverageRawdata, GU32 unTIA, GU32 unILed
 * @param[out]  
 *
 * @return  CTR
 */
GF32 gh3x2x_demo_mp_calc_LPCTR(GF32 fAverageRawdata, GU32 unTIA, GU32 unILed)
{
	GF32 fLP_CTR = 0.0;
	fLP_CTR = 1.0 * (RAWDATA_TO_VOL((GD64)fAverageRawdata - ADC_OFFSET) / (2*Tia_Switch(unTIA)) / (unILed*100.0 / 255));
	
	if((GD64)fLP_CTR < 0.00001)
		fLP_CTR = 0.0;
	
	return fLP_CTR;
}

/**
 * @fn     GF32 gh3x2x_demo_mp_ecg_cal_HPF_Ratio(GS32 nRawdataArr[], GU16 usLength)
 *
 * @brief  cal HPF amplitude ratio
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength
 * @param[out]  
 *
 * @return  HPF_Ratio
 */
GF32 gh3x2x_demo_mp_ecg_cal_HPF_Ratio(GS32 nRawdataArr[], GU16 usLength)
{
	GU16 i = 0;
	GF32 fHPF_Ratio = 0.0;
	GU32 sum[2] = { 0 };
    GU32 max = 0;
    
	GU16 start_index = 0;

    max = nRawdataArr[0];
    for (i = 0; i < usLength / 2 - 10; i++)
    {
        if (nRawdataArr[i] > max)
        {
            max = nRawdataArr[i];
            start_index = i;
        }
    }
    for (i = start_index; i < start_index + 3; i++)
    {
        sum[0] += nRawdataArr[i];
        sum[1] += nRawdataArr[usLength / 2 + i];
    }
    sum[0] = sum[0] / 3;
    sum[1] = sum[1] / 3;

    fHPF_Ratio = (GF32)(sum[1] - 8388608.0) / (GF32)(sum[0] - 8388608);
	
	return fHPF_Ratio;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GF32 *fFreq, \
													GU8 uchLpType, GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep)
 *
 * @brief  Cal the amplitude and frequency of the signal
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchLpType, GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep
				nRawdataArr:Data to be processed
				usLength:Data len
 				uchLpType:Low pass filter cut-off frequency
				uchHpType:High pass filter cut-off frequency
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				sCalStart:FFT start begin point
				sCalEnd:FFT start end point
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fAmp, GF32 *fFreq
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GF32 *fFreq, GU8 uchLpType, \
											GU8 uchHpType, GU8 uchFsType, GS16 sCalStart, GS16 sCalEnd, GF32 fRefFreq, GU8 uchStep)
{
	GU16 i = 0;
	GU8 ret = 1;
	GU16 fftlen = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8)); //Reuse GH3x2xMp_RawdataBuf memory
	fArrHandle_Temp = (GF32 *)GH3x2xMp_RawdataBuf; //Reuse GH3x2xMp_RawdataBuf memory
	
	if(uchStep <= 0)
		uchStep = 1;
	
	usLength = usLength / uchStep; //sampling
	
	if(usLength > MAX_RAWDATA_SAMPLE_NUM)
		usLength = MAX_RAWDATA_SAMPLE_NUM;
	
	gh3x2x_demo_mp_ecg_signal_filter(nRawdataArr, usLength, uchLpType, uchHpType, uchFsType);
	
	for(i = 0; i < usLength; i++)
	{
		nRawdataArr[i] *= 100; //in order to keep two decimal places
	}
	
	for(i = 0; i < usLength; i++)
	{
		nRawdataArr[i] = nRawdataArr[i]* ADC_REFER / ADC_MAX / GAIN * 1000; //uV
	}
	

	GU32 sampleRate = 500;
	if (uchFsType == 0)
	{
		sampleRate = 250;
	}
	else if (uchFsType == 1)
	{
		sampleRate = 500;
	}
	else if (uchFsType == 2)
	{
		sampleRate = 1000;
	}
	else
	{
		sampleRate = 500;
	}
	sampleRate /= uchStep;
	
	
	if (sCalEnd >= usLength || sCalEnd < sCalStart)
		fftlen = usLength - sCalStart;
	else
		fftlen = sCalEnd - sCalStart;
	
	for(i = 0; i < fftlen; i++)
	{
		fArrHandle_Temp[i] = (GF32)nRawdataArr[i + sCalStart] / 100;
	}
	
	//cal amp
	GF32 amp_1=0;
	GF32 freq_1=0;
	gh3x2x_demo_mp_ecg_fft_cal_amp_freq(fArrHandle_Temp, fftlen, fftlen, sampleRate, &amp_1, &freq_1, fRefFreq, 1);
	
	//memset(fArrHandle_Temp, 0, 2*MAX_FFT_LENGTH*sizeof(GF32));
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8)); //fArrHandle_Temp reuse GH3x2xMp_RawdataBuf memory
	for(i = 0; i < fftlen; i++)
	{
		fArrHandle_Temp[i] = (GF32)nRawdataArr[i + sCalStart] / 100;
	}
	
	//cal freq
	GF32 amp_2=0;
	GF32 freq_2=0;
	GU32 fftlen_2 = MAX_FFT_LENGTH;
	gh3x2x_demo_mp_ecg_fft_cal_amp_freq(fArrHandle_Temp, fftlen, fftlen_2, sampleRate, &amp_2, &freq_2, fRefFreq, 2);
	
	*fAmp = amp_1;
	*fFreq = freq_2;
	
	fArrHandle_Temp = NULL;
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cmmr_calculate(GS32 nRawdataArr[], GU16 usLength, GF32 *fCmmr, GU8 uchFsType, GF32 fRefFreq)
 *
 * @brief  Cal the ecg-cmmr
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GU8 uchFsType, GF32 fRefFreq
				nRawdataArr:Data to be processed
				usLength:Data len
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				fRefFreq:For reference frequency, find the maximum value near fref±5hz; if <0, find the maximum value between 0.1hz~fs/2
 * @param[out]  GF32 *fCmmr
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cmmr_calculate(GS32 nRawdataArr[], GU16 usLength, GF32 *fCmmr, GU8 uchFsType, GF32 fRefFreq)
{
	GU8 ret = 1;
	
	GF32 amp = 0;
	GF32 freq = 0;
	
	gh3x2x_demo_mp_ecg_cal_signal_amp_freq(nRawdataArr, usLength, &amp, &freq, 1, 5, uchFsType, 200, -1, fRefFreq, 1);
	
	if (amp) {
        *fCmmr = (20 * log10((GF32)1e6 / amp));
  } else {
        *fCmmr = 0;
  }
	GH3X2X_MP_LOG_L2("Gh3x2x mp cal cmmr freq: %.5f \r\n", freq);
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ecg_cal_signal_WaveTimeDomin_Amp(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq)
 *
 * @brief  Cal ecg-DCRange WaveTimeDomin Amp
 *
 * @attention   None
 *
 * @param[in]   GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq
				nRawdataArr:Data to be processed
				usLength:Data len
				uchFsType:Sampling rate type of rawdata, 0,250Hz; 1,500Hz; 2,1000Hz; others, 500Hz
				fRefFreq: Reference frequency
 * @param[out]  GF32 *fAmp
 *
 * @return  ret
 */
GU8 gh3x2x_demo_mp_ecg_cal_signal_WaveTimeDomin_Amp(GS32 nRawdataArr[], GU16 usLength, GF32 *fAmp, GU8 uchFsType, GF32 fRefFreq)
{
	GU8 ret = 1;
	GS32 i = 0;
	GS32 j = 0;
	
	GU32 peakMax = 0;
	GU32 peakMin = 0;
	GU32 counter = 0;
	GU32 temp = 0;
	GU32 unT = 0, unT_0 = 0;

	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8)); //Reuse GH3x2xMp_RawdataBuf memory
	fArrHandle_Temp = (GF32 *)GH3x2xMp_RawdataBuf; //Reuse GH3x2xMp_RawdataBuf memory
	
	
	//Sampling rate selection
	GU32 sampleRate = 500;
	if(uchFsType == 0)
	{
		sampleRate = 250;
	}
	else if (uchFsType == 1)
	{
		sampleRate = 500;
	}
	else if(uchFsType == 2)
	{
		sampleRate = 1000;
	}
	else
	{
		sampleRate = 500;
	}
	
	unT_0 = (GU32)sampleRate / fRefFreq;
	unT = unT_0 + unT_0 / 4;
	//find from the end
	for (i = usLength - 1; i >= unT; i -= unT_0)
	{
		counter++;
		//find min and max
		peakMax = nRawdataArr[i];
		peakMin = nRawdataArr[i];
		for (j = 0; j < unT; j++)
		{
			temp = nRawdataArr[i - j];
			if (temp > peakMax)
				peakMax = temp;
			if (temp < peakMin)
				peakMin = temp;
		}
		fArrHandle_Temp[counter - 1] = (GF32)(peakMax - peakMin);
	}
	
	
	//Minimum output amplitude
	GF32 ampMin = 0;
	for(i = 0; i < counter; i++)
	{
		ampMin += fArrHandle_Temp[i];
	}
	ampMin = ampMin / counter;
	
	*fAmp = (GF32)ampMin * ADC_REFER / ADC_MAX / GAIN * 1000; //uV
	
	fArrHandle_Temp = NULL;
	
	return ret;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_LPCTR_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ---- LP CTR test
 *
 * @attention   None
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_LPCTR_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 retry_time = 3;
	GU8 i = 0;
	GU8 LP_CTR_is_ok = 1;
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG CTR test start! \r\n");
	
	//GS32 PPG_RawdataArr[MAX_PPG_RAWDATA_SAMPLE_NUM] = {0};
	GU32 RawdataBuf_Len = 0;
	if((pstTestPara->PPG_CTRRawdata_Num > MAX_PPG_RAWDATA_SAMPLE_NUM) \
		|| (0 == pstTestPara->PPG_CTRRawdata_Num)) //max is MAX_PPG_RAWDATA_SAMPLE_NUM
	{
		test_code = MT_ERROR_LED_LP_RAWDATA_ABNORMAL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, CTR test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	
	GU8 Slot_Cfg_EN = pstTestPara->stLEDPDConfig.LED_Channel;
	GU8 LED_PD_NUM = 0; //Number of LED*PD to be tested
	
	if(SLOT_ONECH_MODE == pstTestPara->stCTRSlotConfig.Slot_Mode)
		LED_PD_NUM = pstTestPara->stLEDPDConfig.PD_Num[0];
	else //SLOT_MULTICH_MODE
		LED_PD_NUM = pstTestPara->stLEDPDConfig.LED_PD_Num;
	
	//tset before operation, chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	
	//write config to gh3x2x
	for(i = 0; i < retry_time; i++)
	{
		if(gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegPPGConfig, sizeof(stGh3x2xMpRegPPGConfig) / sizeof(stGh3x2xMpRegPPGConfig[0])))
		{
			break;
		}
	}
	if( i == retry_time)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, CTR test set reg config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//according to SLOT_CFG_EN cal which slots need to be enabled, and enable slot
	GU8 SlotEnableNum = 0;
	if(!gh3x2x_demo_mp_set_slot_enable(Slot_Cfg_EN, &SlotEnableNum) || SlotEnableNum < 1)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, CTR test set slot enable fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//set slot config
	for(i = 0; i < MAX_SLOT_NUM; i++)
	{
		//set the enabled slot config
		if((Slot_Cfg_EN >> i) & 0x01)
		{
			pstTestPara->stCTRSlotConfig.Slot_Cfg_N = i;
			pstTestPara->stCTRSlotConfig.PD_N = pstTestPara->stLEDPDConfig.PD_Channel[i];
			pstTestPara->stCTRSlotConfig.ADC_N = i % 4; //ADC value can only be 0~3
			pstTestPara->stCTRSlotConfig.LED_N = i;
			
			if(!gh3x2x_demo_mp_set_slot_config(pstTestPara->stCTRSlotConfig))
			{
				test_code = MT_ERROR_COMMUNICATE_FAIL;
				GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, CTR test set slot config fail, error code is: %d! \r\n", test_code);
				return test_code;
			}
		}
	}
	
	//get rawdata
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	RawdataBuf_Len = (pstTestPara->PPG_CTRRawdata_Num + pstTestPara->PPG_CTRRawdata_SkipNum)*LED_PD_NUM;
	if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
	{
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, CTR test read rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop

	//start cal noise and CTR, check CTR
	GU16 usChannel = 0; //only 0~31, LED(x) PD(y) channel is:x*MAX_ADC_NUM + y
	for(i = 0; i < MAX_SLOT_NUM; i++)
	{
		 if(!((Slot_Cfg_EN >> i) & 0x01))//Only cal the slots that need to be tested
		 {
			continue;
		 }
		 for(GU32 j = 0; j < MAX_ADC_NUM; j++)
		 {
			if (!((pstTestPara->stLEDPDConfig.PD_Channel[i] >> j) & 0x01))//Only cal the PD that needs to be tested
			{
				continue;
			}
			
			if(SLOT_MULTICH_MODE == pstTestPara->stCTRSlotConfig.Slot_Mode)
				usChannel = i*MAX_ADC_NUM + j;
			else
				usChannel = j;
			
			//handle rawdata
			memset(GH3x2xMp_RawdataHandleArr, 0, MAX_PPG_RAWDATA_SAMPLE_NUM*sizeof(GS32));
			if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_PPG, GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_CTRRawdata_SkipNum, \
														i, j, GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
			{
				test_code = MT_ERROR_LED_LP_RAWDATA_ABNORMAL;
				GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LP CTR handle rawdata error, error code is: %d! \r\n", test_code);
				return test_code;
			}
			//calc noise, averge rawdata			
			gh3x2x_demo_mp_calc_noise(GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_CTRRawdata_Num, \
										&pstTestResult->stCTRRawHandleResult.AvergeRawdata[usChannel], &pstTestResult->stCTRRawHandleResult.Noise[usChannel]);
			//calc CTR
			pstTestResult->LED_LPCTR[usChannel] = gh3x2x_demo_mp_calc_LPCTR(pstTestResult->stCTRRawHandleResult.AvergeRawdata[usChannel], \
														pstTestPara->stCTRSlotConfig.TIA_RF, pstTestPara->stCTRSlotConfig.LED_Drv);
			
			//check
			if(pstTestResult->LED_LPCTR[usChannel] < stGh3x2xMpLimit.PPG_CTR_MinLimit[usChannel])
			{
				LP_CTR_is_ok = 0;
				GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LP_CTR[%d]: %.5f less than min limit: %.5f !\r\n", usChannel, \
										pstTestResult->LED_LPCTR[usChannel], stGh3x2xMpLimit.PPG_CTR_MinLimit[usChannel]);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test result info, LP_CTR[%d] is: %.5f, min limit is: %.5f, LP_CTR test is pass! \r\n", usChannel, \
										pstTestResult->LED_LPCTR[usChannel], stGh3x2xMpLimit.PPG_CTR_MinLimit[usChannel]);
			}
		 }
	}
	if(!LP_CTR_is_ok)
	{
		test_code = MT_ERROR_LED_LP_RAWDATA_ABNORMAL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LP CTR over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--CTR test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_Noise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ---- noise test
 *
 * @attention   PPG Noise test requires automatic dimming. 
 * @attention   It is not possible to test all leds in parallel using multi-channel mode, only serial test
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
extern GU8 HPF_FirstFlag;
GU8 gh3x2x_demo_mp_PPG_Noise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 L = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--Noise test start! \r\n");
	
	//GS32 PPG_RawdataArr[MAX_PPG_RAWDATA_SAMPLE_NUM] = {0};
	GU32 RawdataBuf_Len = 0;
	if((pstTestPara->PPG_NoiseRawdata_Num > MAX_PPG_RAWDATA_SAMPLE_NUM) \
		|| (0 == pstTestPara->PPG_NoiseRawdata_Num)) //max is MAX_PPG_RAWDATA_SAMPLE_NUM
	{
		test_code = MT_ERROR_LP_NOISE_ABNORMAL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	GU8 Slot_Cfg_EN = 0;
	GU8 LED_PD_NUM = 0; //Number of LED*PD to be tested
	
	//tset before operation, chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegPPGConfig, sizeof(stGh3x2xMpRegPPGConfig) / sizeof(stGh3x2xMpRegPPGConfig[0])))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test set reg config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	for(L = 0; L < MAX_LED_NUM; L++)
	{
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		Slot_Cfg_EN = pstTestPara->stLEDPDConfig.PD_Channel[L];
		if(0 == Slot_Cfg_EN) //judge current LED whether need test
			continue;
		LED_PD_NUM = pstTestPara->stLEDPDConfig.PD_Num[L];
		
		//according to SLOT_CFG_EN cal which slots need to be enabled, and enable slot
		GU8 SlotEnableNum = 0;
		if(!gh3x2x_demo_mp_set_slot_enable(Slot_Cfg_EN, &SlotEnableNum) || SlotEnableNum < 1)
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test set slot enable fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		
		if(!gh3x2x_demo_mp_set_LED_ACG_cfg(L, stGh3x2xPPG_AutoDimming.LED_ACG_FS[L], pstTestPara))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test set LED ACG config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		for (int j = 0; j < MAX_ADC_NUM; j++)//Configure each slot reg, because there are 4 PDs at most, so 4 slots are used at most
        {
            if ((Slot_Cfg_EN >> j) & 0x01)
            {
				pstTestPara->stNoiseSlotConfig.Slot_Cfg_N = j;
				pstTestPara->stNoiseSlotConfig.PD_N = 1 << j;
				pstTestPara->stNoiseSlotConfig.ADC_N = j % 4; //ADC only value is 0~3
				pstTestPara->stNoiseSlotConfig.LED_N = L; //Use the same LED for different slots in the same test

                if(!gh3x2x_demo_mp_set_slot_config(pstTestPara->stNoiseSlotConfig))
				{
					test_code = MT_ERROR_COMMUNICATE_FAIL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test set slot config fail, error code is: %d! \r\n", test_code);
					return test_code;
				} 
            }
        }
		
		//get rawdata
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		RawdataBuf_Len = (pstTestPara->PPG_NoiseRawdata_Num + pstTestPara->PPG_NoiseRawdata_SkipNum)*LED_PD_NUM;
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		
		GU16 usChannel = 0; //only 0~31, LED(x) PD(y) channel is:x*MAX_ADC_NUM + y
		GF32 Noise_temp = 0;
		GF32 AvergeRawdata_temp = 0;
		for (i = 0; i < MAX_SLOT_NUM; i++)
		{
			if((Slot_Cfg_EN >> i) & 0x01)
			{
				usChannel = L*MAX_ADC_NUM + i;
				//handle rawdata
				memset(GH3x2xMp_RawdataHandleArr, 0, MAX_PPG_RAWDATA_SAMPLE_NUM*sizeof(GS32));
				if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_PPG, GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_NoiseRawdata_SkipNum, \
														i, i, GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
				{
					test_code = MT_ERROR_LP_NOISE_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise test handle rawdata error, error code is: %d! \r\n", test_code);
					return test_code;
				}
				//calc noise, averge rawdata
				gh3x2x_demo_mp_calc_noise(GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_NoiseRawdata_Num, &pstTestResult->stNoiseRawHandleResult.AvergeRawdata[usChannel], \
														&pstTestResult->stNoiseRawHandleResult.Noise[usChannel]);
				HPF_FirstFlag = 1;
				for(int k = 0; k < pstTestPara->PPG_NoiseRawdata_Num; k++)
				{
					GH3x2xMp_RawdataHandleArr[k] = gh3x2x_mp_high_pass_filter((GD64)GH3x2xMp_RawdataHandleArr[k]); //filter
				}
				//calc noise, averge rawdata after filter
				gh3x2x_demo_mp_calc_noise(GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_NoiseRawdata_Num, &AvergeRawdata_temp, &Noise_temp);
				
				//only need noise in filter  
				pstTestResult->stNoiseRawHandleResult.Noise[usChannel] = RAWDATA_TO_VOL((GD64)Noise_temp);
				pstTestResult->LED_Noise[usChannel] = pstTestResult->stNoiseRawHandleResult.Noise[usChannel];
				
				//check noise whether over limit
				if((pstTestResult->LED_Noise[usChannel] > stGh3x2xMpLimit.PPG_Noise_MaxLimit) \
					|| ((GD64)pstTestResult->LED_Noise[usChannel] <= 0.00001))
				{
					test_code = MT_ERROR_LP_NOISE_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, Noise[%d]: %.5f more than max limit: %.5f or less than 0.00001, error code is: %d! \r\n", usChannel, \
											pstTestResult->LED_Noise[usChannel], stGh3x2xMpLimit.PPG_Noise_MaxLimit, test_code);
				}
				else
				{
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test result info, Noise[%d] is: %.5f, max limit is: %.5f, Noise test is pass! \r\n", usChannel, \
											pstTestResult->LED_Noise[usChannel], stGh3x2xMpLimit.PPG_Noise_MaxLimit);
				}
			}
		}
	}

	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--Noise test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_BaseNoise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ----base noise test
 *
 * @attention   base noise test has nothing to do with LEDs, so only one slot is needed to test the data of 4 PDs.
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_BaseNoise_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 j = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--BaseNoise test start! \r\n");

	//GS32 PPG_RawdataArr[MAX_PPG_RAWDATA_SAMPLE_NUM] = {0};
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	if((pstTestPara->PPG_BaseNoiseRawdata_Num > MAX_PPG_RAWDATA_SAMPLE_NUM) \
		|| (0 == pstTestPara->PPG_BaseNoiseRawdata_Num)) //max is MAX_PPG_RAWDATA_SAMPLE_NUM
	{
		test_code = MT_ERROR_LPBASE_NOISE_ABNORMAL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	GU8 Slot_Cfg_EN = 0x01; //only open one slot
	GU8 PD_Channel_EN = pstTestPara->stLEDPDConfig.PD_Channel_AllSlot; //PD channel to be tested
	GU8 LED_PD_NUM = pstTestPara->stLEDPDConfig.PD_Num_AllSlot; //Number of PD to be tested
	
	//tset before operation, chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegPPGConfig, sizeof(stGh3x2xMpRegPPGConfig) / sizeof(stGh3x2xMpRegPPGConfig[0])))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test set reg config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	//according to SLOT_CFG_EN cal which slots need to be enabled, and enable slot
	GU8 SlotEnableNum = 0;
	if(!gh3x2x_demo_mp_set_slot_enable(Slot_Cfg_EN, &SlotEnableNum) || SlotEnableNum < 1)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test set slot enable fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	for(i = 0; i < MAX_SLOT_NUM; i++) //Timeslot register configuration, slot_cfgx corresponds to LEDx
	{
		if ((Slot_Cfg_EN >> i) & 0x01)
        {
			pstTestPara->stBaseNoiseSlotConfig.Slot_Cfg_N = i;
			pstTestPara->stBaseNoiseSlotConfig.PD_N = PD_Channel_EN;
			pstTestPara->stBaseNoiseSlotConfig.ADC_N = i % 4; //ADC only value is 0~3
			pstTestPara->stBaseNoiseSlotConfig.LED_N = i; //Use the same LED for different slots in the same test

            if(!gh3x2x_demo_mp_set_slot_config(pstTestPara->stBaseNoiseSlotConfig))
			{
				test_code = MT_ERROR_COMMUNICATE_FAIL;
				GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test set slot config fail, error code is: %d! \r\n", test_code);
				return test_code;
			} 
		}
	}
	
	//get rawdata
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	RawdataBuf_Len = (pstTestPara->PPG_BaseNoiseRawdata_Num + pstTestPara->PPG_BaseNoiseRawdata_SkipNum)*LED_PD_NUM;
	if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
	{
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test read rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop

	GU16 usChannel = 0; //only 0~31, LED(x) PD(y) channel is:x*MAX_ADC_NUM + y
	for (i = 0; i < MAX_SLOT_NUM; i++)
	{
		if (!((Slot_Cfg_EN >> i) & 0x01))//Only calculate the slots that need to be tested
		{
			continue;
		}
		for(j = 0; j < MAX_ADC_NUM; j++)
		{
			if((PD_Channel_EN >> j) & 0x01) //Only calculate the PD that needs to be tested
			{
				if(SLOT_MULTICH_MODE == pstTestPara->stBaseNoiseSlotConfig.Slot_Mode)
					usChannel = i*MAX_ADC_NUM + j;
				else
					usChannel = j;
				
				//handle rawdata
				memset(GH3x2xMp_RawdataHandleArr, 0, MAX_PPG_RAWDATA_SAMPLE_NUM*sizeof(GS32));
				if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_PPG, GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_BaseNoiseRawdata_SkipNum, \
															i, j, GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
				{
					test_code = MT_ERROR_LPBASE_NOISE_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise test handle rawdata error, error code is: %d! \r\n", test_code);
					return test_code;
				}
				//calc noise, averge rawdata
				gh3x2x_demo_mp_calc_noise(GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_BaseNoiseRawdata_Num, \
										  &pstTestResult->stBaseNoiseRawHandleResult.AvergeRawdata[usChannel], &pstTestResult->stBaseNoiseRawHandleResult.Noise[usChannel]);
				pstTestResult->stBaseNoiseRawHandleResult.Noise[usChannel] = RAWDATA_TO_VOL((GD64)pstTestResult->stBaseNoiseRawHandleResult.Noise[usChannel]);
				
				pstTestResult->LED_BaseNoise[usChannel] = pstTestResult->stBaseNoiseRawHandleResult.Noise[usChannel];
				pstTestResult->LED_DCOffset[usChannel] = RAWDATA_TO_VOL((GD64)pstTestResult->stBaseNoiseRawHandleResult.AvergeRawdata[usChannel] - ADC_OFFSET) / 1000; //Unit is mV
				
				//check base noise
				if((pstTestResult->LED_BaseNoise[usChannel] > stGh3x2xMpLimit.PPG_BaseNoise_MaxLimit) \
					|| ((GD64)pstTestResult->LED_BaseNoise[usChannel] <= 0.00001))
				{
					test_code = MT_ERROR_LPBASE_NOISE_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseNoise[%d]: %.5f more than max limit: %.5f or less than 0.00001, error code is: %d! \r\n", usChannel, \
											pstTestResult->LED_BaseNoise[usChannel], stGh3x2xMpLimit.PPG_BaseNoise_MaxLimit, test_code);
				}
				else
				{
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test result info, BaseNoise[%d] is: %.5f, max limit is: %.5f, BaseNoise test is pass! \r\n", usChannel, \
											pstTestResult->LED_BaseNoise[usChannel], stGh3x2xMpLimit.PPG_BaseNoise_MaxLimit);
				}
				
				//check dc_offset
				if((pstTestResult->LED_DCOffset[usChannel] < stGh3x2xMpLimit.PPG_DCOffset_MinLimit) \
					|| (pstTestResult->LED_DCOffset[usChannel] > stGh3x2xMpLimit.PPG_DCOffset_MaxLimit))
				{
					test_code = MT_ERROR_LPBASE_DC_OFFSET_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, BaseDCOffset[%d]: %.5f more than max limit: %.5f or less than min limit: %.5f, error code is: %d! \r\n", \
											usChannel, pstTestResult->LED_DCOffset[usChannel], stGh3x2xMpLimit.PPG_DCOffset_MaxLimit, \
											stGh3x2xMpLimit.PPG_DCOffset_MinLimit, test_code);
				}
				else
				{
					GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test result info, BaseDCOffset[%d] is: %.5f, max limit is: %.5f and min limit is: %.5f, BaseDCOffset test is pass! \r\n", \
										usChannel, pstTestResult->LED_DCOffset[usChannel], stGh3x2xMpLimit.PPG_DCOffset_MaxLimit, stGh3x2xMpLimit.PPG_DCOffset_MinLimit);
				}
			}
		}
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--BaseNoise test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_PPG_LeakLight_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
 *
 * @brief  PPG test items ----leak light test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_PPG_LeakLight_Test(stGh3x2xMp_PPGTestParam *pstTestPara, stGh3x2xMp_PPGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 j = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--LeakLight test start! \r\n");
	
	//GS32 PPG_RawdataArr[MAX_PPG_RAWDATA_SAMPLE_NUM] = {0};
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	if((pstTestPara->PPG_LeakLightRawdata_Num > MAX_PPG_RAWDATA_SAMPLE_NUM) \
		|| (0 == pstTestPara->PPG_LeakLightRawdata_Num)) //max is MAX_PPG_RAWDATA_SAMPLE_NUM
	{
		test_code = MT_ERROR_LEAK_LEDLP_RAWDATA_ABNORMAL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	GU8 Slot_Cfg_EN = 0;
	GU8 LED_PD_NUM = 0; //Number of LED*PD to be tested
	if(SLOT_ONECH_MODE == pstTestPara->stLeakLightSlotConfig.Slot_Mode)
		LED_PD_NUM = pstTestPara->stLEDPDConfig.PD_Num[0];
	else //SLOT_MULTICH_MODE
		LED_PD_NUM = pstTestPara->stLEDPDConfig.LED_PD_Num;
	
	//must up the rubber before test
	GU8 prompt_msg[] = PPG_PROMPT_UP_RUBBER_MSG;
	gh3x2x_mp_prompt_message(prompt_msg, sizeof(prompt_msg) / sizeof(GU8));
	
	//tset before operation, chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegPPGConfig, sizeof(stGh3x2xMpRegPPGConfig) / sizeof(stGh3x2xMpRegPPGConfig[0])))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test set reg config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//according to SLOT_CFG_EN cal which slots need to be enabled, and enable slot
	GU8 SlotEnableNum = 0;
	Slot_Cfg_EN = pstTestPara->stLEDPDConfig.LED_Channel;
	if(!gh3x2x_demo_mp_set_slot_enable(Slot_Cfg_EN, &SlotEnableNum) || SlotEnableNum < 1)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test set slot enable fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	for (i = 0; i < MAX_SLOT_NUM; i++) ////Timeslot register configuration, slot_cfgx corresponds to LEDx
	{
		if ((Slot_Cfg_EN >> i) & 0x01)
		{
			pstTestPara->stLeakLightSlotConfig.Slot_Cfg_N = i;
			pstTestPara->stLeakLightSlotConfig.PD_N = pstTestPara->stLEDPDConfig.PD_Channel[i];
			pstTestPara->stLeakLightSlotConfig.ADC_N = i % 4; //ADC only value is 0~3
			pstTestPara->stLeakLightSlotConfig.LED_N = i; //Use the same LED for different slots in the same test

            if(!gh3x2x_demo_mp_set_slot_config(pstTestPara->stLeakLightSlotConfig))
			{
				test_code = MT_ERROR_COMMUNICATE_FAIL;
				GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test set slot config fail, error code is: %d! \r\n", test_code);
				return test_code;
			} 
		}
	}
	
	//get rawdata
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	RawdataBuf_Len = (pstTestPara->PPG_LeakLightRawdata_Num + pstTestPara->PPG_LeakLightRawdata_SkipNum)*LED_PD_NUM;
	if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
	{
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test read rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
	
	GU16 usChannel = 0; //only 0~31, LED(x) PD(y) channel is:x*MAX_ADC_NUM + y
	for (i = 0; i < MAX_SLOT_NUM; i++)
	{
		if (!((Slot_Cfg_EN >> i) & 0x01))//Only calculate the slots that need to be tested
		{
			continue;
		}
		for(j = 0; j < MAX_ADC_NUM; j++)
		{
			if((pstTestPara->stLEDPDConfig.PD_Channel[i] >> j) & 0x01) //Only calculate the PD that needs to be tested
			{
				if(SLOT_MULTICH_MODE == pstTestPara->stLeakLightSlotConfig.Slot_Mode)
					usChannel = i*MAX_ADC_NUM + j;
				else
					usChannel = j;
				
				//handle rawdata
				memset(GH3x2xMp_RawdataHandleArr, 0, MAX_PPG_RAWDATA_SAMPLE_NUM*sizeof(GS32));
				if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_PPG, GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_LeakLightRawdata_SkipNum, \
														i, j, GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
				{
					test_code = MT_ERROR_LEAK_LEDLP_RAWDATA_ABNORMAL;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakLight test handle rawdata error, error code is: %d! \r\n", test_code);
					return test_code;
				}
				
				//calc noise, averge rawdata
				gh3x2x_demo_mp_calc_noise(GH3x2xMp_RawdataHandleArr, pstTestPara->PPG_LeakLightRawdata_Num, \
										  &pstTestResult->stLeakLightRawHandleResult.AvergeRawdata[usChannel + LED0_LLPRAWDATA_INDEX], &pstTestResult->stLeakLightRawHandleResult.Noise[usChannel + LED0_LLPRAWDATA_INDEX]);
				//calc leak light CTR
				pstTestResult->LED_LLPCTR[usChannel] = gh3x2x_demo_mp_calc_LPCTR(pstTestResult->stLeakLightRawHandleResult.AvergeRawdata[usChannel + LED0_LLPRAWDATA_INDEX], \
																				pstTestPara->stLeakLightSlotConfig.TIA_RF, pstTestPara->stLeakLightSlotConfig.LED_Drv);
				
				if(((GD64)pstTestResult->LED_LPCTR[usChannel] > 0.00001) \
					&& ((GD64)stGh3x2xMpLimit.PPG_LeakRatio_Throld[usChannel] > 0.00001)) //check LPCTR and LeakRatio limit whether > 0
				{
					pstTestResult->LED_LeakRatio[usChannel] = pstTestResult->LED_LLPCTR[usChannel] / pstTestResult->LED_LPCTR[usChannel];
					if(pstTestResult->LED_LeakRatio[usChannel] > stGh3x2xMpLimit.PPG_LeakRatio_Throld[usChannel] \
						&& pstTestResult->LED_LLPCTR[usChannel] > stGh3x2xMpLimit.PPG_LeakCTR_MaxLimit[usChannel])
					{
						//Requires two conditions to over limit, the test is fail
						test_code = MT_ERROR_LEAK_AVGER_ERR;
						GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LeakCTR[%d]: %.5f more than max limit: %.5f and LeakRadio[%d]: %.5f more than max limit: %.5f, error code is: %d! \r\n", \
											usChannel, pstTestResult->LED_LLPCTR[usChannel], stGh3x2xMpLimit.PPG_LeakCTR_MaxLimit[usChannel], \
											usChannel, pstTestResult->LED_LeakRatio[usChannel], stGh3x2xMpLimit.PPG_LeakRatio_Throld[usChannel], test_code);
					}
					else
					{
						GH3X2X_MP_LOG_L2("Gh3x2x mp PPG test result info, LeakCTR[%d] is: %.5f, max limit is: %.5f, and LeakRadio[%d] is: %.5f, max limit: %.5f, LeakLight test is pass! \r\n", \
											usChannel, pstTestResult->LED_LLPCTR[usChannel], stGh3x2xMpLimit.PPG_LeakCTR_MaxLimit[usChannel], \
											usChannel, pstTestResult->LED_LeakRatio[usChannel], stGh3x2xMpLimit.PPG_LeakRatio_Throld[usChannel]);
					}
				}
				else
				{
					test_code = MT_ERROR_LEAK_AVGER_ERR;
					GH3X2X_MP_LOG_L1("Gh3x2x mp PPG test fail, LPCTR: %.5f or LeakRatio_Throld: %.3f less than 0.00001, error code is: %d! \r\n", \
										pstTestResult->LED_LPCTR[usChannel], stGh3x2xMpLimit.PPG_LeakRatio_Throld[usChannel], test_code);
					pstTestResult->LED_LeakRatio[usChannel] = 0;
				}
				
			}
		}
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp PPG--LeakLight test end! \r\n");
	return test_code;

}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_LeadOnOff_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----Lead on/off test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_LeadOnOff_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GU8 retry_times = 3;
	GU8 ret_lead_on = 1;
	GU8 ret_lead_off = 1;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--LeadOnOff test start! \r\n");
	
	GU32 sample_num = pstTestPara->stECGSignal_LeadOnOff.Rawdata_Num;
	GU32 skip_num = pstTestPara->stECGSignal_LeadOnOff.Rawdata_SkipNum;
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	if((sample_num > MAX_RAWDATA_SAMPLE_NUM) || (sample_num <= 0))
	{
		test_code = MT_ERROR_ECG_LEADONOFF_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on/off test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//lead on test
	//set ecg relay io status---lead on
	if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_LEAD_ON, pstTestPara->stECGSignal_LeadOnOff, NONE_SPEC))
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on test set ecg io status fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//rst gh3x2x,set low and set high later
	gh3x2x_demo_mp_reset_operation();
	
	//wait int
	//ret_lead_on = gh3x2x_demo_mp_wait_for_int(0x00, 100);
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on/off test set reg config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	
	if(!gh3x2x_demo_mp_wait_for_int(0x02, 500))
	{
		ret_lead_on = 0;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on fail! \r\n");
	}
	
	//lead off test
	//write reg
	for(i = 0; i < retry_times; i++)
	{
		gh3x2x_demo_mp_write_reg(0x000A, 0x0020); //Test FIFO threshold can be adjusted down
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
		if((0x0020 == gh3x2x_demo_mp_read_reg(0x000A)) && (0x0001 == gh3x2x_demo_mp_read_reg(0x0108)))
		{
			break;
		}
	}
	if( i == retry_times)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on/off test write reg fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//set ecg relay io status---lead off
	if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_LEAD_OFF, pstTestPara->stECGSignal_LeadOnOff, NONE_SPEC))
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead off test set ecg io status fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//get rawdata
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	RawdataBuf_Len = sample_num + skip_num;
	if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
	{
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on/off test read rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
	
	//lead off test according to GH3x2xMp_RawdataBuf
	GH3X2X_Memcpy(GH3x2xMp_RawdataHandleArr, GH3x2xMp_RawdataBuf + skip_num*4, sample_num*sizeof(GS32));
	GU16 temp = 0;
	if(0 == gh3x2x_mp_ecg_lead_off_dectection(GH3x2xMp_RawdataHandleArr, sample_num, 3, &temp))
	{
		ret_lead_off = 0;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead off detect fail! \r\n");
	}
	
	//print rawdata
	gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1);
	
	if(!ret_lead_on || !ret_lead_off)
	{
		test_code = MT_ERROR_ECG_LEADONOFF_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, lead on/off test fail, error code is: %d! \r\n", test_code);
	}
	else
	{
		GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, lead on/off test pass! \r\n");
	}
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--LeadOnOff test end! \r\n");
	return test_code;

}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_Noise_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----Noise test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_Noise_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	//GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--Noise test start! \r\n");
	
	GU32 sample_num = pstTestPara->stECGSignal_Noise.Rawdata_Num;
	GU32 skip_num = pstTestPara->stECGSignal_Noise.Rawdata_SkipNum;
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	//check noise test sample num must > 500 or < MAX_RAWDATA_SAMPLE_NUM
	if((sample_num <= 500) || (sample_num > MAX_RAWDATA_SAMPLE_NUM))
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, Noise test set the number of samples over limit(must greater than 500, \
							less than MAX_RAWDATA_SAMPLE_NUM), error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//set ecg io status---noise test
	if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_NOISE, pstTestPara->stECGSignal_Noise, NONE_SPEC))
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, Noise test set ecg io status fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	gh3x2x_mp_delay_ms(100);
	
	//tset before operation, chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, Noise test set reg lead config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, Noise test set reg default config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
	//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	gh3x2x_demo_mp_write_reg(0x1EA, 0x433); //Fast recovery, after sampling starts
	RawdataBuf_Len = sample_num + skip_num;
	if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
	{
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, noise test read rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
	
	//ECG collection is fixed to slot0 ADC3, 
	if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, \
											GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, noise test handle rawdata error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//filter
	gh3x2x_demo_mp_ecg_signal_filter(GH3x2xMp_RawdataHandleArr, sample_num, 1, 4, 1);
	
	//cal noise
	GF32 avg_temp = 0;
	gh3x2x_demo_mp_calc_noise(&GH3x2xMp_RawdataHandleArr[500], sample_num - 500, &avg_temp, &pstTestResult->ECG_Noise);
	pstTestResult->ECG_Noise = pstTestResult->ECG_Noise * ADC_REFER / ADC_MAX * 1000 / GAIN;// /20; //? /GAIN
	
	//judge ECG_Noise whether over limit
	if(pstTestResult->ECG_Noise > stGh3x2xMpLimit.ECG_Noise_MaxLimit)
	{
		test_code = MT_ERROR_ECG_NOISE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, Noise: %.5f more than max limit: %.5f, error code is: %d! \r\n", \
							pstTestResult->ECG_Noise, stGh3x2xMpLimit.ECG_Noise_MaxLimit, test_code);
		return test_code;
	}
	else
	{
		GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, Noise is: %.5f, max limit is: %.5f, Noise test is pass! \r\n", \
							pstTestResult->ECG_Noise, stGh3x2xMpLimit.ECG_Noise_MaxLimit);
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--Noise test end! \r\n");
	return test_code;
	
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_HPF_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----HPF test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_HPF_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--HPF test start! \r\n");
	
	GU16 sample_num = 0;
	GU16 skip_num = 0;

	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	GU8 HPFRatio_cnt = 0;
	
	//HPF test must 2 signals in a group
	if((pstTestPara->HPF_ECGSignalNum > ECG_MAX_SIGNAL_NUM) || (0 == pstTestPara->HPF_ECGSignalNum) \
		|| (0 != (pstTestPara->HPF_ECGSignalNum % 2))) //must greater than 0 and a multiple of 2, not greater than ECG_MAX_SIGNAL_NUM
	{
		test_code = MT_ERROR_ECG_HPF_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set signal num error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	
	for(i = 0; i < pstTestPara->HPF_ECGSignalNum; i++)
	{
		sample_num = pstTestPara->stECGSignal_HPF[i].Rawdata_Num;
		skip_num = pstTestPara->stECGSignal_HPF[i].Rawdata_SkipNum;
		//check HPF test sample num must > 200
		if((sample_num < 200) || (sample_num > MAX_RAWDATA_SAMPLE_NUM))
		{
			test_code = MT_ERROR_ECG_HPF_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set the number of samples over limit(must greater than 200, less than MAX_RAWDATA_SAMPLE_NUM), error code is: %d! \r\n", test_code);
			return test_code;
		}
	
		
		//set ecg io status---HPF test
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_HPF, pstTestPara->stECGSignal_HPF[i], NONE_SPEC))
		{
			test_code = MT_ERROR_ECG_HPF_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set ecg io status or signal voltage error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//tset before operation, chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set reg lead config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set reg default config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
		
		//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		//generation the other one signal channel 
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_HPF, pstTestPara->stECGSignal_HPF[i], ECG_HPF_SPEC))
		{
			test_code = MT_ERROR_ECG_HPF_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test set signal voltage fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		RawdataBuf_Len = sample_num + skip_num;
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
	
		//ECG collection is fixed to slot0 ADC3, 
		if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, \
												GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
		{
			test_code = MT_ERROR_ECG_HPF_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF test handle rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//2 signals in a group, Calculate the latter one
		if (0 == ((i + 1) % 2))
		{
			GF32 ratio = gh3x2x_demo_mp_ecg_cal_HPF_Ratio(GH3x2xMp_RawdataHandleArr, sample_num);
			pstTestResult->ECG_HPFRatio[HPFRatio_cnt++] = ratio;
			if((ratio < stGh3x2xMpLimit.ECG_HPF_MinLimit) \
				|| (ratio > stGh3x2xMpLimit.ECG_HPF_MaxLimit))
			{
				test_code = MT_ERROR_ECG_HPF_ERR;
				GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, HPF ratio%d: %.5f less than min limit: %.5f or more than max limit: %.5f, error code is: %d! \r\n", \
									HPFRatio_cnt-1, ratio, stGh3x2xMpLimit.ECG_HPF_MinLimit, stGh3x2xMpLimit.ECG_HPF_MaxLimit, test_code);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, HPF ratio%d is: %.5f, min limit is: %.5f and max limit is: %.5f, HPF test is pass! \r\n", \
									HPFRatio_cnt-1, ratio, stGh3x2xMpLimit.ECG_HPF_MinLimit, stGh3x2xMpLimit.ECG_HPF_MaxLimit);
			}
		}
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--HPF test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_SIGNAL_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----SIGNAL test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_SIGNAL_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU32 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--SIGNAL test start! \r\n");
	
	GU16 sample_num = 0;
	GU16 skip_num = 0;
	GU16 sampling_step = 1;
	
	GU32 RawdataBuf_Len[ECG_MAX_SIGNAL_NUM] = {0};
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	GF32 nStandardAmp = 0.0;
	
	//not greater than ECG_MAX_SIGNAL_NUM
	if((pstTestPara->SIGNAL_ECGSignalNum > ECG_MAX_SIGNAL_NUM) || (0 == pstTestPara->SIGNAL_ECGSignalNum))
	{
		test_code = MT_ERROR_ECG_SIGNAL_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test set signal num error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	
	for(i = 0; i < pstTestPara->SIGNAL_ECGSignalNum; i++)
	{
		sample_num = pstTestPara->stECGSignal_SIGNAL[i].Rawdata_Num;
		skip_num = pstTestPara->stECGSignal_SIGNAL[i].Rawdata_SkipNum;
		sampling_step = pstTestPara->stECGSignal_SIGNAL[i].Sampling_Step;
		
		//check noise test sample num
		if((sample_num <= 0) || (sampling_step <= 0))
		{
			test_code = MT_ERROR_ECG_SIGNAL_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test set the number of samples or sampling step <= 0, error code is: %d! \r\n", test_code);
		    return test_code;
		}
		
		//Check whether fft array need used space is in the range of GH3x2xMp_RawdataBuf
		//fft array space reuse GH3x2xMp_RawdataBuf memory
		if((MAX_FFT_LENGTH * 2 * 4) > (RAWDATA_BUF_SIZE / 2))
		{
			test_code = MT_ERROR_ECG_SIGNAL_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test fft array need used space more than GH3x2xMp_RawdataBuf memory, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//set ecg io status---SIGNAL test
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_SIGNAL, pstTestPara->stECGSignal_SIGNAL[i], NONE_SPEC))
		{
			test_code = MT_ERROR_ECG_SIGNAL_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test set ecg io status or signal fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_mp_delay_ms(20); //wait
		
		//chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test set reg lead config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test set reg default config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
		//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		gh3x2x_demo_mp_write_reg(0x1EA, 0x433); //Fast recovery, after sampling starts
		RawdataBuf_Len[i] = sample_num + skip_num;
		
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len[i]*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		
		//ECG collection is fixed to slot0 ADC3, 
		if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, GH3x2xMp_RawdataBuf, \
													RawdataBuf_Len[i], sampling_step))
		{
			test_code = MT_ERROR_ECG_SIGNAL_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL test handle rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
			
		//cal
		gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GH3x2xMp_RawdataHandleArr, sample_num, &pstTestResult->ECG_Amp[i], &pstTestResult->ECG_Freq[i], \
													0, 0, 1, 0, sample_num, pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, sampling_step);
		
		//check limit
		if(i==0) //The first frequency data is processed separately
        {
            GF32 fTargetVpp = pstTestPara->stECGSignal_SIGNAL[i].ECG_Vol * 1000 / (GF32)5.7;
			GF32 amp_difference = fabs(1 - pstTestResult->ECG_Amp[i] / fTargetVpp);
			GF32 freq_difference = fabs(1 - pstTestResult->ECG_Freq[i] / pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq);
			nStandardAmp = pstTestResult->ECG_Amp[i];
            if((amp_difference > stGh3x2xMpLimit.ECG_MaxAMP) \
				|| (freq_difference > stGh3x2xMpLimit.ECG_MaxFreq))
			{
				test_code = MT_ERROR_ECG_SIGNAL_ERR;
				GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL %.2fHz test difference between amp: %.5fmV or freq: %.5fHz with target amp: %.5fmV or freq: %.2f more than MaxAmpRange: %.5f or MaxFreqRange: %.5f, error code is: %d! \r\n", \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, pstTestResult->ECG_Amp[i], pstTestResult->ECG_Freq[i], fTargetVpp, \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, stGh3x2xMpLimit.ECG_MaxAMP, stGh3x2xMpLimit.ECG_MaxFreq, test_code);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, SIGNAL %.2fHz test difference between amp: %.5fmV freq: %.5fHz with target amp: %.5fmV freq: %.2f is pass, MaxAmpRange is: %.5f or MaxFreqRange is: %.5f !\r\n", \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, pstTestResult->ECG_Amp[i], pstTestResult->ECG_Freq[i], fTargetVpp, \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, stGh3x2xMpLimit.ECG_MaxAMP, stGh3x2xMpLimit.ECG_MaxFreq);
			}
            
        }
        else
        {
			GF32 amp_difference = fabs(1 - pstTestResult->ECG_Amp[i] / nStandardAmp);
            if(amp_difference > stGh3x2xMpLimit.ECG_SIGNAL_MaxLimit)
            {
				test_code = MT_ERROR_ECG_SIGNAL_ERR;
				GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, SIGNAL %.2fHz test difference between amp: %.5fmV with Standard amp: %.5fmV over more than MaxRange: %.5f, freq cal is: %.5fHz, error code is: %d! \r\n", \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, pstTestResult->ECG_Amp[i], nStandardAmp, \
									stGh3x2xMpLimit.ECG_SIGNAL_MaxLimit, pstTestResult->ECG_Freq[i], test_code);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, SIGNAL %.2fHz test difference between amp: %.5fmV with Standard amp: %.5fmV is pass, MaxRange is: %.5f, freq cal is: %.5fHz! \r\n", \
									pstTestPara->stECGSignal_SIGNAL[i].ECG_Freq, pstTestResult->ECG_Amp[i], nStandardAmp, \
									stGh3x2xMpLimit.ECG_SIGNAL_MaxLimit, pstTestResult->ECG_Freq[i]);
			}
        }
	}
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--SIGNAL test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_RLD_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----RLD test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_RLD_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--RLD test start! \r\n");
	
	GU32 sample_num = pstTestPara->stECGSignal_RLD.Rawdata_Num;
	//GU32 skip_num = pstTestPara->stECGSignal_RLD.Rawdata_SkipNum;
	//GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	//check RLD test sample num 
	if((sample_num > MAX_RAWDATA_SAMPLE_NUM) || (sample_num <= 0))
	{
		test_code = MT_ERROR_ECG_RLD_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, RLD test set the number of samples over limit, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//set ecg io status---RLD test
	if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_RLD, pstTestPara->stECGSignal_RLD, NONE_SPEC))
	{
		test_code = MT_ERROR_ECG_RLD_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, RLD test set ecg io status fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//chip resume
	gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
	gh3x2x_mp_delay_ms(5);
	if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, RLD test set reg lead config fail, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
	gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
	gh3x2x_mp_delay_ms(100); //Discard a frame of data
	
	//send cmd to ecg_board for get adc, reuse GH3x2xMp_RawdataBuf array
	GH3x2xMp_RawdataBuf[0] = 0x55;
	GH3x2xMp_RawdataBuf[1] = UART_CMD_GETADC;
	GH3x2xMp_RawdataBuf[2] = 0;
	GH3x2xMp_RawdataBuf[3] = 0;
	gh3x2x_mp_ecg_uart_send(GH3x2xMp_RawdataBuf, 4);
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	//time out 10ms for rev data
	GU16 rev_len = gh3x2x_mp_ecg_uart_receive(GH3x2xMp_RawdataBuf, 10);
	gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
	if(rev_len < 4)
	{
		test_code = MT_ERROR_COMMUNICATE_FAIL;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, RLD test rev data error through uart, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//check limit
	pstTestResult->ECG_RLD = (GF32)(GH3x2xMp_RawdataBuf[5] << 8) + GH3x2xMp_RawdataBuf[4];
	GF32 MinRLDThrshold = stGh3x2xMpLimit.ECG_RLD_MaxAmp * (1 - 0.2);
	GF32 MaxRLDThrshold = stGh3x2xMpLimit.ECG_RLD_MaxAmp * (1 + 0.2);
	if((pstTestResult->ECG_RLD > MaxRLDThrshold) || (pstTestResult->ECG_RLD < MinRLDThrshold))
	{
		test_code = MT_ERROR_ECG_RLD_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, RLD test amp: %.2fmV less than min limit: %.2fmV or more than max limit: %.2fmV, error code is: %d! \r\n", \
							pstTestResult->ECG_RLD, MinRLDThrshold, MaxRLDThrshold, test_code);
	}
	else
	{
		GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, RLD amp is: %.2fmV, min limit is: %.2fmV and max limit is: %.2fmV, RLD test is pass! \r\n", \
							pstTestResult->ECG_RLD, MinRLDThrshold, MaxRLDThrshold);
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--RLD test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_CMRR_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----CMRR test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_CMRR_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--CMRR test start! \r\n");
	
	GU32 sample_num = 0;
	GU32 skip_num =  0;
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	//not greater than ECG_MAX_SIGNAL_NUM
	if((pstTestPara->CMRR_ECGSignalNum > ECG_MAX_SIGNAL_NUM) || (0 == pstTestPara->CMRR_ECGSignalNum))
	{
		test_code = MT_ERROR_ECG_CMMR_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test set signal num error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//Check whether fft array need used space is in the range of GH3x2xMp_RawdataBuf
	//fft array space reuse GH3x2xMp_RawdataBuf memory
	if((MAX_FFT_LENGTH * 2 * 4) > (RAWDATA_BUF_SIZE / 2))
	{
		test_code = MT_ERROR_ECG_CMMR_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test fft array need used space more than GH3x2xMp_RawdataBuf memory, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	
	for(i = 0; i < pstTestPara->CMRR_ECGSignalNum; i++)
	{
		sample_num = pstTestPara->stECGSignal_CMRR[i].Rawdata_Num;
		skip_num = pstTestPara->stECGSignal_CMRR[i].Rawdata_SkipNum;
		//check RLD test sample num must > 200, < MAX_RAWDATA_SAMPLE_NUM
		if((sample_num > MAX_RAWDATA_SAMPLE_NUM) || (sample_num <= 200))
		{
			test_code = MT_ERROR_ECG_CMMR_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test set the number of samples over limit(must greater than 200, less than MAX_RAWDATA_SAMPLE_NUM), error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//set ecg io status---CMRR test
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_CMMR, pstTestPara->stECGSignal_CMRR[i], NONE_SPEC))
		{
			test_code = MT_ERROR_ECG_CMMR_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test set io status or signal fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_mp_delay_ms(20); //wait
		
		//chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test set reg lead config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test set reg default config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
		//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		gh3x2x_demo_mp_write_reg(0x1EA, 0x433); //Fast recovery, after sampling starts
		RawdataBuf_Len = sample_num + skip_num;
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		
		//ECG collection is fixed to slot0 ADC3, 
		if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, \
												GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
		{
			test_code = MT_ERROR_ECG_CMMR_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR test handle rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_ecg_cmmr_calculate(GH3x2xMp_RawdataHandleArr, sample_num, &pstTestResult->ECG_CMRR[i], \
												1, pstTestPara->stECGSignal_CMRR[i].ECG_Freq);
		
		//check limit
		if ((pstTestResult->ECG_CMRR[i] < stGh3x2xMpLimit.ECG_CMRR_MinLimit) \
			|| (pstTestResult->ECG_CMRR[i] >= 1000))
		{
			test_code = MT_ERROR_ECG_CMMR_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, CMRR%d is: %.5f less than min limit:%.5f or more than 1000, error code is: %d! \r\n", \
								i, pstTestResult->ECG_CMRR[i], stGh3x2xMpLimit.ECG_CMRR_MinLimit, test_code);
		}
		else
		{
			GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, CMRR%d is: %.5f, min limit is:%.5f and max limit is 1000, CMRR test is pass! \r\n", \
								i, pstTestResult->ECG_CMRR[i], stGh3x2xMpLimit.ECG_CMRR_MinLimit);
		}
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--CMRR test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_LIN_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----LIN test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_LIN_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--LIN test start! \r\n");
	
	GU32 sample_num = 0;
	GU32 skip_num =  0;
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	GF32 LIN_Amp[ECG_MAX_SIGNAL_NUM] = {0}; //LIN test must two signals in a group
	GF32 LIN_Freq = 0;
	GF32 LIN_Ratio = 0;
	GU8 LIN_RatioCnt = 0;
	
	if((pstTestPara->LIN_ECGSignalNum > ECG_MAX_SIGNAL_NUM) || (0 == pstTestPara->LIN_ECGSignalNum) \
		|| (0 != (pstTestPara->LIN_ECGSignalNum % 2))) //must greater than 0 and a multiple of 2, not greater than ECG_MAX_SIGNAL_NUM
	{
		test_code = MT_ERROR_ECG_LIN_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test set signal num error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	//Check whether fft array need used space is in the range of GH3x2xMp_RawdataBuf
	//fft array space reuse GH3x2xMp_RawdataBuf memory
	if((MAX_FFT_LENGTH * 2 * 4) > (RAWDATA_BUF_SIZE / 2))
	{
		test_code = MT_ERROR_ECG_LIN_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test fft array need used space more than GH3x2xMp_RawdataBuf memory, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	for(i = 0; i < pstTestPara->LIN_ECGSignalNum; i++)
	{
		sample_num = pstTestPara->stECGSignal_LIN[i].Rawdata_Num;
		skip_num = pstTestPara->stECGSignal_LIN[i].Rawdata_SkipNum;
		//check LIN test sample num 
		if((sample_num > MAX_RAWDATA_SAMPLE_NUM) || (sample_num <= 0))
		{
			test_code = MT_ERROR_ECG_LIN_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test set the number of samples over limit, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//set ecg io status---LIN test
		GU16 HighImpedance =  (i % 2) ? 1 : 0;
		pstTestPara->stECGSignal_LIN[i].bECG_HighImpedance = HighImpedance;
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_LIN, pstTestPara->stECGSignal_LIN[i], NONE_SPEC))
		{
			test_code = MT_ERROR_ECG_LIN_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test set io status or signal fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_mp_delay_ms(20); //wait
		
		//chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test set reg lead config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test set reg default config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
		//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		gh3x2x_demo_mp_write_reg(0x1EA, 0x433); //Fast recovery, after sampling starts
		RawdataBuf_Len = sample_num + skip_num;
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		
		//ECG collection is fixed to slot0 ADC3, 
		if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, \
												GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
		{
			test_code = MT_ERROR_ECG_LIN_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN test handle rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//cal
		gh3x2x_demo_mp_ecg_cal_signal_amp_freq(GH3x2xMp_RawdataHandleArr, sample_num, &LIN_Amp[i], &LIN_Freq, 0, 0, 1, \
												0, sample_num, pstTestPara->stECGSignal_LIN[i].ECG_Freq, 1);
		
		GH3X2X_MP_LOG_L2("Gh3x2x mp LIN freq:%.5f \r\n", LIN_Freq);
		if(i && (i % 2))
		{
			if(LIN_Amp[i] > 0)
				LIN_Ratio = fabs(LIN_Amp[i - 1] - LIN_Amp[i]) / LIN_Amp[i];
			else
				LIN_Ratio = 1;
			pstTestResult->ECG_LIN_Ratio[LIN_RatioCnt++] = LIN_Ratio;
			
			//check limit
			if(LIN_Ratio > stGh3x2xMpLimit.ECG_LIN_MaxLimit)
			{
				test_code = MT_ERROR_ECG_LIN_ERR;
				GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, LIN ratio%d: %.5f more than max limit: %.5f, amp[0] is: %.3f and amp[1] is: %.3f, error code is: %d! \r\n", \
										LIN_RatioCnt-1, LIN_Ratio, stGh3x2xMpLimit.ECG_LIN_MaxLimit, LIN_Amp[i - 1], LIN_Amp[i], test_code);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, LIN ratio%d is: %.5f, max limit is: %.5f, LIN test is pass, amp[0] is: %.3f and amp[1] is: %.3f ! \r\n", \
										LIN_RatioCnt-1, LIN_Ratio, stGh3x2xMpLimit.ECG_LIN_MaxLimit, LIN_Amp[i - 1], LIN_Amp[i]);
			}
		}
	}
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--LIN test end! \r\n");
	return test_code;
}

/**
 * @fn     GU8 gh3x2x_demo_mp_ECG_DCRange_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
 *
 * @brief  ECG test items ----DCRange test
 *
 * @attention  
 *
 * @param[in]   stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult
 * @param[out]  
 *
 * @return  error code
 */
GU8 gh3x2x_demo_mp_ECG_DCRange_Test(stGh3x2xMp_ECGTestParam *pstTestPara, stGh3x2xMp_ECGTestResult *pstTestResult)
{
	GU8 test_code = 0;
	GU8 i = 0;
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--DCRange test start! \r\n");
	
	GU32 sample_num = 0;
	GU32 skip_num =  0;
	GU32 RawdataBuf_Len = 0;
	memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
	memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
	
	GF32 DCRange_Amp[ECG_MAX_SIGNAL_NUM] = {0}; //DCRange test must 3 signals in a group
	GU8 DCRange_AmpCnt = 0;
	//GF32 DCRange_Freq = 0;
	GF32 DCRange_Ratio[2] = {0};
	GU8 DCRange_RatioCnt = 0;
	
	
	//DCRange test must 3 signals in a group
	if((pstTestPara->DCRange_ECGSignalNum > ECG_MAX_SIGNAL_NUM) || (0 == pstTestPara->DCRange_ECGSignalNum) \
		|| (0 != (pstTestPara->DCRange_ECGSignalNum % 3))) //must greater than 0 and a multiple of 3, not greater than ECG_MAX_SIGNAL_NUM
	{
		test_code = MT_ERROR_ECG_DCRANGE_ERR;
		GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test set signal num error, error code is: %d! \r\n", test_code);
		return test_code;
	}
	
	for(i = 0; i < pstTestPara->DCRange_ECGSignalNum; i++)
	{
		sample_num = pstTestPara->stECGSignal_DCRange[i].Rawdata_Num;
		skip_num = pstTestPara->stECGSignal_DCRange[i].Rawdata_SkipNum;
		//check DCRange test sample num
		if((sample_num > MAX_RAWDATA_SAMPLE_NUM) || (sample_num <= 0))
		{
			test_code = MT_ERROR_ECG_DCRANGE_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test set the number of samples over limit, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//set ecg io status---LIN test
		if(!gh3x2x_mp_ecg_signal_and_io_status_set(ECG_SIGNAL_AND_IO_STATUS_DC_RANGE, pstTestPara->stECGSignal_DCRange[i], NONE_SPEC))
		{
			test_code = MT_ERROR_ECG_DCRANGE_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test set io status or signal fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_mp_delay_ms(20); //wait
		
		//chip resume
		gh3x2x_demo_mp_send_cmd(GH3X2X_MP_CMD_RESUME);
		gh3x2x_mp_delay_ms(5);
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGLeadConfg, sizeof(stGh3x2xMpRegECGLeadConfg) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test set reg lead config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		if(!gh3x2x_demo_mp_set_chip_config(stGh3x2xMpRegECGConfgDefault, sizeof(stGh3x2xMpRegECGConfgDefault) / sizeof(stGh3x2xReg)))
		{
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test set reg default config fail, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		gh3x2x_demo_mp_write_reg(0x0108, 0x0001); //slotcfg0 enable
	
		//get rawdata, The ECG test only collects data from one slot and ADC at a time, and LEDPDnum is fixed at 1.
		memset(GH3x2xMp_RawdataBuf, 0, RAWDATA_BUF_SIZE*sizeof(GU8));
		memset(GH3x2xMp_RawdataHandleArr, 0, MAX_RAWDATA_SAMPLE_NUM*sizeof(GS32));
		gh3x2x_demo_mp_sampling_mode(START_SAMPLING); //start
		gh3x2x_demo_mp_write_reg(0x1EA, 0x433); //Fast recovery, after sampling starts
		RawdataBuf_Len = sample_num + skip_num;
		if(!gh3x2x_demo_mp_get_rawdata(0xAAAA, GH3x2xMp_RawdataBuf, RawdataBuf_Len*4)) //every rawdata have 4 bytes
		{
			gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
			test_code = MT_ERROR_COMMUNICATE_FAIL;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test read rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		gh3x2x_demo_mp_sampling_mode(STOP_SAMPLING); //stop
		
		//ECG collection is fixed to slot0 ADC3, 
		if(!gh3x2x_demo_mp_handle_rawdata(SPEC_MODE_ECG, GH3x2xMp_RawdataHandleArr, skip_num, 0, 3, \
												GH3x2xMp_RawdataBuf, RawdataBuf_Len, 1))
		{
			test_code = MT_ERROR_ECG_DCRANGE_ERR;
			GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange test handle rawdata error, error code is: %d! \r\n", test_code);
			return test_code;
		}
		
		//cal amp
		gh3x2x_demo_mp_ecg_cal_signal_WaveTimeDomin_Amp(GH3x2xMp_RawdataHandleArr, sample_num, &DCRange_Amp[i], 1, \
															pstTestPara->stECGSignal_DCRange[i].ECG_Freq);
		if(0 == ((i + 1) % 3))
		{
			if(DCRange_Amp[i - 2] > (GF32)0.1)
			{
				DCRange_Ratio[0] = fabs(DCRange_Amp[i - 2] - DCRange_Amp[i - 1]) / DCRange_Amp[i - 2];
				DCRange_Ratio[1] = fabs(DCRange_Amp[i - 2] - DCRange_Amp[i]) / DCRange_Amp[i - 2];
			}
			else
			{
				DCRange_Ratio[0] = 1;
				DCRange_Ratio[1] = 1;
			}
			pstTestResult->ECG_DCRange_Ratio[DCRange_RatioCnt++] = DCRange_Ratio[0];
			pstTestResult->ECG_DCRange_Ratio[DCRange_RatioCnt++] = DCRange_Ratio[1];
			pstTestResult->ECG_DCRange_Amp[DCRange_AmpCnt++] = DCRange_Amp[i - 2];
			pstTestResult->ECG_DCRange_Amp[DCRange_AmpCnt++] = DCRange_Amp[i - 1];
			pstTestResult->ECG_DCRange_Amp[DCRange_AmpCnt++] = DCRange_Amp[i];
			
			if(DCRange_Ratio[0] > stGh3x2xMpLimit.ECG_DCRange_MaxLimit \
				|| DCRange_Ratio[1] > stGh3x2xMpLimit.ECG_DCRange_MaxLimit)
			{
				test_code = MT_ERROR_ECG_DCRANGE_ERR;
				GH3X2X_MP_LOG_L1("Gh3x2x mp ECG test fail, DCRange ratio[%d]: %.5f and ratio[%d]: %.5f more than max limit: %.5f, amp[0] is: %.3f and amp[1] is: %.3f and amp[2] is: %.3f, error code is: %d! \r\n", \
									DCRange_RatioCnt-2, DCRange_Ratio[0], DCRange_RatioCnt-1, DCRange_Ratio[1], stGh3x2xMpLimit.ECG_DCRange_MaxLimit, \
									DCRange_Amp[i - 2], DCRange_Amp[i - 1], DCRange_Amp[i], test_code);
			}
			else
			{
				GH3X2X_MP_LOG_L2("Gh3x2x mp ECG test result info, DCRange ratio[%d] is: %.5f and ratio[%d] is: %.5f, max limit is: %.5f, DCRange test is pass, amp[0] is: %.3f and amp[1] is: %.3f and amp[2] is: %.3f! \r\n", \
									DCRange_RatioCnt-2, DCRange_Ratio[0], DCRange_RatioCnt-1, DCRange_Ratio[1], stGh3x2xMpLimit.ECG_DCRange_MaxLimit, \
									DCRange_Amp[i - 2], DCRange_Amp[i - 1], DCRange_Amp[i]);
			}
		}
	}
	
	
	GH3X2X_MP_LOG_L2("Gh3x2x mp ECG--DCRange test end! \r\n");
	return test_code;
	
}

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
