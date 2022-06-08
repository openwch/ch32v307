
/********************************** (C) COPYRIGHT *********************************
* File Name          : Moduleconfig.H
* Description        :上位机通信数据结构定义
*                      
**********************************************************************************/
#ifndef __MODULECONFIG_H__
#define __MODULECONFIG_H__

#include "debug.h"

/*存储配置信息的地址*/
#define PAGE_WRITE_START_ADDR     ((uint32_t)0x0803FF00) /* Start from (256K - 256B) */
#define PAGE_WRITE_END_ADDR       ((uint32_t)0x08040000) /* End at 256K */
#define FLASH_PAGE_SIZE           256

#define NET_MODULE_DATA_LENGTH    60                     //与模块通信时数据区的最大长度
#define MODULE_CFG_LEN            44                     //配置buff长度
                                                                                      
//通信命令码                                                                          
#define NET_MODULE_CMD_SET        0X01                   //配置网络中的模块
#define NET_MODULE_CMD_GET        0X02                   //获取某个模块的配置
#define NET_MODULE_CMD_RESET      0X03                   //获取某个模块的配置
#define NET_MODULE_CMD_SEARCH     0X04                   //搜索网络中的模块
                                                                                      
//应答命令码                                                                          
#define NET_MODULE_ACK_SET        0X81                   //回应配置命令码
#define NET_MODULE_ACK_GET        0X82                   //回应获取命令码
#define NET_MODULE_ACK_RESET      0X83                   //获取某个模块的配置
#define NET_MODULE_ACK_SEARCH     0X84                   //回应所搜命令码
                                                                                      
#define NET_MODULE_FLAG           "WCHNET_MODULE"        //用来标识上位机通信
#define checkcode1                0X11                   //用来标识配置信息
#define checkcode2                0x22

//网络通信结构体
typedef struct NET_COMM {
  unsigned char flag[16];                                //通信标识，因为都是用广播方式进行通信的，所以这里加一个固定值
  unsigned char cmd;                                     //命令头
  unsigned char id[6];                                   //标识，CH563MAC地址
  unsigned char len;                                     //数据区长度
  unsigned char dat[NET_MODULE_DATA_LENGTH];             //数据区缓冲区
}net_comm,*pnet_comm;

//模块标识
#define  NET_MODULE_TYPE_TCP_S    0X00                   //模块作为TCP SERVER
#define  NET_MODULE_TYPE_TCP_C    0X01                   //模块作为TCP CLIENT
#define  NET_MODULE_TYPE_UDP_S    0X02                   //模块作为UDP SERVER
#define  NET_MODULE_TYPE_UDP_C    0X03                   //模块作为UDP CLIENT
#define  NET_MODULE_TYPE_NONE     0X04                   //模块的默认配置，不开启任何模式

//模块的配置结构
typedef struct MODULE_CFG {
  unsigned char module_name[21];                         //模块本身的IP地址
  unsigned char type;                                    //标识模块处于那模式(TCP/UDP server/client)
  unsigned char src_ip[4];                               //模块本身的IP地址
  unsigned char mask[4];                                 //模块本身的子网掩码
  unsigned char getway[4];                               //模块对应的网关地址
  unsigned char src_port[2];                             //模块源端口
  unsigned char dest_ip[4];                              //目的IP地址
  unsigned char dest_port[2];                            //目的端口
  unsigned char cfg_flag[2];                             //用于读取EEEPROM时验证配置信息
}module_cfg,*pmodule_cfg;

extern u8 MACAddr[6];
extern u8 Configbuf[MODULE_CFG_LEN];
extern u8  brocastIp[4];                                 /*广播IP地址，不可改*/
extern u16 brocastPort;                                  /*上位机通讯端口号*/

/*erase Data-Flash block, minimal block is 256B, return SUCCESS if success*/
extern void EEPROM_ERASE(uint32_t Page_Address, u32 Length );

/*write Data-Flash data block, return FLASH_Status*/
extern FLASH_Status EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

/*read Data-Flash data block */
extern void EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length );

extern u8 Default_cfg[MODULE_CFG_LEN];
extern pmodule_cfg CFG;
extern u8 ParseConfigbuf(u8 *buf);
#endif
