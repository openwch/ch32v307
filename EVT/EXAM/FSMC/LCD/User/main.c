/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   FSMC routine to operate TFTLCD:
  LCD--PIN:
    PD11--FSMC_A16
    PD12--FSMC_A17
    PD5 --FSMC_NEW
    PD4 --FSMC_NOE
    PA15--LCDRST#
    PD14--FSMC_D0
    PD15--FSMC_D1
    PD0 --FSMC_D2
    PD1--FSMC_D3
    PE7--FSMC_D4
    PE8 --FSMC_D5
    PE9 --FSMC_D6
    PE10--FSMC_D7
    PE11--FSMC_D8
    PE12--FSMC_D9
    PE13--FSMC_D10
    PE14--FSMC_D11
    PE15--FSMC_D12
    PD8 --FSMC_D13
    PD9--FSMC_D14
    PD10--FSMC_D15
    PB14--IO_BLCTR
    PA8 --IO_MISO_NC
    PB3 --IO_MOSI_SDA
    PB15--IO_TKINT
    PC13--IO_BUSY_NC
    PC0 --IO_TKRST#
    PB4 --IO_CLK
*/

#include "debug.h"
#include "lcd.h"

/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
 	u8 x=0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	LCD_Reset_GPIO_Init();
	//LCD reset
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	Delay_Ms(100);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);

    LCD_Init();
	POINT_COLOR=RED;		
	 
    while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,40,210,24,24,"CH32V307");
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
		LCD_ShowString(30,90,200,16,16,"WCH");
	    x++;
		if(x==12)x=0;			   		 
		Delay_Ms(1000);	

	} 

}







