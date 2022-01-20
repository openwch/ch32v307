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
  USBHS设备的简易枚举过程例程：
  USBHS_DM(PB6)、USBHS_DM(PB7)
  本例程演示 U盘文件系统,以字节为单位读写文件，包括文件创建、删除、修改文件
  属性，修改文件名。
  注：文件系统支持 FAT12/FAT16/FAT32。

*/

#include "debug.h"
#include "ch32v30x_usbhs_host.h"
#include "string.h"
#include "CHRV3UFI.h"

__attribute__ ((aligned(4))) UINT8   RxBuffer[ MAX_PACKET_SIZE  ];  // IN, must even address
__attribute__ ((aligned(4))) UINT8   TxBuffer[ MAX_PACKET_SIZE  ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpTxBuf[ MAX_PACKET_SIZE ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpRxBuf[ MAX_PACKET_SIZE ];  // OUT, must even addres
__attribute__ ((aligned(4))) UINT8   buf[ 128 ];  // OUT, must even address


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

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8 ret;
    UINT8 i,t;
    UINT16 TotalCount = 0;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
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
                ret = USBHS_HostEnum();
                if( ret == ERR_SUCCESS )
                {
                    printf("enumerate success\n");
                    Delay_Ms(100);
                    // U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH103子程序库继续完成后续工作
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    for ( i = 0; i != 10; i ++ )
                    {
                        printf( "Wait DiskReady\r\n" );
                        ret = CHRV3DiskReady( );                                 //等待U盘准备好
                        if ( ret == ERR_SUCCESS )
                        {
//                            printf("CHRV3DiskStatus:%02x\n",CHRV3DiskStatus);
                            break;
                        }
                        else
                        {
                            printf("%02x\r\n",ret);
                            printf("CHRV3DiskStatus:%02x\n",CHRV3DiskStatus);
                        }
                        Delay_Ms( 50 );
                    }

                  if ( CHRV3DiskStatus >= DISK_MOUNTED )
                    {
                        /* 读文件 */
                        strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/NEWFILE.C" );     //设置将要操作的文件路径和文件名/C51/NEWFILE.C
                        ret = CHRV3FileOpen( );                                       //打开文件
                        if ( ret == ERR_MISS_DIR || ret == ERR_MISS_FILE )            //没有找到文件
                        {
                            //创建文件演示
                            printf( "Find No File And Create\r\n" );
                            strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* 新文件名,在根目录下,中文文件名 */
                            ret = CHRV3FileCreate( );                                        /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
                            mStopIfError( ret );
                            printf( "ByteWrite\n" );
                            //实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
                            i = sprintf( (PCHAR)buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,简单演示功能。\xd\xa");  /*演示 */
                            for(t=0; t<10; t++)
                            {
                                mCmdParam.ByteWrite.mByteCount = i;                          /* 指定本次写入的字节数 */
                                mCmdParam.ByteWrite.mByteBuffer = buf;                       /* 指向缓冲区 */
                                ret = CHRV3ByteWrite( );                                       /* 以字节为单位向文件写入数据 */
                                mStopIfError( ret );
                                printf("成功写入 %02X次\r\n",(UINT16)t);
                            }
                            //演示修改文件属性
                            printf( "Modify\r\n" );
                            mCmdParam.Modify.mFileAttr = 0xff;   //输入参数: 新的文件属性,为0FFH则不修改
                            mCmdParam.Modify.mFileTime = 0xffff;   //输入参数: 新的文件时间,为0FFFFH则不修改,使用新建文件产生的默认时间
                            mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //输入参数: 新的文件日期: 2015.05.18
                            mCmdParam.Modify.mFileSize = 0xffffffff;  // 输入参数: 新的文件长度,以字节为单位写文件应该由程序库关闭文件时自动更新长度,所以此处不修改
                            i = CHRV3FileModify( );   //修改当前文件的信息,修改日期
                            mStopIfError( i );
                            printf( "Close\r\n" );
                            mCmdParam.Close.mUpdateLen = 1;     /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
                            i = CHRV3FileClose( );
                            mStopIfError( i );

                            /* 删除某文件 */
    //          printf( "Erase\n" );
    //          strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //将被删除的文件名,在根目录下
    //          i = CHRV3FileErase( );  //删除文件并关闭
    //          if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //显示错误
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
                                mStopIfError( ret );
                                printf("成功写入 %02X次\r\n",(UINT16)t);
                            }

                            ///////////二、读取文件前N字节/////////////////////////////////////////
                            TotalCount = 100;                                      //设置准备读取总长度100字节
                            printf( "读出的前%d个字符是:\r\n",TotalCount );
                            while ( TotalCount ) {                                 //如果文件比较大,一次读不完,可以再调用CH103ByteRead继续读取,文件指针自动向后移动
                                if ( TotalCount > (MAX_PATH_LEN-1) ) t = MAX_PATH_LEN-1;/* 剩余数据较多,限制单次读写的长度不能超过 sizeof( mCmdParam.Other.mBuffer ) */
                                else t = TotalCount;                                 /* 最后剩余的字节数 */
                                mCmdParam.ByteRead.mByteCount = t;                   /* 请求读出几十字节数据 */
                                mCmdParam.ByteRead.mByteBuffer= &buf[0];
                                ret = CHRV3ByteRead( );                                /* 以字节为单位读取数据块,单次读写的长度不能超过MAX_BYTE_IO,第二次调用时接着刚才的向后读 */
                                TotalCount -= mCmdParam.ByteRead.mByteCount;           /* 计数,减去当前实际已经读出的字符数 */
                                for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ ) printf( "%c", mCmdParam.ByteRead.mByteBuffer[i] );  /* 显示读出的字符 */
                                if ( mCmdParam.ByteRead.mByteCount < t ) {             /* 实际读出的字符数少于要求读出的字符数,说明已经到文件的结尾 */
                                    printf( "\r\n" );
                                    printf( "文件已经结束\r\n" );
                                    break;
                                }
                            }

                          ///////////三、从指定位置读取文件N字节/////////////////////////////////////////
                            printf( "Close\r\n" );
                            i = CHRV3FileClose( );                                    /* 关闭文件 */
                            mStopIfError( i );
                        }

                    }
                }
                else
                {
                    printf("enum error：%02x\n",ret);
                }
            }
            else
            {
                USBHS_HostInit(DISABLE);
                USBHS_HostInit(ENABLE);
                CHRV3DiskStatus = DISK_DISCONNECT;
                printf("disconnect\n");
            }
        }
	}
}




