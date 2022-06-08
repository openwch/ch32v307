/********************************** (C) COPYRIGHT *********************************
* File Name          : mail.h
* Author             : WCH
* Version            : V1.0
* Date               : 2020/05/07
* Description        : Define for mail.c
**********************************************************************************/
#ifndef __MAIL_H__
#define __MAIL_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "mailcmd.h"

/* ********************************************************************************************************************
* 工作类型
*
*  1  只发送邮件，发送完成后退出登陆并关闭socket连接；
*  2  只接收邮件，接收完成【获取邮件列表】后退出登陆并关闭socket连接；
*  3  只接收邮件，接收完成【读取邮件后删除】后退出登陆并关闭socket连接；
*  4  发送邮件且接受邮件，发送完成后开始接收邮件，接收完成后退出登陆;
*  5  发送邮件且接受邮件，接收完成后回复邮件，发送完成后发送退出登陆，删除邮件后退出收邮件登陆。
*  说明：此代码接收与发送用的两个独立的socket连接；
*         如果还需进行其他的操作，eg：发送完接收，接收完继续发送操作
*                                     只需在mail.c代码中的Check_Response（）
*                                     子程序中接收完成后，将命令号置为 SMTP_SEND_START；
***********************************************************************************************************************/
#ifndef    mail_work_mode
    #define  mail_work_mode             1
#endif
#if   (mail_work_mode == 1)
    #define send_mail                   1         // 发送邮件
    #define    send_over_quit           1         // 发送完成退出
#elif (mail_work_mode == 2)
    #define receive_mail                1         // 接收邮件
    #define    receive_over_quit        1         // 读取邮件后退出
#elif (mail_work_mode == 3)
    #define receive_mail                1         // 接收邮件
    #define    receive_dele_quit        1         // 读完邮件后删除并退出
#elif (mail_work_mode == 4)
    #define send_mail                   1         // 发送邮件
    #define send_over_receive           1         // 发送完接收
    #define receive_mail                0         // 接收邮件
    #define receive_over_quit           1         // 读取邮件后退出
#elif (mail_work_mode == 5)
    #define receive_mail                1         // 接收邮件
    #define receive_over_reply          1         // 读取邮件后回复邮件
    #define send_mail                   0         // 发送邮件
    #define send_over_quit              1         // 发送完退出
#endif

//================================================================================
extern  u8 OrderType;                             //    命令类型
extern  u8 CheckType;
extern  u8 ReceDatFlag;                           //    收到数据标志位
extern  u16 ReceLen;
//=================================================================================
#define attach_max_len           512              // 发送附件最大长度，默认为512
#define POP3_SERVER_PORT         110              // 收邮件目的端点号
#define SMTP_SERVER_PORT         25               // 发邮件目的端点号
#define DIALOG                    0               // 1 打印调试信息  0 关闭调试信息
#define POP3_LOG_EN               1               //将获取的文件在串口输出
typedef struct
{
    u8 Socket;
    u8 g_MIME;                                    // 有无附件标志位
    char  m_strFile[48];                          // 附件名字
    char  m_strSendFrom[48];                      // 发件人地址
    char  m_strSendTo[48];                        // 收件人地址
    char  m_strSMTPServer[32];                    // 服务器名称
    char  m_strUSERID[32];                        // 用户名
    char  m_strPASSWD[32];                        // 密码
    char  m_strSubject[32];                       // 主题
    char  m_strSenderName[32];                    // 发送人名字
}SMTP;
typedef struct
{
    u8 Socket;
    u8 EncodeType;                                // 邮件使用的编码方式
    u8 AnalyMailDat;                              // 分析邮件结束标志位
    u8 identitycheck;                             // 验证附件内容标志位
    u8 RefreshTime;                               // 没搜查到邮件继续搜索标志位
    u8 DiscnntFlag;
    u8 ReceFlag;
    char  sBufTime[40];                           // 保存接收邮件的时间
    char  Ccname[48];                             // 抄送的名字
    char  DecodeRName[32];                        // 保存接收到邮件解码后的发件人名字
    char  pPop3Server[32];                        // POP服务器
    char  pPop3UserName[32];                      // POP登陆用户名
    char  pPop3PassWd[32];                        // POP登陆密码
}POP;
/* smtp发送邮件相关参数 */
extern const char *m_Server;                      // 服务器名称
extern const char *m_UserName;                    // 用户名
extern const char *m_PassWord;                    // 密码
extern const char *m_SendFrom;                    // 发件人地址
extern const char *m_SendName;                    // 发送人名字
extern const char *m_SendTo;                      // 收件人地址
extern const char *m_Subject;                     // 主题
extern const char *m_FileName;                    // 附件名字(如果不发送附件,则置为"\0")
/* pop接收邮件相关 */
extern const char *p_Server;                      // POP服务器
extern const char *p_UserName;                    // POP登陆用户名
extern const char *p_PassWord;                    // POP登陆密码

