/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/09
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "iap.h"
#include "string.h"
#include "flash.h"
#include "core_riscv.h"

/******************************************************************************/

iapfun jump2app;
u32 Program_addr = FLASH_Base;
u32 Verify_addr = FLASH_Base;
u32 User_APP_Addr_offset = 0x5000;
u8 Verify_Star_flag = 0;
u8 Fast_Program_Buf[390];
u32 CodeLen = 0;
u8 End_Flag = 0;
u8 EP2_Rx_Buffer[USBD_DATA_SIZE+4];
#define  isp_cmd_t   ((isp_cmd  *)EP2_Rx_Buffer)

/*********************************************************************
 * @fn      RecData_Deal
 *
 * @brief   USB deal data
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SUCCESS - SUCCESS
 *          ERR_End - End
 */
u8 RecData_Deal(void)
{
    u8 i, s, Lenth;

    Lenth = isp_cmd_t->other.buf[1];

    switch ( isp_cmd_t->other.buf[0]) {
    case CMD_IAP_ERASE:
        FLASH_Unlock_Fast();
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_PROM:
        for (i = 0; i < Lenth; i++) {
            Fast_Program_Buf[CodeLen + i] = isp_cmd_t->program.data[i];
        }
        CodeLen += Lenth;
        if (CodeLen >= 256) {
            FLASH_Unlock_Fast();
            FLASH_ErasePage_Fast(Program_addr);
            CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
            CodeLen -= 256;
            for (i = 0; i < CodeLen; i++) {
                Fast_Program_Buf[i] = Fast_Program_Buf[256 + i];
            }

            Program_addr += 0x100;

        }
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_VERIFY:

        if (Verify_Star_flag == 0) 
        {
        Verify_Star_flag = 1;
          if(CodeLen != 0)
          {
            for (i = 0; i < (256 - CodeLen); i++) 
            {
                Fast_Program_Buf[CodeLen + i] = 0xff;
            }
            FLASH_ErasePage_Fast(Program_addr);
            CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
            CodeLen = 0;
          }
        }  
        s = ERR_SUCCESS;
        for (i = 0; i < Lenth; i++) {
            if (isp_cmd_t->verify.data[i] != *(u8*) (Verify_addr + i)) {
                s = ERR_ERROR;
                break;
            }
        }

        Verify_addr += Lenth;
        break;

    case CMD_IAP_END:
        Verify_Star_flag = 0;
        End_Flag = 1;
        Program_addr = FLASH_Base;
        Verify_addr = FLASH_Base;
        s = ERR_End;

        FLASH_ErasePage_Fast(CalAddr & 0xFFFFFF00);
        FLASH->CTLR |= ((uint32_t)0x00008000);
        FLASH->CTLR |= ((uint32_t)0x00000080);

        break;
    case CMD_JUMP_IAP:

        s = ERR_SUCCESS;
        break;
    default:
        s = ERR_ERROR;
        break;
    }

    return s;
}

/*********************************************************************
 * @fn      UART_RecData_Deal
 *
 * @brief   UART deal data
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SUCCESS - SUCCESS
 *          ERR_End - End
 */
u8 UART_RecData_Deal(void)
{
    u8 i, s, Lenth;

    Lenth = isp_cmd_t->UART.Len;
    switch ( isp_cmd_t->UART.Cmd) {
    case CMD_IAP_ERASE:

        FLASH_Unlock_Fast();
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_PROM:
        for (i = 0; i < Lenth; i++) {
            Fast_Program_Buf[CodeLen + i] = isp_cmd_t->UART.data[i];
        }
        CodeLen += Lenth;
        if (CodeLen >= 256) {
            FLASH_Unlock_Fast();
            FLASH_ErasePage_Fast(Program_addr);
            CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
            CodeLen -= 256;
            for (i = 0; i < CodeLen; i++) {
                Fast_Program_Buf[i] = Fast_Program_Buf[256 + i];
            }

            Program_addr += 0x100;

        }
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_VERIFY:

        if (Verify_Star_flag == 0)
        {
        Verify_Star_flag = 1;
          if(CodeLen != 0)
          {
            for (i = 0; i < (256 - CodeLen); i++)
            {
                Fast_Program_Buf[CodeLen + i] = 0xff;
            }
            FLASH_ErasePage_Fast(Program_addr);
            CH32_IAP_Program(Program_addr, (u32*) Fast_Program_Buf);
            CodeLen = 0;
          }
        }
        s = ERR_SUCCESS;
        for (i = 0; i < Lenth; i++) {
            if (isp_cmd_t->UART.data[i] != *(u8*) (Verify_addr + i)) {
                s = ERR_ERROR;
                break;
            }
        }

        Verify_addr += Lenth;
        break;

    case CMD_IAP_END:
        Verify_Star_flag = 0;
        End_Flag = 1;
        Program_addr = FLASH_Base;
        Verify_addr = FLASH_Base;
        s = ERR_End;
        FLASH_ErasePage_Fast(CalAddr & 0xFFFFFF00);
        FLASH->CTLR |= ((uint32_t)0x00008000);
        FLASH->CTLR |= ((uint32_t)0x00000080);

        break;

    case CMD_JUMP_IAP:

        s = ERR_SUCCESS;
        break;
    default:
        s = ERR_ERROR;
        break;
    }

    return s;
}

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      PA0_Check
 *
 * @brief   Check PA0 state
 *
 * @return  1 - IAP
 *          0 - APP
 */
