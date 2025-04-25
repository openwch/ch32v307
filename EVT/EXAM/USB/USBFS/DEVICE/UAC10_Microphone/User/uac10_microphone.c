#include "debug.h"
#include "uac10_microphone.h"
#include "ch32v30x_usbfs_device.h"

#define DEF_UART_RX_BUF_SIZE            (2048)

uint8_t UART2_Rx_Buffer[DEF_UART_RX_BUF_SIZE];

typedef struct __PACKED
{
    uint8_t header1;
    uint8_t header2;
    uint16_t length;
    uint8_t data[DEF_USBD_EP3_FS_SIZE];
} data_pack_t, *p_data_pack_t;

typedef struct
{
    volatile uint32_t dma_cur_cnt;
    volatile uint32_t dma_last_cnt;
    volatile uint32_t remain_len;
    volatile uint32_t deal_ptr;
    volatile uint8_t data_valid;
} uart_rx_t;

volatile uart_rx_t uart_rx;

uac_microphone_unit_t uac_microphone_unit =
{
    .feature_unit.mute = 0,
    .feature_unit.volume_l = 0x0100,
    .feature_unit.volume_r = 0x0100,
};

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the I2S3 DMA Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

void UART2_Rx_Init()
{
    USART_InitTypeDef USART_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 4500000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);
    USART_ClearFlag(USART2, USART_FLAG_TC);
    USART_Cmd(USART2, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    DMA_Rx_Init(DMA1_Channel6, (uint32_t)&USART2->DATAR, (uint32_t)UART2_Rx_Buffer, DEF_UART_RX_BUF_SIZE);
    DMA_Cmd(DMA1_Channel6, ENABLE);
    memset((uint8_t *)&uart_rx, 0, sizeof(uart_rx_t));
}

void UAC_UART_Rx_Handle()
{
    uint32_t len, payload_start, buf_remain_len;
    uint8_t f_next_byte = 0;
    uint8_t header1, header2;

    if(uart_rx.data_valid)
    {
        uart_rx.dma_cur_cnt = DMA1_Channel6->CNTR;
        if(uart_rx.dma_cur_cnt != uart_rx.dma_last_cnt)
        {
            if(uart_rx.dma_last_cnt > uart_rx.dma_cur_cnt)
            {
                len = uart_rx.dma_last_cnt - uart_rx.dma_cur_cnt;
            }
            else
            {
                len = DEF_UART_RX_BUF_SIZE - uart_rx.dma_cur_cnt + uart_rx.dma_last_cnt;
            }
            uart_rx.dma_last_cnt = uart_rx.dma_cur_cnt;

            if((uart_rx.remain_len + len) > DEF_UART_RX_BUF_SIZE)
            {
                // printf("Overflow\n");
            }
            else
            {
                uart_rx.remain_len += len;
            }
        }

        if(uart_rx.remain_len >= 4)
        {
            header1 = UART2_Rx_Buffer[uart_rx.deal_ptr];
            if(header1 == 0x5A)
            {
                header2 = UART2_Rx_Buffer[(uart_rx.deal_ptr + 1) % DEF_UART_RX_BUF_SIZE];
                if(header2 == 0x78)
                {
                    len = UART2_Rx_Buffer[(uart_rx.deal_ptr + 2) % DEF_UART_RX_BUF_SIZE];
                    len |= (UART2_Rx_Buffer[(uart_rx.deal_ptr + 3) % DEF_UART_RX_BUF_SIZE] << 8);

                    if(uart_rx.remain_len >= (len + 4))
                    {
                        if((USBFSD->UEP3_TX_CTRL & USBFS_UEP_T_RES_MASK) == USBFS_UEP_T_RES_NAK)
                        {
                            if(len > 0 && len <= DEF_USBD_EP3_FS_SIZE)
                            {
                                payload_start = (uart_rx.deal_ptr + 4) % DEF_UART_RX_BUF_SIZE;
                                buf_remain_len = DEF_UART_RX_BUF_SIZE - payload_start;
                                if(len <= buf_remain_len)
                                {
                                    memcpy(USBFS_EP3_Buf, &UART2_Rx_Buffer[payload_start], len);
                                }
                                else
                                {
                                    memcpy(USBFS_EP3_Buf, &UART2_Rx_Buffer[payload_start], buf_remain_len);
                                    memcpy(USBFS_EP3_Buf + buf_remain_len, UART2_Rx_Buffer, len - buf_remain_len);
                                }
                                USBFSD->UEP3_TX_LEN = len;
                                USBFSD->UEP3_DMA = (uint32_t)USBFS_EP3_Buf;
                                USBFSD->UEP3_TX_CTRL = (USBFSD->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NONE;
                            }

                            uart_rx.remain_len -= (len + 4);
                            uart_rx.deal_ptr += (len + 4);
                            if(uart_rx.deal_ptr >= DEF_UART_RX_BUF_SIZE)
                            {
                                uart_rx.deal_ptr -= DEF_UART_RX_BUF_SIZE;
                            }
                        }
                    }
                }
                else
                {
                    f_next_byte = 1;
                }
            }
            else
            {
                f_next_byte = 1;
            }
        }

        if(f_next_byte)
        {
            uart_rx.remain_len--;
            uart_rx.deal_ptr++;
            if(uart_rx.deal_ptr >= DEF_UART_RX_BUF_SIZE)
            {
                uart_rx.deal_ptr = 0;
            }
        }
    }
    else
    {
        if(DMA1_Channel6->CNTR != DEF_UART_RX_BUF_SIZE)
        {
            uart_rx.data_valid = 1;
        }
        else
        {
            USBFSD->UEP3_TX_CTRL = (USBFSD->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NONE;
        }
    }
}

void UAC_Stream_Receive_Ctrl(uint8_t s)
{
    memset(USBFS_EP3_Buf, 0, DEF_USBD_EP3_FS_SIZE);
    if(s)
    {
        UART2_Rx_Init();
    }
    else
    {
        USART_Cmd(USART2, DISABLE);
        DMA_Cmd(DMA1_Channel6, DISABLE);
        USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);
    }
}