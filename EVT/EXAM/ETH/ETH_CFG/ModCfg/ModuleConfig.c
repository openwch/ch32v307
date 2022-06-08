/********************************** (C) COPYRIGHT *********************************
* File Name          : ModuleConfig.c
* Author             : WCH
* Version            : V1.1
* Date               : 2020/05/09
* Description        : 模块配置文件
**********************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ModuleConfig.h"
#include "WCHNET.h"

/*erase Data-Flash block, minimal block is 256B, return SUCCESS if success*/
void EEPROM_ERASE(uint32_t Page_Address, u32 Length) {
    u32 NbrOfPage, EraseCounter;

    FLASH_Unlock_Fast();
    NbrOfPage = Length / FLASH_PAGE_SIZE;

    for (EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++) {
        FLASH_ErasePage_Fast( Page_Address + (FLASH_PAGE_SIZE * EraseCounter)); //Erase 256B
    }
    FLASH_Lock_Fast();
}

/*write Data-Flash data block, return FLASH_Status*/
FLASH_Status EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length )
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

/*read Data-Flash data block */
void EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length )
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

/*配置命令解析*/
u8 ParseConfigbuf(u8 *buf)
{   
    u16 i;
    u32 len;
    pnet_comm net = (pnet_comm)buf;                                                 //net通信结构体
 
   i = memcmp(net->flag,NET_MODULE_FLAG,sizeof(NET_MODULE_FLAG));                   //判断网络命令标识
   if(i) return 0;
   switch(net->cmd)
   {
     case  NET_MODULE_CMD_SEARCH :                                                  //搜寻模块命令
           memcpy(net->id,MACAddr,6);
           net->cmd = NET_MODULE_ACK_SEARCH ;
           sprintf((char *)net->dat,"%d.%d.%d.%d",(u16)CFG->src_ip[0],(u16)CFG->src_ip[1],(u16)CFG->src_ip[2],(u16)CFG->src_ip[3]);
           net->len = strlen((char *)net->dat);
           sprintf((char*)&net->dat[net->len + 1],"%s",CFG->module_name);
           net->len += strlen((char*)&net->dat[net->len + 1]) + 1;
           len = sizeof(net_comm);
           WCHNET_SocketUdpSendTo(0,buf,&len,brocastIp,brocastPort);
           printf("CMD_SEARCH********************\n");
           return 0;

     case  NET_MODULE_CMD_SET :                                                     //配置模块命令
           i = memcmp(net->id,MACAddr,6);
           if(i) return 0;
           net->cmd = NET_MODULE_ACK_SET ;
           net->len = 0;   
           memcpy(Configbuf,net->dat,MODULE_CFG_LEN);
           CFG->cfg_flag[0]=checkcode1;                                             //在结尾添上验证码
           CFG->cfg_flag[1]=checkcode2;
           EEPROM_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
           EEPROM_WRITE(PAGE_WRITE_START_ADDR,Configbuf,MODULE_CFG_LEN );
           len = sizeof(net_comm);
           WCHNET_SocketUdpSendTo(0,buf,&len,brocastIp,brocastPort);
           printf("CMD_SET***********************\n");
           return 1;

    case   NET_MODULE_CMD_GET :                                                     //获取模块命令
           i =memcmp(net->id,MACAddr,6); 
           if(i) return 0;
           net->cmd  = NET_MODULE_ACK_GET ;
           net->len = (MODULE_CFG_LEN-2);                                           //返回配置信息给上位机的时候，要将最后两字节的验证码去掉
           memcpy(net->dat,Configbuf,(MODULE_CFG_LEN-2))  ;
           len = sizeof(net_comm);
           WCHNET_SocketUdpSendTo(0,buf,&len,brocastIp,brocastPort);
           printf("CMD_GET***********************\n");
           return 0;
         
    case   NET_MODULE_CMD_RESET :                                                   //复位模块命令
           i =memcmp(net->id,MACAddr,6);
           if(i) return 0;     
           EEPROM_ERASE(PAGE_WRITE_START_ADDR,FLASH_PAGE_SIZE);
           EEPROM_WRITE(PAGE_WRITE_START_ADDR,Default_cfg,MODULE_CFG_LEN );
           net->cmd = NET_MODULE_ACK_RESET ;
           len = sizeof(net_comm);
           printf("CMD_RESET*********************\n");
           return 1;
   default:
           return 0 ;
   }

}
