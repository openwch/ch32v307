/********************************** (C) COPYRIGHT *******************************
* File Name          : UDisk_Func_CreatDir.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/11/22
* Description        : USB full-speed port host operation functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*******************************************************************************/
/* Header File */
#include "Udisk_Operation.h"

/*******************************************************************************
* Function Name  : CreateDirectory
* Description    : 新建目录并打开,如果目录已经存在则直接打开，目录名在mCmdParam.Create.mPathName中,与文件名规则相同
* Input          :
* Output         : None
* Return         : ERR_SUCCESS = 打开目录成功或者创建目录成功,
                   ERR_FOUND_NAME = 已经存在同名文件,
                   ERR_MISS_DIR = 路径名无效或者上级目录不存在
*******************************************************************************/
uint8_t CreateDirectory( void )
{
    uint8_t   i, j;
    uint32_t  UpDirCluster;
    uint8_t * DirXramBuf;
    uint8_t  *DirConstData;
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
    if ( j == 0 || (j == 2 && mCmdParam.Create.mPathName[1] == ':') )
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
                *(pDISK_FAT_BUF+0x1A) = ( (uint8_t *)&CHRV3vStartCluster )[3];//自身的起始簇号
                *(pDISK_FAT_BUF+0x1B) = ( (uint8_t *)&CHRV3vStartCluster )[2];
                *(pDISK_FAT_BUF+0x14) = ( (uint8_t *)&CHRV3vStartCluster )[1];
                *(pDISK_FAT_BUF+0x15) = ( (uint8_t *)&CHRV3vStartCluster )[0];
                *(pDISK_FAT_BUF+0x20+0x1A) = ( (uint8_t *)&UpDirCluster )[3];//上级目录的起始簇号
                *(pDISK_FAT_BUF+0x20+0x1B) = ( (uint8_t *)&UpDirCluster )[2];
                *(pDISK_FAT_BUF+0x20+0x14) = ( (uint8_t *)&UpDirCluster )[1];
                *(pDISK_FAT_BUF+0x20+0x15) = ( (uint8_t *)&UpDirCluster )[0];
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
 * @fn      UDisk_USBH_CreatDirectory
 *
 * @brief   Demo Function For UDisk Create Directory (EXAM9)
 *
 * @return  none
 */
void UDisk_USBH_CreatDirectory( void )
{
    uint8_t  i;
    uint8_t  ret;

    ret = UDisk_USBH_DiskReady( );
    if( ( ret == DISK_READY )&&( UDisk_Opeation_Flag == 1 ) )
    {
        UDisk_Opeation_Flag = 0;
        printf("CHRV3DiskStatus:%02x\r\n",CHRV3DiskStatus);
        printf( "Create Level 1 Directory /YEAR2004 \r\n" );
        strcpy( mCmdParam.Create.mPathName, "/YEAR2004" );             //目录名,该目录建在根目录下
        ret = CreateDirectory( );                                      //新建或者打开目录
        mStopIfError( ret );
        /* 目录新建或者打开成功,下面在这个子目录中新建一个演示文件 */
        printf( "Create New File /YEAR2004/DEMO2004.TXT \r\n" );
        strcpy( mCmdParam.Create.mPathName, "/YEAR2004/DEMO2004.TXT" );//文件名
        ret = CHRV3FileCreate( );                                      //新建文件并打开,如果文件已经存在则先删除后再新建
        mStopIfError( ret );
        printf( "Write some data to file DEMO2004.TXT \r\n" );
        i = sprintf( Com_Buffer, "演示文件\xd\xa" );
        mCmdParam.ByteWrite.mByteCount = i;                            //指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO
        mCmdParam.ByteWrite.mByteBuffer = Com_Buffer;                  //指向缓冲区
        ret = CHRV3ByteWrite( );                                       //以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO
        mStopIfError( ret );
        printf( "Close file DEMO2004.TXT \r\n" );
        mCmdParam.Close.mUpdateLen = 1;                                //自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度
        ret = CHRV3FileClose( );
        mStopIfError( ret );
        /* 下面新建二级子目录,方法与前面的一级子目录完全相同 */
        printf( "Create Level 2 Directory /YEAR2004/MONTH05 \r\n" );
        strcpy( mCmdParam.Create.mPathName, "/YEAR2004/MONTH05" );    //目录名,该目录建在YEAR2004子目录下,YEAR2004目录必须事先存在
        ret = CreateDirectory( );                                     //新建或者打开目录
        mStopIfError( ret );
        printf( "Close\r\n" );
        mCmdParam.Close.mUpdateLen = 0;                               //对于目录不需要自动更新文件长度
        ret = CHRV3FileClose( );                                      //关闭目录,目录不需要关闭,关闭只是为了防止下面误操作
        mStopIfError( ret );
    }
}


