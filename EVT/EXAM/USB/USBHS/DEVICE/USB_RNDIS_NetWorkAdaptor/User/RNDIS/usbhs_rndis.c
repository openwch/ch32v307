/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs_rndis.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/07/31
* Description        : usbhs_rndis program body.
*********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "usbhs_rndis.h"

/*********************************************
   RNDIS specific management functions
 *********************************************/
#define ETH_HEADER_SIZE                         14
#define RNDIS_MAX_PACKET_SIZE                   ETH_HEADER_SIZE + RNDIS_MTU
#define ETH_MIN_PACKET_SIZE                     60
#define RNDIS_RX_BUFFER_SIZE                    (RNDIS_MAX_PACKET_SIZE + sizeof(rndis_data_packet_t))
#define RNDIS_TX_BUFFER_SIZE                    (RNDIS_MAX_PACKET_SIZE + sizeof(rndis_data_packet_t))

#define OID_LIST_LENGTH                         (sizeof(OIDSupportedList) / sizeof(*OIDSupportedList))
#define ENC_BUF_SIZE                            (OID_LIST_LENGTH * 4 + 32)

#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define INFBUF                                  ((uint32_t *)((uint8_t *)&(m->RequestId) + m->InformationBufferOffset))
#define CFGBUF                                  ((rndis_config_parameter_t *) INFBUF)
#define PARMNAME                                ((uint8_t *)CFGBUF + CFGBUF->ParameterNameOffset)
#define PARMVALUE                               ((uint8_t *)CFGBUF + CFGBUF->ParameterValueOffset)
#define PARMVALUELENGTH CFGBUF->ParameterValueLength
#define PARM_NAME_LENGTH                        25

#define MAC_OPT NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA | \
            NDIS_MAC_OPTION_RECEIVE_SERIALIZED  | \
            NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  | \
            NDIS_MAC_OPTION_NO_LOOPBACK

const uint32_t OIDSupportedList[] =
{
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_VENDOR_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_MEDIA_CONNECT_STATUS,
    OID_GEN_MAXIMUM_SEND_PACKETS,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_MAC_OPTIONS
};

volatile uint8_t  RNDIS_Spd_ChangeReset = 0;
uint8_t  MAC_Address[ 6 ];
uint8_t  encapsulated_buffer[ ENC_BUF_SIZE ];
uint32_t oid_packet_filter = 0x0000000;
uint32_t rndis_network_state = RNDIS_STATUS_MEDIA_DISCONNECT;
rndis_state_t  rndis_state;
usb_eth_stat_t usb_eth_stat = { 0, 0, 0, 0 };

uint32_t Rndis_LinkSpeed = RNDID_LINKSPD_100M;

__attribute__ ((aligned(4))) uint8_t RNDIS_RX_Buffer[ RNDIS_RX_BUFFER_SIZE ];
__attribute__ ((aligned(4))) uint8_t RNDIS_TX_Buffer[ RNDIS_TX_BUFFER_SIZE ];
__attribute__ ((aligned(4))) uint8_t RNDIS_EP1_Buffer[ DEF_USB_EP1_HS_SIZE ] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

volatile uint8_t USBHS_UsbTxFlag;
volatile uint8_t USBHS_UsbRxFlag;
volatile uint16_t USBHS_UsbTxLen;
volatile uint16_t USBHS_UsbRxLen;
static uint32_t USBHS_UsbTxAddr;

/* Ring Buffer */
__attribute__((__aligned__(4))) RING_BUFF_COMM U2E_Trance_Manage;/* usb to eth 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t U2E_PackAdr[DEF_U2E_MAXBLOCKS];/* usb to eth 包地址记录 */
__attribute__((__aligned__(4))) uint32_t U2E_PackLen[DEF_U2E_MAXBLOCKS];/* usb to eth 包长度记录 */

__attribute__((__aligned__(4))) RING_BUFF_COMM E2U_Trance_Manage;/* eth to usb 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t E2U_PackAdr[DEF_E2U_MAXBLOCKS];/* eth to usb 包地址记录 */
__attribute__((__aligned__(4))) uint32_t E2U_PackLen[DEF_E2U_MAXBLOCKS];/* eth to usb 包长度记录 */

