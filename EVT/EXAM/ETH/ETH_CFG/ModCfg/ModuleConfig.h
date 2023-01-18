/********************************** (C) COPYRIGHT *******************************
* File Name          : ModuleConfig.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/06/10
* Description        : Module configuration related command codes
*                      and configuration structures.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __MODULECONFIG_H__
#define __MODULECONFIG_H__

#include "debug.h"

/*Address where configuration information is stored*/
#define PAGE_WRITE_START_ADDR     ((uint32_t)0x0803FF00) /* Start from (256K - 256B) */
#define PAGE_WRITE_END_ADDR       ((uint32_t)0x08040000) /* End at 256K */
#define FLASH_PAGE_SIZE           256

#define NET_MODULE_DATA_LENGTH    60                     //Maximum length of the data area
#define MODULE_CFG_LEN            44                     //The length of configuration buff
                                                                                      
/*Communication command code*/
#define NET_MODULE_CMD_SET        0X01                   //Configure module parameters
#define NET_MODULE_CMD_GET        0X02                   //Get module configuration
#define NET_MODULE_CMD_RESET      0X03                   //reset module
#define NET_MODULE_CMD_SEARCH     0X04                   //search module
                                                                                      
/*ACK command code*/
#define NET_MODULE_ACK_SET        0X81                   //Respond to configuration command
#define NET_MODULE_ACK_GET        0X82                   //Response to get configure command
#define NET_MODULE_ACK_RESET      0X83                   //Respond to reset command
#define NET_MODULE_ACK_SEARCH     0X84                   //Respond to search commands
                                                                                      
#define NET_MODULE_FLAG           "WCHNET_MODULE"        //identify the host computer communication
#define checkcode1                0X11                   //Identity configuration information
#define checkcode2                0x22

/*communication structure*/
typedef struct NET_COMM {
    /*The communication identifier,
     *  because they are all communicated
     *  by broadcasting, a fixed value is
     *  added here.*/
    unsigned char flag[16];
    unsigned char cmd;                                     //command header
    unsigned char id[6];                                   //ID, WCHNET MAC address
    unsigned char len;                                     //data length
    unsigned char dat[NET_MODULE_DATA_LENGTH];             //data buffer
}net_comm,*pnet_comm;

/*Module Communication Mode Identification*/
#define  NET_MODULE_TYPE_TCP_S    0X00                   //TCP SERVER
#define  NET_MODULE_TYPE_TCP_C    0X01                   //TCP CLIENT
#define  NET_MODULE_TYPE_UDP_S    0X02                   //UDP SERVER
#define  NET_MODULE_TYPE_UDP_C    0X03                   //UDP CLIENT
#define  NET_MODULE_TYPE_NONE     0X04                   //no mode is enabled

/*Configuration structure*/
typedef struct MODULE_CFG {
  unsigned char module_name[21];       //module name
  unsigned char type;                  //The module is in default mode (no mode is enabled by default)
  unsigned char src_ip[4];             //The IP address of the module
  unsigned char mask[4];               //The subnet mask of the module
  unsigned char getway[4];             //The gateway address of the module
  unsigned char src_port[2];           //Module source port
  unsigned char dest_ip[4];            //destination IP address
  unsigned char dest_port[2];          //destination port
  unsigned char cfg_flag[2];           //Verification code, used to verify configuration information
}module_cfg,*pmodule_cfg;

extern u8 MACAddr[6];
extern u8  brocastIp[4];
extern u8 Configbuf[MODULE_CFG_LEN];
extern u8 Default_cfg[MODULE_CFG_LEN];
extern u16 brocastPort;
extern pmodule_cfg CFG;

extern u8 ParseConfigbuf(u8 *buf);

extern void CFG_ERASE(uint32_t Page_Address, u32 Length );

extern void CFG_READ( u32 StartAddr, u8 *Buffer, u32 Length );

extern FLASH_Status CFG_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

#endif
