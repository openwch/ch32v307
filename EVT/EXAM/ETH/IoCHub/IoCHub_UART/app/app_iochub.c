#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "app_iochub.h"
#include "debug.h"
#include "app_net.h"
#include "app_uart.h"
#include "lwrb.h"

u8 IocHub_VER[3] = {0};
u8 IocHub_DeviceID[8] = {0};
u8 IocHub_NATType;
u8 IocHub_NETType;

static vu32 iochubLocalTime = 0;

IoCHubHANDLE sessionHdl;

#if LAN_SCAN_FUNC_ENABLE
uint8_t lanDeviceListMaxNum = IOC_LAN_DEVICE_NUM;
IOCHUB_LAN_DEVICE_INFO lanDeviceInf[IOC_LAN_DEVICE_NUM] = {0};
#endif

u8 IoCHubMemBuf[IOCHUB_MEMP_SIZE]; 

u8 localDeviceID[8]      = {0};
u8 logindeviceSecret[16] = {0};

u8 wchIochubSerIp[4] = {58, 213, 74, 190};
u16 wchIochubSerPort = 38089;
u16 wchIochubScrPort = 24560;

u8 SessionYzCode[8] = "12345678";
u8 desDeviceID[8] = {0};

u8 USART_RX_BUF[USART_REC_LEN];     
u8 USART_TX_BUF[50] = {0};

u16 USART_RX_STA=0;

volatile uint8_t sendflg = 0;
volatile uint8_t sessionopenflag = 0;
extern lwrb_t pRbUartToIoCHub[COMNUM];
void Uart1Write(char *fmt, ...);
char *AT_GetParam(char *buf, char sep);

void WCHIOCHUB_TimeIsr(void)
{
    iochubLocalTime++;
}

uint32_t WCHIOCHUB_GetTimeIsr(void)
{
    return iochubLocalTime;
}

void WCHIOCHUB_regCallback(u16 state)
{
      printf("login state: %u\r\n", state);

      switch (state)
      {
    	  case IOCHUB_SUCCESS:
    	  case IOCHUB_REGISTERED:
          {
              WCHIOCHUB_GetLocalID(IocHub_DeviceID);
              printf("DeviceID: %.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x\r\n",
                    IocHub_DeviceID[0], IocHub_DeviceID[1], IocHub_DeviceID[2], IocHub_DeviceID[3],
                    IocHub_DeviceID[4], IocHub_DeviceID[5], IocHub_DeviceID[6], IocHub_DeviceID[7]);
          }
          break;

    	  case IOCHUB_LOGIN_FAIL:
          {

          }
          break;
          default:
              break;
      }

      return;
}

uint8_t session_exit_flag = 0;
void WCHIOCHUB_sessionStateCallBackP (IoCHubHANDLE pDev, u16 states) {

      printf ("IoCHubHANDLE:%u,states:%u\n", pDev, states);
      switch (states) {
		case IOCHUB_SUCCESS: {
			static uint16_t transtype = 0XFFFF;
			uint8_t nodeid[8] = {0};
			uint8_t nodeIP[4] = {0};
			if (transtype != WCHIOCHUB_GetTransferType (pDev)) {
				transtype = WCHIOCHUB_GetTransferType (pDev);
				WCHIOCHUB_GetNodeID (pDev, nodeid);
				WCHIOCHUB_GetNodeIP (pDev, nodeIP);
				printf ("nodeid: %.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x\r\n",
						nodeid[0], nodeid[1], nodeid[2], nodeid[3],
						nodeid[4], nodeid[5], nodeid[6], nodeid[7]);
				printf ("nodeIP: %d.%d.%d.%d\r\n",
						nodeIP[0], nodeIP[1], nodeIP[2], nodeIP[3]);
				printf ("IoCHub_TransferType: %u\r\n", transtype);
				if (transtype == IOCHUB_SESSION_NOT_EXIT) {
					sessionopenflag &= (~(1 << pDev)) & 0xff;
				} else {
					if (!(sessionopenflag & (1 << pDev))) {
						sessionopenflag |= (1 << pDev);
					}
					
				}
			}
		} break;
		case IOCHUB_SESSION_EXIT: {
            if(session_exit_flag<2)
            {
                session_exit_flag++;
            }
            else 
            {
                session_exit_flag=0;
                WCHIOCHUB_CloseSession(pDev);
                printf("Session has been closed.\r\n");
            }
        }break;
		default:
				break;
      }
      return;
}

