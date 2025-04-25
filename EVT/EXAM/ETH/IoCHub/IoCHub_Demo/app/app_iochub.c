#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "app_iochub.h"
#include "debug.h"
#include "app_net.h"

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

u8 IoCHubMemBuf[IOC_MEMP_SIZE]; 

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
volatile uint8_t speedflg = 0;

void Uart1Write(char *fmt, ...);

void WCHIOCHUB_TimeIsr(void)
{
    iochubLocalTime++;
}

uint32_t WCHIOCHUB_GetTimeIsr(void)
{
    return iochubLocalTime;
}

void WCHIOCHUB_dataRecvCallback(IoCHubHANDLE pDev, u8* databuf, u16 datalen)
{
    u8* recvdata = databuf;
	printf("sessionHdl:%u, TransferType:%u, len:%u\r\n",pDev,WCHIOCHUB_GetTransferType(pDev), datalen);
	printf("recvdata: ");
	while(datalen--)
	{
	    printf("%02x ",*recvdata++);
	}
	printf("\r\n");
    return;
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

void WCHIOCHUB_sessionStateCallBackP (IoCHubHANDLE pDev, u16 states) {
      printf ("IoCHubHANDLE:%u,states:%u\n", pDev, states);
      switch (states) {
      case IOCHUB_SUCCESS:
              printf ("IoCHub_TransferType: %u\r\n", WCHIOCHUB_GetTransferType (pDev));
              break;
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

void IoCHub_Init (void)
{
    IOCHUB_CONFIG Cfg = {0};
    uint8_t maxConnNum = CONNECT_NUM;    
	
	if(maxConnNum >MAX_CONNECT_NUM)
    	maxConnNum = MAX_CONNECT_NUM;
    Cfg.maxConnNum = maxConnNum;
    Cfg.sendDataSize = IOC_FRAME_SIZE;
    Cfg.ringBufLen = IOC_BUF_SIZE;
    Cfg.iochubBuf = IoCHubMemBuf;
    Cfg.pTcpSendF = TCP_Send;
    Cfg.pUdpSendF = UDP_Send;
    Cfg.pIoChubGetTimeIsr = WCHIOCHUB_GetTimeIsr;
    Cfg.pTcpConnectF = WCHNET_CreatTcpSocket;
    Cfg.pUdpConnectF = WCHNET_CreatUdpSocket;
    Cfg.pConnectCloseF = WCHNET_SocketClose;
    Cfg.pDataRecvCB = WCHIOCHUB_dataRecvCallback;
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
	};
	u8 *cmdPtr[14];

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
			Uart1Write("IOCAT+SDKVER_OK\r\n");
			 WCHIOCHUB_GetSDKVer(IocHub_VER);
			Uart1Write("SDKVER: V%u.%u.%u\r\n", IocHub_VER[0], IocHub_VER[1], IocHub_VER[2]);

			break;
		case 1: //IOCAT+NATTYPE
			Uart1Write("IOCAT+NATTYPE_OK\r\n");
			 printf("NATTYPE: %x\r\n", WCHIOCHUB_GetNATType());

			break;
		case 2: //IOCAT+TRANSTYPE=0
		{
			Uart1Write("IOCAT+TRANSTYPE_OK\r\n");
			char* pDev = strchr((char*)buf, '=')+1;
			 Uart1Write("sessionHdl:%c,TransferType: %u\r\n",*pDev, WCHIOCHUB_GetTransferType(*pDev-'0'));
		}
			break;
		case 3: //IOCAT+START
			Uart1Write("IOCAT+START_OK\r\n");
			{
				if(!WCHIOCHUB_GetDeviceSerState())
				WCHIOCHUB_Start(wchIochubSerIp,
								wchIochubSerPort,
								wchIochubScrPort,
								localDeviceID,
								logindeviceSecret,
								0x00,
								0x01,
								WCHIOCHUB_regCallback); /*Æô¶¯·þÎñ*/
			}
			break;
		case 4: //IOCAT+STOP
			Uart1Write("IOCAT+STOP_OK\r\n");
			if(WCHIOCHUB_GetDeviceSerState())
			WCHIOCHUB_Stop();

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
			Uart1Write("IOCAT+ACCESSMODE=_OK\r\n");
			char* accessMode = strchr((char*)buf, '=')+1;
			char* YzCode = strchr((char*)buf, ',')+1;
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
			Uart1Write("IOCAT+Open=OK\r\n");
			IoCHubHANDLE pDev;
			char* aNodeid = strchr((char*)buf, '=')+1;
			char* YzCode = strchr((char*)buf, ',')+1;
			iochub_ascii2Hex((uint8_t*)aNodeid, 16, hNodeid);
			WCHIOCHUB_OpenSession(&pDev, hNodeid, (uint8_t*)YzCode);
			Uart1Write("sessionHdl = %u\r\n",pDev);
		}
			break;
		case 9://IOCAT+Close=
		{
			Uart1Write("IOCAT+Close=OK\r\n");
			char* pDev = strchr((char*)buf, '=')+1;
			*pDev -= '0';
			Uart1Write("pDev:%d\r\n",*pDev );
			WCHIOCHUB_CloseSession(*pDev);
		}
		break;
		case 10://IOCAT+DATA=
		{
			Uart1Write("IOCAT+DATA=_OK\r\n");
			char* pDev = strchr((char*)buf, '=')+1;
			char* sendData = strchr((char*)buf, ',')+1;
			size_t len = strlen(sendData);
			*pDev -= '0';
			WCHIOCHUB_DataSend(*pDev,(uint8_t*)sendData, len);
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
			Uart1Write("IOCAT+SETLANSCAN=_OK\r\n");
			char* pValue = strchr((char*)buf, '=')+1;
			*pValue -= '0';
			WCHIOCHUB_SetLanScan(WCHIOCHUB_lanScanCallBackP,lanDeviceInf,lanDeviceListMaxNum,*pValue);
		}
		break;
		case 13://IOCAT+LANSCAN=10;
		{
			Uart1Write("IOCAT+LANSCAN=_OK\r\n");
			char* scantime = strtok((char*)buf, "=");
			scantime = strtok(NULL, "=");
			*scantime = (uint8_t)atoi(scantime);
			WCHIOCHUB_LanScan((uint8_t)*scantime);
		}
		break;
#endif
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
    vsprintf((char *)USART_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART_TX_BUF); 
	

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
	u8 buf[40] = {0};
	u8 len = 0;

	if ((USART_RX_STA & 0x8000))
	{
		len = Uart1Read(buf, sizeof(buf));
		Uart1Action(buf, len);
		ClearUSART_RX_BUF( );
		USART_RX_STA = 0;  
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


