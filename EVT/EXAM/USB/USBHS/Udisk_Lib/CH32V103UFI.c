/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CHRV3 **
**  KEIL423, gcc 8.20          **
*****************************************
*/
/* CHRV3 U盘主机文件系统接口, 支持: FAT12/FAT16/FAT32 */

//#define DISK_BASE_BUF_LEN		512	/* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在本文件中定义缓冲区,而由应用程序在调用CHRV3LibInit之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

//#define NO_DEFAULT_ACCESS_SECTOR	    1		/* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_DISK_CONNECT		1		/* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */
//#include "CHRV3SFR.H"

#include "debug.h"
#include "ch32v30x.h"
#include "usb_host_config.h"
#include "CHRV3UFI.h"

uint8_t USBHostTransact( uint8_t endp_pid, uint8_t tog, uint32_t timeout )
{
#if DEF_USB_PORT_FS_EN
    uint8_t  r, trans_rerty;
    uint16_t i;
    USBOTG_H_FS->HOST_TX_CTRL = USBOTG_H_FS->HOST_RX_CTRL = 0;
    if( tog & 0x80 )
    {
        USBOTG_H_FS->HOST_RX_CTRL = 1<<2;
    }
    if( tog & 0x40 )
    {
        USBOTG_H_FS->HOST_TX_CTRL = 1<<2;
    }
    trans_rerty = 0;
    do
    {
        USBOTG_H_FS->HOST_EP_PID = endp_pid;       // Specify token PID and endpoint number
        USBOTG_H_FS->INT_FG = USBFS_UIF_TRANSFER;  // Allow transmission
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBOTG_H_FS->INT_FG & USBFS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBOTG_H_FS->HOST_EP_PID = 0x00;  // Stop USB transfer
        if( ( USBOTG_H_FS->INT_FG & USBFS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }
        else
        {
            /* Complete transfer */
            if( USBOTG_H_FS->INT_ST & USBFS_UIS_TOG_OK )
            {
                return ERR_SUCCESS;
            }
            r = USBOTG_H_FS->INT_ST & USBFS_UIS_H_RES_MASK;  // USB device answer status
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }
            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_rerty;
            }
            else switch ( endp_pid >> 4 )
            {
                case USB_PID_SETUP:
                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) && ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        Delay_Us( 15 );
        if( USBOTG_H_FS->INT_FG & USBFS_UIF_DETECT )
        {
            Delay_Us( 200 );
            if( USBFSH_CheckRootHubPortEnable( ) == 0 )
            {
                return ERR_USB_DISCON;  // USB device disconnect event
            }
        }
    }while( ++trans_rerty < 10 );

    return ERR_USB_TRANSFER; // Reply timeout
#elif DEF_USB_PORT_HS_EN
    uint8_t   r, trans_retry;
    uint16_t  i;
    USBHSH->HOST_TX_CTRL = USBHSH->HOST_RX_CTRL = 0;
    if( tog & 0x80 )
    {
        USBHSH->HOST_RX_CTRL = 1<<3;
    }
    if( tog & 0x40 )
    {
        USBHSH->HOST_TX_CTRL = 1<<3;
    }
    trans_retry = 0;
    do
    {
        USBHSH->HOST_EP_PID = endp_pid; // Set the token for the host to send the packet
        USBHSH->INT_FG = USBHS_UIF_TRANSFER;
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBHSH->HOST_EP_PID = 0x00;
        if( ( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }

        if( USBHSH->INT_FG & USBHS_UIF_DETECT )
        {
            USBHSH->INT_FG = USBHS_UIF_DETECT;
            Delay_Us( 200 );
            if( USBHSH->MIS_ST & USBHS_UIF_TRANSFER )
            {
                if( USBHSH->HOST_CTRL & USBHS_UH_SOF_EN )
                {
                    return ERR_USB_CONNECT;
                }
            }
            else
            {
                return ERR_USB_DISCON;
            }
        }
        else if( USBHSH->INT_FG & USBHS_UIF_TRANSFER ) // The packet transmission was successful
        {
            r = USBHSH->INT_ST & USBHS_UIS_H_RES_MASK;
            if( ( endp_pid >> 4 ) == USB_PID_IN )
            {
                if( USBHSH->INT_ST & USBHS_UIS_TOG_OK )
                {
                    return ERR_SUCCESS; // Packet token match
                }
            }
            else
            {
                if( ( r == USB_PID_ACK ) || ( r == USB_PID_NYET ) )
                {
                    return ERR_SUCCESS;
                }
            }
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }

            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_retry;
            }
            else switch( endp_pid >> 4  )
            {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) || ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        else
        {
            USBHSH->INT_FG = USBHS_UIF_DETECT | USBHS_UIF_TRANSFER | USBHS_UIF_SUSPEND | USBHS_UIF_HST_SOF | USBHS_UIF_FIFO_OV | USBHS_UIF_SETUP_ACT;
        }
        Delay_Us( 15 );
    } while( ++trans_retry < 10 );

    return ERR_USB_TRANSFER;
#endif
}

