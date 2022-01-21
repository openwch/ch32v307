/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/09/09
* Description        : Main program body.
*******************************************************************************/
/*
 * @note
 * 基于CH32V307实现8串口网络服务器
 * 默认波特率为115200
 * 网络处于TCP client模式，目的IP为192.168.1.100，目的端口号为5000
 *
 * uart1 <-> socket0  |  uart2 <-> socket1 | ... | uart8 <-> socket7
 *
 * 该例程中，CH32V307 的RAM配置为128KB，Flash配置为192KB（使用WCHISPTool工具进行配置）
 *
 *  默认使用串口1作为shell 调试，可自行关闭，用于串口服务器
 * */

#include "ch32v30x.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"
#include <board.h>
#include "bsp_uart.h"


#include <sys/socket.h>
#include "netdb.h"
/* Global typedef */

/* Global define */


/* Global Variable */
int sock[8];


rt_thread_t tid_eth_receive = RT_NULL;
rt_thread_t tid_uart_receive = RT_NULL;


void uart_tx(void)
{
	uint16_t read_buf = 0;

	for(uint8_t i=1;i<8;i++)
	{
		if(uart_data_t[i].uart_tx_dma_state == IDEL)
		{
			if(uart_data_t[i].tx_read != uart_data_t[i].tx_write)
			{
				read_buf = (uart_data_t[i].tx_read)%UART_TX_BUF_NUM;
				switch(i)
				{
					/*uart1*/
					case 0:
						/* update uart tx dma address */
						DMA1_Channel4->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];

						/* update uart tx dma length */
						DMA1_Channel4->CNTR = uart_data_t[i].TX_data_length[read_buf];

						/* set uart tx dma state busy */
						uart_data_t[i].uart_tx_dma_state = BUSY;

						/* enable uart tx dma */
						DMA_Cmd(DMA1_Channel4,ENABLE);
					break;

					/*uart2*/
					case 1:
						DMA1_Channel7->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA1_Channel7->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA1_Channel7,ENABLE);
					break;

					/*uart3*/
					case 2:
						DMA1_Channel2->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA1_Channel2->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA1_Channel2,ENABLE);
					break;

					/*uart4*/
					case 3:
						DMA2_Channel5->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel5->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel5,ENABLE);
					break;

					/*uart5*/
					case 4:
						DMA2_Channel4->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel4->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel4,ENABLE);
					break;

					/*uart6*/
					case 5:
						DMA2_Channel6->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel6->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel6,ENABLE);
					break;

					/*uart7*/
					case 6:
						DMA2_Channel8->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel8->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel8,ENABLE);
					break;

					/*uart8*/
					case 7:
						DMA2_Channel10->MADDR = (uint32_t) &uart_data_t[i].TX_buffer[read_buf];
						DMA2_Channel10->CNTR = uart_data_t[i].TX_data_length[read_buf];
						uart_data_t[i].uart_tx_dma_state = BUSY;
						DMA_Cmd(DMA2_Channel10,ENABLE);
					break;

					default:
						break;
				}
			}
		}

	}
}

