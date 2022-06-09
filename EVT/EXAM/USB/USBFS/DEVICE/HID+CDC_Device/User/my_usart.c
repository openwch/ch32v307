#include  "my_usart.h"

void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USART2_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USART2_CFG(__uint32_t USART_BaudRate,int8_t USART_WordLength,int8_t USART_StopBits,int8_t USART_Parity)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    USART_InitTypeDef USART_InitStructure={0};
    NVIC_InitTypeDef  NVIC_InitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB , ENABLE);

  /* USART2 TX-->A.2   RX-->A.3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength;
    USART_InitStructure.USART_StopBits = USART_StopBits;
    USART_InitStructure.USART_Parity = USART_Parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    printf("USART_BaudRate=%d\n",USART_BaudRate);
    printf("USART_WordLength=%d\n",USART_WordLength);
    printf("USART_StopBits=%d\n",USART_StopBits);
    printf("USART_Parity=%d\n",USART_Parity);

    USART_Init(USART2, &USART_InitStructure);

    USART_ClearFlag(USART2,USART_FLAG_IDLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    USART_Cmd(USART2, ENABLE);

}

/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 global interrupt request.
 *
 * @return  none
 */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //空闲中断
    {
        USART_ClearFlag(USART2,USART_FLAG_IDLE);
        USART_ReceiveData(USART2);

        DMA_Cmd(DMA1_Channel6, DISABLE );
        rx_len = BufferLen - DMA_GetCurrDataCounter(DMA1_Channel6);//获得接收到的字节数
        copy_data(ReceiveBuffer, rx_len);
        rx_flag=1;

        myDMA_Enable( DMA1_Channel6 );
    }

    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET)//溢出错误标志
    {
        USART_ReceiveData(USART2);
    }
}
