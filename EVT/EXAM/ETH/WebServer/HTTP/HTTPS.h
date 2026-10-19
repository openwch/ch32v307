/********************************** (C) COPYRIGHT *******************************
 * File Name          : HTTPS.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/05/31
 * Description        : HTTP related parameters.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef	__HTTPS_H__
#define	__HTTPS_H__
#include "debug.h"
#include "wchnet.h"

/*Address where configuration
 * information is stored*/
#define PAGE_WRITE_START_ADDR     ((uint32_t)0x0803FC00) /* Start from 255K */
#define PAGE_WRITE_END_ADDR       ((uint32_t)0x08040000) /* End at 256K */
#define FLASH_PAGE_SIZE           256

#define BASIC_CFG_ADDR            ((uint32_t)0x0803FC00)
#define PORT_CFG_ADDR             ((uint32_t)0x0803FD00)
#define LOGIN_CFG_ADDR            ((uint32_t)0x0803FE00)
#define BASIC_CFG_LEN             (sizeof(Basic_Cfg_t))
#define PORT_CFG_LEN              (sizeof(Port_Cfg_t))
#define LOGIN_CFG_LEN             (sizeof(Login_Cfg_t))


#define MAX_URL_SIZE              32
#define HTTP_SERVER_PORT          80

/* HTTP request method*/
#define	METHOD_ERR		          0
#define	METHOD_GET		          1
#define	METHOD_HEAD		          2
#define	METHOD_POST		          3

/* HTTP request URL */
#define	PTYPE_ERR		          0
#define	PTYPE_HTML	              1
#define	PTYPE_PNG		          2
#define	PTYPE_CSS		          3
#define PTYPE_GIF                 4

/*WCHNET communication Mode*/
#define MODE_TCPSERVER            0
#define MODE_TCPCLIENT            1

/* HTML Doc. for ERROR */
#define RES_HTMLHEAD_OK "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:"

#define RES_PNGHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length:"
  
#define RES_CSSHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length:"

#define RES_GIFHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\nContent-Length:"

/*Answer sent when the requested resource type cannot be determined.
 *MakeHttpResponse used to leave its header pointer NULL in that case.*/
#define RES_HTMLHEAD_ERR "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length:"

#define RES_END "\r\n\r\n"


typedef struct Basic_Cfg                        //Basic configuration parameters
{
	u8 flag[2];                                 //Configuration information verification code: 0x57,0xab
	u8 mac[6];
	u8 ip[4];
	u8 mask[4];
	u8 gateway[4];
} Basic_Cfg_t;


typedef struct Port_Cfg                          //Port configuration parameters
{
    u8 flag[2];                                 //Configuration information verification code: 0x57,0xab
    u8 mode;
    u8 src_port[2];
    u8 des_ip[4];
    u8 des_port[2];
    u8 reserved;                                //keeps sizeof() a multiple of 4, see the assertion below
} Port_Cfg_t;


typedef struct Login_Cfg                         //Login configuration parameters
{
    u8  flag[2];                                 //Configuration information verification code: 0x57,0xab
    char  user[11];
    char  pass[11];
} Login_Cfg_t;

/*WEB_READ/WEB_WRITE move the configuration in and out of the flash in words,
 *so the size of every stored structure has to be a multiple of 4. PORT_CFG_LEN
 *used to be 11, which made both helpers touch one byte past the structure.*/
_Static_assert((sizeof(Basic_Cfg_t) % 4) == 0, "Basic_Cfg_t size must be a multiple of 4");
_Static_assert((sizeof(Port_Cfg_t) % 4) == 0, "Port_Cfg_t size must be a multiple of 4");
_Static_assert((sizeof(Login_Cfg_t) % 4) == 0, "Login_Cfg_t size must be a multiple of 4");


typedef struct _st_http_request                  //Browser request information
{
	char	METHOD;					
	char	TYPE;					
	char	URL[MAX_URL_SIZE];
}st_http_request;

typedef struct Para_Tab                         //Configuration information parameter table
{
	char *para;                                 //Configuration item name
	char value[30];                             //Configuration item value
}Parameter;

/*Word aligned because WEB_READ/WEB_WRITE move them in words.*/
extern __attribute__((aligned(4))) Basic_Cfg_t Basic_CfgBuf;

extern __attribute__((aligned(4))) Login_Cfg_t Login_CfgBuf;

extern __attribute__((aligned(4))) Port_Cfg_t  Port_CfgBuf;

extern st_http_request http_request;

extern __attribute__((aligned(4))) u8 Basic_Default[BASIC_CFG_LEN];

extern __attribute__((aligned(4))) u8 Login_Default[LOGIN_CFG_LEN];

extern __attribute__((aligned(4))) u8 Port_Default[PORT_CFG_LEN];

extern char httpweb[200] ;

extern u8 HTTPDataBuffer[];

extern u8 socket;

extern void ParseHttpRequest(st_http_request *, char *);	

extern void ParseURLType(char *, char *);

void MakeHttpResponse(char *buf, char type, u32 len );

extern char *DataLocate(char *buf,char *name);

extern void copy_flash(const char *html, u32 len);

extern void Init_Para_Tab(void) ;

extern void Web_Server(void);

extern void WEB_ERASE(u32 Page_Address, u32 Length );

extern FLASH_Status WEB_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

extern void WEB_READ( u32 StartAddr, u8 *Buffer, u32 Length );

#endif	
