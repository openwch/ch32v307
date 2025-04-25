#include "debug.h"
#include "uac10_headphone.h"
#include "ch32v30x_usbfs_device.h"

uac_headphone_unit_t uac_headphone_unit =
{
    .feature_unit.mute = 0,
    .feature_unit.volume_l = 0x0100,
    .feature_unit.volume_r = 0x0100,
};

typedef struct __PACKED
{
    uint16_t header;
    uint16_t length;
} data_pack_t, *p_data_pack_t;

typedef struct
{
    volatile uint8_t tx_flag;
} uart_tx_t;

uart_tx_t uart_tx;
uint8_t UART2_Tx_Buffer[DEF_UART2_TX_BUF_SIZE];

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

void UART2_Tx_Init(void)
{
    USART_InitTypeDef USART_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_SetBits(GPIOA, GPIO_Pin_2);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_Cmd(USART2, DISABLE);

    USART_InitStructure.USART_BaudRate = 4500000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);

    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    DMA_Tx_Init(DMA1_Channel7, (uint32_t)&USART2->DATAR, (uint32_t)Data_Buffer, DEF_RING_BUFFER_PACK_SIZE);
    uart_tx.tx_flag = 0;
}

void UAC_UART_Tx_Handle(void)
{
    p_data_pack_t p_data;

    if(uart_tx.tx_flag)
    {
        if(USART2->STATR & USART_FLAG_TC)
        {
            USART2->STATR = (uint16_t)(~USART_FLAG_TC);
            USART2->CTLR3 &= (~USART_DMAReq_Tx);
            uart_tx.tx_flag = 0x00;
        }
    }
    else
    {
        if(RingBuffer_Comm.RemainPack)
        {
            p_data = (p_data_pack_t)&Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_RING_BUFFER_PACK_SIZE];
            p_data->header = 0x785A;
            p_data->length = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];

            USART_ClearFlag(USART2, USART_FLAG_TC);
            DMA1_Channel7->CFGR &= (uint16_t)(~DMA_CFGR1_EN);
            DMA1_Channel7->MADDR = (uint32_t)p_data;
            DMA1_Channel7->CNTR = p_data->length + 4;
            DMA1_Channel7->CFGR |= DMA_CFGR1_EN;
            USART2->CTLR3 |= USART_DMAReq_Tx;

            uart_tx.tx_flag = 1;

            NVIC_DisableIRQ(USBFS_IRQn);
            RingBuffer_Comm.RemainPack--;
            RingBuffer_Comm.DealPtr++;
            if(RingBuffer_Comm.DealPtr == DEF_RING_BUFFER_MAX_BLKS)
            {
                RingBuffer_Comm.DealPtr = 0;
            }
            NVIC_EnableIRQ(USBFS_IRQn);
        }
    }
}