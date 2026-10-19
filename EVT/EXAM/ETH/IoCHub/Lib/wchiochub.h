/********************************** (C) COPYRIGHT *******************************
 * File Name          : wchiochub.h
 * Author             : WCH
 * Version            : V1.2.1
 * Date               : 2025/04/24
 * Description        : This file contains the headers of 
*                    the IoCHub protocol stack library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __WCHIOCHUB_H_
#define __WCHIOCHUB_H_

#include <stdint.h>

/* The following values are fixed and cannot be changed */

#define DEFAULT_MAX_CONNECT_NUM					(7UL)    	/*The maximum number of supported connections is 7*/
#define IOCHUB_HDLSIZE							(44UL+12UL+20UL)
#define IOCHUB_PKTLEN_NUM						(4UL)
#define IOCHUB_FRAME_NUM						(2)

#define LAN_SCAN_FUNC_ENABLE					1

/****************ErrorCode*******************/
#define IOCHUB_ERR_T
#define IOCHUB_SUCCESS                          0UL         /*The operation was successful*/
#define IOCHUB_REGISTERING                      100UL       /*Registration for connection in progress*/
#define IOCHUB_REGISTERED                       101UL       /*The equipment has been registered*/
#define IOCHUB_REGISTER_FAIL                    102UL       /*Registration failed*/
#define IOCHUB_LOGIN_FAIL                       103UL       /*Login failed*/
#define IOCHUB_CNT_COMM_SER_FAIL                104UL       /*The connection communication service failed*/
#define IOCHUB_SER_EXCEPTIONS                   105UL       /*Service exception (requires retry)*/
#define IOCHUB_CNT_COMM_SER_TIMEOUT             106UL       /*The connection communication service has timed out*/
#define IOCHUB_GET_DEVICEID_FAIL                107UL       /*Failed to obtain the local identification*/
#define IOCHUB_UPLOAD_NATTYPE_FAIL              108UL       /*Failed to report the NAT type*/
#define IOCHUB_INPUT_IP_FORMAT_ERR              109UL       /*The input IP and port formats are incorrect*/
#define IOCHUB_NODE_ID_OCCUPY                   110UL       /*Node ID occupied (in use)*/

#define IOCHUB_DEVICE_UNREGISTER                200UL       /*The equipment is not registered*/
#define IOCHUB_LAN_SCANNING                     201UL       /*Local area network discovery is underway*/
#define IOCHUB_BROADCAST_PORT_USED              202UL       /*The broadcast port is occupied (default broadcast port: 10000)*/
#define IOCHUB_UNINIT                           203UL       /*The IoCHub library has not been initialized*/
#define IOCHUB_LAN_SCANNING_DISABLE             204UL       /*The local area network of this machine was found to be unavailable*/
#define IOCHUB_LAN_SCAN_OVER                    205UL       /* The local area network scan is completed */
#define IOCHUB_LIB_INIT_FAIL                    206UL       /* Failed initialization of the IoCHub library (new) */

#define IOCHUB_SESSION_EXIT                     300UL       /*The conversation already exists*/
#define IOCHUB_LOAD_FULL                        301UL       /*The number of session connections has reached the upper limit*/
#define IOCHUB_SESSION_OPENING                  302UL       /*A conversation is being opened.*/
#define IOCHUB_DEVICE_OFFLINE                   303UL       /*The current device is not logged in and connected*/
#define IOCHUB_DST_DEVICE_OFFLINE               304UL       /*The target session device is not online*/
#define IOCHUB_DST_DEVICE_LOAD_FULL             305UL       /*The number of session connections of the target device has reached the upper limit*/
#define IOCHUB_SESSION_OPEN_FAIL                306UL       /*Failed to open the conversation*/
#define IOCHUB_SESSION_OPEN_TIMEOUT             307UL       /*Open session timeout*/
#define IOCHUB_REG_BROADCAST_FAIL               308UL       /*Failed to register for the broadcast*/
#define IOCHUB_REGISTERED_BROADCAST             309UL       /*Registered for broadcasting*/
#define IOCHUB_DSTID_IS_LOCALID                 310UL       /*The target session device ID is the local ID*/
#define IOCHUB_SESSION_CLOSE_FAIL               311UL       /*Failed to close the session*/
#define IOCHUB_LAWLESS_DEVID                    312UL       /*Illegal device ID*/
#define IOCHUB_DSTDEV_BUSY                      313UL       /*The target device is busy (needs to retry)*/
#define IOCHUB_SESSION_CAPTCHA_FORMAT_ERR       314UL       /*The format of the session verification code is incorrect*/
#define IOCHUB_SESSION_CAPTCHA_NOT_MATCH        315UL       /*The session verification code does not match*/
#define IOCHUB_REJECT_SESSION_CONNECT           316UL       /*Reject the session connection*/
#define IOCHUB_SESSION_EXCEPTION                317UL       /* Session anomaly */
#define IOCHUB_EXISTS_UNCLOSED_SESSION          318UL       /* There are unclosed P2P sessions */

