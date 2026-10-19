/********************************** (C) COPYRIGHT *******************************
* File Name          : Udisk_Func_BasicOp.c
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
#include "UDisk_Operation.h"
/*******************************************************************************/
/* Variable Definition */
__attribute__((aligned(4)))  uint8_t  MY_DATA_BUF[ DISK_BASE_BUF_LEN ];   /* MY_DATA_BUFPoints to the disk data buffer in external RAM; its length is at least one sector, used to cache user data */
uint8_t  *pCodeStr;

/*********************************************************************
 * @fn      UDisk_USBH_ByteOperation
 *
 * @brief   Demo Function For UDisk File Byte-operation
 *          including Create\Modify\Read\Erase (EXAM1)
 *
 * @return  none
 */
void UDisk_USBH_ByteOperation( void )
{
    uint8_t ret;
    uint8_t i,t;
    uint16_t TotalCount = 0;

    UDisk_USBH_DiskReady( );
    if( (CHRV3DiskStatus >= DISK_MOUNTED)&&( UDisk_Opeation_Flag == 1 ) )
    {
        UDisk_Opeation_Flag = 0;
        printf("CHRV3DiskStatus:%02x\r\n",CHRV3DiskStatus);
        /* Read file */
        strcpy( (char *)mCmdParam.Open.mPathName, "/NEWFILE.C" ); //Set the path and file name to operate on: /NEWFILE.C
        ret = CHRV3FileOpen( );                                       //Open file
        if ( ret == ERR_MISS_DIR || ret == ERR_MISS_FILE )            //File not found
        {
            //File creation demonstration
            printf( "Find No File And Create\r\n" );
            strcpy( (char *)mCmdParam.Create.mPathName, "/NEWFILE.C" );  //New file name, in the root directory; a Chinese file name
            ret = CHRV3FileCreate( );                                        //Create and open a new file; if the file already exists, delete it first
            mStopIfError( ret );
            printf( "ByteWrite\r\n" );
            //In practice the write length should be checked against the defined buffer length; if it is larger, the data must be written in several passes
            i = sprintf( (char *)Com_Buffer,"Note: \xd\xaThis program reads and writes the USB drive file byte by byte, as a simple demonstration.\xd\xa");
            for(t=0; t<10; t++)
            {
                mCmdParam.ByteWrite.mByteCount = i;                           //Number of bytes to write this time
                mCmdParam.ByteWrite.mByteBuffer = Com_Buffer;                 //Pointer to the buffer
                ret = CHRV3ByteWrite( );                                      //Write data to the file in bytes
                mStopIfError( ret );
                printf("Successfully wrote %02X times\r\n",(uint16_t)t);
            }
            //Demonstrate modifying a file attribute
            printf( "Modify\r\n" );
            mCmdParam.Modify.mFileAttr = 0xff;   //Input: new file attribute; 0FFH means no change
            mCmdParam.Modify.mFileTime = 0xffff;   //Input: new file time; 0FFFFH means no change, and the default time of the newly created file is used
            mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //Input: new file date: 2015.05.18
            mCmdParam.Modify.mFileSize = 0xffffffff;  // Input: new file length in bytes; the length is updated automatically when the library closes the file, so it is not modified here
            i = CHRV3FileModify( );   //Modify the information of the current file; modify the date
            mStopIfError( i );
            printf( "Close\r\n" );
            mCmdParam.Close.mUpdateLen = 1;     //File length is calculated automatically. Data is written in bytes. It is recommended to let the library close the file so that the file length is updated automatically
            i = CHRV3FileClose( );
            mStopIfError( i );

            /* Delete a file */
//            printf( "Erase\n" );
//            strcpy( (char *)mCmdParam.Create.mPathName, "/OLD" );  //Name of the file to be deleted, in the root directory
//            i = CHRV3FileErase( );  //Delete the file and close it
//            if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (uint16_t)i );  //Display the error
        }
        else
        {
            /* 1. Write to a file */
            printf( "ByteWrite\r\n" ); //To append new data to the end of the original file, move the file pointer
            mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  //Move to the end of the file
            CHRV3ByteLocate( );           //In practice the write length should be checked against the defined buffer length; if it is larger, the data must be written in several passes
            i = sprintf( (char *)Com_Buffer,"Note: \xd\xaThis program reads and writes the USB drive file byte by byte, as a simple demonstration.\xd\xa"); //demonstration text
            for(t=0; t<10; t++)
            {
                mCmdParam.ByteWrite.mByteCount = i;               //Number of bytes to write this time
                mCmdParam.ByteWrite.mByteBuffer = Com_Buffer;     // Pointer to the buffer
                ret = CHRV3ByteWrite( );                          // Write data to the file in bytes
                mStopIfError( ret );
                printf("Successfully wrote %02X times\r\n",(uint16_t)t);
            }

            //Demonstrate modifying a file attribute
            printf( "Modify\r\n" );
            mCmdParam.Modify.mFileAttr = 0xff;   //Input: new file attribute; 0FFH means no change
            mCmdParam.Modify.mFileTime = 0xffff;   //Input: new file time; 0FFFFH means no change, and the default time of the newly created file is used
            mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //Input: new file date: 2015.05.18
            mCmdParam.Modify.mFileSize = 0xffffffff;  // Input: new file length in bytes; the length is updated automatically when the library closes the file, so it is not modified here
            i = CHRV3FileModify( );   //Modify the information of the current file; modify the date
            mStopIfError( i );
            printf( "Close\r\n" );
            mCmdParam.Close.mUpdateLen = 1;     //File length is calculated automatically. Data is written in bytes. It is recommended to let the library close the file so that the file length is updated automatically
            i = CHRV3FileClose( );
            mStopIfError( i );

            /* 2. Read the first N bytes of a file */
            TotalCount = 60;                                                  //Set the total length to read to 100 bytes
            strcpy( (char *)mCmdParam.Open.mPathName, "/NEWFILE.C" );     //Set the path and file name to operate on: /NEWFILE.C
            CHRV3FileOpen( );                                                 //Open file
            printf( "The first %d characters read are:\r\n",TotalCount );
            while ( TotalCount )
            {
                //If the file is too large to read in one call, call CH103ByteRead again to continue; the file pointer advances automatically
                if ( TotalCount > (MAX_PATH_LEN-1) )
                {
                    t = MAX_PATH_LEN-1; // A large amount of data remains; the length of a single read/write must not exceed sizeof( mCmdParam.Other.mBuffer )
                }
                else
                {
                    t = TotalCount; //Number of bytes remaining at the end
                }
                mCmdParam.ByteRead.mByteCount = t;                   //Request to read a few dozen bytes of data
                mCmdParam.ByteRead.mByteBuffer= &Com_Buffer[0];
                ret = CHRV3ByteRead( );                              //Read a data block in bytes. A single read must not exceed MAX_BYTE_IO. A second call continues reading from where the previous one stopped
                TotalCount -= mCmdParam.ByteRead.mByteCount;         //Counter; subtract the number of characters actually read so far
                for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ )
                {
                    printf( "%c", mCmdParam.ByteRead.mByteBuffer[i] ); //Display the characters read
                }
                printf( "\r\n" );

                if ( mCmdParam.ByteRead.mByteCount < t ) //The number of characters actually read is smaller than requested, which means the end of the file has been reached
                {
                    printf( "\r\n" );
                    printf( "End of file reached\r\n" );
                    break;
                }
            }
            i = CHRV3FileClose( ); //Close file
            mStopIfError( i );
        }
    }
}