uint8_t HostCtrlTransfer( uint8_t *DataBuf, uint8_t *RetLen )
{
    uint8_t  ret;
    uint16_t retlen;
    retlen = (uint16_t)(*RetLen);
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_CtrlTransfer( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, DataBuf, &retlen );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_CtrlTransfer( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, DataBuf, &retlen );
#endif
    return ret;
}

void CopySetupReqPkg( const char * pReqPkt )
{
    uint8_t i;
    for(i = 0; i != sizeof(USB_SETUP_REQ); i++)
    {
#if DEF_USB_PORT_FS_EN
        ((char *)pUSBFS_SetupRequest)[i] = *pReqPkt;
#elif DEF_USB_PORT_HS_EN
        ((char *)pUSBHS_SetupRequest)[i] = *pReqPkt;
#endif
        pReqPkt++;
    }
}

uint8_t CtrlGetDeviceDescrTB( void )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_GetDeviceDescr( &RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_GetDeviceDescr( &RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer );
#endif
    return ret;
}

uint8_t CtrlGetConfigDescrTB( void )
{
    uint16_t len;
    uint8_t  ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_GetConfigDescr( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer, 256, &len );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_GetConfigDescr( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer, 256, &len );
#endif
    return ret;
}

uint8_t CtrlSetUsbConfig( uint8_t cfg )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_SetUsbConfig( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, cfg );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_SetUsbConfig( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, cfg );
#endif
    return ret;
}

uint8_t CtrlSetUsbAddress( uint8_t addr )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_SetUsbAddress( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, addr );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_SetUsbAddress( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, addr );
#endif
    return ret;
}

uint8_t CtrlClearEndpStall( uint8_t endp )
{
    uint8_t ret;
#if DEF_USB_PORT_FS_EN
    ret = USBFSH_ClearEndpStall( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, endp );
#elif DEF_USB_PORT_HS_EN
    ret = USBHSH_ClearEndpStall( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, endp );
#endif
    return ret;
}

#ifndef FOR_ROOT_UDISK_ONLY
uint8_t CtrlGetHubDescr( void )
{

}

uint8_t HubGetPortStatus( uint8_t HubPortIndex )
{

}

uint8_t HubSetPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{

}

uint8_t HubClearPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{

}
#endif

CMD_PARAM_I	mCmdParam;						/* 命令参数 */
#if		DISK_BASE_BUF_LEN > 0
//uint8_t	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((at(BA_RAM+SZ_RAM/2)));	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
uint8_t	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));	        /* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
//UINT8	DISK_FAT_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));	            /* 外部RAM的磁盘FAT数据缓冲区,缓冲区长度为一个扇区的长度 */
#endif

/* 以下程序可以根据需要修改 */

#ifndef	NO_DEFAULT_ACCESS_SECTOR		/* 在应用程序中定义NO_DEFAULT_ACCESS_SECTOR可以禁止默认的磁盘扇区读写子程序,然后用自行编写的程序代替它 */
//if ( use_external_interface ) {  // 替换U盘扇区底层读写子程序
//    CHRV3vSectorSize=512;  // 设置实际的扇区大小,必须是512的倍数,该值是磁盘的扇区大小
//    CHRV3vSectorSizeB=9;   // 设置实际的扇区大小的位移数,512则对应9,1024对应10,2048对应11
//    CHRV3DiskStatus=DISK_MOUNTED;  // 强制块设备连接成功(只差分析文件系统)
//}