extern u8 MACAddr[6];                             //MAC地址
extern char     AttachmentData[attach_max_len];
extern POP      *p_pop3;
extern SMTP     *p_smtp;
extern char     MailBodyData[128];

/* socketsmtp 相关定义,SMTP发送邮件    */                                          
extern const u8  SocketsmtpIP[4];                 // Socket smtp目的IP地址
extern const u16  SmtpSourPrt;                    // Socket smtp源端口号

/* socketpop3 相关定义, POP收邮件*/                                              
extern const u8  Socketpop3IP[4];                 // Socket pop3目的IP地址
extern const u16  Pop3SourPrt;                    // Socket pop3源端口号

/************************************************************************************************************************
* POP可选命令
************************************************************************************************************************/
#ifdef receive_mail 
    #define POP_RTER        1                     // 1 处理server邮件的全部文本
    #define POP_DELE        1                     // 2 标记删除
//    #define POP_RSET      1                     // 3 处理撤销所有的DELE命令
//    #define POP_TOP       1                     // 4 返回n号邮件的前m行内容
//    #define POP_UIDL      1                     // 5 处理server返回用于该指定邮件的唯一标识，如果没有指定，返回所有的。
    #define POP_REFRESH     1                     // 接收邮件过程中没收到邮件继续查询标志 
#endif

#define g_strBoundary    "18ac0781-9ae4-4a2a-b5f7-5479635efb6b"                      /* 边界 */
#define g_strEncode      "base64"                                                    /* 编码方式 */
#define g_strcharset     "gb2312"                                                    /* windows格式 (linux格式"utf-8") */
#define g_xMailer        "X-Mailer: X-WCH-Mail Client Sender\r\n"                    /* X-Mailer内容     */
#define g_Encoding       "Content-Transfer-Encoding: quoted-printable\r\nReply-To: " /* Encoding 内容 */
#define g_Custom         "X-Program: CSMTPMessageTester"                             /* X-Program内同，可修改 */
#define g_FormatMail     "This is a multi-part message in MIME format."              /* 邮件有多种内容，一般是有附件 */
#define g_AttachHead     "\r\nContent-Transfer-Encoding: quoted-printable\r\n" 
#define g_AttachHedType  "text/plain"
#define g_MailHedType    "multipart/mixed"

