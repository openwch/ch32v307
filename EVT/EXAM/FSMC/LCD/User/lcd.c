/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the TFTLCD.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "lcd.h"
#include "stdlib.h"
#include "font.h" 

/* LCD brush and background colors */
u16 POINT_COLOR=0x0000;
u16 BACK_COLOR=0xFFFF;
  
_lcd_dev lcddev;
	 
/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write register
 *
 * @param   regval - register value
 *
 * @return  none
 */
void LCD_WR_REG(u16 regval)
{   
	LCD->LCD_REG=regval;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write data
 *
 * @param   data
 *
 * @return  none
 */
void LCD_WR_DATA(u16 data)
{	 
	LCD->LCD_RAM=data;		 
}

/*********************************************************************
 * @fn      LCD_RD_DATA
 *
 * @brief   Read data
 *
 * @return  ram - read data
 */
u16 LCD_RD_DATA(void)
{
	vu16 ram;
	ram=LCD->LCD_RAM;	
	return ram;	 
}					   

/*********************************************************************
 * @fn      LCD_WriteReg
 *
 * @brief   Write value to register
 *
 * @param   LCD_Reg - register addr
 *          LCD_RegValue - value
 *
 * @return  none
 */
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;
	LCD->LCD_RAM = LCD_RegValue;
}	   

/*********************************************************************
 * @fn      LCD_ReadReg
 *
 * @brief   Read value from register
 *
 * @param   LCD_Reg - register addr
 *
 * @return  register value
 */
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);
	delay_us(5);		  
	return LCD_RD_DATA();
}   

/*********************************************************************
 * @fn      LCD_WriteRAM_Prepare
 *
 * @brief   Write GRAM prepare
 *
 * @return  none
 */
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 

/*********************************************************************
 * @fn      LCD_WriteRAM
 *
 * @brief   Write GRAM
 *
 * @param   RGB_Code - colour value
 *
 * @return  none
 */
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;
}

/*********************************************************************
 * @fn      LCD_SetCursor
 *
 * @brief   set Cursor
 *
 * @param   Xpos - Abscissa
 *          Ypos - Ordinate
 *
 * @return  none
 */
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
	}
} 		 

/*********************************************************************
 * @fn      LCD_Scan_Dir
 *
 * @brief   set scan direction
 *
 * @param   dir - direction
 *
 * @return  none
 */
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  

	if((lcddev.dir==1&&lcddev.id==0x9341)){
        switch(dir)
        {
            case 0:dir=6;break;
            case 1:dir=7;break;
            case 2:dir=4;break;
            case 3:dir=5;break;
            case 4:dir=1;break;
            case 5:dir=0;break;
            case 6:dir=3;break;
            case 7:dir=2;break;
        }
	}

	if(lcddev.id==0x9341)
	{
		switch(dir)
		{
			case L2R_U2D:
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U:
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D:
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U:
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R:
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L:
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R:
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L:
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
        dirreg=0X36;
        regval|=0X08;
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}

        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);

  	}
}     

/*********************************************************************
 * @fn      LCD_Display_Dir
 *
 * @brief   set display direction
 *
 * @param   dir - direction
 *
 * @return  none
 */
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)
	{
		lcddev.dir=0;
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}

	}else
	{	  				
		lcddev.dir=1;
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}

	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);
}	 

/*********************************************************************
 * @fn      LCD_Init
 *
 * @brief   Init LCD
 *
 * @return  none
 */
void LCD_Init(void)
{ 					
 	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 

    /*   RS��PG0����PD12  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* CS: PG12����PD11*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD,GPIO_Pin_11);

    readWriteTiming.FSMC_AddressSetupTime = 0x01;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x0f;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    writeTiming.FSMC_AddressSetupTime = 0x00;
    writeTiming.FSMC_AddressHoldTime = 0x00;
    writeTiming.FSMC_DataSetupTime = 0x03;
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
 
	delay_ms(50);
  	lcddev.id=LCD_ReadReg(0x0000);
  	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)
	{
		LCD_WR_REG(0XD3);				   
		lcddev.id=LCD_RD_DATA();	//dummy read 	
 		lcddev.id=LCD_RD_DATA();
  		lcddev.id=LCD_RD_DATA();
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();
	} 
 	printf(" LCD ID:%x\r\n",lcddev.id);
	if(lcddev.id==0X9341)
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1B);   //VRH[5:0] 
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATA(0XB7); 
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A); 
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 		 
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);	 
		LCD_WR_REG(0x11); //Exit Sleep
		delay_ms(120);
		LCD_WR_REG(0x29); //display on	
	}
	
	LCD_Display_Dir(0);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	LCD_Clear(WHITE);
}  

/*********************************************************************
 * @fn      LCD_Clear
 *
 * @brief   Clear screen
 *
 * @param   color - fill color
 *
 * @return  none
 */
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height;
    LCD_SetCursor(0x00,0x0000);
	LCD_WriteRAM_Prepare();
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
}  

/*********************************************************************
 * @fn      LCD_ShowString
 *
 * @brief   Display string
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          width - String width
 *          height - String height
 *          size - font size
 *          *p - String start address
 *
 * @return  none
 */
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
    u8 x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' '))
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}

/*********************************************************************
 * @fn      LCD_Pow
 *
 * @brief   m^n function
 *
 * @return  result - m^n
 */
u32 LCD_Pow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}

/*********************************************************************
 * @fn      LCD_ShowNum
 *
 * @brief   Display number
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          len - number lenth
 *          size -font size
 *          color - font color
 *          num - value(0~4294967295)
 *
 * @return  none
 */
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
    u8 t,temp;
    u8 enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/LCD_Pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
                continue;
            }else enshow=1;

        }
        LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
    }
}

/*********************************************************************
 * @fn      LCD_ShowChar
 *
 * @brief   Displays a character in the specified position
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          num - Displays a character
 *          size -font size
 *          mode - superposition mode
 *
 * @return  none
 */
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp,t1,t;
    u16 y0=y;
    u8 csize=(size/8+((size%8)?1:0))*(size/2);
    num=num-' ';
    for(t=0;t<csize;t++)
    {
        if(size==12)temp=asc2_1206[num][t];
        else if(size==16)temp=asc2_1608[num][t];
        else if(size==24)temp=asc2_2412[num][t];
        else return;
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
            else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
            temp<<=1;
            y++;
            if(y>=lcddev.height)return;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=lcddev.width)return;
                break;
            }
        }
    }
}

/*********************************************************************
 * @fn      LCD_Fast_DrawPoint
 *
 * @brief   fast draw point
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          color - font color
 *
 * @return  none
 */
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
    if(lcddev.id==0X9341)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF);
    }
    LCD->LCD_REG=lcddev.wramcmd;
    LCD->LCD_RAM=color;
}