#define IOCHUB_SESSION_NOT_EXIT                 400UL       /*The conversation does not exist.*/
#define IOCHUB_TXBUF_OVERFLOW                   401UL       /*Send buffer overflow*/
#define IOCHUB_RXBUF_OVERFLOW                   402UL       /*Receive buffer overflow*/
 
#define IOCHUB_SOCKET_CREATE_FAIL               500UL       /*Socket creation failed.*/
#define IOCHUB_SOCKET_CONNECT_TIMEOUT           501UL       /*Socket connection timeout*/
#define IOCHUB_SOCKET_CONNECT_FAIL              502UL       /*Socket connection failed*/
#define IOCHUB_SOCKET_READ_ABNORMAL             503UL       /*Socket reading exception*/
#define IOCHUB_SOCKET_WRITE_ABNORMAL            504UL       /*Socket write exception*/
#define IOCHUB_SOCKET_CHANNEL_ABNORMAL          505UL       /*Socket channel exception*/
#define IOCHUB_REMOTE_ADDR_UNREACHABLE          506UL       /*The remote address is unreachable*/
#define IOCHUB_REMOTE_CONN_RESET                507UL       /* Remote CONN RESET reset or local network disabling */

#define IOCHUB_DEVICE_DISC_ABNORMAL             600UL       /*Abnormal disconnection of the equipment*/
#define IOCHUB_UDPUPDATE_REMOTEADDR_ABNORMAL    601UL       /*The UDP update of the remote connection address is abnormal*/

#define IOCHUB_REQUEST_PARAM_INVALID            700UL       /*The request parameters are invalid*/
#define IOCHUB_DEVICE_CONNTED_FORBID_CFGPARAM   701UL       /*The device has been connected and parameter configuration cannot be performed*/
#define IOCHUB_HANDLE_INVALID                   702UL       /*Invalid handle*/
#define IOCHUB_PARAM_INVALID                    703UL       /*Configuration parameters are invalid.*/


typedef uint8_t IoCHubHANDLE;

typedef struct
{
    uint8_t addr[4];
    uint16_t port;
} UDP_ADDR_S;

typedef struct
{
	uint8_t dstDevID[8];
	uint8_t dstDevIP[4];
	uint16_t dstDevPort;
	uint16_t msgSeq;
	uint16_t msgSeqFlag;
	uint16_t localMsgSeq;
	uint8_t transferType;
	uint8_t flag;
	uint8_t taskID;
	uint8_t TransTimeoutCount;
}IOCHUB_SESSION_HANDLE;

typedef struct
{
	uint8_t DevID[8]; 
	uint8_t LanIP[4]; 
} IOCHUB_LAN_DEVICE_INFO;


extern uint8_t SocketIdForSer;
extern uint8_t socketIsSucc;
extern uint16_t iochubScrPort;
extern uint8_t localIP[4];
extern UDP_ADDR_S udpAddr;
extern IoCHubHANDLE sessionHdl;