__attribute__ ((aligned(4))) uint8_t USB_TxBuff[ DEF_ETH_TX_USBPACK ] = { 0 };
__attribute__ ((aligned(4))) uint8_t USB_RxBuff[ DEF_ETH_RX_USBPACK ] = { 0 };

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef *DMARxDealTabs[ETH_RXBUFNB];/* MAC recv descriptor, 4-byte aligned， eth to usb 接收描述符包状态归属 */

/* Phy Status */
volatile uint8_t  ETH_LastStatus = 0;
volatile uint8_t  ECM_Pack_Filter = 0;
volatile uint8_t  ETH_NETWork_Status = 0;
volatile uint16_t LastPhyStatus = 0;

/* Transe Status */
volatile uint32_t volatile U2E_PackCnounter;
volatile uint32_t volatile E2U_PackCnounter;
uint8_t  PhyInit_Flag;

static void _rndis_query_cmplt32(int status, uint32_t data)
{
    rndis_query_cmplt_t *c;
    c = (rndis_query_cmplt_t *)encapsulated_buffer;
    c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
    c->MessageLength = sizeof(rndis_query_cmplt_t) + 4;
    c->InformationBufferLength = 4;
    c->InformationBufferOffset = 16;
    c->Status = status;
    *(uint32_t *)(c + 1) = data;
    USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
}

static void _rndis_query_cmplt(int status, const void *data, int size)
{
    rndis_query_cmplt_t *c;
    c = (rndis_query_cmplt_t *)encapsulated_buffer;
    c->MessageType = REMOTE_NDIS_QUERY_CMPLT;
    c->MessageLength = sizeof(rndis_query_cmplt_t) + size;
    c->InformationBufferLength = size;
    c->InformationBufferOffset = 16;
    c->Status = status;
    memcpy(c + 1, data, size);
    USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
}

static void _rndis_query( void )
{
    switch (((rndis_query_msg_t *)encapsulated_buffer)->Oid)
    {
        case OID_GEN_SUPPORTED_LIST:         _rndis_query_cmplt(RNDIS_STATUS_SUCCESS, OIDSupportedList, 4 * OID_LIST_LENGTH); return;
        case OID_GEN_VENDOR_DRIVER_VERSION:  _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00001000);  return;
        case OID_802_3_CURRENT_ADDRESS:      _rndis_query_cmplt(RNDIS_STATUS_SUCCESS, &MAC_Address, 6); return;
        case OID_802_3_PERMANENT_ADDRESS:    _rndis_query_cmplt(RNDIS_STATUS_SUCCESS, &MAC_Address, 6); return;
        case OID_GEN_MEDIA_SUPPORTED:        _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_MEDIA_IN_USE:           _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_PHYSICAL_MEDIUM:        _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIUM_802_3); return;
        case OID_GEN_HARDWARE_STATUS:        _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_GEN_LINK_SPEED:             _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, Rndis_LinkSpeed / 100); return;
        case OID_GEN_VENDOR_ID:              _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0x00FFFFFF); return;
        case OID_GEN_VENDOR_DESCRIPTION:     _rndis_query_cmplt(RNDIS_STATUS_SUCCESS, RNDIS_VENDOR, strlen(RNDIS_VENDOR) + 1); return;
        case OID_GEN_CURRENT_PACKET_FILTER:  _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, oid_packet_filter); return;
        case OID_GEN_MAXIMUM_FRAME_SIZE:     _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_MAX_PACKET_SIZE - ETH_HEADER_SIZE); return;
        case OID_GEN_MAXIMUM_TOTAL_SIZE:     _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_MAX_PACKET_SIZE); return;
        case OID_GEN_TRANSMIT_BLOCK_SIZE:    _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_MAX_PACKET_SIZE); return;
        case OID_GEN_RECEIVE_BLOCK_SIZE:     _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, RNDIS_MAX_PACKET_SIZE); return;
        case OID_GEN_MEDIA_CONNECT_STATUS:   _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, NDIS_MEDIA_STATE_CONNECTED); return;
        case OID_GEN_RNDIS_CONFIG_PARAMETER: _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_MAXIMUM_LIST_SIZE:    _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 1); return;
        case OID_802_3_MULTICAST_LIST:       _rndis_query_cmplt32(RNDIS_STATUS_NOT_SUPPORTED, 0); return;
        case OID_802_3_MAC_OPTIONS:          _rndis_query_cmplt32(RNDIS_STATUS_NOT_SUPPORTED, 0); return;
        case OID_GEN_MAC_OPTIONS:            _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, /*MAC_OPT*/ 0); return;
        case OID_802_3_RCV_ERROR_ALIGNMENT:  _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_XMIT_ONE_COLLISION:   _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_802_3_XMIT_MORE_COLLISIONS: _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        case OID_GEN_XMIT_OK:                _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.txok); return;
        case OID_GEN_RCV_OK:                 _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.rxok); return;
        case OID_GEN_RCV_ERROR:              _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.rxbad); return;
        case OID_GEN_XMIT_ERROR:             _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, usb_eth_stat.txbad); return;
        case OID_GEN_RCV_NO_BUFFER:          _rndis_query_cmplt32(RNDIS_STATUS_SUCCESS, 0); return;
        default:                             _rndis_query_cmplt(RNDIS_STATUS_FAILURE, NULL, 0); return;
    }
}

