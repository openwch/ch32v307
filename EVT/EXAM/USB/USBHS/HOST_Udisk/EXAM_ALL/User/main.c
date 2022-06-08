/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
  *功能1：
  USBHS的简易枚举过程例程U盘文件扇区读写示例程序：
    演示查找/C51/CH559HFT.C文件，如果找到则扇区读出文件内容；
    演示创建/LIUNEWFILE.TXT文件，循环扇区写入指定字符串，并修改文件属性；
    演示删除/OLD.TXT文件；
  *功能2：
    与功能1同时使用使用一个。
    演示查找/C51/CH559HFT.C文件，如果找到则扇区读出文件内容；
    演示创建/LIUNEWFILE.TXT文件，循环扇区写入指定字符串，并修改文件属性；
    演示删除/OLD.TXT文件；
  *功能3：
  USBHS的U盘目录创建文件例程：
    该程序在U盘根目录下新建/YEAR2004子目录，然后在该目录下新建文件DEMO2004.TXT并字节写入数据;
    最后在/YEAR2004目录下创建子目录/MONTH05目录
  *功能4：
  USBHS的U盘文件枚举例程：
    查找/C51/CH559HFT.C文件，如果没有找到则枚举根目录下不超过10000个文件包括目录(实际不限制上限)
  *功能4：
  USBHS的U盘目录创建长文件名文件例程
  OTG_FS_DM(PA11)、OTG_FS_DP(PA12)。
    创建长文件名
    支持: FAT12/FAT16/FAT32
*/

#include "debug.h"
#include "ch32v30x_usbhs_host.h"
#include "UDisk_LongName.h"
#include "string.h"
#include "CHRV3UFI.h"

