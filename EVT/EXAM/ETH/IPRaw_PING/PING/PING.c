/********************************** (C) COPYRIGHT ******************************
 * File Name          : PING.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2022/05/27
 * Description        : WCHNET PING function
 *******************************************************************************/
/* 头文件包含 */
#include "PING.h"

/* 变量定义 */
u8 sendErrCnt;
u8 unreachCnt;
u8 timeoutCnt;
u8 sucRecCnt;
u8 sendBuf[100];
u8 ICMPCnt = 0;
u8 ICMPSeq = 0;
u8 ICMPSuc = 0;
u8 ICMPTmpVal = 0;
u32 timeCnt;
/**********************************************************************************
 * Function Name  : InitParameter
 * Description    : 变量初始化
 * Input          : None
 * Output         : None
 * Return         : None
 **********************************************************************************/
void InitParameter(void)
{
    unreachCnt = 0;
    timeoutCnt = 0;
    sucRecCnt = 0;
    ICMPSuc = ICMP_KEEP_NO;
}

/**********************************************************************************
 * Function Name  : InitPing
 * Description    : Ping初始化
 * Input          : None
 * Output         : None
 * Return         : None
 **********************************************************************************/
void InitPING(void)
{
    IcmpHeader head;
    u32 check_sum = 0;
    u8 i;

    ICMPCnt++;
    ICMPSeq += 1;
    head.i_type = ICMP_HEAD_TYPE;
    head.i_code = ICMP_HEAD_CODE;
    head.i_id = ICMP_HEAD_ID;
    head.i_seq = ICMP_HEAD_SEQ + ICMPSeq;
    memset(head.i_data, 0, sizeof(head.i_data));
    for(i = 0; i < ICMP_DATA_BYTES; i++) {
        if(i < 26)
            head.i_data[i] = i + 'a';
        else
            head.i_data[i] = i + 'a' - 26;
        if(i % 2 == 0)
            check_sum += head.i_data[i] << 8;
        else
            check_sum += head.i_data[i];
    }
    check_sum += head.i_type << 8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    head.i_cksum = check_sum >> 16;
    head.i_cksum += check_sum & 0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(sendBuf, 0, sizeof(sendBuf));
    memcpy(sendBuf, &head, sizeof(head));
    sendBuf[2] = head.i_cksum >> 8;
    sendBuf[3] = head.i_cksum & 0xff;
    sendBuf[4] = head.i_id >> 8;
    sendBuf[5] = head.i_id & 0xff;
    sendBuf[6] = head.i_seq >> 8;
    sendBuf[7] = head.i_seq & 0xff;
}

/**********************************************************************************
 * Function Name  : Respond_Ping
 * Description    : 初始化回应PING包的数据
 * Input          : None
 * Output         : None
 * Return         : None
 **********************************************************************************/
void Respond_PING(u8 *pDat)
{
    IcmpHeader head;
    u32 check_sum = 0;
    u8 i;

    head.i_type = ICMP_HEAD_REPLY;
    head.i_code = pDat[1];
    head.i_id = (pDat[4] << 8) + pDat[5];
    head.i_seq = (pDat[6] << 8) + pDat[7];

    check_sum += head.i_type << 8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    for (i = 0; i < 32; i++) {
        head.i_data[i] = pDat[i + 8];
        if (i % 2 == 0)
            check_sum += head.i_data[i] << 8;
        else
            check_sum += head.i_data[i];
    }
    head.i_cksum = check_sum >> 16;
    head.i_cksum += check_sum & 0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(sendBuf, 0, sizeof(sendBuf));
    memcpy(sendBuf, &head, sizeof(head));
    sendBuf[2] = head.i_cksum >> 8;
    sendBuf[3] = head.i_cksum & 0xff;
    sendBuf[4] = head.i_id >> 8;
    sendBuf[5] = head.i_id & 0xff;
    sendBuf[6] = head.i_seq >> 8;
    sendBuf[7] = head.i_seq & 0xff;
}

/**********************************************************************************
 * Function Name  : WCHNET_ICMPRecvData
 * Description    : 接收数据
 * Input          : None
 * Output         : None
 * Return         : None
 **********************************************************************************/
void WCHNET_ICMPRecvData(u32 len, u8 *pDat)
{
    u16 tmp = 0;

    ICMPTmpVal = ICMPSuc;
    ICMPSuc = 3;
    if (len == 40) {
        if (pDat[0] == ICMP_HEAD_REPLY) {
            if (pDat[1] == ICMP_HEAD_CODE) {
                tmp = pDat[4];
                tmp = tmp << 8;
                tmp += pDat[5];
                if (tmp == ICMP_HEAD_ID) {
                    tmp = pDat[6];
                    tmp = (tmp << 8);
                    tmp += pDat[7] - ICMPSeq;
                    if (tmp == ICMP_HEAD_SEQ) {
                        ICMPSuc = ICMP_RECV_SUC;
                    }
                }
            }
        }
        if (pDat[0] == ICMP_HEAD_TYPE) {
            if (pDat[1] == ICMP_HEAD_CODE) {
                Respond_PING(pDat);
                ICMPSuc = ICMP_REPLY;
            }
        }
    } else {
        if (pDat[0] == 3) {
            if (pDat[1] == 1) {
                ICMPSuc = ICMP_UNRECH;
            }
        }
    }
}