typedef uint8_t(*TcpSendP)(uint8_t, uint8_t*, uint32_t);
typedef uint8_t(*UdpSendP)(uint8_t, uint8_t*, uint32_t, uint8_t*, uint16_t);
typedef void(*TcpConnectP)(uint32_t, uint32_t, uint8_t*, uint8_t*);
typedef void(*UdpConnectP)(uint32_t, uint8_t*);
typedef uint8_t(*ConnectionCloseP )(uint8_t, uint8_t);
typedef void (*IocHubRegCallBackP)(uint16_t);
typedef void (*IocHubSessionStateCallBackP)(uint8_t,uint16_t);
#if LAN_SCAN_FUNC_ENABLE
typedef void (*LanScanDeviceCallBackP)(uint8_t,IOCHUB_LAN_DEVICE_INFO*);
#endif
typedef uint32_t (*RTC_GetCycle)(void);

typedef enum CHIP_TYPE
{
	E_CH32V30x_D8C = 1,
	E_CH32V30x_D8,
	E_CH32V20x_D8W,
	E_CH32V20x_D8,
	E_CH32V20x_D6
}CHIP_TYPE;

/* Parameter configuration structure */
typedef struct TAG_IOCHUB_CONFIG
{
    uint16_t frameSize;
    uint16_t poolBufSize;
	uint8_t* IoCHubBuf;
    RTC_GetCycle pIoChubGetTimeIsr;
    TcpSendP pTcpSendF;
    UdpSendP pUdpSendF;
    TcpConnectP pTcpConnectF;
    UdpConnectP pUdpConnectF;
    ConnectionCloseP pConnectCloseF;
	IocHubSessionStateCallBackP pSessionStateCB;
	CHIP_TYPE chipType;
    uint16_t wndSize;
    uint16_t wndTimeOut;
	uint16_t recvBufSize;
	uint8_t maxConnNum;
    uint8_t rfu; 
} IOCHUB_CONFIG;

/*******************************************************************************
 * @fn          iochub_hex2Ascii
 *
 * @brief       hex to Ascii
 *
 * input parameters
 *
 * @param       *input The hex code to be converted
 *
 * @param       inputLen Input length
 *
 * output parameters
 *
 * @param       *output Conversion output
 *
 * @return      None
 */
void iochub_hex2Ascii(uint8_t *input, uint8_t inputLen, uint8_t *output);

/*******************************************************************************
 * @fn          iochub_ascii2Hex
 *
 * @brief       Ascii to hex
 *
 * input parameters
 *
 * @param       *input The ASCII code to be converted
 *
 * @param       inputLen Input length
 *
 * output parameters
 *
 * @param       *output Conversion output
 *
 * @return      None
 */
void iochub_ascii2Hex(uint8_t *input, uint8_t inputLen, uint8_t *output);

/*******************************************************************************
 * @fn          iocos_memcpy
 *
 * @brief       Copies len bytes from memory area *scr to memory area *dst.
 *
 * input parameters
 *
 * @param       dst Pointer to the destination array to store the copied content, 
 *					type-cast to a void* pointer.
 *
 * @param       scr Point to the data source to be copied, and the type is forcibly 
 *					converted to a void* pointer.
 *
 * @param       len The number of bytes to be copied.
 *
 * output parameters
 *
 * @param       dst Return a pointer to the target storage area dst.
 *
 * @return      None
 */
void iocos_memcpy(void* dst ,const void* scr, uint32_t len);

/*******************************************************************************
 * @fn          iocos_memset
 *
 * @brief       Set a memory area to the specified value.
 *
 * input parameters
 *
 * @param       dst A pointer pointing to the memory area to be filled.
 *
 * @param       c The value to be set is usually an unsigned character.
 *
 * @param       len The number of bytes to be set to this value.
 *
 * output parameters
 *
 * @param       dst Return a pointer to the storage area str.
 *
 * @return      None
 */
void iocos_memset(void* dst , int8_t c, uint32_t len);

