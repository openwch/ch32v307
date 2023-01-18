/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "ov.h"

/*
 *@Note
  DVP routine to operate OV2640 camera JPEG mode:
    Output picture data through UART2 (PA2), display pictures through serial port image software,
    or take 0xFF from a frame of data, starting with 0xD8;0xFF, data at the end of 0xD9, modify the
    file format to display pictures.
  DVP--PIN:
    D10--PD6
    D11--PD2
    D8--PC10
    D9--PC12
    DPWDN--PC3
    DPCLK--PA6
    D7--PB9
    D6--PB8
    D5--PB6
    D4--PC11
    D3--PC9
    D2--PC8
    D1--PA10
    D0--PA9
    RESET--PC13
    SDA--PB11
    HERF--PA4
    SDCLK--PB10
    SYNC--PA5

    Use UART2 (PA2) serial port output, #define DEBUG DEBUG_UART2 in debug.h
    UART1(PA9) is occupied by DVP
 */


/* DVP Work Mode */
#define JPEG_MODE     1
/* DVP Work Mode Selection */
#define DVP_Work_Mode    JPEG_MODE


UINT32  JPEG_DVPDMAaddr0 = 0x20005000;
UINT32  JPEG_DVPDMAaddr1 = 0x20005000 + OV2640_JPEG_WIDTH;

UINT32  RGB565_DVPDMAaddr0 = 0x20005000;
UINT32  RGB565_DVPDMAaddr1 = 0x20005000 + RGB565_COL_NUM;

volatile UINT32 frame_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 href_cnt = 0;

void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      UART2_Send_Byte
 *
 * @brief   UART2 send one byte data.
 *
 * @param   t - UART send Data.
 *
 * @return  none
 */
void UART2_Send_Byte(u8 t)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, t);
}

/*********************************************************************
 * @fn      DVP_Init
 *
 * @brief   Init DVP
 *
 * @return  none
 */
void DVP_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure={0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DVP, ENABLE);

    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;

#if (DVP_Work_Mode == JPEG_MODE)
    /* VSYNC��HSYNC - High level active */
    DVP->CR0 |= RB_DVP_D8_MOD | RB_DVP_V_POLAR | RB_DVP_JPEG;
    DVP->CR1 &= ~(RB_DVP_ALL_CLR| RB_DVP_RCV_CLR);

    DVP->COL_NUM = OV2640_JPEG_WIDTH;

    DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;        //DMA addr0
    DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;        //DMA addr1
#endif

    /* Set frame capture rate */
    DVP->CR1 &= ~RB_DVP_FCRC;
    DVP->CR1 |= DVP_RATE_25P;  //25%

    /* Interupt Enable */
    DVP->IER |= RB_DVP_IE_STP_FRM;
    DVP->IER |= RB_DVP_IE_FIFO_OV;
    DVP->IER |= RB_DVP_IE_FRM_DONE;
    DVP->IER |= RB_DVP_IE_ROW_DONE;
    DVP->IER |= RB_DVP_IE_STR_FRM;

    NVIC_InitStructure.NVIC_IRQChannel = DVP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DVP->CR1 |= RB_DVP_DMA_EN;  //enable DMA
    DVP->CR0 |= RB_DVP_ENABLE;  //enable DVP
}

u32 DVP_ROW_cnt=0;

/*********************************************************************
 * @fn      DVP_IRQHandler
 *
 * @brief   This function handles DVP exception.
 *
 * @return  none
 */
void DVP_IRQHandler(void)
{
    if (DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        /* Write 0 clear 0 */
        DVP->IFR &= ~RB_DVP_IF_ROW_DONE;  //clear Interrupt

#if (DVP_Work_Mode == JPEG_MODE)
        href_cnt++;

        if (addr_cnt%2)     //buf1 done
        {
            addr_cnt++;
            DVP->DMA_BUF1 += OV2640_JPEG_WIDTH *2;
        }
        else                //buf0 done
        {
            addr_cnt++;
            DVP->DMA_BUF0 += OV2640_JPEG_WIDTH *2;
        }

#endif

    }

    if (DVP->IFR & RB_DVP_IF_FRM_DONE)
    {
        DVP->IFR &= ~RB_DVP_IF_FRM_DONE;  //clear Interrupt

#if (DVP_Work_Mode == JPEG_MODE)
        DVP->CR0 &= ~RB_DVP_ENABLE;       //disable DVP

        /* Use uart2 send JPEG data */
        {
            UINT32 i;
            UINT8 val;

            href_cnt = href_cnt*OV2640_JPEG_WIDTH;

            for(i=0; i<href_cnt; i++){
                val = *(UINT8*)(0x20005000+i);
                UART2_Send_Byte(val);
            }
        }

        DVP->CR0 |= RB_DVP_ENABLE;  //enable DVP

        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;        //DMA addr0
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;        //DMA addr1
        href_cnt = 0;

        addr_cnt =0;

#endif

    }

    if (DVP->IFR & RB_DVP_IF_STR_FRM)
    {
        DVP->IFR &= ~RB_DVP_IF_STR_FRM;  //clear Interrupt

        frame_cnt++;
    }

    if (DVP->IFR & RB_DVP_IF_STP_FRM)
    {
        DVP->IFR &= ~RB_DVP_IF_STP_FRM;  //clear Interrupt

    }

    if (DVP->IFR & RB_DVP_IF_FIFO_OV)
    {
        DVP->IFR &= ~RB_DVP_IF_FIFO_OV;   //clear Interrupt

#if 0
        printf("FIFO OV\r\n");

#endif
    }

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(921600);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    while(OV2640_Init())
    {
        printf("Camera Model Err\r\n");
        Delay_Ms(1000);
    }

    Delay_Ms(1000);

    RGB565_Mode_Init();
    Delay_Ms(1000);

#if (DVP_Work_Mode == JPEG_MODE)
    printf("JPEG_MODE\r\n");
    JPEG_Mode_Init();
    Delay_Ms(1000);

#endif

    DVP_Init();

    while(1);
}