#if LAN_SCAN_FUNC_ENABLE
void WCHIOCHUB_lanScanCallBackP(uint8_t num,IOCHUB_LAN_DEVICE_INFO*lanDeviceList)
{
	printf("%u >> %02x%02x%02x%02x%02x%02x%02x%02x->%u:%u:%u:%u\r\n",num,
		  lanDeviceList->DevID[0],lanDeviceList->DevID[1],lanDeviceList->DevID[2],lanDeviceList->DevID[3],
		  lanDeviceList->DevID[4],lanDeviceList->DevID[5],lanDeviceList->DevID[6],lanDeviceList->DevID[7],
		  lanDeviceList->LanIP[0],lanDeviceList->LanIP[1],lanDeviceList->LanIP[2],lanDeviceList->LanIP[3]);
}
#endif

void IoCHub_Init (void) {
        IOCHUB_CONFIG Cfg = {0};
        uint8_t maxConnNum = MAX_CONNECT_NUM;    

	if(maxConnNum >DEFAULT_MAX_CONNECT_NUM)
    	maxConnNum = DEFAULT_MAX_CONNECT_NUM;
    Cfg.maxConnNum = maxConnNum;
    Cfg.frameSize = IOCHUB_FRAME_SIZE;
    Cfg.poolBufSize = IOCHUB_POOL_BUF_SIZE;
	Cfg.wndSize = IOCHUB_WND_SIZE;
	Cfg.wndTimeOut = IOCHUB_WND_TIMEOUT;
	Cfg.recvBufSize = IOCHUB_RECV_BUF_SIZE;
    Cfg.IoCHubBuf = IoCHubMemBuf;
    Cfg.pTcpSendF = TCP_Send;
    Cfg.pUdpSendF = UDP_Send;
    Cfg.pIoChubGetTimeIsr = WCHIOCHUB_GetTimeIsr;
    Cfg.pTcpConnectF = WCHNET_CreatTcpSocket;
    Cfg.pUdpConnectF = WCHNET_CreatUdpSocket;
    Cfg.pConnectCloseF = WCHNET_SocketClose;
    Cfg.pSessionStateCB = WCHIOCHUB_sessionStateCallBackP;
    Cfg.chipType  = E_CH32V30x_D8C;

    WCHIOCHUB_Init (&Cfg);
    WCHIOCHUB_GetSDKVer(IocHub_VER);
    printf("IOCHUB_VER: V%u.%u.%u\r\n", IocHub_VER[0], IocHub_VER[1], IocHub_VER[2]);

    // WCHIOCHUB_SetAccessMode(1,SessionYzCode);
}

void WCHIOCHUB_StartEn (void)
{
	if(!WCHIOCHUB_GetDeviceSerState())
    WCHIOCHUB_Start (wchIochubSerIp,
                     wchIochubSerPort,
                     wchIochubScrPort,
                     localDeviceID, 
                     logindeviceSecret,
                     0x00,
                     0x01,
                     WCHIOCHUB_regCallback); 
}

u8 CmpMemory(u8 *ptr1, u8 *ptr2, u8 cmdlen)
{
    while (cmdlen--)
    {
        if (*ptr1++ != *ptr2++) 
        {
            return 0;
        }
    }
    return 1; 
}

