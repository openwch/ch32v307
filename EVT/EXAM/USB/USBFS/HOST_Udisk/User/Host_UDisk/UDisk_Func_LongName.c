/********************************** (C) COPYRIGHT *******************************
* File Name          : Udisk_Func_longname.c
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

/*******************************************************************************/
/* Variable Definition */
uint8_t    LongNameBuf[ LONG_NAME_BUF_LEN ];
/*
长文件名示例(UNICODE编码的大小端 必须与UNICODE_ENDIAN定义相同)
以下是LongName里编码内容:
建立长文件名，输入两个参数： 1.采用(unicode 大端)，字符串末尾用两个0表示结束;2.ANSI编码短文件名.TXT
*/
uint8_t LongName[ ] =
#if UNICODE_ENDIAN == 1
{
    0x5E, 0xFA, 0x7A, 0xCB, 0x95, 0x7F, 0x65, 0x87, 0x4E, 0xF6, 0x54, 0x0D, 0xFF, 0x0C, 0x8F, 0x93,
    0x51, 0x65, 0x4E, 0x24, 0x4E, 0x2A, 0x53, 0xC2, 0x65, 0x70, 0xFF, 0x1A, 0x00, 0x20, 0x00, 0x31,
    0x00, 0x2E, 0x91, 0xC7, 0x75, 0x28, 0x00, 0x28, 0x00, 0x75, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x63,
    0x00, 0x6F, 0x00, 0x64, 0x00, 0x65, 0x00, 0x20, 0x59, 0x27, 0x7A, 0xEF, 0x00, 0x29, 0xFF, 0x0C,
    0x5B, 0x57, 0x7B, 0x26, 0x4E, 0x32, 0x67, 0x2B, 0x5C, 0x3E, 0x75, 0x28, 0x4E, 0x24, 0x4E, 0x2A,
    0x00, 0x30, 0x88, 0x68, 0x79, 0x3A, 0x7E, 0xD3, 0x67, 0x5F, 0x00, 0x3B, 0x00, 0x32, 0x00, 0x2E,
    0x00, 0x41, 0x00, 0x4E, 0x00, 0x53, 0x00, 0x49, 0x7F, 0x16, 0x78, 0x01, 0x77, 0xED, 0x65, 0x87,
    0x4E, 0xF6, 0x54, 0x0D, 0x00, 0x2E, 0x00, 0x54, 0x00, 0x58, 0x00, 0x54
};
#else
{
    0xFA, 0x5E, 0xCB, 0x7A, 0x7F, 0x95, 0x87, 0x65, 0xF6, 0x4E, 0x0D, 0x54, 0x0C, 0xFF, 0x93, 0x8F,
    0x65, 0x51, 0x24, 0x4E, 0x2A, 0x4E, 0xC2, 0x53, 0x70, 0x65, 0x1A, 0xFF, 0x20, 0x00, 0x31, 0x00,
    0x2E, 0x00, 0xC7, 0x91, 0x28, 0x75, 0x28, 0x00, 0x75, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x63, 0x00,
    0x6F, 0x00, 0x64, 0x00, 0x65, 0x00, 0x20, 0x00, 0x27, 0x59, 0xEF, 0x7A, 0x29, 0x00, 0x0C, 0xFF,
    0x57, 0x5B, 0x26, 0x7B, 0x32, 0x4E, 0x2B, 0x67, 0x3E, 0x5C, 0x28, 0x75, 0x24, 0x4E, 0x2A, 0x4E,
    0x30, 0x00, 0x68, 0x88, 0x3A, 0x79, 0xD3, 0x7E, 0x5F, 0x67, 0x3B, 0x00, 0x32, 0x00, 0x2E, 0x00,
    0x41, 0x00, 0x4E, 0x00, 0x53, 0x00, 0x49, 0x00, 0x16, 0x7F, 0x01, 0x78, 0xED, 0x77, 0x87, 0x65,
    0xF6, 0x4E, 0x0D, 0x54, 0x2E, 0x00, 0x54, 0x00, 0x58, 0x00, 0x54, 0x00
};
#endif