/*******************************************************************************
 * @fn          iocos_memcmp
 *
 * @brief       Compare the first len bytes of storage area str1 and storage area str2.
 *
 * input parameters
 *
 * @param       str1 A pointer to a memory block
 *
 * @param       str2 A pointer to a memory block
 *
 * @param       len The number of bytes to be compared.
 *
 * output parameters
 *
 * @param       None
 *
 * @return      0- str1 is equal to str2.
 *				1- str1 is not equal to str2.
 */
int32_t iocos_memcmp(const void* str1 ,const void* str2, uint32_t len);
/*******************************************************************************
 * @fn          WCHIOCHUB_Init
 *
 * @brief       Initialization of IoCHub
 *
 * input parameters
 *
 * @param       * Cfg Configuration parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Register the status of successful execution of the instruction
 */
void WCHIOCHUB_Init(IOCHUB_CONFIG* pCfg);

/*******************************************************************************
 * @fn          WCHIOCHUB_DeInit
 *
 * @brief       IoCHub cleans up resources
 *
 * input parameters
 *
 * @param       * Cfg Configure the parameter structure
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Register the status of successful execution of the instruction 
 * 									0-The operation was successful.
 *									Error code-Failed to clean up resources
 */
uint16_t WCHIOCHUB_DeInit(void);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetCurrentCfg
 *
 * @brief       Obtain the current parameter configuration of IoCHub
 *
 * input parameters
 *
 * @param       * Cfg Configuration parameters
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Register the status of successful execution of the instruction
 */
void WCHIOCHUB_GetCurrentCfg(IOCHUB_CONFIG* pCfg);

/*******************************************************************************
 * @fn          WCHIOCHUB_start
 *
 * @brief       Start the IoCHub service
 *
 * input parameters
 *
 * @param serAddr 
 * @param serPort 
 * @param localPort 
 * @param deviceID 
 * @param deviceSecret
 * @param deviceLockFlag(Equipment lock): 0x00:Not locked, 0x01: Locked, default 0x00
 * @param commMode(Communication mode): 1: Automatic, 2: Manual, default: 1
 * @param pRegCB        Connect the status callback function in the authentication stage
 *
 * output parameters
 *
 * @param       None
 *
 * @return uint16_t   0 - The service was successfully started
 *          	      Error code - Startup failed
 */
uint16_t WCHIOCHUB_Start(uint8_t *serAddr, uint16_t serPort, uint16_t localPort,
                         uint8_t *deviceID, uint8_t *deviceSecret, uint8_t deviceLockFlag,
						 uint8_t commMode,IocHubRegCallBackP pRegCB);

/*******************************************************************************
 * @fn          WCHIOCHUB_Stop
 *
 * @brief       Stop the IoCHub service
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       None
 *
 * @return      0			-The service was successfully stopped.
 *				Error code  - Service stop failed
 *
 */
uint16_t WCHIOCHUB_Stop(void);

/*******************************************************************************
 * @fn          WCHIOCHUB_DataRecv
 *
 * @brief      Pass the data obtained from the NET protocol stack to IoCHub
 *
 * input parameters
 *
 * @param       len Length to be written
 *
 * @param       *wData Data to be written
 *
 * output parameters
 *
 * @param       None
 *
 * @return      Execution completion status
 */
uint16_t WCHIOCHUB_NetDataToUsrData(uint8_t *recvDataBuf, uint32_t recvDataSize);

/*******************************************************************************
 * @fn          WCHIOCHUB_DataSend
 *
 * @brief       Client-side data transmission
 *
 * input parameters
 *
 * @param  uint8_t* sendDataBuf
 *
 * @param  uint16_t dataLen
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Execution completion status
 */
uint16_t WCHIOCHUB_DataSend(IoCHubHANDLE sessionHdl,uint8_t*  sendDataBuf, uint16_t* sendDataSize);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetSessionSendableLen
 *
 * @brief       get the length of data that can be sent before the session sends it.
 *
 * input parameters
 *
 * @param  pDev 	- Specifies the session handle
 *
 * @param  sendLen 	- pointer to the length to be sent
 *
 * output parameters
 *
 * @param  sendLen 	- pointer to the length to be sent
 *
 * @return      @IOCHUB_ERR_T
 */