/*******************************************************************************
* Function Name  : WCHNET_PINGSendData
* Description    : 发送数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WCHNET_PINGSendData( u8 *PSend, u32 Len,u8 index )
{
    u32 length;
    u8 i,count=0;

    length = Len;
    count = 0;

    while(1){
        Len = length;
        i = WCHNET_SocketSend(index,PSend,&Len);                              /* 将MyBuf中的数据发送 */
        mStopIfError(i);                                                        /* 检查错误 */
        if(Len == 0){
            count++;
            if(count>2){
                return;
            }
        }
        length -= Len;                                                          /* 将总长度减去以及发送完毕的长度 */
        PSend += Len;                                                           /* 将缓冲区指针偏移 */
        if(length)continue;                                                     /* 如果数据未发送完毕，则继续发送 */
        break;                                                                  /* 发送完毕，退出 */
    }
    ICMPSuc++;
}

/**********************************************************************************
 * Function Name  : WCHNET_PINGCmd
 * Description    : 查询状态执行相应命令
 * Input          : None
 * Output         : None
 * Return         : None
 **********************************************************************************/
void WCHNET_PINGCmd(void)
{
    if (ICMPSuc < ICMP_KEEP_NO) {
        switch (ICMPSuc) {
        case ICMP_SOKE_CON:
            ICMPSuc = 1;
            timeCnt = 0;
            WCHNET_PINGSendData(sendBuf, 40, 0);
            printf("Ping %d.%d.%d.%d with %d bytes of data.\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3], (u16) ICMP_DATA_BYTES);
            break;
        case ICMP_SEND_ERR:
            if (sendErrCnt > 10) {
                ICMPSuc = ICMP_KEEP_NO;
                sendErrCnt = 0;
            }
            if (timeCnt > 250) {
                printf("send data fail!\n");
                timeCnt = 0;
                sendErrCnt++;
                WCHNET_PINGSendData(sendBuf, 40, 0);
            }
            break;
        case ICMP_SEND_SUC:
            if (timeCnt > 1000) {
                printf("Request timeout.\n");
                timeoutCnt++;
                if (ICMPCnt < 4) {
                    ICMPSuc = 1;
                    InitPING();
                    timeCnt = 0;
                    WCHNET_PINGSendData(sendBuf, 40, 0);
                } else {
                    printf(
                            "Ping statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\r\n",
                            (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                            (u16) DESIP[3], (u16) (4 - timeoutCnt),
                            (u16) timeoutCnt, (u16) (timeoutCnt * 25));
                    ICMPSuc = ICMP_KEEP_NO;
                }
            }
            break;
        case ICMP_RECV_ERR:                                               // error unknown
            printf("receive unknown data.\r\n");
            ICMPSuc = ICMP_KEEP_NO;
            break;
        case ICMP_RECV_SUC:                                               // success
            printf("Reply from %d.%d.%d.%d: bytes=%d time<4ms\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3], (u16) ICMP_DATA_BYTES);
            sucRecCnt++;
            if (ICMPCnt < 4) {
                ICMPSuc = 1;
                InitPING();
                timeCnt = 0;
                WCHNET_PINGSendData(sendBuf, 40, 0);
            } else {
                printf(
                        "Ping statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\r\n",
                        (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                        (u16) DESIP[3], (u16) sucRecCnt,
                        (u16) (4 - sucRecCnt),
                        (u16) ((4 - sucRecCnt) * 25));
                ICMPSuc = ICMP_KEEP_NO;
            }
            break;
        case ICMP_UNRECH:                                                         // unreachable
            printf("Reply from %d.%d.%d.%d: Destination host unreachable.\r\n",
                    (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                    (u16) DESIP[3]);
            unreachCnt++;
            if (ICMPCnt < 4) {
                ICMPSuc = 1;
                InitPING();
                timeCnt = 0;
                WCHNET_PINGSendData(sendBuf, 40, 0);
            } else {
                printf(
                        "Ping statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\r\n",
                        (u16) DESIP[0], (u16) DESIP[1], (u16) DESIP[2],
                        (u16) DESIP[3], (u16) unreachCnt,
                        (u16) (4 - unreachCnt),
                        (u16) ((4 - unreachCnt) * 25));
                ICMPSuc = ICMP_KEEP_NO;
            }
            break;
        case ICMP_REPLY:                                         // icmp ping 包
            WCHNET_PINGSendData(sendBuf, 40, 0);
            break;
        case ICMP_REPLY_SUC:                                     // icmp ping 包
            printf("Reply ping.\r\n");
            ICMPSuc = ICMPTmpVal;
            break;
        case ICMP_KEEP_NO:
            break;
        default:
            break;
        }
    }
}

/*********************************** endfile **********************************/