#ifdef    receive_over_reply
extern const    char *g_mailbody;
extern const    char *g_autograph1;
extern const    char *g_autograph2;
extern char     MacAddr[6];
extern char     MacAddrC[18];
#endif
/************************************************************************************************************************
* 命令代码 
* 0x01-0x0f 为接收邮件命令码
****************** 0x01-0x07 为常用命令
****************** 0x09-0x0e 为P可选命令
****************** 0x01-0x02-0x03-0x06 为确认状态
****************** 0x04-0x05 0x09-0x0e 为操作状态
* 0x10-0x1f 为发送邮件命令码
****************** 0x10-0x17 需按顺序执行
****************** 后面的为可选择操作
* 0x00         不做任何操作
************************************************************************************************************************/
#define COMMAND_UNUSEFULL       0x00
//POP RECEIVE CODE
#define POP_RECEIVE_USER        0x01    // 认证用户
#define POP_RECEIVE_PASS        0x02    // 认证密码
#define POP_RECEIVE_STAT        0x03    // 邮箱统计资料
#define POP_RECEIVE_LIST        0x04    // 返回指定邮件的大小
#define POP_RECEIVE_RTER        0x05    // 读取邮件的全部文本
#define POP_RECEIVE_QUIT        0x06    // 退出登陆
#define POP_CLOSE_SOCKET        0x07    // 关闭pop的socket
#define POP_RECEIVE_START       0x08    // 启动接收邮件
#define POP_RECEIVE_DELE        0x09    // 标记删除
#define POP_RECEIVE_RSET        0x0a    // 撤销所有的DELE命令
#define POP_RECEIVE_TOP         0x0b    // 处理返回n号邮件的前m行内容，m必须是自然数
#define POP_RECEIVE_UIDL        0x0e    // 返回用于该指定邮件的唯一标识
#define POP_ERR_CHECK           0x0F    // POP握手出错退出登陆并关闭socket
//SMTP SEND CODE
#define SMTP_SEND_HELO          0x10    // 发送hello命令
#define SMTP_SEND_AUTH          0x11    // 发送登陆命令
#define SMTP_SEND_USER          0x12    // 发送用户名
#define SMTP_SEND_PASS          0x13    // 发送密码
#define SMTP_SEND_MAIL          0x14    // 发送发送者地址
#define SMTP_SEND_RCPT          0x15    // 发送接收者地址
#define SMTP_SEND_DATA          0x16    // 发送发送数据命令
#define SMTP_DATA_OVER          0x17    // 发送邮件内容
#define SMTP_SEND_QUIT          0x18    // 退出登陆
#define SMTP_CLOSE_SOCKET       0x19    // 关闭smtp的socket
#define SMTP_SEND_START         0x1E    // 启动发送邮件
#define SMTP_ERR_CHECK          0x1F    // smtp握手出错退出登陆并关闭socket
/* *********************************************************************************************************************
* 握手信号核对错误代码
************************************************************************************************************************/
//POP ERR CHECK CODE
#define POP_ERR_CNNT            0x20     // pop连接错误--------------可能是端口号，服务器IP等设置错误
#define POP_ERR_USER            0x21     // 认证用户名检测错误-------用户名格式不对，或USER命令码错误（命令码错误建议查看发送的命令码核对是否正确，后同）
#define POP_ERR_PASS            0x22     // 登陆失败-----------------用户名或密码错误，或PASS命令码错误
#define POP_ERR_STAT            0x23     // 获取邮件信息错误---------STAT命令码错误，如出现失败重新登陆，如还是失败检查其他问题
#define POP_ERR_LIST            0x24     // 获取邮件信息列表---------LIST命令码错误
#define POP_ERR_RETR            0x25     // 读取指定邮件号信息错误---读取的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_DELE            0x26     // 删除某封邮件失败---------删除的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_QUIT            0x59     // 退出登陆失败-------------QUIT命令码错误
#define POP_ERR_RSET            0x28     // 恢复删除的邮件-----------RSET命令码错误（命令需在退出之前执行才能有效）
#define POP_ERR_TOP             0x2B     // 读取某封邮件前m行失败----读取的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_UIDL            0x2C     // 获取某封邮件ID失败-------邮件不存在，或UIDL命令码错误，或输入的参数不正确
#define POP_ERR_UNKW            0x2F     // pop错误未知--------------错误未知
//SMTP ERR CHECK CODE
#define SMTP_ERR_CNNT           0x31     // smtp连接错误-------------可能是端口号，服务器IP等设置错误
#define SMTP_ERR_HELO           0x32     // 发送helo命令失败---------HELO命令码错误，或者主机名格式错误
#define SMTP_ERR_AUTH           0x33     // 请求登陆命令失败---------AUTH命令码错误
#define SMTP_ERR_USER           0x34     // 发送用户名未识别---------用户名格式不对，或USER命令码错误
#define SMTP_ERR_PASS           0x35     // 登陆失败-----------------用户名或密码错误，或PASS命令码错误
#define SMTP_ERR_MAIL           0x36     // 发件人地址发送失败-------发件人地址不正确，或MAIL命令码错误
#define SMTP_ERR_RCPT           0x37     // 发送接收地址出错---------收件人地址不正确，或RCPT命令码错误
#define SMTP_ERR_DATA           0x38     // 请求发送数据-------------DATA命令码错误
#define SMTP_ERR_DATA_END       0x39     // 发送数据过程出错
#define SMTP_ERR_QUIT           0x3A     // 退出登陆失败
#define SMTP_ERR_UNKNOW         0x3F     // smtp错误未知
//
#define send_data_timeout       0x10     // 发送数据超时
#define send_data_success       0x14     // 发送数据超时
/*******************************************************************************
* 常用命令代码 
*******************************************************************************/
//POP3  CODE
#define OPEN_SOCKET_RECEIVE       0xA0    // 打开接收socket
#define LOGIN_MAILBOX_RECEIVE     0xA1    // 登陆接收邮箱
#define READ_MAIL_STAT            0xA2    // 读取邮件总列表
#define READ_MAIL_LIST            0xA3    // 读取邮件列表
#define READ_MAIL_MAILBODYDATA    0xA4    // 读取邮件正文内容
#define READ_MAIL_ATTACHMENT      0xA5    // 读取邮件附件内容
#define READ_MAIL_ADDRESS         0xA6    // 读取发件人地址
#define READ_MAIL_TIME            0xA7    // 读取发送时间
#define READ_MAIL_SUBJECT         0xA8    // 读取邮件主题
#define QUIT_RECEIVE_MAIL         0xA9    // 退出登陆并关闭socket
//SMTP  CODE
#define OPEN_SOCKET_SEND          0xB0    // 打开发送socket
#define LOGIN_MAILBOX_SEND        0xB1    // 登陆发送邮箱
#define SEND_MAIL_UNATTACH        0xB2    // 发送邮件不包含附件
#define SEND_MAIL_CONTATTACH      0xB3    // 发送邮件包含附件
#define QUIT_SEND_MAIL            0xB4    // 退出登陆并关闭socket
/*******************************************************************************
* 核对握手信息的错误类型 
* 成功返回CHECK_SUCCESS。并命令操作码置为下一条需操作的命令
* 错误返回对应的错误代码 
* 部分命令无握手信号，则不需要核对
*******************************************************************************/
#define CHECK_SUCCESS             0x00
#define uncheck                   0x00
//POP CHECK CODE
// 检查应答信息，正确返回“+0k” ，错误返回“-ERR”
#define POP_CHECK_CNNT            0x50    
#define POP_CHECK_USER            0x51    
#define POP_CHECK_PASS            0x52    
#define POP_CHECK_STAT            0x53
#define POP_CHECK_LIST            0x54    
#define POP_CHECK_RETR            0x55
#define POP_CHECK_DELE            0x56
#define POP_CHECK_SEAR            0x57
#define POP_CHECK_QUIT            0x58
#define POP_CHECK_RSET            0x59
#define POP_CHECK_APOP            0x5A
#define POP_CHECK_TOP             0x5B
#define POP_CHECK_UIDL            0x5C
//SMTP CHECK CODE
#define SMTP_CHECK_CNNT           0x60
#define SMTP_CHECK_HELO           0x61
#define SMTP_CHECK_AUTH           0x62
#define SMTP_CHECK_USER           0x63
#define SMTP_CHECK_PASS           0x64
#define SMTP_CHECK_MAIL           0x65
#define SMTP_CHECK_RCPT           0x66
#define SMTP_CHECK_DATA           0x67
#define SMTP_CHECK_DATA_END       0x68
#define SMTP_CHECK_QUIT           0x69
/******************************************************************************/
void WCHNET_SendData( char *PSend, u32 Len,u8 type,u8 index  );               /* 发送数据 */

u8 WCHNET_CheckResponse( char *recv_buff,u8 check_type );                     /* 检查应答信息 */

void WCHNET_CreatTcpPop3( void );                                             /* 创建pop3连接 */

void WCHNET_CreatTcpSmtp( void );                                             /* 创建smtp连接 */

void WCHNET_ReplyMailBody( void );                                            /* 回复邮件的正文内容 */

void WCHNET_MailQuery( void );

void mStopIfError(u8 iError);

void WCHNET_HandleGlobalInt( void );

//================================================================================
#endif
