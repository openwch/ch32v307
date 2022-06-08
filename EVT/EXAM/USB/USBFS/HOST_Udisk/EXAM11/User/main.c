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
  USBHS的U盘文件枚举例程：
    查找/C51/CH559HFT.C文件，如果没有找到则枚举根目录下不超过10000个文件包括目录(实际不限制上限)
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
    printf( "Error: %02X\r\n", (UINT16)iError );                 // 显示错误
    /* 遇到错误后,应该分析错误码以及CHRV3DiskStatus状态,例如调用CHRV3DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CHRV3DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CHRV3DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CHRV3DiskReady等) */
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
    UINT8 s;
    UINT8 i;
    UINT8   *pCodeStr;
    UINT16 j;
    USART_Printf_Init(115200);
    Delay_Init();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("USBFS HOST Test EXAM 11\r\n");
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
                     /* 读取原文件 */
                    printf( "Open\r\n" );
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
                    printf( "List file %s\n", pCodeStr );
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