u8 PA0_Check(void)
{
    u8 i, cnt=0;

    GPIO_Cfg_init();

    for(i=0; i<10; i++){
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0) cnt++;
        Delay_Ms(5);
    }

    if(cnt>6) return 0;
    else return 1;
}

/*********************************************************************
 * @fn      USART3_CFG
 *
 * @brief   baudrate:UART3 baudrate
 *
 * @return  none
 */
void USART3_CFG(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init( USART3, &USART_InitStructure );
    USART_Cmd( USART3, ENABLE );
}

/*********************************************************************
 * @fn      UART3_SendMultiyData
 *
 * @brief   Deal device Endpoint 1 OUT.
 *
 * @param   l: Data length.
 *
 * @return  none
 */
void UART3_SendMultiyData(u8* pbuf, u8 num)
{
    u8 i = 0;

    while(i<num)
    {
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        USART_SendData(USART3, pbuf[i]);
        i++;
    }
}

/*********************************************************************
 * @fn      UART3_SendMultiyData
 *
 * @brief   USART3 send date
 *
 * @param   pbuf - Packet to be sent
 *          num - the number of date
 *
 * @return  none
 */
void UART3_SendData(u8 data)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, data);
}

/*********************************************************************
 * @fn      Uart3_Rx
 *
 * @brief   Uart1 receive date
 *
 * @return  none
 */
u8 Uart3_Rx(void)
{
    while( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData( USART3);
}

/*********************************************************************
 * @fn      UART_Rx_Deal
 *
 * @brief   UART Rx data deal
 *
 * @return  none
 */
void UART_Rx_Deal(void)
{
    u8 i, s;
    u16 Data_add = 0;

    if (Uart3_Rx() == Uart_Sync_Head1)
    {
        if (Uart3_Rx() == Uart_Sync_Head2)
        {
            isp_cmd_t->UART.Cmd = Uart3_Rx();
            Data_add += isp_cmd_t->UART.Cmd;
            isp_cmd_t->UART.Len = Uart3_Rx();
            Data_add += isp_cmd_t->UART.Len;

            if(isp_cmd_t->UART.Cmd == CMD_IAP_ERASE ||isp_cmd_t->UART.Cmd == CMD_IAP_VERIFY)
            {
                isp_cmd_t->other.buf[2] = Uart3_Rx();
                Data_add += isp_cmd_t->other.buf[2];
                isp_cmd_t->other.buf[3] = Uart3_Rx();
                Data_add += isp_cmd_t->other.buf[3];
                isp_cmd_t->other.buf[4] = Uart3_Rx();
                Data_add += isp_cmd_t->other.buf[4];
                isp_cmd_t->other.buf[5] = Uart3_Rx();
                Data_add += isp_cmd_t->other.buf[5];
            }
            if ((isp_cmd_t->other.buf[0] == CMD_IAP_PROM) || (isp_cmd_t->other.buf[0] == CMD_IAP_VERIFY))
            {
                for (i = 0; i < isp_cmd_t->UART.Len; i++) {
                    isp_cmd_t->UART.data[i] = Uart3_Rx();
                    Data_add += isp_cmd_t->UART.data[i];
                }
            }
            if (Uart3_Rx() == (uint8_t)(Data_add & 0xFF))
            {
                if(Uart3_Rx() == (uint8_t)(Data_add >>8))
                {
                    if (Uart3_Rx() == Uart_Sync_Head2)
                    {
                        if (Uart3_Rx() == Uart_Sync_Head1)
                        {
                            s = UART_RecData_Deal();

                            if (s != ERR_End)
                            {
                                UART3_SendData(Uart_Sync_Head1);
                                UART3_SendData(Uart_Sync_Head2);
                                UART3_SendData(0x00);
                                if (s == ERR_ERROR)
                                {
                                    UART3_SendData(0x01);
                                }
                                else
                                {
                                    UART3_SendData(0x00);
                                }
                                UART3_SendData(Uart_Sync_Head2);
                                UART3_SendData(Uart_Sync_Head1);
                            }
                        }
                    }
                }
            }
        }
    }
}