void eth_receive_entry(void *parameter)
{
	int bytes_received;
	uint8_t write_buf = 0;

	struct timeval timeout={

		.tv_sec = 0,
		.tv_usec = 1000,
	};

    /* wait eth phy enable */
    rt_thread_delay(2000);

    struct sockaddr_in serveraddr;
	struct hostent *host;
	char *url= "192.168.1.100";
	int port = 5000;
	host = (struct hostent*)gethostbyname(url);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr = *((struct  in_addr *)host->h_addr);
	rt_memset(&(serveraddr.sin_zero), 0, sizeof(serveraddr.sin_zero));


	for(uint8_t i=0;i<8;i++)
	{
		if((sock[i]=socket(AF_INET,SOCK_STREAM,0)) == -1)
		{
			rt_kprintf("Socket error sock=%d\n",sock[i]);
			return;
		}

		if (connect(sock[i], (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) == -1)
		{
			rt_kprintf("Connect fail!\n");
			closesocket(sock[i]);
			return;
		}
		else
		{
			rt_kprintf("Connect sock_%d  successful\n",i);
			/* set socket receive timeout  --us */
			setsockopt(sock[i],SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
		}
		rt_thread_mdelay(300);
	}

	rt_thread_mdelay(3000);
	if (tid_uart_receive != RT_NULL)
	{
		/*tcp connect success, startup uart_receive thread*/
		rt_thread_startup(tid_uart_receive);
	}

	while(1)
	{
		/* if use uart1, i is initialized to 0*/
		for(uint8_t i=1;i<8;i++)
		{
			if(sock[i] >= 0)
			{
				/* receive buf not busy */
				if( (uart_data_t[i].tx_write - uart_data_t[i].tx_read) < UART_TX_BUF_NUM )
				{
					write_buf = (uart_data_t[i].tx_write)%UART_TX_BUF_NUM;

					bytes_received = recv(sock[i], &uart_data_t[i].TX_buffer[write_buf], ETH_RECEIVE_SIZE, 0);
					{
						if (bytes_received < 0)
						{
							/* receive timeout, continue the receiving of next socket */
							continue;
						}
						else if (bytes_received == 0)
						{
							closesocket(sock[i]);
							rt_kprintf("\n sock_%d received error,close the socket.\r\n",i);
							break;
						}
						else
						{
							/* eth receive data success */
							uart_data_t[i].TX_data_length[write_buf] = bytes_received;

							uart_data_t[i].tx_write++;
						}
					}
				}
				else {
					rt_kprintf("buf_%d eth receive buf busy\n",i);
				}
			}

		}
	}
}

void uart_receive_entry(void *parameter)
{
	uint16_t temp = 0;
	int ret = -1;

    uint16_t len = 0;
    uint16_t len_1 = 0;

	while(1)
	{
		uart_tx();

		/* if use uart1, i is initialized to 0*/
		for(uint8_t i=1;i<8;i++)
		{
			switch(i)
			{
			    /*uart1*/
				case 0:
					temp = DMA1_Channel5->CNTR;
				break;

				/*uart2*/
				case 1:
					temp = DMA1_Channel6->CNTR;
				break;

				/*uart3*/
				case 2:
					temp = DMA1_Channel3->CNTR;
				break;

				/*uart4*/
				case 3:
					temp = DMA2_Channel3->CNTR;
				break;

				/*uart5*/
				case 4:
					temp = DMA2_Channel2->CNTR;
				break;

				/*uart6*/
				case 5:
					temp = DMA2_Channel7->CNTR;
				break;

				/*uart7*/
				case 6:
					temp = DMA2_Channel9->CNTR;
				break;

				/*uart8*/
				case 7:
					temp = DMA2_Channel11->CNTR;
				break;
				default:
					break;
			}

			/* uart rx dma CNTR not equal to last, indicating that data is received*/
			if(temp!=uart_data_t[i].last_RX_DMA_length)
			{
				/* calculate the length of the received data */
				uart_data_t[i].rx_write += (uart_data_t[i].last_RX_DMA_length - temp) & (UART_RX_DMA_SIZE - 1);

				/* update last rx dma CNTR */
				uart_data_t[i].last_RX_DMA_length = temp;

				/*calculate the length of the RX_buffer */
				len = uart_data_t[i].rx_write - uart_data_t[i].rx_read;

				/* RX_buffer is full */
				if(len > UART_RX_DMA_SIZE)
				{
					rt_kprintf("uart%d RX DMA Buf is full \n",i);
					len = 0;
					uart_data_t[i].rx_read = 0;
					uart_data_t[i].rx_write = 0;
				}

				/* send data starting from rx_read to RX_buffer end */
				len_1 = MIN(len,UART_RX_DMA_SIZE-(uart_data_t[i].rx_read&(UART_RX_DMA_SIZE-1)));

	            if(len_1 != 0)
	            {
	            	ret = send(sock[i], &uart_data_t[i].RX_buffer[uart_data_t[i].rx_read&(UART_RX_DMA_SIZE-1)], len_1, 0);
					if (ret < 0)
					{
						closesocket(sock[i]);
						rt_kprintf("\n send error,close the socket.\r\n");
						continue;
					}
					else if (ret == 0)
					{
						rt_kprintf("\n Send warning,send function return 0.\r\n");
					}
					else
					{
						uart_data_t[i].rx_read += len_1;
					}
	            }

	            /* send the rest of the data(if any) at beginning of RX_buffer  */
	            if( (len-len_1) != 0)
	            {

	                ret = send(sock[i], &uart_data_t[i].RX_buffer[uart_data_t[i].rx_read&(UART_RX_DMA_SIZE-1)], (len-len_1), 0);
					if (ret < 0)
					{
						closesocket(sock[i]);
						rt_kprintf("\n send error,close the socket.\r\n");
						continue;
					}
					else if (ret == 0)
					{
						rt_kprintf("\n Send warning,send function return 0.\r\n");
					}
					else
					{
						uart_data_t[i].rx_read += (len-len_1);
					}
	            }
			}
		}
	}
}


int main(void)
{

    BSP_Uart_Init();  /* uart init */

    tid_eth_receive = rt_thread_create("eth_rev",
    		                            eth_receive_entry, RT_NULL,
    									2048,
    		                            8, 20);
	if (tid_eth_receive != RT_NULL)
	{
		rt_thread_startup(tid_eth_receive);
	}

	tid_uart_receive = rt_thread_create("uart_rev",
			                            uart_receive_entry, RT_NULL,
										1024,
										8, 10);

    return 0;
	while(1)
	{
	    rt_thread_mdelay(500);
	}
}




/**********************************************************************/
/* uart2  tx */
void DMA1_Channel7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel7_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA1_IT_TC7))
    {
        DMA_Cmd(DMA1_Channel7,DISABLE);
        uart_data_t[1].uart_tx_dma_state = IDEL;
        uart_data_t[1].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC7);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}