/*
 * Return the parameter that follows the first `sep` in `buf`, or NULL when the
 * separator is missing or nothing follows it. The AT port is an external
 * interface, so a malformed frame must not be able to walk off the buffer.
 */
char *AT_GetParam(char *buf, char sep)
{
    char *p = strchr(buf, sep);

    if (p == NULL)
    {
        return NULL;
    }
    p++;
    if (*p == '\0')
    {
        return NULL;
    }
    return p;
}

void Uart1Action(u8 *buf, u8 len)
{
	u8 i = 0;

	u8 cmd0[] = "IOCAT+SDKVER";
	u8 cmd1[] = "IOCAT+NATTYPE";
	u8 cmd2[] = "IOCAT+TRANSTYPE=";
	u8 cmd3[] = "IOCAT+START";
	u8 cmd4[] = "IOCAT+STOP";
	u8 cmd5[] = "IOCAT+LOCALID";
	u8 cmd6[] = "IOCAT+ACCESSMODE=";
	u8 cmd7[] = "IOCAT+ACCESSMODE";
	u8 cmd8[] = "IOCAT+Open=";
	u8 cmd9[] = "IOCAT+Close=";
	u8 cmd10[] = "IOCAT+DATA=";
	u8 cmd11[] = "IOCAT+DATA";
#if LAN_SCAN_FUNC_ENABLE
	u8 cmd12[] ="IOCAT+SETLANSCAN=";
	u8 cmd13[] ="IOCAT+LANSCAN=";
#endif
	u8 cmd14[] ="IOCAT+SENDT=";
	u8 cmdLen[] = {

		sizeof(cmd0) - 1,
		sizeof(cmd1) - 1,
		sizeof(cmd2) - 1,
		sizeof(cmd3) - 1,
		sizeof(cmd4) - 1,
		sizeof(cmd5) - 1,
		sizeof(cmd6) - 1,
		sizeof(cmd7) - 1,
		sizeof(cmd8) - 1,
		sizeof(cmd9) - 1,
		sizeof(cmd10) - 1,
		sizeof(cmd11) - 1,
#if LAN_SCAN_FUNC_ENABLE
		sizeof(cmd12) - 1,
		sizeof(cmd13) - 1,
#endif
	sizeof(cmd14) - 1,
	};
	u8 *cmdPtr[15];

	cmdPtr[0] = &cmd0[0];
	cmdPtr[1] = &cmd1[0];
	cmdPtr[2] = &cmd2[0];
	cmdPtr[3] = &cmd3[0];
	cmdPtr[4] = &cmd4[0];
	cmdPtr[5] = &cmd5[0];
	cmdPtr[6] = &cmd6[0];
	cmdPtr[7] = &cmd7[0];
	cmdPtr[8] = &cmd8[0];
	cmdPtr[9] = &cmd9[0];
	cmdPtr[10] = &cmd10[0];
	cmdPtr[11] = &cmd11[0];
#if LAN_SCAN_FUNC_ENABLE
	cmdPtr[12] = &cmd12[0];
	cmdPtr[13] = &cmd13[0];
#endif
	cmdPtr[14] = &cmd14[0];
	for (i = 0; i < sizeof(cmdLen); i++) 
	{
		if (len >= cmdLen[i]) 
		{
			if (CmpMemory(buf, cmdPtr[i], cmdLen[i])) 
			{
				break; 
			}
		}
	}

	switch (i) 
	{
		case 0: //IOCAT+SDKVER
		{
			Uart1Write("IOCAT+SDKVER_OK\r\n");
			WCHIOCHUB_GetSDKVer(IocHub_VER);
			Uart1Write("SDKVER: V%u.%u.%u\r\n", IocHub_VER[0], IocHub_VER[1], IocHub_VER[2]);
		}
			break;
		case 1: //IOCAT+NATTYPE
		{
			Uart1Write("IOCAT+NATTYPE_OK\r\n");
			printf("NATTYPE: %x\r\n", WCHIOCHUB_GetNATType());
		}
			break;
		case 2: //IOCAT+TRANSTYPE=0
		{
			char* pDev = AT_GetParam((char*)buf, '=');
			if (pDev == NULL)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+TRANSTYPE_OK\r\n");
			 Uart1Write("sessionHdl:%c,TransferType: %u\r\n",*pDev, WCHIOCHUB_GetTransferType(*pDev-'0'));
		}
			break;
		case 3: //IOCAT+START
		{
			Uart1Write("IOCAT+START_OK\r\n");
				if(!WCHIOCHUB_GetDeviceSerState())
				WCHIOCHUB_Start(wchIochubSerIp,
								wchIochubSerPort,
								wchIochubScrPort,
								localDeviceID,
								logindeviceSecret,
								0x00,
								0x01,
								WCHIOCHUB_regCallback);
		}
			break;
		case 4: //IOCAT+STOP
		{
			Uart1Write("IOCAT+STOP_OK\r\n");
			if(WCHIOCHUB_GetDeviceSerState())
				WCHIOCHUB_Stop();
		}
			break;
		case 5: //IOCAT+LOCALID
		{
			uint8_t nodeID[8] = {0};
			Uart1Write("IOCAT+LOCALID_OK\r\n");
			WCHIOCHUB_GetLocalID(nodeID);
			Uart1Write("nodeID = %02x%02x%02x%02x%02x%02x%02x%02x\r\n",
					   nodeID[0],nodeID[1],nodeID[2],nodeID[3],nodeID[4],nodeID[5],nodeID[6],nodeID[7]);
		}
			break;
		case 6: //IOCAT+ACCESSMODE=1,a2345678
		{
			char* accessMode = AT_GetParam((char*)buf, '=');
			char* YzCode = AT_GetParam((char*)buf, ',');
			if ((accessMode == NULL) || (YzCode == NULL))
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+ACCESSMODE=_OK\r\n");
			*accessMode-= '0';
			WCHIOCHUB_SetAccessMode(*accessMode,(uint8_t*)YzCode);
		}
			break;
		case 7://IOCAT+ACCESSMODE
		{
			Uart1Write("IOCAT+ACCESSMODE_OK\r\n");
			uint8_t accessMode;
			uint8_t YzCode[9]={0};
			WCHIOCHUB_GetAccessMode(&accessMode,YzCode);
			Uart1Write("accessMode = %u\r\n",accessMode);
			Uart1Write("YzCode = %s\r\n",YzCode);
		}
			break;
		case 8://IOCAT+Open=72725dd1070ed332,12345678
		{
			uint8_t hNodeid[8] = {0};
			IoCHubHANDLE pDev;
			char* aNodeid = AT_GetParam((char*)buf, '=');
			char* YzCode = AT_GetParam((char*)buf, ',');
			if ((aNodeid == NULL) || (YzCode == NULL))
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			/*iochub_ascii2Hex reads 16 characters, they all have to sit in
			 * front of the comma so that the read stays inside the buffer.*/
			if ((YzCode - aNodeid) < 17)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+Open=OK\r\n");
			iochub_ascii2Hex((uint8_t*)aNodeid, 16, hNodeid);
			WCHIOCHUB_OpenSession(&pDev, hNodeid, (uint8_t*)YzCode);
			Uart1Write("sessionHdl = %u\r\n",pDev);
		}
			break;
		case 9://IOCAT+Close=
		{
			char* pDev = AT_GetParam((char*)buf, '=');
			if (pDev == NULL)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+Close=OK\r\n");
			*pDev -= '0';
			Uart1Write("pDev:%d\r\n",*pDev );
			WCHIOCHUB_CloseSession(*pDev);
		}
		break;
		case 10://IOCAT+DATA=
		{
			char* pDev = AT_GetParam((char*)buf, '=');
			char* sendData = AT_GetParam((char*)buf, ',');
			uint16_t len;
			if ((pDev == NULL) || (sendData == NULL))
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			len = strlen(sendData);
			Uart1Write("IOCAT+DATA=_OK\r\n");
			*pDev -= '0';
			WCHIOCHUB_DataSend(*pDev,(uint8_t*)sendData, &len);
		}
		break;
		case 11://IOCAT+DATA
		{
			Uart1Write("IOCAT+DATA_OK\r\n");

		}
		break;
#if LAN_SCAN_FUNC_ENABLE
		case 12://IOCAT+SETLANSCAN=1;
		{
			char* pValue = AT_GetParam((char*)buf, '=');
			if (pValue == NULL)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+SETLANSCAN=_OK\r\n");
			*pValue -= '0';
			WCHIOCHUB_SetLanScan(WCHIOCHUB_lanScanCallBackP,lanDeviceInf,lanDeviceListMaxNum,*pValue);
		}
		break;
		case 13://IOCAT+LANSCAN=10;
		{
			char* scantime = strtok((char*)buf, "=");
			scantime = strtok(NULL, "=");
			if (scantime == NULL)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+LANSCAN=_OK\r\n");
			*scantime = (uint8_t)atoi(scantime);
			WCHIOCHUB_LanScan((uint8_t)*scantime);
		}
		break;
#endif
		case 14://IOCAT+SENDT=;
		{
			char* pValue = AT_GetParam((char*)buf, '=');
			if (pValue == NULL)
			{
				Uart1Write("IOCAT+PARAM_ERR\r\n");
				break;
			}
			Uart1Write("IOCAT+SENDT=_OK\r\n");
			*pValue -= '0';
			sendflg = *pValue;
		}
		break;
    default:
        break;
    }
}