/*********************************************************************
 * @fn      UDisk_USBH_SectorOperation
 *
 * @brief   Demo Function For UDisk File Sector-operation
 *          including Create\Modify\Read\Erase (EXAM6)
 *
 * @return  none
 */
void UDisk_USBH_SectorOperation( void )
{
    uint8_t  ret, SecCount, s;
    uint8_t  i;
    uint16_t tmp;
    uint8_t  tmpbuf[64];

    ret = UDisk_USBH_DiskReady( );
    if( ( ret == DISK_READY )&&( UDisk_Opeation_Flag == 1 ) )
    {
        UDisk_Opeation_Flag = 0;
        /* Query the physical capacity of the disk */
        printf( "DiskSize\r\n" );
        i = CHRV3DiskQuery( );
        mStopIfError( i );
        printf( "TotalSize = %u MB \n", (unsigned int)( ( (uint64_t)mCmdParam.Query.mTotalSector * ( 1 << CHRV3vSectorSizeB ) ) >> 20 ) );  //Display the capacity in MB

        /* Read the original file */
        printf( "Open\r\n" );
        strcpy( mCmdParam.Open.mPathName, "/NEWFILE.TXT" );//File name; the file is located in the C51 subdirectory
        s = CHRV3FileOpen( );                       //Open file
        if ( s == ERR_MISS_DIR || s == ERR_MISS_FILE )//File not found
        {
            printf( "File not found\r\n" );
        }
        else                                        //The file was found, or an error occurred
        {
            printf( "Query\r\n" );
            i = CHRV3FileQuery( );                  //Query the information of the current file
            mStopIfError( i );
            printf( "Read\r\n" );
            CHRV3vFileSize = CHRV3vFileSize+(sizeof( MY_DATA_BUF )-1);    //Length of the original file
            SecCount = CHRV3vFileSize/ sizeof( MY_DATA_BUF )  ;//Calculate the number of sectors of the file. Since reads and writes are in sectors, CHRV3vSectorSize-1 is added first so that the partial sector at the end of the file is included
            printf( "Size=%ld, Sec=%d\r\n", CHRV3vFileSize, (uint16_t)SecCount );
            while(SecCount--)
            {
                mCmdParam.Read.mSectorCount = sizeof( MY_DATA_BUF )/512;  //Read all the data; if it spans more than 2 sectors, only 2 sectors are read
                mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];//Start address of the file data buffer
                i = CHRV3FileRead( );                    //Read data from the file
                mStopIfError( i );
                if(SecCount == 0) break;
                /*
                for(tmp=0; tmp<sizeof( MY_DATA_BUF ); tmp++)
                {
                printf("%02X ",(uint16_t)MY_DATA_BUF[tmp]);
                }
                printf("\n");
                */
            }
            tmp = (CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1))%sizeof( MY_DATA_BUF );
            if((tmp == 0)&&(CHRV3vFileSize != 0)) tmp = sizeof( MY_DATA_BUF );
            CHRV3vFileSize = CHRV3vFileSize-(sizeof( MY_DATA_BUF )-1);    //Restore the length of the original file
            /*
            for(i=0; i<tmp; i++)
            {
              printf("%02X ",(uint16_t)MY_DATA_BUF[i]);
            }
            printf("\n");
            */
            /*
                         If the file is too large to read in one call, call CHRV3FileRead again to continue; the file pointer advances automatically
             while ( 1 )
             {
               c = 4;   Four sectors are read at a time; the larger the buffer defined, the more sectors can be read in one pass
               mCmdParam.Read.mSectorCount = c;   Number of sectors to read
               mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  Start address of the file data buffer
               CHRV3FileRead();   The file pointer advances automatically after reading; process the data
               if ( mCmdParam.Read.mSectorCount < c ) break;   A smaller actual sector count means the end of the file has been reached
             }
                                               To start reading or writing at a specified position, move the file pointer
              mCmdParam.Locate.mSectorOffset = 3;  Skip the first 3 sectors of the file and start reading or writing
              i = CHRV3FileLocate( );
              mCmdParam.Read.mSectorCount = 10;
              mCmdParam.Read.mDataBuffer = &MY_DATA_BUF[0];  Start address of the file data buffer
              CHRV3FileRead();   Read directly from byte (CHRV3vSectorSizeH*256*3) of the file; the first 3 sectors are skipped
                                               To append new data to the end of the original file, move the file pointer
              i = CHRV3FileOpen( );
              mCmdParam.Locate.mSectorOffset = 0xffffffff;  Move to the end of the file, in sectors. If the original file is 3 bytes long, appending starts at byte CHRV3vSectorSizeH
              i = CHRV3FileLocate( );
              mCmdParam.Write.mSectorCount = 10;
              mCmdParam.Write.mDataBuffer = &MY_DATA_BUF[0];
              CHRV3FileWrite();   Append data after the original file content
                                               CHRV3FileRead lets you define the start address of the data buffer yourself
              mCmdParam.Read.mSectorCount = 2;
              mCmdParam.Read.mDataBuffer = 0x50;  The data read is placed in the buffer starting at 50H; the start address of the buffer must be specified
              CHRV3FileRead();   Read 2 sectors from the file into the given buffer
                                                CHRV3FileWrite lets you define the start address of the data buffer yourself
              mCmdParam.Wiite.mSectorCount = 2;
              mCmdParam.Write.mDataBuffer = 0x50;  Write the data in the buffer starting at 50H
              CHRV3FileWrite();   Write the data in the given buffer into the file as 2 sectors
            */
            printf( "Close\r\n" );
            i = CHRV3FileClose( );                            //Close file
            mStopIfError( i );
        }
        printf( "Create\r\n" );
        strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );//New file name, in the root directory; a Chinese file name
        s = CHRV3FileCreate( );                               //Create and open a new file; if the file already exists, delete it first */
        mStopIfError( s );
        printf( "Write\r\n" );
        strcpy( tmpbuf, "0000ABCDEFGHIJKLMNOPQRSTUVWXYZ\xd\xa" );//Prepare to write file data
        for(i=0; i<(DISK_BASE_BUF_LEN/sizeof(tmpbuf)); i++)
        {
            tmp=i*sizeof(tmpbuf);
            strcpy(&MY_DATA_BUF[tmp],tmpbuf);
        }
        for(tmp=0; tmp<sizeof(MY_DATA_BUF); tmp++)
        {
            printf("%02X",(uint16_t)MY_DATA_BUF[tmp]);
        }
        printf("\r\n");
        for(s=0; s<10; s++)
        {
            mCmdParam.Write.mSectorCount = 1;                 //Write the data of all sectors
            mCmdParam.Write.mDataBuffer = &MY_DATA_BUF[0];    //Start address of the file data buffer
            i = CHRV3FileWrite( );                            //Write data to the file
            mStopIfError( i );
            printf("Successfully wrote %02X times\r\n",(uint16_t)s);
        }
        /* printf( "Modify\n" );
           mCmdParam.Modify.mFileAttr = 0xff;   Input: new file attribute; 0FFH means no change
           mCmdParam.Modify.mFileTime = 0xffff;   Input: new file time; 0FFFFH means no change, and the default time of the newly created file is used
           mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  Input: new file date: 2015.05.18
           mCmdParam.Modify.mFileSize = 0xffffffff;   Input: new file length in bytes; the length is updated automatically when the library closes the file, so it is not modified here
           i = CHRV3FileModify( );   Modify the information of the current file; modify the date
           mStopIfError( i );
        */
        printf( "Close\r\n" );
        mCmdParam.Close.mUpdateLen = 1;                        //File length is calculated automatically. Data is written in bytes. It is recommended to let the library close the file so that the file length is updated automatically
        i = CHRV3FileClose( );
        mStopIfError( i );
        /* Delete a file */
        /*printf( "Erase\n" );
          strcpy( mCmdParam.Create.mPathName, "/OLD.TXT" );  Name of the file to be deleted, in the root directory
          i = CHRV3FileErase( );  Delete the file and close it
          if ( i != ERR_SUCCESS ) printf( "Error File not exist: %02X\n", (uint16_t)i );  Display the error
        */
        printf( "USB drive demonstration completed\r\n" );
    }
}


