/********************************** (C) COPYRIGHT *********************************
* File Name          : HTTPS.H
* Author             : WCH
* Version            : V1.0
* Date               : 2022/05/16
* Description        : WCHNET库-HTTPS
**********************************************************************************/
#ifndef	__HTTPS_H__
#define	__HTTPS_H__
#include "debug.h"
#include "WCHNET.h"

/*存储配置信息的地址*/
#define PAGE_WRITE_START_ADDR     ((uint32_t)0x0803FC00) /* Start from 255K */
#define PAGE_WRITE_END_ADDR       ((uint32_t)0x08040000) /* End at 256K */
#define FLASH_PAGE_SIZE           256

#define BASIC_CFG_ADDR            ((uint32_t)0x0803FC00)
#define PORT_CFG_ADDR             ((uint32_t)0x0803FD00)
#define LOGIN_CFG_ADDR            ((uint32_t)0x0803FE00)
#define BASIC_CFG_LEN             40
#define PORT_CFG_LEN              40
#define LOGIN_CFG_LEN             40


#define MAX_URL_SIZE              128
#define HTTP_SERVER_PORT          80

/* HTTP请求方法 */
#define	METHOD_ERR		          0
#define	METHOD_GET		          1
#define	METHOD_HEAD		          2
#define	METHOD_POST		          3

/* HTTP请求类型 */
#define	PTYPE_ERR		          0
#define	PTYPE_HTML	              1
#define	PTYPE_PNG		          2
#define	PTYPE_CSS		          3
#define PTYPE_GIF                 4

/*WCHNET网页通信协议定义*/
#define MODE_TCPSERVER            0
#define MODE_TCPCLIENT            1

/* HTML Doc. for ERROR */
#define RES_HTMLHEAD_OK	"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"      /*HTML类型的响应报文，最后两个\r\n必须要有*/

#define RES_PNGHEAD_OK	"HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n"      /*PNG类型的响应报文，最后两个\r\n必须要有*/
  
#define RES_CSSHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n"       /*CSS类型的响应报文，最后两个\r\n必须要有*/

#define RES_GIFHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n"      /*GIF类型的响应报文，最后两个\r\n必须要有*/
 /*-----------------*/

typedef struct Basic_Cfg                        //基础设置配置参数
{
	u8 flag[2];                                 //配置信息验证码0x57,0xab
	u8 mac[6];
	u8 ip[4];
	u8 mask[4];
	u8 gateway[4];
} *Basic_Cfg;


typedef struct Port_Cfg                         //端口设置配置参数
{
    u8 flag[2];                                 //配置信息验证码0x57,0xab
    u8 mode;
    u8 src_port[2];
    u8 des_ip[4];
    u8 des_port[2];
} *Port_Cfg;


typedef struct Login_Cfg                        //登录设置配置参数
{
    u8  flag[2];                                //配置信息验证码0x57,0xab
    u8  user[10];
    u8  pass[10];
} *Login_Cfg;


typedef struct _st_http_request                 //浏览器请求信息
{
	char	METHOD;					
	char	TYPE;					
	char	URL[MAX_URL_SIZE];
}st_http_request;

typedef struct Para_Tab                         //配置信息参数表
{
	char *para;                                 //配置项名称
	char value[20];                             //配置项数值
}Parameter;

extern Basic_Cfg Basic_CfgBuf;

extern Login_Cfg Login_CfgBuf;

extern Port_Cfg  Port_CfgBuf;

extern st_http_request *http_request;

extern u8 basicbuf[BASIC_CFG_LEN];

extern u8 portbuf[PORT_CFG_LEN];

extern u8 loginbuf[LOGIN_CFG_LEN];

extern u8 Basic_Default[BASIC_CFG_LEN];

extern u8 Login_Default[LOGIN_CFG_LEN];

extern u8 Port_Default[PORT_CFG_LEN];

extern u8 httpweb[200] ;                        /*用来保存http响应报文*/

extern u8 RecvBuffer[];

extern u8 flag;

extern u8 socket;

extern void ParseHttpRequest(st_http_request *, char *);	

extern void ParseURLType(char *, char *);

extern void MakeHttpResponse(unsigned char *, char);			

extern char *GetURLName(char* url);

extern char *DataLocate(char *buf,char *name);

extern void copy_flash(const char *html, u32 len);

extern void Init_Para_Tab(void) ;

extern void Web_Server(void);

/*erase Data-Flash block, minimal block is 256B*/
extern void EEPROM_ERASE(u32 Page_Address, u32 Length );

/*write Data-Flash data block, return FLASH_Status*/
extern FLASH_Status EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

/*read Data-Flash data block */
extern void EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length );

#endif	
