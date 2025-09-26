#include "gh_drv.h"

#define  GH3X2X_SLOT0_CTRL_3_REG_ADDR               (0x0110)
#define  GH3X2X_SLOT_CTRL_OFFSET                    (0x001C)    
#define  GH3X2X_RG_SLOT_TMR0_REG_ADDR               (0x01EC)
#define  GH3X2X_SLOT_ADC_INT_TIME_LSB               (0)
#define  GH3X2X_SLOT_ADC_INT_TIME_MSB               (3)
#define  GH3X2X_SLOT_BG_LEVEL_LSB                   (4)
#define  GH3X2X_SLOT_BG_LEVEL_MSB                   (5)
#define  GH3X2X_SLOT_BG_CANCEL_LSB                  (10)
#define  GH3X2X_SLOT_BG_CANCEL_MSB                  (10)

static const GU16 gusSlotTimeList[3][7] = {
{26,    36,    46,    56,    95,    174,    331},
{48,    67,    87,    107,    186,    343,    658},
{69,    99,    128,    158,    276,    512,    985}
};


static void GH3x2x_SetIntTime(GU8 uchSlotNo, GU8 uchIntTimeIndex)
{    
    GH3X2X_WriteRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo,GH3X2X_SLOT_ADC_INT_TIME_LSB,GH3X2X_SLOT_ADC_INT_TIME_MSB, uchIntTimeIndex);
}

static void GH3x2x_SetSlotTime(GU8 uchSlotNo, GU16 usSlotTime)
{    
    GH3X2X_WriteReg(GH3X2X_RG_SLOT_TMR0_REG_ADDR + 2*uchSlotNo, usSlotTime);
}

/**
 * @fn      void GH3x2x_ChangeIntTime(GU8 uchSlotNo , GU8  chIntTimeIndex)
 * @brief  change slot int time 
 *
 * @attention   None.
 *
 * @param[in]   uchSlotNo: slot number
 * @param[in]   uchIntTimeIndex: Int time index，0:10us 1：20us 2:30us 3:39us 4:79us 5:158us 6:316us
 * @param[out]  None
 *
 * @return  None
 */
void GH3x2x_ChangeIntTime(GU8 uchSlotNo, GU8 uchIntTimeIndex)
{
    GU8 uchBglevel = 0;    //slot bg level
    GU8 uchBgCancel = 0;   //slot bg cancel
    GU16 usTimeSlot = 0;
    GU8 uchCfgIndex = 0;   
    GU8 uchSlotIndex = 0;

    //read bg level
    uchBglevel = GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo,GH3X2X_SLOT_BG_LEVEL_LSB,GH3X2X_SLOT_BG_LEVEL_MSB);
    //read bg cancel
    uchBgCancel= GH3X2X_ReadRegBitField(GH3X2X_SLOT0_CTRL_3_REG_ADDR + GH3X2X_SLOT_CTRL_OFFSET*uchSlotNo,GH3X2X_SLOT_BG_CANCEL_LSB,GH3X2X_SLOT_BG_CANCEL_MSB);

    if(uchBglevel > 2)
    {
        uchBglevel = 2;
    }
    usTimeSlot = gusSlotTimeList[uchBglevel][uchIntTimeIndex];
    if(uchBgCancel > 0)
    {
        usTimeSlot += 104;
    }
    usTimeSlot += 80;  

    //find cfg index
    for(GU8 uchSlotCnt = 0; uchSlotCnt < 8; uchSlotCnt ++)
    {
        if(0 == (uchSlotCnt&0x01)) //0,2,4,6
        {
            uchCfgIndex = GH3X2X_ReadRegBitField(0x0100 + (uchSlotCnt&0xFE), 0,3);
        }
        else  //1,3,5,7
        {
           uchCfgIndex = GH3X2X_ReadRegBitField(0x0100 + (uchSlotCnt&0xFE), 8,11);
        }

        if(uchCfgIndex == uchSlotNo)
        {
            uchSlotIndex = uchSlotCnt;
        }
    }

    GH3x2x_SetIntTime(uchSlotNo,uchIntTimeIndex);
    GH3x2x_SetSlotTime(uchSlotIndex,usTimeSlot);
}

//示例代码
#if 0
void main()
{
    GU8 usStartReg;
    usStartReg = GH3X2X_ReadReg(0x0000);  //read start reg
    
    if(usStartReg & 0x0001)
    {
        GH3X2X_WriteReg(0x0000,usStartReg&(0xFFFE));   //停止采样，确保更改积分时间前停止采样
        GH3x2x_ChangeIntTime(0,1);
        GH3X2X_WriteReg(0x0000,usStartReg|0x0001);   //重新采样，更改积分时间后重新开始采样
    }
    else
    {
        GH3x2x_ChangeIntTime(0,1);
    }
}
#endif