static void _rndis_handle_config_parm(const char *data, int keyoffset, int valoffset, int keylen, int vallen)
{
    (void)data;
    (void)keyoffset;
    (void)valoffset;
    (void)keylen;
    (void)vallen;
}

static void _rndis_packetFilter(uint32_t newfilter)
{
    (void)newfilter;
}

static void _rndis_handle_set_msg( void )
{
    rndis_set_cmplt_t *c;
    rndis_set_msg_t *m;
    rndis_Oid_t oid;

    c = (rndis_set_cmplt_t *)encapsulated_buffer;
    m = (rndis_set_msg_t *)encapsulated_buffer;

    oid = m->Oid;
    c->MessageType = REMOTE_NDIS_SET_CMPLT;
    c->MessageLength = sizeof(rndis_set_cmplt_t);
    c->Status = RNDIS_STATUS_SUCCESS;

    switch (oid)
    {
        /* Parameters set up in 'Advanced' tab */
        case OID_GEN_RNDIS_CONFIG_PARAMETER:
            {
                rndis_config_parameter_t *p;
                char *ptr = (char *)m;
                ptr += sizeof(rndis_generic_msg_t);
                ptr += m->InformationBufferOffset;
                p = (rndis_config_parameter_t *)ptr;
                _rndis_handle_config_parm(ptr, p->ParameterNameOffset, p->ParameterValueOffset, p->ParameterNameLength, p->ParameterValueLength);
            }
            break;

        /* Mandatory general OIDs */
        case OID_GEN_CURRENT_PACKET_FILTER:
            oid_packet_filter = *INFBUF;
            if (oid_packet_filter)
            {
                _rndis_packetFilter(oid_packet_filter);
                rndis_state = rndis_data_initialized;
            }
            else
            {
                rndis_state = rndis_initialized;
            }
            break;

        case OID_GEN_CURRENT_LOOKAHEAD:
            break;

        case OID_GEN_PROTOCOL_OPTIONS:
            break;

        /* Mandatory 802_3 OIDs */
        case OID_802_3_MULTICAST_LIST:
            break;

        /* Power Managment: fails for now */
        case OID_PNP_ADD_WAKE_UP_PATTERN:
        case OID_PNP_REMOVE_WAKE_UP_PATTERN:
        case OID_PNP_ENABLE_WAKE_UP:
        default:
            c->Status = RNDIS_STATUS_FAILURE;
            break;
    }

    /* c->MessageID is same as before */
    USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
    return;
}



/*********************************************************************
 * @fn      RNDIS_MSG_Recv
 *
 * @brief   Receives rndis message from usb endp0
 *
 * @return  none
 */