__attribute__ ((aligned(4))) UINT8   RxBuffer[ MAX_PACKET_SIZE  ];  // IN, must even address
__attribute__ ((aligned(4))) UINT8   TxBuffer[ MAX_PACKET_SIZE  ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpTxBuf[ MAX_PACKET_SIZE ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpRxBuf[ MAX_PACKET_SIZE ];  // OUT, must even addres
__attribute__ ((aligned(4))) UINT8   buf[ 128 ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   MY_DATA_BUF[ DISK_BASE_BUF_LEN ];   /* MY_DATA_BUF指向外部RAM的磁盘数据缓冲区,缓冲区长度为至少一个扇区的长度,用于用户数据缓存*/

/* 检查操作状态,如果错误则显示错误代码并停机 */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* 操作成功 */
    }
    printf( "Error:%02x\r\n", iError );  /* 显示错误 */
    /* 遇到错误后,应该分析错误码以及CHRV3DiskStatus状态,例如调用CHRV3DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
         建议出错后的处理步骤:
         1、调用一次CHRV3DiskReady,成功则继续操作,例如Open,Read/Write等
         2、如果CHRV3DiskReady不成功,那么强行将从头开始操作 */
    while(1)
    {  }
}

/*******************************************************************************
* Function Name  : CreateDirectory
* Description    : 新建目录并打开,如果目录已经存在则直接打开，目录名在mCmdParam.Create.mPathName中,与文件名规则相同
* Input          :
* Output         : None
* Return         : ERR_SUCCESS = 打开目录成功或者创建目录成功,
                   ERR_FOUND_NAME = 已经存在同名文件,
                   ERR_MISS_DIR = 路径名无效或者上级目录不存在
*******************************************************************************/
UINT8   CreateDirectory( void )
{
    UINT8   i, j;
    UINT32  UpDirCluster;
    PUINT8 DirXramBuf;
    UINT8  *DirConstData;
    j = 0xFF;
    for ( i = 0; i != sizeof( mCmdParam.Create.mPathName ); i ++ )    //检查目录路径
    {
        if ( mCmdParam.Create.mPathName[ i ] == 0 )
        {
            break;
        }
        if ( mCmdParam.Create.mPathName[ i ] == PATH_SEPAR_CHAR1 || mCmdParam.Create.mPathName[ i ] == PATH_SEPAR_CHAR2 )
        {
            j = i;                                                     //记录上级目录
        }
    }
    i = ERR_SUCCESS;
    if ( (j == 0) || ((j == 2) && (mCmdParam.Create.mPathName[1] == ':') ))
    {
        UpDirCluster = 0;                                              //在根目录下创建子目录
    }
    else
    {
        if ( j != 0xFF )                                               //对于绝对路径应该获取上级目录的起始簇号
        {
            mCmdParam.Create.mPathName[ j ] = 0;
            i = CHRV3FileOpen( );                                      //打开上级目录
            if ( i == ERR_SUCCESS )
            {
                i = ERR_MISS_DIR;                                      //是文件而非目录
            }
            else if ( i == ERR_OPEN_DIR )
            {
                i = ERR_SUCCESS;                                       //成功打开上级目录
            }
            mCmdParam.Create.mPathName[ j ] = PATH_SEPAR_CHAR1;        //恢复目录分隔符
        }
        UpDirCluster = CHRV3vStartCluster;                             //保存上级目录的起始簇号
    }
    if ( i == ERR_SUCCESS )                                            //成功获取上级目录的起始簇号
    {
        i = CHRV3FileOpen( );                                          //打开本级子目录
        if ( i == ERR_SUCCESS )
        {
            i = ERR_FOUND_NAME;                                        //是文件而非目录
        }
        else if ( i == ERR_OPEN_DIR )
        {
            i = ERR_SUCCESS;                                           //目录已经存在
        }
        else if ( i == ERR_MISS_FILE )                                 //目录不存在,可以新建
        {
            i = CHRV3FileCreate( );                                    //以创建文件的方法创建目录
            if ( i == ERR_SUCCESS )
            {
                if ( pDISK_FAT_BUF == pDISK_BASE_BUF )
                {
                    memset(pDISK_FAT_BUF,0,CHRV3vSectorSize);     //如果FILE_DATA_BUF与DISK_BASE_BUF合用则必须清除磁盘缓冲区
                }
                DirXramBuf = pDISK_FAT_BUF;                            //文件数据缓冲区
                DirConstData = ".          \x10\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x21\x30\x0\x0\x0\x0\x0\x0..         \x10\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x21\x30\x0\x0\x0\x0\x0\x0";
                for ( i = 0x40; i != 0; i -- )                         //目录的保留单元,分别指向自身和上级目录
                {
                    *DirXramBuf = *DirConstData;
                    DirXramBuf ++;
                    DirConstData ++;
                }
                *(pDISK_FAT_BUF+0x1A) = ( (PUINT8)&CHRV3vStartCluster )[3];//自身的起始簇号
                *(pDISK_FAT_BUF+0x1B) = ( (PUINT8)&CHRV3vStartCluster )[2];
                *(pDISK_FAT_BUF+0x14) = ( (PUINT8)&CHRV3vStartCluster )[1];
                *(pDISK_FAT_BUF+0x15) = ( (PUINT8)&CHRV3vStartCluster )[0];
                *(pDISK_FAT_BUF+0x20+0x1A) = ( (PUINT8)&UpDirCluster )[3];//上级目录的起始簇号
                *(pDISK_FAT_BUF+0x20+0x1B) = ( (PUINT8)&UpDirCluster )[2];
                *(pDISK_FAT_BUF+0x20+0x14) = ( (PUINT8)&UpDirCluster )[1];
                *(pDISK_FAT_BUF+0x20+0x15) = ( (PUINT8)&UpDirCluster )[0];
//              for ( count = 0x40; count != CHRV3vSectorSizeH*256; count ++ ) {  /* 清空目录区剩余部分 */
//                  *DirXramBuf = 0;
//                  DirXramBuf ++;
//              }
                mCmdParam.Write.mSectorCount = 1;
                mCmdParam.Write.mDataBuffer = pDISK_FAT_BUF;                //指向文件数据缓冲区的起始地址
                i = CHRV3FileWrite( );                                      //向文件写入数据
                if ( i == ERR_SUCCESS )
                {
                    DirXramBuf = pDISK_FAT_BUF;
                    for ( i = 0x40; i != 0; i -- )                          //清空目录区
                    {
                        *DirXramBuf = 0;
                        DirXramBuf ++;
                    }
                    for ( j = 1; j != CHRV3vSecPerClus; j ++ )
                    {
                        if ( pDISK_FAT_BUF == pDISK_BASE_BUF )
                        {
                            memset(pDISK_FAT_BUF,0,CHRV3vSectorSize);   //如果FILE_DATA_BUF与DISK_BASE_BUF合用则必须清除磁盘缓冲区
                        }
                        mCmdParam.Write.mSectorCount = 1;
                        mCmdParam.Write.mDataBuffer = pDISK_FAT_BUF;         //指向文件数据缓冲区的起始地址
                        i = CHRV3FileWrite( );                               //清空目录的剩余扇区
                        if ( i != ERR_SUCCESS )
                        {
                            break;
                        }
                    }
                    if ( j == CHRV3vSecPerClus )                              //成功清空目录
                    {
                        mCmdParam.Modify.mFileSize = 0;                       //目录的长度总是0
                        mCmdParam.Modify.mFileDate = 0xFFFF;
                        mCmdParam.Modify.mFileTime = 0xFFFF;
                        mCmdParam.Modify.mFileAttr = 0x10;                    //置目录属性
                        i = CHRV3FileModify( );                               //将文件信息修改为目录
                    }
                }
            }
        }
    }
    return( i );
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8  ret,s,SecCount;
    UINT8  i;
    UINT16 tmp;
    UINT8  tmpbuf[64];
    UINT8  *pCodeStr;
    UINT16 j;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf("USBHS HOST Test EXAM ALL\r\n");
	pHOST_TX_RAM_Addr = TxBuffer;
	pHOST_RX_RAM_Addr = RxBuffer;
	USBHS_HostInit(ENABLE);
    ret = CHRV3LibInit();
	while(1)
	{
        if( USBHSH->INT_FG & USBHS_DETECT_FLAG )
        {
            USBHSH->INT_FG = USBHS_DETECT_FLAG;
            if( USBHSH->MIS_ST & USBHS_ATTCH )
            {
                ret = USBHS_HostEnum( );
                if( ret == ERR_SUCCESS )
                {
                    printf( "Enum Succeed\r\n" );
                    Delay_Ms(100);
                    // U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH103子程序库继续完成后续工作
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    for ( i = 0; i != 10; i ++ )
                    {
                        printf( "Wait DiskReady\r\n" );
                        s = CHRV3DiskReady( );                                 //等待U盘准备好
                        if ( s == ERR_SUCCESS )
                        {
                            printf("CHRV3DiskStatus:%02x\n",CHRV3DiskStatus);
#if 1
#if 0
                            /*====================== 以下演示字节操作的过程 ==============================*/
                            printf( "字节操作的过程 \n" );
                            /* 读文件 */
                            strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/NEWFILE.C" );     //设置将要操作的文件路径和文件名/C51/NEWFILE.C
                            ret = CHRV3FileOpen( );                                       //打开文件
                            if ( ret == ERR_MISS_DIR || ret == ERR_MISS_FILE )            //没有找到文件
                            {
                                //创建文件演示
                                printf( "Find No File And Create\r\n" );
                                strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* 新文件名,在根目录下,中文文件名 */
                                ret = CHRV3FileCreate( );                                        /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
//                                mStopIfError( ret );
                                printf( "ByteWrite\n" );
                                //实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
                                i = sprintf( (PCHAR)buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,简单演示功能。\xd\xa");  /*演示 */
                                for(t=0; t<10; t++)
                                {
                                    mCmdParam.ByteWrite.mByteCount = i;                          /* 指定本次写入的字节数 */
                                    mCmdParam.ByteWrite.mByteBuffer = buf;                       /* 指向缓冲区 */
                                    ret = CHRV3ByteWrite( );                                       /* 以字节为单位向文件写入数据 */
//                                    mStopIfError( ret );
                                    printf("成功写入 %02X次\r\n",(UINT16)t);
                                }
                                //演示修改文件属性
                                printf( "Modify\r\n" );
                                mCmdParam.Modify.mFileAttr = 0xff;   //输入参数: 新的文件属性,为0FFH则不修改
                                mCmdParam.Modify.mFileTime = 0xffff;   //输入参数: 新的文件时间,为0FFFFH则不修改,使用新建文件产生的默认时间
                                mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //输入参数: 新的文件日期: 2015.05.18
                                mCmdParam.Modify.mFileSize = 0xffffffff;  // 输入参数: 新的文件长度,以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改
                                i = CHRV3FileModify( );   //修改当前文件的信息,修改日期
//                                mStopIfError( i );
                                printf( "Close\r\n" );
                                mCmdParam.Close.mUpdateLen = 1;     /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
                                i = CHRV3FileClose( );
//                                mStopIfError( i );

                                /* 删除某文件 */
//                                    printf( "Erase\n" );
//                                    strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //将被删除的文件名,在根目录下
//                                    i = CHRV3FileErase( );  //删除文件并关闭
//                                    if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //显示错误
                            }
                            else
                            {
                                //如果希望将新数据添加到原文件的尾部,可以移动文件指针
                                ///////////一、写入文件/////////////////////////////////////////
                                printf( "ByteWrite\r\n" );
                                mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  //移到文件的尾部
                                CHRV3ByteLocate( );
                                //实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
                                i = sprintf( (PCHAR)buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,简单演示功能。\xd\xa");  /*演示 */
                                for(t=0; t<10; t++)
                                {
                                    mCmdParam.ByteWrite.mByteCount = i;                          /* 指定本次写入的字节数 */
                                    mCmdParam.ByteWrite.mByteBuffer = buf;                       /* 指向缓冲区 */
                                    ret = CHRV3ByteWrite( );                                       /* 以字节为单位向文件写入数据 */
//                                    mStopIfError( ret );
                                    printf("成功写入 %02X次\r\n",(UINT16)t);
                                }

                                ///////////二、读取文件前N字节/////////////////////////////////////////
                                TotalCount = 100;                                      //设置准备读取总长度100字节
                                printf( "读出的前%d个字符是:\r\n",TotalCount );
                                while ( TotalCount )
                                {                                                      //如果文件比较大,一次读不完,可以再调用CH103ByteRead继续读取,文件指针自动向后移动
                                    if ( TotalCount > (MAX_PATH_LEN-1) ) t = MAX_PATH_LEN-1;/* 剩余数据较多,限制单次读写的长度不能超过 sizeof( mCmdParam.Other.mBuffer ) */
                                    else t = TotalCount;                                 /* 最后剩余的字节数 */
                                    mCmdParam.ByteRead.mByteCount = t;                   /* 请求读出几十字节数据 */
                                    mCmdParam.ByteRead.mByteBuffer= &buf[0];
                                    ret = CHRV3ByteRead( );                                /* 以字节为单位读取数据块,单次读写的长度不能超过MAX_BYTE_IO,第二次调用时接着刚才的向后读 */
                                    TotalCount -= mCmdParam.ByteRead.mByteCount;           /* 计数,减去当前实际已经读出的字符数 */
                                    for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ )
                                    {
                                        printf( "%c", mCmdParam.ByteRead.mByteBuffer[i] );  /* 显示读出的字符 */
                                    }
                                    if ( mCmdParam.ByteRead.mByteCount < t )
                                    {
                                        /* 实际读出的字符数少于要求读出的字符数,说明已经到文件的结尾 */
                                        printf( "\r\n" );
                                        printf( "文件已经结束\r\n" );
                                        break;
                                    }
                                }

                              ///////////三、从指定位置读取文件N字节/////////////////////////////////////////
                                printf( "Close\r\n" );
                                i = CHRV3FileClose( );                                    /* 关闭文件 */
//                                mStopIfError( i );
                                printf( "字节操作演示完成\r\n" );
                                printf( "\r\n" );
                            }
#else
                            /*====================== 以下演示扇区操作的过程 ==============================*/
                            printf( "演示扇区操作\r\n" );
#if DISK_BASE_BUF_LEN
//                            if ( DISK_BASE_BUF_LEN < CHRV3vSectorSize )
//                            {  /* 检查磁盘数据缓冲区是否足够大,CHRV3vSectorSize是U盘的实际扇区大小 */
//                                printf( "Too large sector size\n" );
//                                goto UnknownUsbDevice;
//                            }
#endif
                            /* 查询磁盘物理容量 */
//                            printf( "DiskSize\n" );
//                            i = CHRV3DiskQuery( );
//                            mStopIfError( i );
//                            printf( "TotalSize = %u MB \n", (unsigned int)( mCmdParam.Query.mTotalSector * CHRV3vSectorSizeH / 2 ) );  //显示为以MB为单位的容量

                            /* 读取原文件 */
                            printf( "Open\r\n" );
                            strcpy( mCmdParam.Open.mPathName, "/C51/CHRV3HFT.C" );//文件名,该文件在C51子目录下
                            s = CHRV3FileOpen( );                       //打开文件
                            if ( s == ERR_MISS_DIR || s == ERR_MISS_FILE )//没有找到文件
                            {
                                printf( "没有找到文件\r\n" );
                            }
                            else                                        //找到文件或者出错
                            {
                                printf( "Query\r\n" );
                                i = CHRV3FileQuery( );                  //查询当前文件的信息
                                mStopIfError( i );
                                printf( "Read\r\n" );
                                CHRV3vFileSize = CHRV3vFileSize+(sizeof( MY_DATA_BUF )-1);    //原文件的长度
                                SecCount = CHRV3vFileSize/ sizeof( MY_DATA_BUF )  ;//计算文件的扇区数,因为读写是以扇区为单位的,先加CHRV3vSectorSize-1是为了读出文件尾部不足1个扇区的部分
                                printf( "Size=%ld, Sec=%d\r\n", CHRV3vFileSize, (UINT16)SecCount );
                                while(SecCount--)
                                {
                                    mCmdParam.Read.mSectorCount = sizeof( MY_DATA_BUF )/512;  //读取全部数据,如果超过2个扇区则只读取2个扇区
                                    mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];//指向文件数据缓冲区的起始地址
                                    i = CHRV3FileRead( );                    //从文件读取数据
                                    mStopIfError( i );
                                    if(SecCount == 0) break;
                                    /*
                                    for(tmp=0; tmp<sizeof( MY_DATA_BUF ); tmp++)
                                    {
                                    printf("%02X ",(UINT16)MY_DATA_BUF[tmp]);
                                    }
                                    printf("\n");
                                    */
                                }
                                tmp = (CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1))%sizeof( MY_DATA_BUF );
                                if((tmp == 0)&&(CHRV3vFileSize != 0)) tmp = sizeof( MY_DATA_BUF );
                                CHRV3vFileSize = CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1);    //恢复原文件的长度
//                                for(i=0; i<tmp; i++)
//                                {
//                                  printf("%02X ",(UINT16)MY_DATA_BUF[i]);
//                                }
//                                printf("\n");

//                                 如果文件比较大,一次读不完,可以再调用CHRV3FileRead继续读取,文件指针自动向后移动
//                                 while ( 1 )
//                                 {
//                                   c = 4;   每次读取4个扇区,缓冲区定义的越大，一次读取的扇区数越多
//                                   mCmdParam.Read.mSectorCount = c;   指定读取的扇区数
//                                   mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  指向文件数据缓冲区的起始地址
//                                   CHRV3FileRead();   读完后文件指针自动后移 处理数据
//                                   if ( mCmdParam.Read.mSectorCount < c ) break;   实际读出的扇区数较小则说明文件已经结束
//                                 }
//                                                                   如果希望从指定位置开始读写,可以移动文件指针
//                                  mCmdParam.Locate.mSectorOffset = 3;  跳过文件的前3个扇区开始读写
//                                  i = CHRV3FileLocate( );
//                                  mCmdParam.Read.mSectorCount = 10;
//                                  mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  指向文件数据缓冲区的起始地址
//                                  CHRV3FileRead();   直接读取从文件的第(CHRV3vSectorSizeH*256*3)个字节开始的数据,前3个扇区被跳过
//                                                                   如果希望将新数据添加到原文件的尾部,可以移动文件指针
//                                  i = CHRV3FileOpen( );
//                                  mCmdParam.Locate.mSectorOffset = 0xffffffff;  移到文件的尾部,以扇区为单位,如果原文件是3字节,则从CHRV3vSectorSizeH个字节处开始添加
//                                  i = CHRV3FileLocate( );
//                                  mCmdParam.Write.mSectorCount = 10;
//                                  mCmdParam.Write.mDataBuffer = &MY_DATA_BUF[0];
//                                  CHRV3FileWrite();   在原文件的后面添加数据
//                                                                   使用CHRV3FileRead可以自行定义数据缓冲区的起始地址
//                                  mCmdParam.Read.mSectorCount = 2;
//                                  mCmdParam.Read.mDataBuffer = 0x50;  将读出的数据放到50H开始的缓冲区中，需要指定缓冲区的起始地址
//                                  CHRV3FileRead();   从文件中读取2个扇区到指定缓冲区
//                                                                    使用CHRV3FileWrite可以自行定义数据缓冲区的起始地址
//                                  mCmdParam.Wiite.mSectorCount = 2;
//                                  mCmdParam.Write.mDataBuffer = 0x50;  将50H开始的缓冲区中的数据写入
//                                  CHRV3FileWrite();   将指定缓冲区中的数据写入2个扇区到文件中

                                printf( "Close\r\n" );
                                i = CHRV3FileClose( );                            //关闭文件
                                mStopIfError( i );
                            }
                            printf( "Create\r\n" );
                            strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );//新文件名,在根目录下,中文文件名
                            s = CHRV3FileCreate( );                               //新建文件并打开,如果文件已经存在则先删除后再新建 */
                            mStopIfError( s );
                            printf( "Write\r\n" );
                            strcpy( tmpbuf, "0000ABCDEFGHIJKLMNOPQRSTUVWXYZ\xd\xa" );//准备写文件数据
                            for(i=0; i<(DISK_BASE_BUF_LEN/sizeof(tmpbuf)); i++)
                            {
                                tmp=i*sizeof(tmpbuf);
                                strcpy(&MY_DATA_BUF[tmp],tmpbuf);
                            }
                            for(tmp=0; tmp<sizeof(MY_DATA_BUF); tmp++)
                            {
                                printf("%02X",(UINT16)MY_DATA_BUF[tmp]);
                            }
                            printf("\r\n");
                            for(s=0; s<10; s++)
                            {
                                mCmdParam.Write.mSectorCount = 1;                 //写入所有扇区的数据
                                mCmdParam.Write.mDataBuffer = &MY_DATA_BUF[0];    //指向文件数据缓冲区的起始地址
                                i = CHRV3FileWrite( );                            //向文件写入数据
                                mStopIfError( i );
                                printf("成功写入 %02X次\r\n",(UINT16)s);
                            }
                            /* printf( "Modify\n" );
                               mCmdParam.Modify.mFileAttr = 0xff;   输入参数: 新的文件属性,为0FFH则不修改
                               mCmdParam.Modify.mFileTime = 0xffff;   输入参数: 新的文件时间,为0FFFFH则不修改,使用新建文件产生的默认时间
                               mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  输入参数: 新的文件日期: 2015.05.18
                               mCmdParam.Modify.mFileSize = 0xffffffff;   输入参数: 新的文件长度,以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改
                               i = CHRV3FileModify( );   修改当前文件的信息,修改日期
                               mStopIfError( i );
                            */
                            printf( "Close\r\n" );
                            mCmdParam.Close.mUpdateLen = 1;                        //自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度
                            i = CHRV3FileClose( );
                            mStopIfError( i );
                            /* 删除某文件 */
                            /*printf( "Erase\n" );
                              strcpy( mCmdParam.Create.mPathName, "/OLD.TXT" );  将被删除的文件名,在根目录下
                              i = CHRV3FileErase( );  删除文件并关闭
                              if ( i != ERR_SUCCESS ) printf( "Error File not exist: %02X\n", (UINT16)i );  显示错误
                            */
                            printf( "扇区操作演示完成\r\n" );
#endif
#endif
#if 1
                            /*==================== 以下演示目录创建文件例程 ============================*/
                            printf( "创建和读取文件\r\n" );
                            printf( "Create Level 1 Directory /YEAR2004 \r\n" );
                            strcpy( mCmdParam.Create.mPathName, "/YEAR2004" );           //目录名,该目录建在根目录下
                            i = CreateDirectory( );                                      //新建或者打开目录
                            mStopIfError( i );
                            /* 目录新建或者打开成功,下面在这个子目录中新建一个演示文件 */
                            printf( "Create New File /YEAR2004/DEMO2004.TXT \r\n" );
                            strcpy( mCmdParam.Create.mPathName, "/YEAR2004/DEMO2004.TXT" );//文件名
                            i = CHRV3FileCreate( );                                       //新建文件并打开,如果文件已经存在则先删除后再新建
                            mStopIfError( i );
                            printf( "Write some data to file DEMO2004.TXT \r\n" );
                            i = sprintf( buf, "演示文件\xd\xa" );
                            mCmdParam.ByteWrite.mByteCount = i;                           //指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO
                            mCmdParam.ByteWrite.mByteBuffer = buf;                        //指向缓冲区
                            i = CHRV3ByteWrite( );                                        //以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO
                            mStopIfError( i );
                            printf( "Close file DEMO2004.TXT \r\n" );
                            mCmdParam.Close.mUpdateLen = 1;                               //自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度
                            i = CHRV3FileClose( );
                            mStopIfError( i );
                            /* 下面新建二级子目录,方法与前面的一级子目录完全相同 */
                            printf( "Create Level 2 Directory /YEAR2004/MONTH05 \r\n" );
                            strcpy( mCmdParam.Create.mPathName, "/YEAR2004/MONTH05" );    //目录名,该目录建在YEAR2004子目录下,YEAR2004目录必须事先存在
                            i = CreateDirectory( );                                       //新建或者打开目录
                            mStopIfError( i );
                            printf( "Close\r\n" );
                            mCmdParam.Close.mUpdateLen = 0;                               //对于目录不需要自动更新文件长度
                            i = CHRV3FileClose( );                                        //关闭目录,目录不需要关闭,关闭只是为了防止下面误操作
                            mStopIfError( i );
                            printf( "目录创建文件例程操作演示完成\r\n" );
#endif
#if 1
                            /*==================== 以下演示创建及读取长文件名 ============================*/
                            printf( "创建和读取长文件名\r\n" );
                            // 复制长文件名(UNICODE 大端)到LongNameBuf里
                            memcpy( LongNameBuf, LongName, LongName_Len );
                            // 末尾用两个0表示结束
                            LongNameBuf[LongName_Len] = 0x00;
                            LongNameBuf[LongName_Len + 1] = 0x00;
                            // 该长文件名的ANSI编码短文件名(8+3格式)
                            strcpy( mCmdParam.Create.mPathName, "\\长文件名.TXT" );
                            i = CHRV3CreateLongName( );
                            if( i == ERR_SUCCESS )
                                printf( "Created Long Name OK!!\r\n" );
                            else
                                printf( "Error Code: %02X\n", (UINT16)i );

                            printf( "Get long Name#\r\n" );
                            strcpy( mCmdParam.Open.mPathName, "\\长文件名.TXT" );
                            // 以上需要输入文件名的完整路径
                            i = CHRV3GetLongName( );
                            if( i == ERR_SUCCESS )
                            {
                                // 长文件名收集完成,以UNICODE编码方式(按UNICODE_ENDIAN定义)
                                // 存放在LongNameBuf缓冲里,长文件名最后用两个0结束.
                                // 以下显示缓冲区里所有数据
                                printf( "LongNameBuf: " );
                                for( j=0; j!=LONG_NAME_BUF_LEN; j++ )
                                    printf( "%02X ", (UINT16)LongNameBuf[j] );
                                printf( "\r\n" );
                            }
                            else
                            {
                                printf( "Error Code: %02X\r\n", (UINT16)i );
                            }
                            printf( "长文件名操作操作演示完成\r\n" );
#endif
#if 1
                            /*==================== 以下演示枚举根目录下文件操作 ============================*/
                            printf( "枚举文件\r\n" );
                            printf( "open\r\n" );
                            strcpy( mCmdParam.Open.mPathName, "/C51/CHRV3HFT.C" );//文件名,该文件在C51子目录下
                            s = CHRV3FileOpen( );                        //打开文件
                            /* 列出文件 */
                            if ( s == ERR_MISS_DIR )
                            {
                                printf("不存在该文件则列出所有文件\r\n");  //C51子目录不存在则列出根目录下的所有文件
                                pCodeStr = "/*";
                            }
                            else
                            {
                                pCodeStr = "/C51/*";                     //* CHRV3HFT.C文件不存在则列出\C51子目录下的以CHRV3开头的文件
                            }
                            printf( "List file %s\r\n", pCodeStr );
                            for ( j = 0; j < 10000; j ++ )               //最多搜索前10000个文件,实际上没有限制
                            {
                                strcpy( mCmdParam.Open.mPathName, pCodeStr );//搜索文件名,*为通配符,适用于所有文件或者子目录
                                i = strlen( mCmdParam.Open.mPathName );
                                mCmdParam.Open.mPathName[ i ] = 0xFF;    //根据字符串长度将结束符替换为搜索的序号,从0到254,如果是0xFF即255则说明搜索序号在CHRV3vFileSize变量中
                                CHRV3vFileSize = j;                      //指定搜索/枚举的序号
                                i = CHRV3FileOpen( );                    //打开文件,如果文件名中含有通配符*,则为搜索文件而不打开
                                /* CHRV3FileEnum 与 CHRV3FileOpen 的唯一区别是当后者返回ERR_FOUND_NAME时那么对应于前者返回ERR_SUCCESS */
                                if ( i == ERR_MISS_FILE )
                                {
                                    break;                                //再也搜索不到匹配的文件,已经没有匹配的文件名
                                }
                                if ( i == ERR_FOUND_NAME )
                                {
                                    /* 搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中 */
                                    printf( "  match file %04d#: %s\r\n", (unsigned int)j, mCmdParam.Open.mPathName );//显示序号和搜索到的匹配文件名或者子目录名
                                    continue;                             //继续搜索下一个匹配的文件名,下次搜索时序号会加1
                                }
                                else
                                {
                                    /* 出错 */
                                    mStopIfError( i );
                                    break;
                                }
                            }
                            printf( "Close\r\n" );
                            i = CHRV3FileClose( );                         //关闭文件
                            printf( "U盘演示完成\r\n" );

#endif
                            break;
                        }
                        else
                        {
                            printf("s = %02x\r\n",s);
                            printf("CHRV3DiskStatus:%02x\r\n",CHRV3DiskStatus);
                        }
//                        Delay_Ms( 50 );
                    }
                }
                else
                {
                    printf( "Enum Failed:%02x\r\n", ret );
                }
            }
            else
            {
                USBHS_HostInit(DISABLE);
                USBHS_HostInit(ENABLE);
                CHRV3DiskStatus = DISK_DISCONNECT;
                printf("disconnect\r\n");
            }
        }
	}
}




