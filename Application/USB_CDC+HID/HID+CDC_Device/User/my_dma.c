#include "my_dma.h"

void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void Usart_Rx_Config()
{
    DMA_InitTypeDef  DMA_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//使能DMA传输
    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DATAR);//DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ReceiveBuffer;//DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;//传输方向，从外设到内存
    DMA_InitStructure.DMA_BufferSize = BufferLen;//DMA通道的缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//数据宽度为8
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//数据宽度为8
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//没有内存到内存
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_IT_TC6);
    DMA_ClearFlag(DMA1_IT_HT6);
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC|DMA_IT_HT, ENABLE);

    /*中断*/
    NVIC_InitStructure.NVIC_IRQChannel    = DMA1_Channel6_IRQn; /* UART2 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel6, ENABLE);
}


//重新恢复DMA指针
void myDMA_Enable( DMA_Channel_TypeDef*DMA_CHx )
{
    USART_ClearITPendingBit( USART2, USART_IT_IDLE );
    DMA_Cmd( DMA_CHx, DISABLE );                                                                                 //关闭DMA1所指示的通道
    DMA_SetCurrDataCounter( DMA_CHx, BufferLen );                                              //DMA通道的DMA缓存的大小
    DMA_Cmd( DMA_CHx, ENABLE );                                                                                      //DMA1所指示的通道
}
//发送len个字节
//buf:发送区首地址
//len:发送的字节数
void uart2_Send( u8 *buf, u16 len )
{
    u16 t;
    for( t = 0; t < len; t++ )                                                                                      //循环发送数据
    {
        while( USART_GetFlagStatus( USART2, USART_FLAG_TC ) == RESET );
        USART_SendData( USART2, buf[t] );
    }
    while( USART_GetFlagStatus( USART2, USART_FLAG_TC ) == RESET );
}


//备份接收到的数据
void copy_data( u8 *buf, u16 len )
{
    u16 t;
    Sendlen_backup = len;                                                                                                    //保存数据长度
    for( t = 0; t < len; t++ )
    {
        SendBuffer[t] = buf[t];                                                                                     //备份接收到的数据，防止在处理数据过程中接收到新数据，将旧数据覆盖掉。
        buf[t]=0;
    }
}

/**
 * @brief  DMA接收全满中断
 * @param
 * @retval
 */
void DMA1_Channel6_IRQHandler(void)
{

    if(DMA_GetFlagStatus(DMA1_FLAG_TC6)!= RESET)
    {
        DMA_ClearFlag(DMA1_FLAG_TC6);        //清除DMA挂起位
        DMA_ClearITPendingBit(DMA1_IT_TC6);  //清除中断标志位


           DMA_Cmd( DMA1_Channel6, DISABLE );                                                                                 //关闭DMA1所指示的通道
           DMA_SetCurrDataCounter( DMA1_Channel6, BufferLen );                                              //DMA通道的DMA缓存的大小
           DMA_Cmd( DMA1_Channel6, ENABLE );



    }
    if(DMA_GetFlagStatus(DMA1_FLAG_HT6)!= RESET)
    {
        DMA_ClearFlag(DMA1_FLAG_HT6);        //清除DMA挂起位
        DMA_ClearITPendingBit(DMA1_IT_HT6);  //清除中断标志位

        DMA_Cmd(DMA1_Channel6, DISABLE );
        rx_len = BufferLen - DMA_GetCurrDataCounter(DMA1_Channel6);//获得接收到的字节数
        copy_data(ReceiveBuffer, rx_len);
        rx_flag=1;

        myDMA_Enable( DMA1_Channel6 );

    }

}

