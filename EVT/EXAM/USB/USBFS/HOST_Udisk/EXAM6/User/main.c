/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/10/26
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
/*
 *@Note
 *@NoteOTG_FS_DM(PA11)、OTG_FS_DP(PA12)。
  USBFS的简易U盘文件扇区读写示例程序。
     支持: FAT12/FAT16/FAT32
*/
#include "stdio.h"
#include "string.h"
#include <ch32vf30x_usbfs_host.h>
#include "debug.h"
#include "CHRV3UFI.h"
__attribute__ ((aligned(4))) UINT8   buf[ 128 ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   MY_DATA_BUF[ DISK_BASE_BUF_LEN ];   /* MY_DATA_BUF指向外部RAM的磁盘数据缓冲区,缓冲区长度为至少一个扇区的长度,用于用户数据缓存*/
/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 检查操作状态,如果错误则显示错误代码并停机
* Input          : UINT8 iError
* Output         : None
* Return         : None
*******************************************************************************/
void    mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;                                                // 操作成功
    }
    printf( "Error: %02X\n", (UINT16)iError );                 // 显示错误
    /* 遇到错误后,应该分析错误码以及CH559DiskStatus状态,例如调用CH559DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CH559DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CH559DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH559DiskReady等) */
    while ( 1 )
    {
//      LED_TMP=0;                                             // LED闪烁
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
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
    UINT8 s,SecCount;
    UINT8 i;
    UINT16 tmp;
    UINT8 tmpbuf[32];
    USART_Printf_Init(115200);
    Delay_Init();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("USBFS HOST Test EXAM 6\r\n");
    pHOST_TX_RAM_Addr = TxBuffer;
    pHOST_RX_RAM_Addr = RxBuffer;
    USBOTG_HostInit(ENABLE);
    s = CHRV3LibInit();
    while(1)
    {
        s = ERR_SUCCESS;
        if ( USBOTG_H_FS->INT_FG & USBHD_UIF_DETECT )
        {
            USBOTG_H_FS->INT_FG = USBHD_UIF_DETECT ;

            s = AnalyzeRootHub( );
            if ( s == ERR_USB_CONNECT )
            {
                printf( "New Device In\r\n" );
                FoundNewDev = 1;
            }
            if( s == ERR_USB_DISCON )
            {
                printf( "Device Out\r\n" );
            }
        }

        if ( FoundNewDev || s == ERR_USB_CONNECT )
        {
            FoundNewDev = 0;
            Delay_Ms( 200 );
            s = USBOTG_HostEnum( RxBuffer );
            if ( s == ERR_SUCCESS )
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
                        printf("CHRV3DiskStatus:%02x\r\n",CHRV3DiskStatus);
    #if DISK_BASE_BUF_LEN
                        //if ( DISK_BASE_BUF_LEN < CHRV3vSectorSize ) {  /* 检查磁盘数据缓冲区是否足够大,CHRV3vSectorSize是U盘的实际扇区大小 */
                        //    printf( "Too large sector size\n" );
                        //    goto UnknownUsbDevice;
                        //  }
    #endif
                        /* 查询磁盘物理容量 */
                        /*      printf( "DiskSize\n" );
                                i = CHRV3DiskQuery( );
                                mStopIfError( i );
                                printf( "TotalSize = %u MB \n", (unsigned int)( mCmdParam.Query.mTotalSector * CHRV3vSectorSizeH / 2 ) );  显示为以MB为单位的容量
                        */
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
    /*                        for(tmp=0; tmp<sizeof( MY_DATA_BUF ); tmp++)
                              {
                                printf("%02X ",(UINT16)MY_DATA_BUF[tmp]);
                              }
                              printf("\n");
    */
                                                    }
                            tmp = (CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1))%sizeof( MY_DATA_BUF );
                            if((tmp == 0)&&(CHRV3vFileSize != 0)) tmp = sizeof( MY_DATA_BUF );
                            CHRV3vFileSize = CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1);    //恢复原文件的长度
    /*
                            for(i=0; i<tmp; i++)
                            {
                              printf("%02X ",(UINT16)MY_DATA_BUF[i]);
                            }
                            printf("\n");
    */
                            /*
                             如果文件比较大,一次读不完,可以再调用CHRV3FileRead继续读取,文件指针自动向后移动
                             while ( 1 ) {
                               c = 4;   每次读取4个扇区,缓冲区定义的越大，一次读取的扇区数越多
                               mCmdParam.Read.mSectorCount = c;   指定读取的扇区数
                               mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  指向文件数据缓冲区的起始地址
                               CHRV3FileRead();   读完后文件指针自动后移
                               处理数据
                               if ( mCmdParam.Read.mSectorCount < c ) break;   实际读出的扇区数较小则说明文件已经结束
                             }
                             如果希望从指定位置开始读写,可以移动文件指针
                              mCmdParam.Locate.mSectorOffset = 3;  跳过文件的前3个扇区开始读写
                              i = CHRV3FileLocate( );
                              mCmdParam.Read.mSectorCount = 10;
                              mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  指向文件数据缓冲区的起始地址
                              CHRV3FileRead();   直接读取从文件的第(CHRV3vSectorSizeH*256*3)个字节开始的数据,前3个扇区被跳过
                              如果希望将新数据添加到原文件的尾部,可以移动文件指针
                              i = CHRV3FileOpen( );
                              mCmdParam.Locate.mSectorOffset = 0xffffffff;  移到文件的尾部,以扇区为单位,如果原文件是3字节,则从CHRV3vSectorSizeH个字节处开始添加
                              i = CHRV3FileLocate( );
                              mCmdParam.Write.mSectorCount = 10;
                              mCmdParam.Write.mDataBuffer = &MY_DATA_BUF[0];
                              CHRV3FileWrite();   在原文件的后面添加数据
                              使用CHRV3FileRead可以自行定义数据缓冲区的起始地址
                              mCmdParam.Read.mSectorCount = 2;
                              mCmdParam.Read.mDataBuffer = 0x50;  将读出的数据放到50H开始的缓冲区中，需要指定缓冲区的起始地址
                              CHRV3FileRead();   从文件中读取2个扇区到指定缓冲区
                              使用CHRV3FileWrite可以自行定义数据缓冲区的起始地址
                              mCmdParam.Wiite.mSectorCount = 2;
                              mCmdParam.Write.mDataBuffer = 0x50;  将50H开始的缓冲区中的数据写入
                              CHRV3FileWrite();   将指定缓冲区中的数据写入2个扇区到文件中
                            */
                            printf( "Close\r\n" );
                            i = CHRV3FileClose( );                            //关闭文件
                            mStopIfError( i );
                        }
                        printf( "Create\r\n" );
                        strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );//新文件名,在根目录下,中文文件名
                        s = CHRV3FileCreate( );                               //新建文件并打开,如果文件已经存在则先删除后再新建 */
                        mStopIfError( s );
                        printf( "Write\r\n" );
                        strcpy( tmpbuf, "000ABCDEFGHIJKLMNOPQRSTUVWXYZ\xd\xa" );//准备写文件数据
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
                        printf( "U盘演示完成\r\n" );
                        break;
                    }
                    else
                    {
                        printf("s = %02x\r\n",s);
                        printf("CHRV3DiskStatus:%02x\r\n",CHRV3DiskStatus);
                    }
//                    Delay_Ms( 50 );
                }
            }
            else printf( "Enum Failed:%02x\r\n", s );
        }
     }
}