uint16_t WCHIOCHUB_GetSessionSendableLen (IoCHubHANDLE sessionHdl, uint16_t* sendLen);

/*******************************************************************************
 * @fn          WCHIOCHUB_DataRecv
 *
 * @brief       Client receives data to read
 *
 * input parameters
 *
 * @param  *recvDataBuf - the first address of receive buffer
 *
 * @param  *recvDataSize - pointer to the length of the data expected to be read
 *
 * output parameters
 *
 * @param  *recvDataBuf - the first address of data buffer
 *
 * @param  *recvDataSize - pointer to the length of the data read actually
 *
 * @return @IOCHUB_ERR_T
 */
uint16_t WCHIOCHUB_DataRecv (IoCHubHANDLE sessionHdl, uint8_t *recvDataBuf, uint16_t *recvDataSize);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetSessionRecvLen
 *
 * @brief       get the length of data that can be read before the session reads it.
 *
 * input parameters
 *
 * @param  pDev 	- Specifies the session handle
 *
 * @param  recvLen 	- pointer to the length to be read
 *
 * output parameters
 *
 * @param  recvLen 	- pointer to the length to be read
 *
 * @return      @IOCHUB_ERR_T
 */
uint16_t WCHIOCHUB_GetSessionRecvLen (IoCHubHANDLE sessionHdl, uint16_t *recvLen);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetSDKVer
 *
 * @brief      Obtain the current version of the library
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       verBuf[4] Version number
 *
 * @return      None
 *
 */
void WCHIOCHUB_GetSDKVer(uint8_t verBuf[3]);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetLocalID
 *
 * @brief      Obtain the unique identifier of this device, devID
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       devcID[8] Device id
 *
 * @return      None
 *
 */
void WCHIOCHUB_GetLocalID(uint8_t devcID[8]);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetNATType
 *
 * @brief      Obtain the current network type
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       None
 *
 * @return      NATType Network type
 *
 */
uint8_t WCHIOCHUB_GetNATType(void);


/*******************************************************************************
 * @fn          WCHIOCHUB_GetTransferType
 *
 * @brief      Obtain the data transmission mode of the specified session
 *
 * input parameters
 *
 * @param       pDev The specified session Handle
 *
 * output parameters
 *
 * @param       None
 *
 * @return      dataTransType Data transmission mode
 *				0:Server forwarding;
 *				1:Point-to-point direct connection;
 *				0xff: session is invalid;
 *
 */
uint16_t WCHIOCHUB_GetTransferType(IoCHubHANDLE sessionHdl);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetDeviceSerState
 *
 * @brief      Obtain the online status of the IoCHub service
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @param       void
 *
 * @return      DeviceSerState Service online status
 *				0:The service is out of service;
 *				1:Service Online;
 *
 */
uint8_t WCHIOCHUB_GetDeviceSerState(void);

/*******************************************************************************
 * @fn          IoCHub_SetAccessMode
 *
 * @brief       Set the access mode mode for session connection access configuration.
 *
 * input parameters
 *
 * @param       modeType	Access mode (1 byte, default 0)
 * 							0:Public access only requires DevID to establish a session connection.;
 * 							1: Restricted access. A DevID and session key are required to establish a session connection;
 * 							2:Private. Requests for establishing session connections are not accepted;
 *
 * @param       sessionKey[8]	Session key (default all 0)
 *
 * output parameters
 *
 * @param       None
 *
 * @return      None
 *
 */
void WCHIOCHUB_SetAccessMode(uint8_t modeType, uint8_t sessionKey[8]);


/*******************************************************************************
 * @fn          IoCHub_GetAccessMode
 *
 * @brief       Obtain the current access mode.
 *
 * input parameters
 *
 * @param       None
 *
 * output parameters
 *
 * @param       *modeType	Access mode,1 byte
 *
 * @param       *sessionKey	Session key
 *
 * @return      None
 *
 */