uint8_t	CHRV3ReadSector( uint8_t SectCount, uint8_t *DataBuf )  /* 从磁盘读取多个扇区的数据到缓冲区中 */
{
    uint8_t	retry;
//	if ( use_external_interface ) return( extReadSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* 数据传输长度 */
		pCBW -> mCBW_Flag = 0x80;
		pCBW -> mCBW_LUN = CHRV3vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
		pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
		pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
		pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CHRV3BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CHRV3IntStatus == ERR_SUCCESS ) {
			return( ERR_SUCCESS );
		}
		CHRV3IntStatus = CHRV3AnalyzeError( retry );
		if ( CHRV3IntStatus != ERR_SUCCESS ) {
			return( CHRV3IntStatus );
		}
	}
	return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}

#ifdef	EN_DISK_WRITE
uint8_t	CHRV3WriteSector( uint8_t SectCount, uint8_t *DataBuf )  /* 将缓冲区中的多个扇区的数据块写入磁盘 */
{
    uint8_t	retry;
//	if ( use_external_interface ) return( extWriteSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* 数据传输长度 */
		pCBW -> mCBW_Flag = 0x00;
		pCBW -> mCBW_LUN = CHRV3vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
		pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
		pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
		pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CHRV3BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CHRV3IntStatus == ERR_SUCCESS ) {
			Delay_Us( 200 );  /* 写操作后延时 */
			return( ERR_SUCCESS );
		}
		CHRV3IntStatus = CHRV3AnalyzeError( retry );
		if ( CHRV3IntStatus != ERR_SUCCESS ) {
			return( CHRV3IntStatus );
		}
	}
	return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR

#ifndef	NO_DEFAULT_DISK_CONNECT			/* 在应用程序中定义NO_DEFAULT_DISK_CONNECT可以禁止默认的检查磁盘连接子程序,然后用自行编写的程序代替它 */

/*
约定: USB设备地址分配规则(参考USB_DEVICE_ADDR)
地址值  设备位置
0x02    内置Root-HUB0下的USB设备或外部HUB
0x03    内置Root-HUB1下的USB设备或外部HUB
0x1x    内置Root-HUB0下的外部HUB的端口x下的USB设备,x为1~n
0x2x    内置Root-HUB1下的外部HUB的端口x下的USB设备,x为1~n
*/

#define		UHUB_DEV_ADDR	(USBHSH->DEV_AD)
#define		UHUB_MIS_STAT	(USBHSH->MIS_ST)
#define		UHUB_HOST_CTRL	(USBHSH->HOST_CTRL)
#define		UHUB_INT_FLAG	(USBHSH->INT_FG)
#define		bUMS_ATTACH		USBHS_UMS_DEV_ATTACH
#define		bUMS_SUSPEND	USBHS_UMS_SUSPEND

/* 检查磁盘是否连接 */
uint8_t	CHRV3DiskConnect( void )
{
    uint8_t	ums, devaddr;
	UHUB_DEV_ADDR = UHUB_DEV_ADDR & 0x7F;
	ums = UHUB_MIS_STAT;
	devaddr = UHUB_DEV_ADDR;
	if ( devaddr == USB_DEVICE_ADDR+1 )
	{
	    /* 内置Root-HUB下的USB设备 */
		if ( ums & bUMS_ATTACH )
		{
		    /* 内置Root-HUB下的USB设备存在 */
			if ( ( ums & bUMS_SUSPEND ) == 0 )
			{
			    /* 内置Root-HUB下的USB设备存在且未插拔 */
				return( ERR_SUCCESS );  /* USB设备已经连接且未插拔 */
			}
			else
			{
			    /* 内置Root-HUB下的USB设备存在 */
				CHRV3DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
				return( ERR_SUCCESS );  /* 外部HUB或USB设备已经连接或者断开后重新连接 */
			}
		}
		else
		{
		    /* USB设备断开 */
mDiskDisconn:
			CHRV3DiskStatus = DISK_DISCONNECT;
			return( ERR_USB_DISCON );
		}
	}
	else
	{
		goto mDiskDisconn;
	}
}
#endif  // NO_DEFAULT_DISK_CONNECT

