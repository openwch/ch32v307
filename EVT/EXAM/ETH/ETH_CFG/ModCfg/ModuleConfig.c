/********************************** (C) COPYRIGHT *******************************
* File Name          : ModuleConfig.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/06/10
* Description        : Module configuration related functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "ModuleConfig.h"
#include "wchnet.h"

/*********************************************************************
 * @fn      CFG_ERASE
 *
 * @brief   erase Data-Flash block, minimal block is 256B
 *
 * @param   Page_Address - the address of the page being erased.
 *          Length - Erased data length
 *
 * @return  none
 */
void CFG_ERASE(uint32_t Page_Address, u32 Length) {
    u32 NbrOfPage, EraseCounter;

    FLASH_Unlock_Fast();
    NbrOfPage = Length / FLASH_PAGE_SIZE;

    for (EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++) {
        FLASH_ErasePage_Fast( Page_Address + (FLASH_PAGE_SIZE * EraseCounter));
    }
    FLASH_Lock_Fast();
}

/*********************************************************************
 * @fn      CFG_WRITE
 *
 * @brief   write Data-Flash data block
 *
 * @param   StartAddr - the address of the page being written.
 *          Buffer - data buff
 *          Length - written data length
 *
 * @return  FLASH_Status
 */
FLASH_Status CFG_WRITE( u32 StartAddr, u8 *Buffer, u32 Length )
{
    u32 address = StartAddr;
    u32 *p_buff = (u32 *)Buffer;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    FLASH_Unlock();
    while((address < (StartAddr + Length)) && (FLASHStatus == FLASH_COMPLETE))
    {
        FLASHStatus = FLASH_ProgramWord(address, *p_buff);
        address += 4;
        p_buff++;
    }
    FLASH_Lock();
    return FLASHStatus;
}

/*********************************************************************
 * @fn      CFG_READ
 *
 * @brief   read Data-Flash data block
 *
 * @param   StartAddr - the address of the page being read.
 *          Buffer - data buff
 *          Length - read data length
 *
 * @return  none
 */
void CFG_READ( u32 StartAddr, u8 *Buffer, u32 Length )
{
    u32 address = StartAddr;
    u32 *p_buff = (u32 *)Buffer;

    while(address < (StartAddr + Length))
    {
        *p_buff = (*(u32 *)address);
        address += 4;
        p_buff++;
    }
}

/*********************************************************************
 * @fn      ParseConfigbuf
 *
 * @brief   Parse Configuration command
 *
 * @param   buf    - data buff.
 *          buflen - number of valid bytes in buf (length actually received)
 *          sockid - socket the datagram arrived on, used for the answer
 *
 * @return  0 - the datagram has been handled, nothing else to do.
 *          1 - the new configuration is in effect, the main loop has to
 *              reset the module so that it is taken into account.
 */
u8 ParseConfigbuf(u8 *buf, u32 buflen, u8 sockid)
{
    u16 i;
    u32 len;
    int n;
    pnet_comm net = (pnet_comm)buf;
    char name[sizeof(CFG->module_name) + 1];

    if((buf == NULL) || (buflen < sizeof(net_comm))) return 0;

    i = memcmp(net->flag,NET_MODULE_FLAG,sizeof(NET_MODULE_FLAG));
    if(i) return 0;
    switch(net->cmd)
    {
        case NET_MODULE_CMD_SEARCH :                                             //search module
            memcpy(net->id,MACAddr,6);
            net->cmd = NET_MODULE_ACK_SEARCH ;
            memcpy(name, CFG->module_name, sizeof(CFG->module_name));
            name[sizeof(CFG->module_name)] = '\0';

            n = snprintf((char *)net->dat, sizeof(net->dat), "%d.%d.%d.%d",
                         (u16)CFG->src_ip[0], (u16)CFG->src_ip[1],
                         (u16)CFG->src_ip[2], (u16)CFG->src_ip[3]);
            if((n < 0) || ((u32)n + 1 >= sizeof(net->dat))) return 0;
            net->len = (u8)n;
            n = snprintf((char *)&net->dat[net->len + 1],
                         sizeof(net->dat) - net->len - 1, "%s", name);
            if(n < 0) return 0;
            net->len = (u8)(net->len + n + 1);
            len = sizeof(net_comm);
            WCHNET_SocketUdpSendTo(sockid,buf,&len,brocastIp,brocastPort);
            printf("**********CMD_SEARCH**********\r\n");
            return 0;

        case NET_MODULE_CMD_SET :                                                //Configure module parameters
            i = memcmp(net->id,MACAddr,6);
            if(i) return 0;
            net->cmd = NET_MODULE_ACK_SET ;
            net->len = 0;
            memcpy(Configbuf,net->dat,MODULE_CFG_LEN);
            CFG->cfg_flag[0]=checkcode1;
            CFG->cfg_flag[1]=checkcode2;
            CFG_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
            if(CFG_WRITE(PAGE_WRITE_START_ADDR,Configbuf,MODULE_CFG_LEN ) != FLASH_COMPLETE)
                printf("write module cfg failed\r\n");       
            len = sizeof(net_comm);
            WCHNET_SocketUdpSendTo(sockid,buf,&len,brocastIp,brocastPort);
            printf("**********CMD_SET*************\r\n");
            return 1;

        case NET_MODULE_CMD_GET :                                                //Get module configuration
            i = memcmp(net->id,MACAddr,6);
            if(i) return 0;
            net->cmd  = NET_MODULE_ACK_GET ;
            /*When returning the configuration information
            * to the host computer, remove the last two
            * bytes of the verification code*/
            net->len = (MODULE_CFG_LEN-2);
            memcpy(net->dat,Configbuf,(MODULE_CFG_LEN-2))  ;
            len = sizeof(net_comm);
            WCHNET_SocketUdpSendTo(sockid,buf,&len,brocastIp,brocastPort);
            printf("************CMD_GET***********\r\n");
            return 0;

        case NET_MODULE_CMD_RESET :                                              //reset module
            i = memcmp(net->id,MACAddr,6);
            if(i) return 0;
            CFG_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
            if(CFG_WRITE(PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN ) != FLASH_COMPLETE)
                printf("restore module cfg failed\r\n");
            net->cmd = NET_MODULE_ACK_RESET;
            len = sizeof(net_comm);
            WCHNET_SocketUdpSendTo(sockid,buf,&len,brocastIp,brocastPort);
            printf("**********CMD_RESET***********\r\n");
            return 1;
        default:
            return 0 ;
    }
}