void WCHIOCHUB_GetAccessMode(uint8_t* modeType, uint8_t sessionKey[8]);

/*******************************************************************************
 * @fn          WCHIOCHUB_OpenSession
 *
 * @brief       Client A initiates a connection request to the server 
 * 				with another client B. Here, the server determines whether 
 * 				the NAT is within the same internal network, exchanges and 
 * 				returns the remote and local IP addresses and ports of the client. 
 * 				After obtaining the data, the client immediately starts drilling 
 * 				the hole.
 *
 * input parameters
 *
 * @param       dstDevID[8] Target device ID
 *
 * @param       sessionKey[8] Target verification code, all zeros by default
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      0-The session connection was initiated successfully.
 *				Error code - Failed to initiate a session connection.
 */
uint16_t WCHIOCHUB_OpenSession(IoCHubHANDLE* sessionHdl, uint8_t dstDevcID[8], uint8_t sessionKey[8]);

/*******************************************************************************
 * @fn          WCHIOCHUB_CloseSession
 *
 * @brief       Close the session
 *
 * input parameters
 *
 * @param       sessionHdl Conversation handle
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
uint16_t WCHIOCHUB_CloseSession(IoCHubHANDLE sessionHdl);


/*******************************************************************************
 * @fn          WCHIOCHUB_SetBrdcstAddr
 *
 * @brief      Set the local area network broadcast address. 
 * 				The default is 255.255.255.255
 *
 * input parameters
 *
 * @param       ADDR Local area network broadcast address
 *
 * output parameters
 *
 * @param       None
 *
 * @return      bStatus_t
 *
 */
uint16_t WCHIOCHUB_SetBrdcstAddr(uint8_t* addr);

#if LAN_SCAN_FUNC_ENABLE
/*******************************************************************************
 * @fn          WCHIOCHUB_SetLanScan
 *
 * @brief       Configure the local area network scanning function
 *
 * input parameters
 *
 * @param       pDevScanCB Scan result callback function
 *
 * @param       *lanDeviceList List of scanning devices
 *
 * @param       lanDeviceListMaxNum The maximum number of scanning devices
 *
 * @param       enable  Enable the scanning function
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
 uint16_t WCHIOCHUB_SetLanScan(LanScanDeviceCallBackP pDevScanCB,
 							  IOCHUB_LAN_DEVICE_INFO *lanDeviceList,
 							  uint8_t lanDeviceListMaxNum,
 							  uint8_t enable);

/*******************************************************************************
 * @fn          WCHIOCHUB_LanScan
 *
 * @brief       Local area network device scanning
 *
 * input parameters
 *
 * @param       scanTime Scanning time (unit: s)
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
 void WCHIOCHUB_LanScan(uint8_t scanTime);
#endif

/*******************************************************************************
 * @fn          WCHIOCHUB_GetNodeID
 *
 * @brief      Get the device ID of the peer node in the session
 *
 * input parameters
 *
 * @param       iochubSessionHdl session handle
 *
 * @param       nodeID[8] Array holding the device ids of the peer nodes
 *
 * output parameters
 *
 * @param       nodeID[8] Device id of the peer node
 *
 * @return      None
 *
 */
void WCHIOCHUB_GetNodeID (IoCHubHANDLE SessionHdl, uint8_t nodeID[8]);

/*******************************************************************************
 * @fn          WCHIOCHUB_GetNodeIP
 *
 * @brief      Get the device IP of the peer node in the session
 *
 * input parameters
 *
 * @param       iochubSessionHdl session handle
 *
 * @param       nodeIP[8] Array holding the device IP DAAR of the peer nodes
 *
 * output parameters
 *
 * @param       nodeIP[8] Device IP ADDR of the peer node
 *
 * @return      None
 *
 */
void WCHIOCHUB_GetNodeIP (IoCHubHANDLE SessionHdl, uint8_t nodeIP[4]);

/* The main task function of the library needs to be continuously called in the main while loop */
void WCHIOCHUB_Process( void );

#endif