#ifndef	NO_DEFAULT_FILE_ENUMER			/* 在应用程序中定义NO_DEFAULT_FILE_ENUMER可以禁止默认的文件名枚举回调程序,然后用自行编写的程序代替它 */
void xFileNameEnumer( void )			/* 文件名枚举回调子程序 */
{
/* 如果指定枚举序号CHRV3vFileSize为0xFFFFFFFF后调用FileOpen，那么每搜索到一个文件FileOpen都会调用本回调程序，
   回调程序xFileNameEnumer返回后，FileOpen递减CHRV3vFileSize并继续枚举直到搜索不到文件或者目录。建议做法是，
   在调用FileOpen之前定义一个全局变量为0，当FileOpen回调本程序后，本程序由CHRV3vFdtOffset得到结构FAT_DIR_INFO，
   分析结构中的DIR_Attr以及DIR_Name判断是否为所需文件名或者目录名，记录相关信息，并将全局变量计数增量，
   当FileOpen返回后，判断返回值如果是ERR_MISS_FILE或ERR_FOUND_NAME都视为操作成功，全局变量为搜索到的有效文件数。
   如果在本回调程序xFileNameEnumer中将CHRV3vFileSize置为1，那么可以通知FileOpen提前结束搜索。以下是回调程序例子 */
#if		0
    uint8_t			i;
    uint16_t	    FileCount;
	PX_FAT_DIR_INFO	pFileDir;
	uint8_t			*NameBuf;
	pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CHRV3vFdtOffset );  /* 当前FDT的起始地址 */
	FileCount = (UINT16)( 0xFFFFFFFF - CHRV3vFileSize );  /* 当前文件名的枚举序号,CHRV3vFileSize初值是0xFFFFFFFF,找到文件名后递减 */
	if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* 检查缓冲区是否足够存放,假定每个文件名需占用12个字节存放 */
		NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* 计算保存当前文件名的缓冲区地址 */
		for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* 复制文件名,长度为11个字符,未处理空格 */
//		if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* 判断是目录名 */
		NameBuf[ i ] = 0;  /* 文件名结束符 */
	}
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER

uint8_t	CHRV3LibInit( void )  /* 初始化CHRV3程序库,操作成功返回0 */
{
    uint8_t s;
    s = CHRV3GetVer( );
	if( s < CHRV3_LIB_VER )
	{
        return( 0xFF );  /* 获取当前子程序库的版本号,版本太低则返回错误 */
	}
	printf( "lib vision:%02x\r\n",s );
#if		DISK_BASE_BUF_LEN > 0
	pDISK_BASE_BUF = & DISK_BASE_BUF[0]; /* 指向外部RAM的磁盘数据缓冲区 */
	pDISK_FAT_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘FAT数据缓冲区,可以与pDISK_BASE_BUF合用以节约RAM */
//	pDISK_FAT_BUF = & DISK_FAT_BUF[0];   /* 指向外部RAM的磁盘FAT数据缓冲区,独立于pDISK_BASE_BUF以提高速度 */
/* 如果希望提高文件存取速度,那么可以在主程序中调用CHRV3LibInit之后,将pDISK_FAT_BUF重新指向另一个独立分配的与pDISK_BASE_BUF同样大小的缓冲区 */
#endif
	CHRV3DiskStatus = DISK_UNKNOWN;  /* 未知状态 */
	CHRV3vSectorSizeB = 9;  /* 默认的物理磁盘的扇区是512B */
	CHRV3vSectorSize = 512; /* 默认的物理磁盘的扇区是512B,该值是磁盘的扇区大小 */
	CHRV3vStartLba = 0;     /* 默认为自动分析FDD和HDD */
	CHRV3vPacketSize = 512;  /* USB存储类设备的最大包长度:64@FS,512@HS/SS,由应用程序初始化,枚举U盘后如果是高速或者超速那么及时更新为512 */

    pTX_DMA_A_REG = (uint32_t *)&(USBHSH->HOST_TX_DMA);  /* 指向发送DMA地址寄存器,由应用程序初始化 */
    pRX_DMA_A_REG = (uint32_t *)&(USBHSH->HOST_RX_DMA);  /* 指向接收DMA地址寄存器,由应用程序初始化 */
    pTX_LEN_REG = (uint16_t *)&(USBHSH->HOST_TX_LEN);    /* 指向发送长度寄存器,由应用程序初始化 */
    pRX_LEN_REG = (uint16_t *)&(USBHSH->RX_LEN);         /* 指向接收长度寄存器,由应用程序初始化 */

	return( ERR_SUCCESS );
}

void mDelaymS( uint16_t n )
{
	Delay_Ms(n);
}