void RNDIS_MSG_Recv( void )
{
    switch (((rndis_generic_msg_t *)encapsulated_buffer)->MessageType)
    {
        case REMOTE_NDIS_INITIALIZE_MSG:
            {
                rndis_initialize_cmplt_t *m;
                m = ((rndis_initialize_cmplt_t *)encapsulated_buffer);
                /* m->MessageID is same as before */
                m->MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
                m->MessageLength = sizeof(rndis_initialize_cmplt_t);
                m->MajorVersion = RNDIS_MAJOR_VERSION;
                m->MinorVersion = RNDIS_MINOR_VERSION;
                m->Status = RNDIS_STATUS_SUCCESS;
                m->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
                m->Medium = RNDIS_MEDIUM_802_3;
                m->MaxPacketsPerTransfer = 1;
                m->MaxTransferSize = RNDIS_RX_BUFFER_SIZE;
                m->PacketAlignmentFactor = 0;
                m->AfListOffset = 0;
                m->AfListSize = 0;
                rndis_state = rndis_initialized;
                USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
            }
            break;

        case REMOTE_NDIS_QUERY_MSG:
            _rndis_query( );
            break;

        case REMOTE_NDIS_SET_MSG:
            _rndis_handle_set_msg( );
            break;

        case REMOTE_NDIS_RESET_MSG:
            {
                rndis_reset_cmplt_t * m;
                m = ((rndis_reset_cmplt_t *)encapsulated_buffer);
                rndis_state = rndis_uninitialized;
                m->MessageType = REMOTE_NDIS_RESET_CMPLT;
                m->MessageLength = sizeof(rndis_reset_cmplt_t);
                m->Status = RNDIS_STATUS_SUCCESS;
                m->AddressingReset = 1; /* Make it look like we did something */
                /* m->AddressingReset = 0; - Windows halts if set to 1 for some reason */
                USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
            }
            break;

        case REMOTE_NDIS_KEEPALIVE_MSG:
            {
                rndis_keepalive_cmplt_t * m;
                m = (rndis_keepalive_cmplt_t *)encapsulated_buffer;
                m->MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
                m->MessageLength = sizeof(rndis_keepalive_cmplt_t);
                m->Status = RNDIS_STATUS_SUCCESS;
            }
            /* We have data to send back */
            USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      RNDIS_Load_Status
 *
 * @brief   Load RNDIS NetWork Status
 *
 * @return  none
 */
void RNDIS_Load_Status( void )
{
    uint16_t phy_stat;
    static uint16_t Loacl_Timer = 0;
    /* Phy status update */
    Loacl_Timer++;
    if( Loacl_Timer == DEF_PHY_QUERY_TIMEOUT )
    {
        Loacl_Timer = 0;
#if PHY_MODE == USE_10M_BASE
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#elif PHY_MODE == USE_MAC_RGMII
        ETH_WritePHYRegister( PHY_ADDRESS, 0x1F, 0x0a43 );
        /*In some cases the status is not updated in time,
         * so read this register twice to get the correct status value.*/
        ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A);
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A );
#else
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#endif
        if( phy_stat != LastPhyStatus )
        {
            ETH_NETWork_Status &= ~DEF_NETSTAT_LINK_RDY;
            LastPhyStatus = phy_stat;
            ETH_PHYLink( );

            /* ETH Status Update */
            if( ETH_NETWork_Status & DEF_NETSTAT_LINK_RDY )
            {
                ETH_NETWork_Status &= ~(DEF_NETSTAT_100MBITS | DEF_NETSTAT_1000MBITS);
                if( ETH->MACCR & ETH_Speed_100M )
                {
                    /* 100M Speed */
                    ETH_NETWork_Status |= DEF_NETSTAT_100MBITS;
                }
                else if( ETH->MACCR & ETH_Speed_1000M )
                {
                    /* 1000M Speed */
                    ETH_NETWork_Status |= DEF_NETSTAT_1000MBITS;
                }
                else
                {
                    /* 10M Speed */
                }
                ETH_NETWork_Status &= ~DEF_NETSTAT_FULLDUPLEX;
                if( ETH->MACCR & ETH_Mode_FullDuplex )
                {
                    /* Full Duplex */
                    ETH_NETWork_Status |= DEF_NETSTAT_FULLDUPLEX;
                }
                else
                {
                    /* half Duplex */
                }
                if( ETH_LastStatus != ETH_NETWork_Status )
                {
                    ETH_LastStatus = ETH_NETWork_Status;
                }
            }
            else
            {
                ETH_NETWork_Status &= ~(DEF_NETSTAT_100MBITS | DEF_NETSTAT_1000MBITS);
                ETH_NETWork_Status &= ~DEF_NETSTAT_FULLDUPLEX;
            }
            if( rndis_state == rndis_data_initialized )
            {
                if( ETH_NETWork_Status & DEF_NETSTAT_LINK_RDY )
                {
                    rndis_network_state = RNDIS_STATUS_MEDIA_CONNECT;
                }
                else
                {
                    rndis_network_state = RNDIS_STATUS_MEDIA_DISCONNECT;
                }
                rndis_indicate_status_t * m;
                m = ((rndis_indicate_status_t *)encapsulated_buffer);
                m->MessageType = REMOTE_NDIS_INDICATE_STATUS_MSG;
                m->MessageLength = sizeof(rndis_indicate_status_t);
                m->Status = rndis_network_state;
                m->StatusBufferLength = sizeof(rndis_network_state);
                m->StatusBufferOffset = 8;
                USBHS_EP1_UpLoad( 8, ( uint32_t )RNDIS_EP1_Buffer );
            }
        }
    }
    if( RNDIS_Spd_ChangeReset == 0 )
    {
        if( ETH_NETWork_Status & DEF_NETSTAT_LINK_RDY )
        {
            /* Speed match */
            if( ETH_NETWork_Status & DEF_NETSTAT_1000MBITS )
            {
                if( Rndis_LinkSpeed != RNDID_LINKSPD_1000M )
                {
                    /* Reset USB */
                    Rndis_LinkSpeed = RNDID_LINKSPD_1000M;
                    RNDIS_Spd_ChangeReset = 1;
                }
            }
            else if( ETH_NETWork_Status & DEF_NETSTAT_100MBITS )
            {
                if( Rndis_LinkSpeed != RNDID_LINKSPD_100M )
                {
                    /* Reset USB */
                    Rndis_LinkSpeed = RNDID_LINKSPD_100M;
                    RNDIS_Spd_ChangeReset = 1;
                }
            }
            else
            {
                if( Rndis_LinkSpeed != RNDID_LINKSPD_10M )
                {
                    /* Reset USB */
                    Rndis_LinkSpeed = RNDID_LINKSPD_10M;
                    RNDIS_Spd_ChangeReset = 1;
                }
            }
        }
    }

    /* If the usb speed does not match the phy speed, perform a usb reset operation to re-upload the network speed */
    if( RNDIS_Spd_ChangeReset == 1 )
    {
        RNDIS_Spd_ChangeReset = 2;
        rndis_state = rndis_uninitialized;
        USBHS_Device_Init( DISABLE );
        Delay_Us(100);
        USBHS_Device_Init( ENABLE );
    }
}

