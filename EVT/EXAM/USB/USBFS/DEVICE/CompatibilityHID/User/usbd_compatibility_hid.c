/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.c
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022��7��8��
 * Description:
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "debug.h"
#include "string.h"
#include "ch32v30x_usbfs_device.h"
#include "usbd_compatibility_hid.h"
#include "ch32v30x_usb.h"
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

__attribute__ ((aligned(4))) uint8_t UART2_RxBuffer[DEF_UART2_BUF_SIZE];  // UART2 Rx Buffer
__attribute__ ((aligned(4))) uint8_t  HID_Report_Buffer[64];              // HID Report Buffer
volatile uint8_t HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;               // HID SetReport flag

volatile uint16_t UART2_TimeOut;                                           // UART2 RX timeout flag
volatile uint8_t  UART2_Tx_Flag = 0;                                       // UART2 TX flag

volatile uint16_t UART2_RX_CurCnt = 0;                                     // UART2 DMA current remain count
volatile uint16_t UART2_RX_LastCnt = 0;                                    // UART2 DMA last remain count
volatile uint16_t UART2_Rx_RemainLen = 0;                                  // UART2 RX data remain len
volatile uint16_t UART2_Rx_Deal_Ptr = 0;                                   // UART2 RX data deal pointer

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Timer2 100us initialisation
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_Prescaler =100;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   TIM2 IRQ handler
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    UART2_TimeOut++;
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

/*********************************************************************
 * @fn      UART2_DMA_Init
 *
 * @brief   UART2 DMA initialization
 *
 * @return  none
 */
void UART2_DMA_Init( void )
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    /* UART2 Tx DMA initialization */
    DMA_Cmd( DMA1_Channel7, DISABLE );
    DMA_DeInit( DMA1_Channel7 );
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Data_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DEF_USBD_FS_PACK_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA1_Channel7, &DMA_InitStructure );

    /* UART2 Rx DMA initialization */
    DMA_Cmd( DMA1_Channel6, DISABLE );
    DMA_DeInit( DMA1_Channel6 );
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART2_RxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DEF_UART2_BUF_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init( DMA1_Channel6, &DMA_InitStructure );
    DMA_Cmd( DMA1_Channel6, ENABLE );
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}


/*********************************************************************
 * @fn      UART2_Init
 *
 * @brief   UART2 DMA initialization
 *
 * @return  none
 */
void UART2_Init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* UART2 GPIO Init */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* UART2 Init */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ClearFlag( USART2, USART_FLAG_TC );
    USART_Cmd(USART2, ENABLE);
}

void UART2_DMA_Tx(uint8_t *pbuf,uint16_t len)
{
    USART_ClearFlag(USART2, USART_FLAG_TC);
    DMA_Cmd( DMA1_Channel7, DISABLE );
    DMA1_Channel7->MADDR = (uint32_t)pbuf;
    DMA1_Channel7->CNTR = (uint32_t)len;
    DMA_Cmd( DMA1_Channel7, ENABLE );
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}

/*********************************************************************
 * @fn      UART2_Rx_Service
 *
 * @brief   UART2 rx service routine that sends the data received by
 *          uart2 via USB-HID.
 *
 * @return  none
 */