/*********************************************************************
 * @fn      UDisk_USBH_Longname
 *
 * @brief   Demo Function For UDisk long-name Operation(EXAM 13)
 *
 * @return  none
 */
void UDisk_USBH_Longname( void )
{
    uint8_t  ret, i, len;
    uint16_t j;

    ret = UDisk_USBH_DiskReady( );
    if( ( ret == DISK_READY )&&( UDisk_Opeation_Flag == 1 ) )
    {
        UDisk_Opeation_Flag = 0;
        /*==================== 以下演示创建及读取长文件名 ============================*/
        // 复制长文件名(UNICODE 大端)到LongNameBuf里
        len = LongName_Len;
        memcpy( LongNameBuf, LongName, len );
        // 末尾用两个0表示结束
        LongNameBuf[len] = 0x00;
        LongNameBuf[len + 1] = 0x00;
        // 该长文件名的ANSI编码短文件名(8+3格式)
        strcpy( mCmdParam.Create.mPathName, "\\长文件名.TXT" );
        i = CHRV3CreateLongName( );
        if( i == ERR_SUCCESS )
        {
            DUG_PRINTF( "Created Long Name OK!\r\n" );
        }
        else
        {
            /* Error code defined in "udisk_operaion.h" */
            DUG_PRINTF( "Error Code: %02X\r\n", (uint16_t)i );
        }

        DUG_PRINTF( "Get long Name#\r\n" );
        strcpy( mCmdParam.Open.mPathName, "\\长文件名.TXT" );
        // 以上需要输入文件名的完整路径
        i = CHRV3GetLongName( );
        if( i == ERR_SUCCESS )
        {
            // 长文件名收集完成,以UNICODE编码方式(按UNICODE_ENDIAN定义)
            // 存放在LongNameBuf缓冲里,长文件名最后用两个0结束.
            // 以下显示缓冲区里所有数据
            DUG_PRINTF( "LongNameBuf: " );
            for( j=0; j!=LONG_NAME_BUF_LEN; j++ )
            {
                DUG_PRINTF( "%02X ", (uint16_t)LongNameBuf[j] );
            }
            DUG_PRINTF( "\r\n" );
        }
        else
        {
            /* Error code defined in "udisk_operaion.h" */
            DUG_PRINTF( "Error Code: %02X\r\n", (uint16_t)i );
        }
    }
}

/*********************************************************************
 * @fn      CheckNameSum
 *
 * @brief   检查长文件名的短文件名检验和
 *
 * @return  计算后的校验和
 */
uint8_t CheckNameSum( uint8_t *p )
{
uint8_t FcbNameLen;
uint8_t Sum;

    Sum = 0;
    for (FcbNameLen=0; FcbNameLen!=11; FcbNameLen++)
        Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *p++;
    return Sum;
}

/*********************************************************************
 * @fn      AnalyzeLongName
 *
 * @brief   整理长文件名 返回有几个的26长度
 *
 * @return  返回有多少个26的长度
 */
uint8_t AnalyzeLongName( void )
{
uint8_t   i, j;
uint16_t  index;

    i = FALSE;
    for( index=0; index!=LONG_NAME_BUF_LEN; index = index + 2 )
    {
        if( ( LongNameBuf[index] == 0 ) && ( LongNameBuf[index+1] == 0 ) )
        {
            i = TRUE;
            break;
        }
    }
    if( ( i == FALSE ) || ( index == 0) )
        return 0;                   // 返回0表示错误的长文件名

    i = index % 26;
    if( i != 0 )
    {
        index += 2;
        if( index % 26 != 0 )       // 加0刚好结束
        {
            for( j=i+2; j!=26; j++ )// 把剩余数据填为0XFF
                LongNameBuf[index++] = 0xff;
        }
    }
    return  (index / 26);
}

