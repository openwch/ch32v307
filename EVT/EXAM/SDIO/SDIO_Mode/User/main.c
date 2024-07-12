/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/07/10
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *
 *   This example provides a simple example of SDIO master and slave communication.
 *  
 * DVP--PIN:
 *   SDCK--PC12
 *   CMD--PD2
 *   SD0--PC8
 *   SD1--PC9
 *   SD2--PC10
 *   SD3--PC11
 *   SD4--PB8
 *   SD5--PB9
 *   SD6--PC6
 *   SD7--PC7
 *   Note: Except for SCK, the rest need to pull up 47K resistors
 *   When in use, Make sure the data receiver is ready before sending the data.
 */

#include "debug.h"
#include "string.h"
#include "sdio.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u32 Resdata;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
#if(mode==Hst)
    printf("Host Mode\r\n");
#elif(mode==Slv)
    printf("Slv Mode\r\n");
#endif

    SD_Init();

#if(mode==Hst)
        Host_Send_Cmd(0x11,0x12);
        Delay_Ms(5);
        printf("R1=%x\r\n",SDIO->RESP1);
        Host_Send_Data(0x12345678,4);

#elif(mode==Slv)
        Slv_Mode_En();
        Slv_R1_Arg(0x13);
        Slv_Rec_CMD();
        Delay_Ms(5);
        Resdata=Slv_Rec_Data(4);
        printf("ResData=%x\r\n",Resdata);
        Slv_Rec_CMD();
#endif
        while(1);

}