/*********************************************************************
 * @fn      USB2ETH_Trance
 *
 * @brief   usb to eth trance
 *
 * @return  none
 */
void USB2ETH_Trance( void )
{
    uint8_t ret;
    rndis_data_packet_t *p;
    uint32_t u2e_deal_ptr;
    if( USBHS_UsbRxFlag )
    {
        p = (rndis_data_packet_t *)RNDIS_RX_Buffer;
        if( USBHS_UsbRxLen >= sizeof(rndis_data_packet_t) )
        {
            if( p->MessageType == REMOTE_NDIS_PACKET_MSG && p->MessageLength == USBHS_UsbRxLen )
            {
                if( USBHS_UsbRxFlag == 1 ) usb_eth_stat.rxok++;
                if( p->DataOffset + offsetof(rndis_data_packet_t, DataOffset) + p->DataLength == USBHS_UsbRxLen )
                {
                    if( U2E_Trance_Manage.RemainPack < ETH_TXBUFNB )
                    {
                        USBHS_UsbRxFlag = 1;
                        memcpy( ( uint8_t* )( U2E_PackAdr[ U2E_Trance_Manage.LoadPtr ] ), &RNDIS_RX_Buffer[p->DataOffset + offsetof(rndis_data_packet_t, DataOffset)], p->DataLength );
                        U2E_PackLen[ U2E_Trance_Manage.LoadPtr ] = p->DataLength;
                        U2E_Trance_Manage.LoadPtr++;
                        if( U2E_Trance_Manage.LoadPtr >= ETH_TXBUFNB )
                        {
                            U2E_Trance_Manage.LoadPtr = 0;
                        }
                        U2E_Trance_Manage.RemainPack++;
                    }
                    else
                    {
                        USBHS_UsbRxFlag = 2;
                    }
                }
            }
        }
        if( USBHS_UsbRxFlag == 1)
        {
            USBHS_UsbRxFlag = 0;
            USBHSD->UEP3_RX_CTRL = ((USBHSD->UEP3_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
        }
    }
    if( U2E_Trance_Manage.RemainPack )
    {
        u2e_deal_ptr = U2E_Trance_Manage.DealPtr;
        ret = ETH_TxPktChainMode( (uint16_t)U2E_PackLen[ u2e_deal_ptr ], (uint32_t *)U2E_PackAdr[ u2e_deal_ptr ] );
        if( ret == ETH_SUCCESS )
        {
            __disable_irq( );
            __NOP( );__NOP( );
            U2E_Trance_Manage.DealPtr++;
            if( U2E_Trance_Manage.DealPtr >= ETH_TXBUFNB )
            {
                U2E_Trance_Manage.DealPtr = 0;
            }
            U2E_Trance_Manage.RemainPack--;
            __enable_irq( );
            __NOP( );__NOP( );
        }
    }
}

/*********************************************************************
 * @fn      ETH2USB_Trance
 *
 * @brief   eth to usb trance
 *
 * @return  none
 */
void ETH2USB_Trance( void )
{
    uint8_t ret;
    uint32_t e2u_deal_ptr;
    rndis_data_packet_t *hdr;
    uint16_t count;

    if( !USBHS_UsbTxFlag )
    {
        count = E2U_Trance_Manage.RemainPack;
        if( count )
        {
            e2u_deal_ptr = E2U_Trance_Manage.DealPtr;
            hdr = (rndis_data_packet_t *)RNDIS_TX_Buffer;
            memset(hdr, 0, sizeof(rndis_data_packet_t));
            hdr->MessageType = REMOTE_NDIS_PACKET_MSG;
            hdr->MessageLength = sizeof(rndis_data_packet_t) + E2U_PackLen[ e2u_deal_ptr ];
            hdr->DataOffset = sizeof(rndis_data_packet_t) - offsetof(rndis_data_packet_t, DataOffset);
            hdr->DataLength = E2U_PackLen[ e2u_deal_ptr ];
            memcpy(RNDIS_TX_Buffer + sizeof(rndis_data_packet_t), ( uint8_t* )E2U_PackAdr[ e2u_deal_ptr ], E2U_PackLen[ e2u_deal_ptr ]);
            USBHS_UsbTxAddr = ( uint32_t )RNDIS_TX_Buffer;
            USBHS_UsbTxLen = sizeof(rndis_data_packet_t) + E2U_PackLen[ e2u_deal_ptr ];
            USBHS_UsbTxFlag = 1;
            __disable_irq( );
            __NOP( );__NOP( );
            DMARxDealTabs[ e2u_deal_ptr ]->Status |= ETH_DMARxDesc_OWN;
            E2U_PackLen[ e2u_deal_ptr ] = 0;
            E2U_PackAdr[ e2u_deal_ptr ] = 0;
            E2U_Trance_Manage.RemainPack--;
            E2U_Trance_Manage.DealPtr++;
            if( E2U_Trance_Manage.DealPtr >= ETH_RXBUFNB )
            {
                E2U_Trance_Manage.DealPtr = 0;
            }
            __enable_irq( );
            __NOP( );__NOP( );
        }
    }

    if( USBHS_UsbTxFlag )
    {
        if( USBHS_UsbTxLen >= USBHS_DevMaxPackLen )
        {
            /* pack size >= USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( USBHS_DevMaxPackLen, USBHS_UsbTxAddr );
            if( ret == 0 )
            {
                /* this pack up load success, goto usb irq-hander for next deals */
                USBHS_UsbTxLen -= USBHS_DevMaxPackLen;
                USBHS_UsbTxAddr += USBHS_DevMaxPackLen;
            }
        }
        else
        {
            /* Pack size < USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( USBHS_UsbTxLen, USBHS_UsbTxAddr );
            if( ret == 0 )
            {
                USBHS_UsbTxFlag = 0;
            }
        }
    }
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = (2000 - 1);    /* 100ms */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef  NVIC_InitTypeStructure = { 0 };
    NVIC_InitTypeStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitTypeStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitTypeStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitTypeStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitTypeStructure );

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      RB_Init
 *
 * @brief   RingBuffer Initialization
 *
 * @return  none
 */
void RB_Init( void )
{
    uint8_t i;
    U2E_Trance_Manage.LoadPtr = 0;
    U2E_Trance_Manage.DealPtr = 0;
    U2E_Trance_Manage.RemainPack = 0;
    for( i = 0; i < ETH_TXBUFNB; i++ )
    {
        U2E_PackLen[ i ] = 0;
        U2E_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACTxBuf[ i*ETH_MAX_PACKET_SIZE ]);
    }

    E2U_Trance_Manage.LoadPtr = 0;
    E2U_Trance_Manage.DealPtr = 0;
    E2U_Trance_Manage.RemainPack = 0;
    for( i = 0; i < ETH_RXBUFNB; i++ )
    {
        E2U_PackLen[ i ] = 0;
        E2U_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACRxBuf[ i*ETH_MAX_PACKET_SIZE ]);
    }
}

/*********************************************************************
 * @fn      ETH_GetMacAddr
 *
 * @brief   Get MAC address
 *
 * @return  none.
 */
void ETH_GetMacAddr( uint8_t *p )
{
    uint8_t i;
    uint8_t *macaddr=(uint8_t *)(ROM_CFG_USERADR_ID+5);

    for(i=0;i<6;i++)
    {
        *p = *macaddr;
        p++;
        macaddr--;
    }
}

/*********************************************************************
 * @fn      ETH_DriverInit
 *
 * @brief   Basic Ethernet initialization
 *
 * @return  none.
 */
void ETH_DriverInit( uint8_t *addr )
{
    uint8_t i;
    /* Ring buffer init */
    RB_Init( );
    /* Used for Time Base */
    TIM2_Init( );
    ETH_Init( addr );
    /* Enable flowConlrol */
    /* PT = 240(10 full eth pack); PLT = 01(28*PT) */
    ETH->MACFCR = 0;
    ETH->MACFCR |= (0xF0<<16);/* 240(64bits time) 10 full eth pack time */
    ETH->MACFCR |= (0x01<<4);
    ETH->MACFCR |= ETH_MACFCR_TFCE;
    NVIC_DisableIRQ( ETH_IRQn );
    /* flush U2E_PackAdr/E2U_PackAdr */
    printf( "TxDesc:\r\n" );
    for( i=0; i<ETH_TXBUFNB; i++ )
    {
        printf( "%08x ", DMATxDscrTab[i].Buffer1Addr );
        U2E_PackAdr[i] = DMATxDscrTab[i].Buffer1Addr;
    }
    printf( "\r\n" );
    printf( "RxDesc:\r\n" );
    for( i=0; i<ETH_RXBUFNB; i++ )
    {
        printf( "%08x ", DMARxDscrTab[i].Buffer1Addr );
        E2U_PackAdr[ i ] = DMARxDscrTab[i].Buffer1Addr;
    }
    printf( "\r\n" );
    NVIC_EnableIRQ( ETH_IRQn );
}

/*********************************************************************
 * @fn      ETH_PhyAbility_Set
 *
 * @brief   Set phy ability if necessary
 *
 * @return  none.
 */
void ETH_PhyAbility_Set( void )
{
#if( PHY_MODE != USE_10M_BASE )
    uint16_t RegValue;
    /* Limit Phy Speed If Necessary */
    if( USBHS_DevSpeed == USBHS_SPEED_FULL )
    {
        /* Read ADAR Register */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANAR );
        RegValue |= (ANAR_10BASE_T | ANAR_10BASE_T_FD);
        RegValue &= ~(ANAR_100BASE_TX_FD | ANAR_100BASE_TX);
        /* Disable ANAR_100BASE_TX_FD&ANAR_100BASE_TX */
        ETH_WritePHYRegister( PHY_ADDRESS, PHY_ANAR, RegValue );
#if( PHY_MODE == USE_MAC_RGMII )
        /* RTL8211FS 0x09 : GBCR (1000Base-T Control Register) */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, 0x09 );
        RegValue &= ~(1<<9);/* Bit9 : 1000Base-T Full duplex not advertised */
        ETH_WritePHYRegister( PHY_ADDRESS, 0x09, RegValue );
#endif
        printf( "USB Full-Speed\r\n" );
        printf( "Local Phy Ability: 10M, Full Duplex\r\n" );
    }
    else
    {
        /* Read ADAR Register */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANAR );
        RegValue |= (ANAR_10BASE_T | ANAR_10BASE_T_FD);
        RegValue |= (ANAR_100BASE_TX | ANAR_100BASE_TX_FD);
        /* Enable ANAR_100BASE_TX_FD&ANAR_100BASE_TX */
        ETH_WritePHYRegister( PHY_ADDRESS, PHY_ANAR, RegValue );
        printf( "USB High-Speed\r\n" );
#if( PHY_MODE == USE_MAC_RGMII )
        printf( "LocalPhy Ability: 10/100/1000M, Full Duplex\r\n" );
#else
        printf( "LocalPhy Ability: 10/100M, Full Duplex\r\n" );
#endif
    }
#else
    printf( "LocalPhy Ability: 10M(Fixed), Full Duplex\r\n" );
#endif
}