/*********************************************************************
 * @fn      CHRV3CreateLongName
 *
 * @brief   创建长文件名,需要输入短文件名的完整路径
 *
 * @return  操作状态
 */
uint8_t CHRV3CreateLongName( void )
{
// 分析 保留文件路径 创建一个空文件 得到FDT偏移和其所在扇区 删除文件
// 向后偏移扇区 可能失败 如FAT12/16处在根目录处 填充完毕后再次创建文件
uint8_t   i;
uint8_t   len;                                // 存放路径的长度
uint16_t  index;                              // 长文件偏移索引
uint16_t  indexBak;                           // 长文件偏移索引备份
uint32_t  Secoffset;                          // 扇区偏移

uint8_t   Fbit;                               // 第一次进入写扇区
uint8_t   Mult;                               // 长文件名长度26的倍数
uint8_t   MultBak;                            // 长文件名长度26的倍数备份

uint16_t  Backoffset;                         // 保存文件偏移备份
uint16_t  BackoffsetBak;                      // 保存偏移备份的备份
uint32_t  BackFdtSector;                      // 保寸偏移上一个扇区
uint8_t   sum;                                // 保存长文件名的校验和

uint8_t   BackPathBuf[MAX_PATH_LEN];    // 保存文件路径

    Mult = AnalyzeLongName( );              // 保存长文件名是26的倍数
    if( Mult == 0 )
        return ERR_LONG_NAME;
    MultBak = Mult;

    i = CHRV3FileOpen();                    // 短文件名存在则返回错误
    if( i == ERR_SUCCESS )
        return ERR_NAME_EXIST;

    i = CHRV3FileCreate( );
    if( i == ERR_SUCCESS )
    {
        Backoffset = CHRV3vFdtOffset;
        BackoffsetBak = Backoffset;
        BackFdtSector = CHRV3vFdtLba;
        sum = CheckNameSum( &DISK_BASE_BUF[Backoffset ] );
        for( i=0; i!=MAX_PATH_LEN; i++ )    // 对文件路径进行备份
            BackPathBuf[i] = mCmdParam.Open.mPathName[i];
        CHRV3FileErase( );                  // 删除此文件

        Secoffset   = 0;                    // 从0开始偏移
        index       = Mult*26;              // 得到长文件名的长度
        indexBak    = index;
        Fbit        = FALSE;                // 默认没有进入
        // 打开上级 进行数据填充数据
        P_RETRY:
        for(len=0; len!=MAX_PATH_LEN; len++)
        {
            if(mCmdParam.Open.mPathName[len] == 0)
                break;                      // 得到字符串长度
        }

        for(i=len-1; i!=0xff; i--)          // 得到上级目录位置
        {
            if((mCmdParam.Open.mPathName[i] == '\\') || (mCmdParam.Open.mPathName[i] == '/'))
                break;
        }
        mCmdParam.Open.mPathName[i] = 0x00;

        if( i==0 )                          // 打开一级目录注意:处在根目录开始的特殊情况
        {
            mCmdParam.Open.mPathName[0] = '/';
            mCmdParam.Open.mPathName[1] = 0;
        }

        i = CHRV3FileOpen();                // 打开上级目录
        if( i == ERR_OPEN_DIR )
        {
            while( 1 )                      // 循环填写 直到完成
            {
                mCmdParam.Locate.mSectorOffset = Secoffset;
                i = CHRV3FileLocate( );
                if( i == ERR_SUCCESS )
                {
                    if( Fbit )             // 第二次进入次写扇区
                    {
                        if( mCmdParam.Locate.mSectorOffset != 0x0FFFFFFFF )
                        {
                            BackFdtSector = mCmdParam.Locate.mSectorOffset;
                            Backoffset = 0;
                        }
                        else
                        {
                            for( i=0; i!=MAX_PATH_LEN; i++ )// 还原文件路径
                                mCmdParam.Open.mPathName[i] = BackPathBuf[i];
                            i = CHRV3FileCreate( );         // 进行空间扩展
                            if( i != ERR_SUCCESS )
                                return i;
                            CHRV3FileErase( );
                            goto P_RETRY;                   // 重新打开上级目录
                        }
                    }

                    if( BackFdtSector == mCmdParam.Locate.mSectorOffset )
                    {
                        mCmdParam.Read.mSectorCount = 1;   // 读一个扇区到磁盘缓冲区
                        mCmdParam.Read.mDataBuffer = &DISK_BASE_BUF[0];
                        i = CHRV3FileRead( );
                        CHRV3DirtyBuffer( );                // 清除磁盘缓冲区
                        if( i!= ERR_SUCCESS )
                            return i;

                        i = ( CHRV3vSectorSize - Backoffset ) / 32;
                        if( Mult > i )
                            Mult = Mult - i;                // 剩余的倍数
                        else
                        {
                            i = Mult;
                            Mult = 0;
                        }

                        for( len=i; len!=0; len-- )
                        {
                            indexBak -= 26;
                            index = indexBak;
                            for( i=0; i!=5; i++)            // 长文件名的1-5个字符
                            {                               // 在磁盘上UNICODE用小端方式存放
                                #if UNICODE_ENDIAN == 1
                                DISK_BASE_BUF[Backoffset + i*2 + 2 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                #else
                                DISK_BASE_BUF[Backoffset + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + i*2 + 2 ] =
                                    LongNameBuf[index++];
                                #endif
                            }

                            for( i =0; i!=6; i++)           // 长文件名的6-11个字符
                            {
                                #if UNICODE_ENDIAN == 1
                                DISK_BASE_BUF[Backoffset + 14 + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + 14 + i*2 ] =
                                    LongNameBuf[index++];
                                #else
                                DISK_BASE_BUF[Backoffset + 14 + i*2 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + 14 + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                #endif
                            }

                            for( i=0; i!=2; i++)            // 长文件名的12-13个字符
                            {
                                #if UNICODE_ENDIAN == 1
                                DISK_BASE_BUF[Backoffset + 28 + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + 28 + i*2 ] =
                                    LongNameBuf[index++];
                                #else
                                DISK_BASE_BUF[Backoffset + 28 + i*2 ] =
                                    LongNameBuf[index++];
                                DISK_BASE_BUF[Backoffset + 28 + i*2 + 1 ] =
                                    LongNameBuf[index++];
                                #endif
                            }

                            DISK_BASE_BUF[Backoffset + 0x0b] = 0x0f;
                            DISK_BASE_BUF[Backoffset + 0x0c] = 0x00;
                            DISK_BASE_BUF[Backoffset + 0x0d] = sum;
                            DISK_BASE_BUF[Backoffset + 0x1a] = 0x00;
                            DISK_BASE_BUF[Backoffset + 0x1b] = 0x00;
                            DISK_BASE_BUF[Backoffset] = MultBak--;
                            Backoffset += 32;
                        }

                        if( !Fbit )
                        {
                            Fbit = TRUE;
                            DISK_BASE_BUF[ BackoffsetBak ] |= 0x40;
                        }
                        CHRV3vLbaCurrent = BackFdtSector;
                        i = CHRV3WriteSector( 1, DISK_BASE_BUF );
                        if( i!= ERR_SUCCESS )
                            return i;

                        if( Mult==0 )
                        {   // 还原文件路径
					        CHRV3FileClose( );
                            for( i=0; i!=MAX_PATH_LEN; i++ )
                                mCmdParam.Open.mPathName[i] = BackPathBuf[i];
                            i = CHRV3FileCreate( );
                            return i;
                        }
                    }
                }
                else
                    return i;
                Secoffset++;
            }
        }
    }
    return i;
}

/*********************************************************************
 * @fn      GetUpSectorData
 *
 * @brief   由当前扇区得到上一个扇区的数据，放在磁盘缓冲区
 *
 * @return  操作状态
 */
uint8_t GetUpSectorData( uint32_t *NowSector )
{
uint8_t  i;
uint8_t  len;             // 存放路径的长度
uint32_t index;           // 目录扇区偏移扇区数

    index = 0;
    for(len=0; len!=MAX_PATH_LEN; len++)
    {
        if(mCmdParam.Open.mPathName[len] == 0)          // 得到字符串长度
            break;
    }

    for(i=len-1; i!=0xff; i--)                          // 得到上级目录位置
    {
        if((mCmdParam.Open.mPathName[i] == '\\') || (mCmdParam.Open.mPathName[i] == '/'))
            break;
    }
    mCmdParam.Open.mPathName[i] = 0x00;

    if( i==0 )  // 打开一级目录注意:处在根目录开始的特殊情况
    {
        mCmdParam.Open.mPathName[0] = '/';
        mCmdParam.Open.mPathName[1] = 0;
        i = CHRV3FileOpen();
        if ( i == ERR_OPEN_DIR )
            goto P_NEXT0;
    }
    else
    {
        i = CHRV3FileOpen();
        if ( i == ERR_OPEN_DIR )
        {
            while( 1 )
            {
                P_NEXT0:
                mCmdParam.Locate.mSectorOffset = index;
                i = CHRV3FileLocate( );
                if( i == ERR_SUCCESS )
                {
                    if( *NowSector == mCmdParam.Locate.mSectorOffset )
                    {
                        if( index==0 )                          // 处于根目录扇区的开始
                            return ERR_NO_NAME;
                        mCmdParam.Locate.mSectorOffset = --index;
                        i = CHRV3FileLocate( );                 // 读上一个扇区的数据
                        if( i == ERR_SUCCESS )
                        {                                       // 以下保存当前所在扇区数
                            *NowSector = mCmdParam.Locate.mSectorOffset;
                            mCmdParam.Read.mSectorCount = 1;   // 读一个扇区到磁盘缓冲区
                            mCmdParam.Read.mDataBuffer = &DISK_BASE_BUF[0];
                            i = CHRV3FileRead( );
                            CHRV3DirtyBuffer( );                // 清除磁盘缓冲区
                            return i;
                        }
                        else
                            return i;
                    }
                }
                else
                    return i;
                index++;
            }
        }
    }
    return i;
}

/*********************************************************************
 * @fn      CHRV3GetLongName
 *
 * @brief   由完整短文件名路径(可以是文件或文件夹)得到相应的长文件名
 *
 * @return  操作状态
 */
uint8_t CHRV3GetLongName( void )
{
// 需要变量扇区大小
// 第一步：打开文件是否找到文件,分析文件是否存在,并得到FDT在此扇区的偏移和所在扇区
// 第二步：分析上面的信息看是否有长文件名存在，是否处于目录的第一个扇区的开始
// 第三步：实现向后偏移一个扇区?读取长文件名(扇区512字节的U盘)
uint8_t   i;
uint16_t  index;          // 在长文件名缓冲区内的索引
uint32_t  BackFdtSector;  // 保寸偏移上一个扇区
uint8_t   sum;            // 保存长文件名的校验和
//uint16_t  Backoffset;     // 保存文件偏移备份
uint16_t  offset;         // 扇区内文件偏移32倍数
uint8_t   FirstBit;       // 长文件名跨越两个扇区标志位
uint8_t   BackPathBuf[MAX_PATH_LEN]; // 保存文件路径

    i = CHRV3FileOpen( );
    if( ( i == ERR_SUCCESS ) || ( i == ERR_OPEN_DIR ) )
    {
        for( i=0; i!=MAX_PATH_LEN; i++ )
            BackPathBuf[i] = mCmdParam.Open.mPathName[i];
        // 以上完成对路径的备份

        sum = CheckNameSum( &DISK_BASE_BUF[CHRV3vFdtOffset] );
        index = 0;
        FirstBit = FALSE;
//        Backoffset = CHRV3vFdtOffset;
        BackFdtSector = CHRV3vFdtLba;
        if( CHRV3vFdtOffset == 0 )
        {
            // 先判断是否处于一个扇区开始 是否处于根目录开始 ，否则向后偏移
            if( FirstBit == FALSE )
                FirstBit = TRUE;
            i = GetUpSectorData( &BackFdtSector );
            if( i == ERR_SUCCESS )
            {
                CHRV3vFdtOffset = CHRV3vSectorSize;
                goto P_NEXT1;
            }
        }
        else
        {
            // 读取偏移后的数据，直到结束。如果不够则向后偏移
            P_NEXT1:
            offset = CHRV3vFdtOffset;
            while( 1 )
            {
                if( offset != 0 )
                {
                    offset = offset - 32;
                    if( ( DISK_BASE_BUF[offset + 11] == ATTR_LONG_NAME )
                        && (  DISK_BASE_BUF[offset + 13] == sum ) )
                    {
                        if( (index + 26) > LONG_NAME_BUF_LEN )
                            return ERR_BUF_OVER;

                        for( i=0; i!=5; i++)            // 长文件名的1-5个字符
                        {                               // 在磁盘上UNICODE用小端方式存放
                            #if UNICODE_ENDIAN == 1
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + i*2 + 2];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + i*2 + 1];
                            #else
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + i*2 + 1];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + i*2 + 2];
                            #endif
                        }

                        for( i =0; i!=6; i++)           // 长文件名的6-11个字符
                        {
                            #if UNICODE_ENDIAN == 1
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 14 + i*2 + 1];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + + 14 + i*2 ];
                            #else
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + + 14 + i*2 ];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 14 + i*2 + 1];
                            #endif

                        }

                        for( i=0; i!=2; i++)            // 长文件名的12-13个字符
                        {
                            #if UNICODE_ENDIAN == 1
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 28 + i*2 + 1];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 28 + i*2 ];
                            #else
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 28 + i*2 ];
                            LongNameBuf[index++] =
                                DISK_BASE_BUF[offset + 28 + i*2 + 1];
                            #endif
                        }

                        if( DISK_BASE_BUF[offset] & 0X40 )
                        {
                            if( ! (((LongNameBuf[index -1] ==0x00)
                                && (LongNameBuf[index -2] ==0x00))
                                || ((LongNameBuf[index -1] ==0xFF)
                                && (LongNameBuf[index -2 ] ==0xFF))))
                            {                           // 处理刚好为26字节长倍数的文件名
                                if(index + 52 >LONG_NAME_BUF_LEN )
                                    return ERR_BUF_OVER;
                                LongNameBuf[ index ] = 0x00;
                                LongNameBuf[ index + 1] = 0x00;
                            }
                            return ERR_SUCCESS;         // 成功完成长文件名收集完成
                        }
                    }
                    else
                        return ERR_NO_NAME;             // 错误的长文件名,程序返回
                }
                else
                {
                    if( FirstBit == FALSE )
                        FirstBit = TRUE;
                    else                                // 否则第二次进入
                    {
                        for( i=0; i!=MAX_PATH_LEN; i++ )// 还原路径
                            mCmdParam.Open.mPathName[i] = BackPathBuf[i];
                    }
                    i = GetUpSectorData( &BackFdtSector );
                    if( i == ERR_SUCCESS )
                    {
                        CHRV3vFdtOffset = CHRV3vSectorSize;
                        goto P_NEXT1;
                    }
                    else
                        return i;
                    // 向后偏移扇区
                }
            }
        }
    }
    return i;                // 返回错误
}