/*********************************************************************
 * @fn      UDisk_USBH_EnumFiles
 *
 * @brief   Demo Function For Enumerating files in UDisk(EXAM11)
 *
 * @return  none
 */
void UDisk_USBH_EnumFiles( void )
{
    uint8_t  i, s, ret;
    uint16_t j;

    ret = UDisk_USBH_DiskReady( );
    if( ( ret == DISK_READY )&&( UDisk_Opeation_Flag == 1 ) )
    {
        UDisk_Opeation_Flag = 0;
        /* Read the original file */
        printf( "Open\r\n" );
        strcpy( mCmdParam.Open.mPathName, "/C51/CHRV3HFT.C" );//File name; the file is located in the C51 subdirectory
        s = CHRV3FileOpen( );                        //Open file
        /* List files */
        if ( s == ERR_MISS_DIR )
        {
            printf("The file does not exist, so all files are listed\r\n");  //C51if the subdirectory does not exist, list all files in the root directory
            pCodeStr = "/*";
        }
        else
        {
            pCodeStr = "/C51/*";                     //CHRV3HFT.CIf the file does not exist, list the files starting with CHRV3 in the \C51 subdirectory
        }
        printf( "List file %s\r\n", pCodeStr );
        for ( j = 0; j < 10000; j ++ )               //At most the first 10000 files are searched; in practice there is no limit
        {
            strcpy( (char *)mCmdParam.Open.mPathName, pCodeStr );//Search for a file name; * is a wildcard that matches all files or subdirectories
            i = strlen( mCmdParam.Open.mPathName );
            mCmdParam.Open.mPathName[ i ] = 0xFF;    //Replace the terminator with the search index according to the string length, from 0 to 254; if it is 0xFF (255), the search index is held in CHRV3vFileSize
            CHRV3vFileSize = j;                      //Index of the search or enumeration
            i = CHRV3FileOpen( );                    //Open the file; if the file name contains the wildcard *, the call searches for files instead of opening one
            /* CHRV3FileEnum The only difference from CHRV3FileOpen is that where the latter returns ERR_FOUND_NAME, the former returns ERR_SUCCESS */
            if ( i == ERR_MISS_FILE )
            {
                break;                                //No further matching files can be found; no file name matches
            }
            if ( i == ERR_FOUND_NAME )
            {
                /* A file name matching the wildcard was found; the file name and its full path are in the command buffer */
                printf( "  match file %04d#: %s\r\n", (unsigned int)j, mCmdParam.Open.mPathName );//Display the index and the matching file name or subdirectory name that was found
                continue;                             //Continue searching for the next matching file name; the index is incremented on the next search
            }
            else
            {
                /* Error */
                mStopIfError( i );
                break;
            }
        }
        printf( "Close\r\n" );
        CHRV3FileClose( );                            //Close file
        printf( "USB drive demonstration completed\r\n" );
    }
}
