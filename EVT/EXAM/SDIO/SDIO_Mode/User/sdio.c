/********************************** (C) COPYRIGHT *******************************
* File Name          : sdio.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/07/10
* Description        : This file contains the headers of the SDIO.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "sdio.h"
#include "string.h"
#include "sdio.h"


SDIO_InitTypeDef SDIO_InitStructure ={0};
SDIO_CmdInitTypeDef SDIO_CmdInitStructure ={0};
SDIO_DataInitTypeDef SDIO_DataInitStructure ={0};


/*********************************************************************
 * @fn      Host_Send_Cmd
 *
 * @brief   The host sends commands
 *
 * @return  none
 */
void Host_Send_Cmd(u8 arg,u8 index)
{
    printf("Send CMD\r\n");
    SDIO_CmdInitStructure.SDIO_Argument =arg;
    SDIO_CmdInitStructure.SDIO_CmdIndex =index;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );
    printf("Send End\r\n");
}

/*********************************************************************
 * @fn      Host_Res_R1
 *
 * @brief   Read the slave reply R1
 *
 * @return  R1
 */
u8 Host_Res_R1()
{
    u8 Res_R1;
    Res_R1=SDIO->RESP1;
    return Res_R1;
}

/*********************************************************************
 * @fn      Host_Send_Data
 *
 * @brief   Host send data
 *
 * @return  none
 */
void Host_Send_Data(u32 data,u8 len)
{
    printf("Send data\r\n");
    SDIO->DCTRL2 |= (1 << 16);   //RANDOM_LEN_EN OPEN
    SDIO->DCTRL2 |= (len << 0);  //Write length
    SDIO->DTIMER = 0xFFFFFFFF;
    SDIO->DLEN = len;
    SDIO->DCTRL &= ~(1 << 2);       //Block mode
//    SDIO->DCTRL |=(1<<2);         //Streaming mode
    SDIO->DCTRL |= (1 << 0);        //Transmission enabled
    SDIO->FIFO = data;              //really data
    while((SDIO->STA&SDIO_FLAG_DATAEND)!=SDIO_FLAG_DATAEND);
    SDIO->DCTRL &= ~(1 << 0);       //Turn off the transfer
    printf("Send End\r\n");
}

/*********************************************************************
 * @fn      Host_Rec_Data
 *
 * @brief   Host receive data
 *
 * @return  data
 */
u32 Host_Rec_Data(u8 len)
{
    u32 data;
    SDIO->DCTRL2 |= (1 << 16);
    SDIO->DCTRL2 |= (len << 0);
    SDIO->DTIMER = 0xFFFFFFFF;
    SDIO->DLEN = len;
    SDIO->DCTRL &= ~(1 << 2);
//    SDIO->DCTRL |=(1<<2);
    SDIO->DCTRL |=(1<<1);
    SDIO->DCTRL |= (1 << 0);
    data= SDIO->FIFO ;
    while((SDIO->STA&SDIO_FLAG_DATAEND)!=SDIO_FLAG_DATAEND);
    SDIO->DCTRL &= ~(1 << 0);
    printf("Rec End\r\n");
    return data;
}

/*********************************************************************
 * @fn      Slv_Rec_CMD
 *
 * @brief   The slave receives and prints commands
 *
 * @return  none
 */
void Slv_Rec_CMD()
{
    printf("SDIO->RESP1=%x\r\n",SDIO->RESP1);
    printf("SDIO->RESP2=%x\r\n",SDIO->RESP2);

}


/*********************************************************************
 * @fn      Slv_Mode_En
 *
 * @brief   The slave enabled
 *
 * @return  none
 */
void Slv_Mode_En()
{
    SDIO->DCTRL2|=(1<<24);
}

/*********************************************************************
 * @fn      Slv_CRC_Err
 *
 * @brief   The slave forces the host CRC to report an error
 *
 * @return  none
 */
void Slv_CRC_Err()
{
    SDIO->DCTRL2|=(1<<25);
}

/*********************************************************************
 * @fn      Slv_R1_Arg
 *
 * @brief   The slave is set to reply to the host's reply command
 *
 * @return  none
 */
void Slv_R1_Arg(u8 cmd)
{
    SDIO->ARG=cmd;
}

/*********************************************************************
 * @fn      Slv_Send_Data
 *
 * @brief   The slave send data
 *
 * @return  none
 */

void Slv_Send_Data(u32 data,u8 len)
{
    SDIO->DCTRL2 |= (1 << 16);
    SDIO->DCTRL2 |= (len << 0);
    SDIO->DTIMER = 0xFFFFFFFF;
    SDIO->DLEN = len;
    SDIO->DCTRL &= ~(1 << 2);
//    SDIO->DCTRL |=(1<<2);
    SDIO->DCTRL |= (1 << 0);
    SDIO->FIFO = data;
    while((SDIO->STA&SDIO_FLAG_DATAEND)!=SDIO_FLAG_DATAEND);
    SDIO->DCTRL &= ~(1 << 0);
}

/*********************************************************************
 * @fn      Slv_Rec_Data
 *
 * @brief   The slave receive data
 *
 * @return  data
 */
u32 Slv_Rec_Data(u8 len)
{
    u32 data;
    SDIO->DCTRL2 |= (1 << 16);
    SDIO->DCTRL2 |= (len << 0);
    SDIO->DTIMER = 0xFFFFFFFF;
    SDIO->DLEN = len;
    SDIO->DCTRL &= ~(1 << 2);
//    SDIO->DCTRL |=(1<<2);
    SDIO->DCTRL |=(1<<1);
    SDIO->DCTRL |= (1<< 0);
    data= SDIO->FIFO ;
    while((SDIO->STA&SDIO_FLAG_DATAEND)!=SDIO_FLAG_DATAEND);
    SDIO->DCTRL &= ~(1 << 0);
    return data;
}

/*********************************************************************
 * @fn      SDIO_Init
 *
 * @brief   Init SDIO Host/Slv.
 *
 */
void SD_Init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_SDIO | RCC_AHBPeriph_DMA2, ENABLE );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

#if(mode==Hst)
    //HST Clk
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

#elif(mode==Slv)
    //SLV
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );
#endif

    /*SDIO_CMD */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    /*SDIO_CMD */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );

    SDIO_DeInit();

    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Enable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
    SDIO_Init( &SDIO_InitStructure );

    SDIO_SetPowerState( SDIO_PowerState_ON );
    SDIO_ClockCmd( ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}