void UART2_Rx_Service( void )
{
    uint16_t pkg_len;
    uint16_t u16_temp;
    uint16_t remain_len;

    /* Get uart2 rx data */
    UART2_RX_CurCnt = DMA_GetCurrDataCounter(DMA1_Channel6);                              // Get DMA remaining count
    if (UART2_RX_LastCnt != UART2_RX_CurCnt)
    {
        if (UART2_RX_LastCnt > UART2_RX_CurCnt)
        {
            u16_temp = UART2_RX_LastCnt - UART2_RX_CurCnt;
        }
        else
        {
            u16_temp = UART2_RX_LastCnt + ( DEF_UART2_BUF_SIZE - UART2_RX_CurCnt );
        }

        UART2_RX_LastCnt = UART2_RX_CurCnt;

        if ((UART2_Rx_RemainLen + u16_temp) > DEF_UART2_BUF_SIZE )
        {
            printf("Uart2 RX_buffer overflow\n");                                           // overflow: New data overwrites old data
        }
        else
        {
            UART2_Rx_RemainLen += u16_temp;
        }
        UART2_TimeOut = 0;
    }

    /* Pass-through uart2 data to USB-HID  */
    if (UART2_Rx_RemainLen)
    {
        /* Determine if the USB endpoint is allowed to upload */
        if (USBFS_Endp_Busy[DEF_UEP2] == 0x00)
        {
            /* Calculate the length of this packet upload via USB. */
            remain_len = UART2_Rx_RemainLen;
            pkg_len = 0x00;
            if (remain_len >= ( DEF_USBD_FS_PACK_SIZE - 1 ))
            {
                remain_len = ( DEF_USBD_FS_PACK_SIZE - 1 );
                pkg_len = remain_len;
            }
            else
            {
                if (UART2_TimeOut > DEF_UART2_TOUT_TIME)
                {
                    pkg_len = remain_len;
                }
            }

            if ( pkg_len > ( DEF_UART2_BUF_SIZE - UART2_Rx_Deal_Ptr )  )
            {
                pkg_len = ( DEF_UART2_BUF_SIZE - UART2_Rx_Deal_Ptr );                       // The data at the end of the buffer will be sent directly.
            }

            /* Upload packet via USB. */
            if (pkg_len)
            {
                USBOTG_FS->UEP2_DMA = (uint16_t)(uint32_t)USBFS_EP2_Buf;
                USBFS_EP2_Buf[0] = pkg_len;
                memcpy(USBFS_EP2_Buf + 1,&UART2_RxBuffer[UART2_Rx_Deal_Ptr],pkg_len);
                USBOTG_FS->UEP2_TX_LEN = pkg_len + 1;
                USBFS_Endp_Busy[DEF_UEP2] = 1;
                USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;            // Start Upload
                UART2_Rx_RemainLen -= pkg_len;
                UART2_Rx_Deal_Ptr += pkg_len;

                if (UART2_Rx_Deal_Ptr >= DEF_UART2_BUF_SIZE)
                {
                    UART2_Rx_Deal_Ptr = 0x00;
                }

            }
        }
    }

}

/*********************************************************************
 * @fn      UART2_Tx_Service
 *
 * @brief   UART2 tx service routine that sends the data received by
 *          USB-HID via uart2.
 *
 * @return  none
 */
void UART2_Tx_Service( void )
{
    uint16_t pkg_len = 0;
    uint8_t *pbuf;
    if (UART2_Tx_Flag)
    {
        if (USART_GetFlagStatus(USART2, USART_FLAG_TC))                                  // Check whether uart2 has finished sending.
        {
            USART_ClearFlag(USART2, USART_FLAG_TC);
            USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
            UART2_Tx_Flag = 0;
            NVIC_DisableIRQ(OTG_FS_IRQn);                                                  // Disable USB interrupts
            RingBuffer_Comm.RemainPack--;
            RingBuffer_Comm.DealPtr++;
            if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
            {
                RingBuffer_Comm.DealPtr = 0;
            }
            NVIC_EnableIRQ(OTG_FS_IRQn);                                                   // Enable USB interrupts
        }
    }
    else
    {
        /* Pass-through USB-HID data to uart2 */
        if(RingBuffer_Comm.RemainPack)
        {
            pkg_len = Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE];      // Get the valid data length
            if (pkg_len)
            {
                if (pkg_len > ( DEF_USBD_FS_PACK_SIZE - 1 ) )
                {
                    pkg_len = DEF_USBD_FS_PACK_SIZE - 1;                                   // Limit the length of this transmission
                }
                pbuf = &Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE] + 1;
                UART2_DMA_Tx( pbuf, pkg_len );
                UART2_Tx_Flag = 1;
            }
            else
            {
                /* drop out */
                NVIC_DisableIRQ(OTG_FS_IRQn);                                                  // Disable USB interrupts
                RingBuffer_Comm.RemainPack--;
                RingBuffer_Comm.DealPtr++;
                if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                {
                    RingBuffer_Comm.DealPtr = 0;
                }
                NVIC_EnableIRQ(OTG_FS_IRQn);                                                   // Enable USB interrupts
            }
        }
    }

    /* Monitor whether the remaining space is available for further downloads */
    if(RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART))
    {
        if(RingBuffer_Comm.StopFlag)
        {
            printf("USB ring buffer full, stop receiving further data.\n");
            RingBuffer_Comm.StopFlag = 0;
            USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
        }
    }

}

/*********************************************************************
 * @fn      HID_Set_Report_Deal
 *
 * @brief   print hid set report data
 *
 * @return  none
 */
void HID_Set_Report_Deal()
{
    uint16_t i;
    if (HID_Set_Report_Flag == SET_REPORT_WAIT_DEAL)
    {
        printf("Set Report:\n");
        for (i = 0; i < 64; ++i)
        {
            printf("%02x ",HID_Report_Buffer[i]);
        }
        printf("\n");
        HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;
        USBOTG_FS->UEP0_TX_LEN  = 0;
        USBOTG_FS->UEP0_TX_CTRL =  USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;

    }
}