/**********************************************************************/
/* uart3  tx */
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel2_IRQHandler(void)
{


    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();
    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_Cmd(DMA1_Channel2,DISABLE);
        uart_data_t[2].uart_tx_dma_state = IDEL;
        uart_data_t[2].tx_read++;
        DMA_ClearITPendingBit(DMA1_IT_TC2);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}





/**********************************************************************/
/* uart4  tx */
void DMA2_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel5_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC5))
    {
        DMA_Cmd(DMA2_Channel5,DISABLE);
        uart_data_t[3].uart_tx_dma_state = IDEL;
        uart_data_t[3].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC5);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}




/**********************************************************************/
/* uart5  tx */
void DMA2_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel4_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC4))
    {
        DMA_Cmd(DMA2_Channel4,DISABLE);
        uart_data_t[4].uart_tx_dma_state = IDEL;
        uart_data_t[4].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC4);

        //rt_kprintf("uart4 TC DMA2_Channel5->CNTR: %d \n",DMA2_Channel5->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}



/**********************************************************************/
/* uart6  tx */
void DMA2_Channel6_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel6_IRQHandler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();


    if(DMA_GetITStatus(DMA2_IT_TC6))
    {
        DMA_Cmd(DMA2_Channel6,DISABLE);
        uart_data_t[5].uart_tx_dma_state = IDEL;
        uart_data_t[5].tx_read++;

        DMA_ClearITPendingBit(DMA2_IT_TC6);
        //rt_kprintf("uart6 TC DMA2_Channel6->CNTR: %d \n",DMA2_Channel6->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}




/**********************************************************************/
/* uart7  tx */
void DMA2_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel8_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC8))
    {
        DMA_Cmd(DMA2_Channel8,DISABLE);
        uart_data_t[6].uart_tx_dma_state = IDEL;
        uart_data_t[6].tx_read++;
        DMA_ClearITPendingBit(DMA2_IT_TC8);
        //rt_kprintf("uart7 TC DMA2_Channel8->CNTR: %d \n",DMA2_Channel8->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}




/**********************************************************************/
/* uart8  tx */
void DMA2_Channel10_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel10_IRQHandler(void)
{

    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();

    if(DMA_GetITStatus(DMA2_IT_TC10))
    {
        DMA_Cmd(DMA2_Channel10,DISABLE);
        uart_data_t[7].uart_tx_dma_state = IDEL;
        uart_data_t[7].tx_read++;

        DMA_ClearITPendingBit(DMA2_IT_TC10);

        //rt_kprintf("uart4 TC DMA2_Channel5->CNTR: %d \n",DMA2_Channel5->CNTR);
    }

    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();
}