void ClearUSART_RX_BUF()
{
	u8 i = 0;
	for (i=0; i<USART_REC_LEN; i++)
	{
		USART_RX_BUF[i] = 0;
	}
}

void Uart1Write(char *fmt, ...)
{
    u16 i;
    va_list ap;
    va_start(ap, fmt);
    /*vsnprintf instead of vsprintf: an over long log line must be truncated,
     *never allowed to run past USART_TX_BUF.*/
    i = (u16)vsnprintf((char *)USART_TX_BUF, sizeof(USART_TX_BUF), fmt, ap);
    va_end(ap);
    if (i > (sizeof(USART_TX_BUF) - 1))
    {
        i = sizeof(USART_TX_BUF) - 1;
    }
	

    for (u16 j = 0; j < i; j++)                 
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
        USART_SendData(USART1, USART_TX_BUF[j]);
    }

}

u8 Uart1Read(u8 *buf, u8 len)
{
	u8 i;
	u16 cntRxd = 0;

	USART_RX_STA &= 0x3FFF;
	cntRxd = USART_RX_STA;
	if (len > (u8) cntRxd)
	{                  	   
		len = (u8) cntRxd;
	}

	for (i = 0; i < len; i++)
	{
		buf[ i ] = USART_RX_BUF[ i ];
	}

	return (len);
}

void AT_Process(void)
{
	u8 buf[41] = {0};                       /*one byte is kept for the terminator*/
	u8 len = 0;

	/*The RX interrupt is masked while the frame is taken over: USART1_IRQHandler
	 * updates USART_RX_STA/USART_RX_BUF and must not clear them mid-frame.*/
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	if ((USART_RX_STA & 0x8000))
	{
		len = Uart1Read(buf, sizeof(buf) - 1);
		ClearUSART_RX_BUF( );
		USART_RX_STA = 0;
	}
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	if (len)
	{
		Uart1Action(buf, len);
	}
}

__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART1);

		if((USART_RX_STA&0x8000)==0)
		{
			if(USART_RX_STA&0x4000)
			{
				if(Res!=0x0a)USART_RX_STA=0;
				else USART_RX_STA|=0x8000;
			}
			else
			{
				if(Res==0x0d) USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;
				}
			}
		}
	}
}


