/********************************** (C) COPYRIGHT *******************************
* File Name          : sdio.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/01/09
* Description        : This file contains the headers of the SDIO.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "sdio.h"
#include "string.h"


SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;

SD_Error CmdError( void );
SD_Error CmdResp7Error( void );
SD_Error CmdResp1Error( u8 cmd );
SD_Error CmdResp3Error( void );
SD_Error CmdResp2Error( void );
SD_Error CmdResp6Error( u8 cmd, u16 *prca );
SD_Error IsCardProgramming( u8 *pstatus );
SD_Error FindSCR( u16 rca, u32 *pscr );
u8 convert_from_bytes_to_power_of_two( u16 NumberOfBytes );


static u8 CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0x01;
static u8 DeviceMode = SD_DMA_MODE;
static u8 StopCondition = 0;
volatile SD_Error TransferError = SD_OK;
volatile u8 TransferEnd = 0;
eMMC_CardInfo eMMCCardInfo;
volatile u8 SD_BUSY=0;
__attribute__( ( aligned( 4 ) ) ) u8 SDIO_DATA_BUFFER[512];

/*********************************************************************
 * @fn      eMMC_Init
 *
 * @brief   Init eMMC.
 *
 * @return  errorstatus: Err code
 */
SD_Error eMMC_Init( void )
{
    NVIC_InitTypeDef NVIC_InitStructure={0};
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    u8 clkdiv = 0;
    SD_Error errorstatus = SD_OK;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_SDIO | RCC_AHBPeriph_DMA2, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );

    SDIO_DeInit();

    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    SDIO->MASK |= (1<<8);
    errorstatus = eMMC_PowerON();

    if( errorstatus == SD_OK )
    {
        errorstatus = eMMC_Read_CID_CSD();
    }
    if( errorstatus == SD_OK )
    {
        errorstatus = SD_SelectDeselect( ( u32 )( eMMCCardInfo.RCA << 16 ) );
    }
    if (errorstatus ==SD_OK )
    {
        errorstatus = eMMC_ReadExtCsd(&eMMCCardInfo);
    }
    if (errorstatus ==SD_OK )
    {
        errorstatus = eMMC_GetCardInfo(&eMMCCardInfo, &CSD_Tab[0], &CID_Tab[0], RCA);
    }
    if( errorstatus == SD_OK )
    {
        errorstatus = SD_EnableWideBusOperation( 2 );
    }
    if (SD_OK == errorstatus)
    {
        errorstatus = eMMC_ReadExtCsd(&eMMCCardInfo);
    }
    if( ( errorstatus == SD_OK ) || ( SDIO_MULTIMEDIA_CARD == CardType ) )
    {
        if( eMMCCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || eMMCCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0 )
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV + 6;
        }
        else
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV+1;   //SDIO_CLK=HCLK/(clkdiv+2)
        }
        SDIO_Clock_Set( clkdiv );
        errorstatus = SD_SetDeviceMode( SD_DMA_MODE );
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      SDIO_Clock_Set
 *
 * @brief   Seet SDIO clock
 *
 * @param   clkdiv - Clock divide factor
 *              
 * @return  SD_Error -  ERR code
 */
void SDIO_Clock_Set( u8 clkdiv )
{
    u32 tmpreg = SDIO->CLKCR;
    tmpreg &= 0XFFFFFF00;
    tmpreg |= clkdiv;
    SDIO->CLKCR = tmpreg;

}

/*********************************************************************
 * @fn      eMMC_PowerON
 *
 * @brief   eMMC Power on
 *
 * @return  SD_Error - ERR code
 */
SD_Error eMMC_PowerON( void )
{
    u8 i = 0;
    SD_Error errorstatus = SD_OK;
    u32 response = 0, count = 0, validvoltage = 0;

    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;  //When the multimedia card is in recognition mode,the frequency of SDIO_CK must be below 400kHz.
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising; //Change this setting to improve collecting samples
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Enable;
    SDIO_Init( &SDIO_InitStructure );

    SDIO_SetPowerState( SDIO_PowerState_ON );
    SDIO_ClockCmd( ENABLE );

    for( i = 0; i < 74; i++ ){
        SDIO_CmdInitStructure.SDIO_Argument = 0x0;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdError();
        if( errorstatus == SD_OK )
        {
            break;
        }
    }
    while( ( !validvoltage ) && ( count < SD_MAX_VOLT_TRIAL ) )
    {
        SDIO_CmdInitStructure.SDIO_Argument = 0x40FF8080;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdResp3Error();
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
        response = SDIO->RESP1;;
        validvoltage = ( ( ( response >> 31 ) == 1 ) ? 1 : 0 );
        count++;
    }
    if( count >= SD_MAX_VOLT_TRIAL )
    {
        errorstatus = SD_INVALID_VOLTRANGE;
        return errorstatus;
    }
    CardType = SDIO_HIGH_CAPACITY_MMC_CARD;
    return( errorstatus );
}

/*********************************************************************
 * @fn      eMMC_PowerOFF
 *
 * @brief   eMMC Power off
 *
 * @return  SD_Error - ERR code
 */
SD_Error eMMC_PowerOFF( void )
{
    SDIO_SetPowerState( SDIO_PowerState_OFF );
    return SD_OK;
}

/*********************************************************************
 * @fn      eMMC_Change_Tran_Mode
 *
 * @brief   eMMC change initial mode to Transform mode.
 *
 * @return  none
 */
void eMMC_Change_Tran_Mode(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );
}
/*********************************************************************
 * @fn      eMMC_Read_CID_CSD
 *
 * @brief   eMMC Read CID Register and CSD Register
 *
 * @return  SD_Error - ERR code
 */
SD_Error eMMC_Read_CID_CSD( void )
{
    SD_Error errorstatus = SD_OK;
    u16 rca = 0x01;
    if( SDIO_GetPowerState() == 0 )
    {
        return SD_REQUEST_NOT_APPLICABLE;
    }
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );
    errorstatus = CmdResp2Error();
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    CID_Tab[0] = SDIO->RESP1;
    CID_Tab[1] = SDIO->RESP2;
    CID_Tab[2] = SDIO->RESP3;
    CID_Tab[3] = SDIO->RESP4;

    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );
    errorstatus = CmdResp6Error( SD_CMD_SET_REL_ADDR, &rca );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    RCA = rca;
    SDIO_CmdInitStructure.SDIO_Argument = ( uint32_t )( rca << 16 );
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp2Error();
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    CSD_Tab[0] = SDIO->RESP1;
    CSD_Tab[1] = SDIO->RESP2;
    CSD_Tab[2] = SDIO->RESP3;
    CSD_Tab[3] = SDIO->RESP4;
    return SD_OK;
}
/*********************************************************************
 * @fn      eMMC_ReadExtCsd
 *
 * @brief   eMMC Read Extended CSD Register
 *
 * @return  SD_Error - ERR code
 */

SD_Error eMMC_ReadExtCsd(eMMC_CardInfo *eMMC)
{
  SD_Error Result = SD_OK;
  u32 count = 0;
  u32 *ExtCsdBuf;
  ExtCsdBuf = (u32 *)(&(eMMC->eMMC_ExtCsd.CsdBuf[0]));

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = (u32)512;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (u32) 9 << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

    /* CMD8 */
  SDIO_CmdInitStructure.SDIO_Argument = 0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_HS_SEND_EXT_CSD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  Result = CmdResp1Error(SD_CMD_HS_SEND_EXT_CSD);

  if (SD_OK != Result)
  {
      return(Result);
  }
  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
      if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
      {
          for (count = 0; count < 8; count++)
          {
              *(ExtCsdBuf + count) = SDIO_ReadData();
          }
          ExtCsdBuf += 8;
      }
  }
  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
      SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
      Result = SD_DATA_TIMEOUT;
      return(Result);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
      SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
      Result = SD_DATA_CRC_FAIL;
      return(Result);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
      SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
      Result = SD_RX_OVERRUN;
      return(Result);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
      SDIO_ClearFlag(SDIO_FLAG_STBITERR);
      Result = SD_START_BIT_ERR;
      return(Result);
  }
  count = SD_DATATIMEOUT;
  while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) && (count > 0))
  {
      *ExtCsdBuf = SDIO_ReadData();
      ExtCsdBuf++;
      count--;
  }
  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return Result;
}
/*********************************************************************
 * @fn       eMMC_GetCardInfo
 *
 * @brief    Get eMMC information
 *
 * @return  SD_Error -  ERR code
 */
SD_Error eMMC_GetCardInfo(eMMC_CardInfo *eMMC, u32 *CSD_Tab, u32 *CID_Tab, u16 Rca)
{
    SD_Error errorstatus = SD_OK;
    u8 tmp = 0;
    eMMC->CardType = (u8)CardType;
    eMMC->RCA = (u16)Rca;
    //CSD
    /* Byte 0 */
    tmp = (u8)((CSD_Tab[0] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.CSDStruct = (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
    eMMC->eMMC_csd.Reserved1 = tmp & 0x03;

    /* Byte 1 */
    tmp = (u8)((CSD_Tab[0] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.TAAC = tmp;

    /* Byte 2 */
    tmp = (u8)((CSD_Tab[0] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.NSAC = tmp;

    /* Byte 3 */
    tmp = (u8)(CSD_Tab[0] & 0x000000FF);
    eMMC->eMMC_csd.MaxBusClkFrec = tmp;

    /* Byte 4 */
    tmp = (u8)((CSD_Tab[1] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.CardComdClasses = tmp << 4;

    /* Byte 5 */
    tmp = (u8)((CSD_Tab[1] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
    eMMC->eMMC_csd.RdBlockLen = tmp & 0x0F;

    /* Byte 6 */
    tmp = (u8)((CSD_Tab[1] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.PartBlockRead = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.DSRImpl = (tmp & 0x10) >> 4;
    eMMC->eMMC_csd.Reserved2 = 0; /*!< Reserved */
    eMMC->eMMC_csd.DeviceSize = (tmp & 0x03) << 10;

    /* Byte 7 */
    tmp = (u8)(CSD_Tab[1] & 0x000000FF);
    eMMC->eMMC_csd.DeviceSize |= (tmp) << 2;

    /* Byte 8 */
    tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.DeviceSize |= (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    eMMC->eMMC_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /* Byte 9 */
    tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    eMMC->eMMC_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    eMMC->eMMC_csd.DeviceSizeMul = (tmp & 0x03) << 1;

    /* Byte 10 */
    tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    eMMC->CardBlockSize = 1 << (eMMC->eMMC_csd.RdBlockLen);
    eMMC->SectorNums = (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[3] << 24) | (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[2] << 16) |
            (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[1] << 8) | (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[0]);
    eMMC->eMMC_csd.EraseGrSize = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.EraseGrMul = (tmp & 0x3F) << 1;

    /* Byte 11 */
    tmp = (u8)(CSD_Tab[2] & 0x000000FF);
    eMMC->eMMC_csd.EraseGrMul |= (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.WrProtectGrSize = (tmp & 0x7F);

    /* Byte 12 */
    tmp = (u8)((CSD_Tab[3] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.ManDeflECC = (tmp & 0x60) >> 5;
    eMMC->eMMC_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    eMMC->eMMC_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

    /* Byte 13 */
    tmp = (u8)((CSD_Tab[3] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.Reserved3 = 0;
    eMMC->eMMC_csd.ContentProtectAppli = (tmp & 0x01);

    /* Byte 14 */
    tmp = (u8)((CSD_Tab[3] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.CopyFlag = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.PermWrProtect = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.TempWrProtect = (tmp & 0x10) >> 4;
    eMMC->eMMC_csd.FileFormat = (tmp & 0x0C) >> 2;
    eMMC->eMMC_csd.ECC = (tmp & 0x03);

    /* Byte 15 */
    tmp = (u8)(CSD_Tab[3] & 0x000000FF);
    eMMC->eMMC_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    eMMC->eMMC_csd.Reserved4 = 1;

    //CID
    /* Byte 0 */
    tmp = (u8)((CID_Tab[0] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ManufacturerID = tmp;

    /* Byte 1 */
    tmp = (u8)((CID_Tab[0] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.OEM_AppliID = tmp << 8;

    /* Byte 2 */
    tmp = (u8)((CID_Tab[0] & 0x000000FF00) >> 8);
    eMMC->eMMC_cid.OEM_AppliID |= tmp;

    /* Byte 3 */
    tmp = (u8)(CID_Tab[0] & 0x000000FF);
    eMMC->eMMC_cid.ProdName1 = tmp << 24;

    /* Byte 4 */
    tmp = (u8)((CID_Tab[1] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdName1 |= tmp << 16;

    /* Byte 5 */
    tmp = (u8)((CID_Tab[1] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.ProdName1 |= tmp << 8;

    /* Byte 6 */
    tmp = (u8)((CID_Tab[1] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ProdName1 |= tmp;

    /* Byte 7 */
    tmp = (u8)(CID_Tab[1] & 0x000000FF);
    eMMC->eMMC_cid.ProdName2 = tmp;

    /* Byte 8 */
    tmp = (u8)((CID_Tab[2] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdRev = tmp;

    /* Byte 9 */
    tmp = (u8)((CID_Tab[2] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.ProdSN = tmp << 24;

    /* Byte 10 */
    tmp = (u8)((CID_Tab[2] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ProdSN |= tmp << 16;

    /* Byte 11 */
    tmp = (u8)(CID_Tab[2] & 0x000000FF);
    eMMC->eMMC_cid.ProdSN |= tmp << 8;

    /* Byte 12 */
    tmp = (u8)((CID_Tab[3] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdSN |= tmp;

    /* Byte 13 */
    tmp = (u8)((CID_Tab[3] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    eMMC->eMMC_cid.ManufactDate = (tmp & 0x0F) << 8;

    /* Byte 14 */
    tmp = (u8)((CID_Tab[3] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ManufactDate |= tmp;

    /* Byte 15 */
    tmp = (u8)(CID_Tab[3] & 0x000000FF);
    eMMC->eMMC_cid.CID_CRC = (tmp & 0xFE) >> 1;
    eMMC->eMMC_cid.Reserved2 = 1;
    return(errorstatus);
}


/*********************************************************************
 * @fn      SD_EnableWideBusOperation
 *
 * @brief   Enable SDIO Wide bus
 *
 * @param   wmode: 0-1bit 1-4bit 2-8bit
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_EnableWideBusOperation( u32 wmode )
{
    SD_Error errorstatus = SD_OK;
    if( wmode > 2 )
    {
        return SD_UNSUPPORTED_FEATURE;
    }
    else
    {
        switch(wmode)
        {
            case 0:
                SDIO_CmdInitStructure.SDIO_Argument = 0x03B70000;
                SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
                SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                SDIO_SendCommand( &SDIO_CmdInitStructure );
                errorstatus = CmdResp1Error( SD_CMD_APP_SD_SET_BUSWIDTH );
            break;
            case 1:
                SDIO_CmdInitStructure.SDIO_Argument = 0x03B70100;
                SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
                SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                SDIO_SendCommand( &SDIO_CmdInitStructure );
                errorstatus = CmdResp1Error( SD_CMD_APP_SD_SET_BUSWIDTH );
            break;
            case 2:
                SDIO_CmdInitStructure.SDIO_Argument = 0x03B70200;
                SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
                SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                SDIO_SendCommand( &SDIO_CmdInitStructure );
                errorstatus = CmdResp1Error( SD_CMD_APP_SD_SET_BUSWIDTH );
            break;
        }
        if( SD_OK == errorstatus )
        {
            SDIO->CLKCR &= ~( 3 << 11 );
            SDIO->CLKCR |= ( u16 )wmode << 11;
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      SD_SetDeviceMode
 *
 * @brief   Select DMA mode or POLLING mode
 *
 * @param   wmode - work mode
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_SetDeviceMode( u32 Mode )
{
    SD_Error errorstatus = SD_OK;
    if( ( Mode == SD_DMA_MODE ) || ( Mode == SD_POLLING_MODE ) )
    {
        DeviceMode = Mode;
    }
    else
    {
        errorstatus = SD_INVALID_PARAMETER;
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      SD_SelectDeselect
 *
 * @brief   Select eMMC
 *
 * @param   addr - RCA address
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_SelectDeselect( u32 addr )
{
    SDIO_CmdInitStructure.SDIO_Argument =  addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );
    return CmdResp1Error( SD_CMD_SEL_DESEL_CARD );
}

/*********************************************************************
 * @fn       SD_ReadBlock
 *
 * @brief   Read block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_ReadBlock( u8 *buf, long long addr, u16 blksize )
{
    SD_Error errorstatus = SD_OK;
    u8 power;
    u32 count = 0, *tempbuff = ( u32 * )buf;
    u32 timeout = SDIO_DATATIMEOUT;
    if( NULL == buf )
    {
        return SD_INVALID_PARAMETER;
    }
    SDIO->DCTRL &= 0x0;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b ;
    SDIO_DataInitStructure.SDIO_DataLength = 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );
    if( SDIO->RESP1 & SD_CARD_LOCKED )
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if( CardType == SDIO_HIGH_CAPACITY_MMC_CARD )
    {
        blksize = 512;
        addr >>= 9;
    }
    if( ( blksize > 0 ) && ( blksize <= 2048 ) && ( ( blksize & ( blksize - 1 ) ) == 0 ) )
    {
        power = convert_from_bytes_to_power_of_two( blksize );
        SDIO_CmdInitStructure.SDIO_Argument =  blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );
        errorstatus = CmdResp1Error( SD_CMD_SET_BLOCKLEN );
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER;
    }
    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4 ;
    SDIO_DataInitStructure.SDIO_DataLength = blksize ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );

    SDIO_CmdInitStructure.SDIO_Argument =  addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );
    errorstatus = CmdResp1Error( SD_CMD_READ_SINGLE_BLOCK );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    if( DeviceMode == SD_POLLING_MODE )
    {
        while( !( SDIO->STA & ( ( 1 << 5 ) | ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 10 ) | ( 1 << 9 ) ) ) )
        {
            if( SDIO_GetFlagStatus( SDIO_FLAG_RXFIFOHF ) != RESET )
            {
                for( count = 0; count < 8; count++ )
                {
                    *( tempbuff + count ) = SDIO->FIFO;
                }
                tempbuff += 8;
                timeout = 0X7FFFFF;
            }
            else
            {
                if( timeout == 0 )
                {
                    return SD_DATA_TIMEOUT;
                }
                timeout--;
            }
        }
        if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
            return SD_DATA_TIMEOUT;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
            return SD_DATA_CRC_FAIL;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_RXOVERR ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_RXOVERR );
            return SD_RX_OVERRUN;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_STBITERR );
            return SD_START_BIT_ERR;
        }
        while( SDIO_GetFlagStatus( SDIO_FLAG_RXDAVL ) != RESET )
        {
            *tempbuff = SDIO_ReadData();
            tempbuff++;
        }
        SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    }
    else if( DeviceMode == SD_DMA_MODE )
    {
        SD_DMA_Config( ( u32 * )buf, blksize, DMA_DIR_PeripheralSRC );
        TransferError = SD_OK;
        StopCondition = 0;
        TransferEnd = 0;
        SDIO->MASK |= ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 5 ) | ( 1 << 9 );
        SDIO_DMACmd( ENABLE );
        while( ( ( DMA2->INTFR & 0X2000 ) == RESET ) && ( TransferEnd == 0 ) && ( TransferError == SD_OK ) && timeout )
        {
            timeout--;
        }
        if( timeout == 0 )
        {
            return SD_DATA_TIMEOUT;
        }
        if( TransferError != SD_OK )
        {
            errorstatus = TransferError;
        }
    }
    return errorstatus;
}

__attribute__( ( aligned( 4 ) ) ) u32 *tempbuff;
/*********************************************************************
 * @fn       SD_ReadMultiBlocks
 *
 * @brief   Read multiple block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *                 nblks - block number
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_ReadMultiBlocks( u8 *buf, long long addr, u16 blksize, u32 nblks )
{
    SD_Error errorstatus = SD_OK;
    u8 power;
    u32 count = 0;
    u32 timeout = SDIO_DATATIMEOUT;
    tempbuff = ( u32 * )buf;
    while( SD_BUSY==1 )
    {
        SD_ProcessIRQSrc();
    }
    SDIO->DCTRL = 0x0;
    if( CardType == SDIO_HIGH_CAPACITY_MMC_CARD )
    {
        blksize = 512;
        addr >>= 9;
    }
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0; ;
    SDIO_DataInitStructure.SDIO_DataLength = 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );

    if( SDIO->RESP1 & SD_CARD_LOCKED )
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if( ( blksize > 0 ) && ( blksize <= 2048 ) && ( ( blksize & ( blksize - 1 ) ) == 0 ) )
    {
        power = convert_from_bytes_to_power_of_two( blksize );
        SDIO_CmdInitStructure.SDIO_Argument =  blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );
        errorstatus = CmdResp1Error( SD_CMD_SET_BLOCKLEN );
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER;
    }
    if( nblks > 1 )
    {
        if( nblks * blksize > SD_MAX_DATA_LENGTH )
        {
            return SD_INVALID_PARAMETER;
        }
        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4; ;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize ;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig( &SDIO_DataInitStructure );

        SDIO_CmdInitStructure.SDIO_Argument =  addr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );
        errorstatus = CmdResp1Error( SD_CMD_READ_MULT_BLOCK );
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
        if( DeviceMode == SD_POLLING_MODE )
        {
            while( !( SDIO->STA & ( ( 1 << 5 ) | ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 9 ) ) ) )
            {
                if( SDIO_GetFlagStatus( SDIO_FLAG_RXFIFOHF ) != RESET )
                {
                    for( count = 0; count < 8; count++ )
                    {
                        *( tempbuff + count ) = SDIO->FIFO;
                    }
                    tempbuff += 8;
                    timeout = 0X7FFFFF;
                }
                else
                {
                    if( timeout == 0 )
                    {
                        return SD_DATA_TIMEOUT;
                    }
                    timeout--;
                }
            }
            if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
                return SD_DATA_TIMEOUT;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
                return SD_DATA_CRC_FAIL;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_RXOVERR ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_RXOVERR );
                return SD_RX_OVERRUN;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_STBITERR );
                return SD_START_BIT_ERR;
            }
            while( SDIO_GetFlagStatus( SDIO_FLAG_RXDAVL ) != RESET )
            {
                *tempbuff = SDIO_ReadData();
                tempbuff++;
            }
            if( SDIO_GetFlagStatus( SDIO_FLAG_DATAEND ) != RESET )
            {
                if( ( SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType ) || ( SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType ) || ( SDIO_HIGH_CAPACITY_SD_CARD == CardType ) )
                {
                    SDIO_CmdInitStructure.SDIO_Argument =  0;
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand( &SDIO_CmdInitStructure );
                    errorstatus = CmdResp1Error( SD_CMD_STOP_TRANSMISSION );
                    if( errorstatus != SD_OK )
                    {
                        return errorstatus;
                    }
                }
            }
            SDIO_ClearFlag( SDIO_STATIC_FLAGS );
        }
        else if( DeviceMode == SD_DMA_MODE )
        {
            SD_DMA_Config( ( u32 * )buf, nblks * blksize, DMA_DIR_PeripheralSRC );
            TransferError = SD_OK;
            StopCondition = 1;
            TransferEnd = 0;
            SDIO_ClearFlag( SDIO_FLAG_DATAEND );
            SDIO->MASK |= ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 5 ) | ( 1 << 9 );
            SDIO->DCTRL |= 1 << 3;
            SD_BUSY=1;
            while( ( ( DMA2->INTFR & 0X4000 ) == RESET )&&( TransferEnd == 0 ) && ( TransferError == SD_OK ) && timeout )
            {
                timeout--;
            }
            if( timeout == 0 )
            {
                return SD_DATA_TIMEOUT;
            }
            if( TransferError != SD_OK )
            {
                errorstatus = TransferError;
            }
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_WriteBlock
 *
 * @brief   Write block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_WriteBlock( u8 *buf, long long addr,  u16 blksize )
{
    SD_Error errorstatus = SD_OK;
    u8  power = 0, cardstate = 0;
    u32 timeout = 0, bytestransferred = 0;
    u32 cardstatus = 0, count = 0, restwords = 0;
    u32 tlen = blksize;
    u32 *tempbuff = ( u32 * )buf;

    if( buf == NULL )
    {
        return SD_INVALID_PARAMETER;
    }

    SDIO->DCTRL = 0x0;

    SDIO_DataInitStructure.SDIO_DataBlockSize = 0; ;
    SDIO_DataInitStructure.SDIO_DataLength = 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );


    if( SDIO->RESP1 & SD_CARD_LOCKED )
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if( CardType == SDIO_HIGH_CAPACITY_MMC_CARD )
    {
        blksize = 512;
        addr >>= 9;
    }
    if( ( blksize > 0 ) && ( blksize <= 2048 ) && ( ( blksize & ( blksize - 1 ) ) == 0 ) )
    {
        power = convert_from_bytes_to_power_of_two( blksize );

        SDIO_CmdInitStructure.SDIO_Argument = blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdResp1Error( SD_CMD_SET_BLOCKLEN );

        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }

    }
    else
    {
        return SD_INVALID_PARAMETER;
    }

    SDIO_CmdInitStructure.SDIO_Argument = ( u32 )RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_SEND_STATUS );

    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    cardstatus = SDIO->RESP1;
    timeout = SD_DATATIMEOUT;
    while( ( ( cardstatus & 0x00000100 ) == 0 ) && ( timeout > 0 ) )
    {
        timeout--;

        SDIO_CmdInitStructure.SDIO_Argument = ( u32 )RCA << 16;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdResp1Error( SD_CMD_SEND_STATUS );
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
        cardstatus = SDIO->RESP1;
    }
    if( timeout == 0 )
    {
        return SD_ERROR;
    }

    SDIO_CmdInitStructure.SDIO_Argument = addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_WRITE_SINGLE_BLOCK );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    StopCondition = 0;

    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4; ;
    SDIO_DataInitStructure.SDIO_DataLength = blksize ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );

    timeout = SDIO_DATATIMEOUT;
    if( DeviceMode == SD_POLLING_MODE )
    {
        while( !( SDIO->STA & ( ( 1 << 10 ) | ( 1 << 4 ) | ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 9 ) ) ) )
        {
            if( SDIO_GetFlagStatus( SDIO_FLAG_TXFIFOHE ) != RESET )
            {
                if( ( tlen - bytestransferred ) < SD_HALFFIFOBYTES )
                {
                    restwords = ( ( tlen - bytestransferred ) % 4 == 0 ) ? ( ( tlen - bytestransferred ) / 4 ) : ( ( tlen - bytestransferred ) / 4 + 1 );

                    for( count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4 )
                    {
                        SDIO_WriteData( *tempbuff );

                    }
                }
                else
                {
                    for( count = 0; count < 8; count++ )
                    {
                        SDIO_WriteData( *( tempbuff + count ) );
                                          }
                    tempbuff += 8;
                    bytestransferred += 32;
                }
                timeout = 0X3FFFFFFF;
            }
            else
            {
                if( timeout == 0 )
                {
                    return SD_DATA_TIMEOUT;
                }
                timeout--;
            }
        }
        if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
            return SD_DATA_TIMEOUT;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
            return SD_DATA_CRC_FAIL;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_TXUNDERR ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_TXUNDERR );
            return SD_TX_UNDERRUN;
        }
        else if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
        {
            SDIO_ClearFlag( SDIO_FLAG_STBITERR );
            return SD_START_BIT_ERR;
        }

        SDIO->ICR = 0X5FF;
    }
    else if( DeviceMode == SD_DMA_MODE )
    {
        SD_DMA_Config( ( u32 * )buf, blksize, DMA_DIR_PeripheralDST );
        TransferError = SD_OK;
        StopCondition = 0;
        TransferEnd = 0;
        SDIO->MASK |= ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 4 ) | ( 1 << 9 );
        SDIO->DCTRL |= 1 << 3;
        while( ( ( DMA2->INTFR & 0X2000 ) == RESET ) && timeout )
        {
            timeout--;
        }
        if( timeout == 0 )
        {
            eMMC_Init();
            return SD_DATA_TIMEOUT;
        }
        timeout = SDIO_DATATIMEOUT;
        while( ( TransferEnd == 0 ) && ( TransferError == SD_OK ) && timeout )
        {
            timeout--;
        }
        if( timeout == 0 )
        {
            return SD_DATA_TIMEOUT;
        }
        if( TransferError != SD_OK )
        {
            return TransferError;
        }
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    errorstatus = IsCardProgramming( &cardstate );
    while( ( errorstatus == SD_OK ) && ( ( cardstate == SD_CARD_PROGRAMMING ) || ( cardstate == SD_CARD_RECEIVING ) ) )
    {
        errorstatus = IsCardProgramming( &cardstate );
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_WriteBlock
 *
 * @brief   Write multiple block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *                 nblks - block number
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_WriteMultiBlocks( u8 *buf, long long addr, u16 blksize, u32 nblks )
{
    SD_Error errorstatus = SD_OK;
    u8  power = 0, cardstate = 0;
    u32 timeout = 0, bytestransferred = 0;
    u32 count = 0, restwords = 0;
    u32 tlen = nblks * blksize;
    u32 *tempbuff = ( u32 * )buf;
    if( buf == NULL )
    {
        return SD_INVALID_PARAMETER;
    }
    SDIO->DCTRL = 0x0;

    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    SDIO_DataInitStructure.SDIO_DataLength = 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig( &SDIO_DataInitStructure );

    if( SDIO->RESP1 & SD_CARD_LOCKED )
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if( CardType == SDIO_HIGH_CAPACITY_MMC_CARD )
    {
        blksize = 512;
        addr >>= 9;
    }
    if( ( blksize > 0 ) && ( blksize <= 2048 ) && ( ( blksize & ( blksize - 1 ) ) == 0 ) )
    {
        power = convert_from_bytes_to_power_of_two( blksize );

        SDIO_CmdInitStructure.SDIO_Argument = blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdResp1Error( SD_CMD_SET_BLOCKLEN );

        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }

    }
    else
    {
        return SD_INVALID_PARAMETER;
    }
    if( nblks > 1 )
    {
        if( nblks * blksize > SD_MAX_DATA_LENGTH )
        {
            return SD_INVALID_PARAMETER;
        }
        if( ( SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType ) || ( SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType ) || ( SDIO_HIGH_CAPACITY_SD_CARD == CardType ) )
        {
            SDIO_CmdInitStructure.SDIO_Argument = ( u32 )RCA << 16;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand( &SDIO_CmdInitStructure );

            errorstatus = CmdResp1Error( SD_CMD_APP_CMD );

            if( errorstatus != SD_OK )
            {
                return errorstatus;
            }

            SDIO_CmdInitStructure.SDIO_Argument = nblks;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand( &SDIO_CmdInitStructure );

            errorstatus = CmdResp1Error( SD_CMD_SET_BLOCK_COUNT );

            if( errorstatus != SD_OK )
            {
                return errorstatus;
            }

        }

        SDIO_CmdInitStructure.SDIO_Argument = addr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand( &SDIO_CmdInitStructure );

        errorstatus = CmdResp1Error( SD_CMD_WRITE_MULT_BLOCK );

        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }

        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4; ;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize ;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT ;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig( &SDIO_DataInitStructure );

        if( DeviceMode == SD_POLLING_MODE )
        {
            timeout = SDIO_DATATIMEOUT;

            while( !( SDIO->STA & ( ( 1 << 4 ) | ( 1 << 1 ) | ( 1 << 8 ) | ( 1 << 3 ) | ( 1 << 9 ) ) ) )
            {
                if( SDIO->STA & ( 1 << 14 ) )
                {
                    if( ( tlen - bytestransferred ) < SD_HALFFIFOBYTES )
                    {
                        restwords = ( ( tlen - bytestransferred ) % 4 == 0 ) ? ( ( tlen - bytestransferred ) / 4 ) : ( ( tlen - bytestransferred ) / 4 + 1 );
                        for( count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4 )
                        {
                            SDIO_WriteData( *tempbuff );
                        }
                    }
                    else
                    {
                        for( count = 0; count < SD_HALFFIFO; count++ )
                        {
                            SDIO_WriteData( *( tempbuff + count ) );
                        }
                        tempbuff += SD_HALFFIFO;
                        bytestransferred += SD_HALFFIFOBYTES;
                    }
                    timeout = 0X3FFFFFFF;
                }
                else
                {
                    if( timeout == 0 )
                    {
                        return SD_DATA_TIMEOUT;
                    }
                    timeout--;
                }
            }
            if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
                return SD_DATA_TIMEOUT;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
                return SD_DATA_CRC_FAIL;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_TXUNDERR ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_TXUNDERR );
                return SD_TX_UNDERRUN;
            }
            else if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
            {
                SDIO_ClearFlag( SDIO_FLAG_STBITERR );
                return SD_START_BIT_ERR;
            }
            if( SDIO_GetFlagStatus( SDIO_FLAG_DATAEND ) != RESET )
            {
                if( ( SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType ) || ( SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType ) || ( SDIO_HIGH_CAPACITY_SD_CARD == CardType ) )
                {
                    SDIO_CmdInitStructure.SDIO_Argument = 0;
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand( &SDIO_CmdInitStructure );

                    errorstatus = CmdResp1Error( SD_CMD_STOP_TRANSMISSION );
                    if( errorstatus != SD_OK )
                    {
                        return errorstatus;
                    }
                }
            }
            SDIO_ClearFlag( SDIO_STATIC_FLAGS );
        }
        else if( DeviceMode == SD_DMA_MODE )
        {
            SD_DMA_Config( ( u32 * )buf, nblks * blksize, DMA_DIR_PeripheralDST );
            TransferError = SD_OK;
            StopCondition = 1;
            TransferEnd = 0;
            SDIO->MASK |= ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 4 ) | ( 1 << 9 );
            SDIO->DCTRL |= 1 << 3;
            timeout = SDIO_DATATIMEOUT;
            while( ( ( DMA2->INTFR & 0X2000 ) == RESET ) && timeout )
            {
                timeout--;
            }
            if( timeout == 0 )
            {
                eMMC_Init();
                return SD_DATA_TIMEOUT;
            }
            timeout = SDIO_DATATIMEOUT;
            while( ( TransferEnd == 0 ) && ( TransferError == SD_OK ) && timeout )
            {
                timeout--;
            }
            if( timeout == 0 )
            {
                eMMC_Init();
                return SD_DATA_TIMEOUT;
            }
            timeout = SDIO_DATATIMEOUT;
            while( ( TransferEnd == 0 ) && ( TransferError == SD_OK ) && timeout )
            {
                SD_ProcessIRQSrc();
                timeout--;
            }

            if( timeout == 0 )
            {
                return SD_DATA_TIMEOUT;
            }
            if( TransferError != SD_OK )
            {
                return TransferError;
            }
        }
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    errorstatus = IsCardProgramming( &cardstate );
    while( ( errorstatus == SD_OK ) && ( ( cardstate == SD_CARD_PROGRAMMING ) || ( cardstate == SD_CARD_RECEIVING ) ) )
    {
        errorstatus = IsCardProgramming( &cardstate );
    }
    return errorstatus;
}

void SDIO_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SDIO_IRQHandler
 *
 * @brief   This function handles SDIO exception.
 *
 * @return  None
 */
void SDIO_IRQHandler( void )
{
    SD_ProcessIRQSrc();
}

/*********************************************************************
 * @fn      SD_ProcessIRQSrc
 *
 * @brief   IRQ deal function
 *
 * @return   SD_Error -  ERR code
 */
SD_Error SD_ProcessIRQSrc( void )
{
    if( SDIO->STA & ( 1 << 8 ) )
    {
        if( StopCondition == 1 )
        {
            SDIO_CmdInitStructure.SDIO_Argument = 0;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand( &SDIO_CmdInitStructure );

            TransferError = CmdResp1Error( SD_CMD_STOP_TRANSMISSION );
        }
        else
        {
            TransferError = SD_OK;
        }
        SDIO->ICR |= 1 << 8;
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferEnd = 1;
        SD_BUSY=0;
        return( TransferError );
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferError = SD_DATA_CRC_FAIL;
        return( SD_DATA_CRC_FAIL );
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferError = SD_DATA_TIMEOUT;
        return( SD_DATA_TIMEOUT );
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_RXOVERR ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_RXOVERR );
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferError = SD_RX_OVERRUN;
        return( SD_RX_OVERRUN );
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_TXUNDERR ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_TXUNDERR );
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferError = SD_TX_UNDERRUN;
        return( SD_TX_UNDERRUN );
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_STBITERR );
        SDIO->MASK &= ~( ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 8 ) | ( 1 << 14 ) | ( 1 << 15 ) | ( 1 << 4 ) | ( 1 << 5 ) | ( 1 << 9 ) );
        TransferError = SD_START_BIT_ERR;
        return( SD_START_BIT_ERR );
    }
    return( SD_OK );
}

/*********************************************************************
 * @fn      CmdError
 *
 * @brief   Errer commend
 *
 * @return   SD_Error -  ERR code
 */
SD_Error CmdError( void )
{
    SD_Error errorstatus = SD_OK;
    u32 timeout = SDIO_CMD0TIMEOUT;
    while( timeout-- )
    {
        if( SDIO_GetFlagStatus( SDIO_FLAG_CMDSENT ) != RESET )
        {
            break;
        }
    }
    if( timeout == 0 )
    {
        return SD_CMD_RSP_TIMEOUT;
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    return errorstatus;
}

/*********************************************************************
 * @fn      CmdResp7Error
 *
 * @brief   R7 Errer commend
 *
 * @return   SD_Error -  ERR code
 */
SD_Error CmdResp7Error( void )
{
    SD_Error errorstatus = SD_OK;
    u32 status=0;
    u32 timeout = SDIO_CMD0TIMEOUT;
    while( timeout-- )
    {
        status = SDIO->STA;
        if( status & ( ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 6 ) ) )
        {
            break;
        }
    }
    if( ( timeout == 0 ) || ( status & ( 1 << 2 ) ) )
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return errorstatus;
    }
    if( ( status ) & ( 1 << 6 ) )
    {
        errorstatus = SD_OK;
        SDIO_ClearFlag( SDIO_FLAG_CMDREND );
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      CmdResp1Error
 *
 * @brief   R1 Errer commend
 *
 * @return   SD_Error -  ERR code
 */
SD_Error CmdResp1Error( u8 cmd )
{
    u32 status;
    while( 1 )
    {
        status = SDIO->STA;
        if( status & ( ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 6 ) ) )
        {
            break;
        }
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return SD_CMD_RSP_TIMEOUT;
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CCRCFAIL ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CCRCFAIL );
        return SD_CMD_CRC_FAIL;
    }
    if( SDIO->RESPCMD != cmd )
    {
        return SD_ILLEGAL_CMD;
    }
    SDIO->ICR = 0X5FF;
    return ( SD_Error )( SDIO->RESP1 & SD_OCR_ERRORBITS );
}

/*********************************************************************
 * @fn      CmdResp3Error
 *
 * @brief   R3 Errer commend
 *
 * @return   SD_Error -  ERR code
 */
SD_Error CmdResp3Error( void )
{
    u32 status;
    while( 1 )
    {
        status = SDIO->STA;
        if( status & ( ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 6 ) ) )
        {
            break;
        }
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return SD_CMD_RSP_TIMEOUT;
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    return SD_OK;
}

/*********************************************************************
 * @fn      CmdResp2Error
 *
 * @brief   R2 Errer commend
 *
 * @return   SD_Error -  ERR code
 */
SD_Error CmdResp2Error( void )
{
    SD_Error errorstatus = SD_OK;
    u32 status=0;
    u32 timeout = SDIO_CMD0TIMEOUT;
    while( timeout-- )
    {
        status = SDIO->STA;
        if( status & ( ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 6 ) ) )
        {
            break;
        }
    }
    if( ( timeout == 0 ) || ( status & ( 1 << 2 ) ) )
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return errorstatus;
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CCRCFAIL ) != RESET )
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag( SDIO_FLAG_CCRCFAIL );
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );
    return errorstatus;
}

/*********************************************************************
 * @fn       CmdResp6Error
 *
 * @brief   R6 Errer commend
 *
 * @param   cmd - Send commend
 *                 *prca - RCA address
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error CmdResp6Error( u8 cmd, u16 *prca )
{
    SD_Error errorstatus = SD_OK;
    u32 status=0;
    u32 rspr1;
    while( 1 )
    {
        status = SDIO->STA;
        if( status & ( ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 6 ) ) )
        {
            break;
        }
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return SD_CMD_RSP_TIMEOUT;
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CCRCFAIL ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CCRCFAIL );
        return SD_CMD_CRC_FAIL;
    }
    if( SDIO->RESPCMD != cmd )
    {
        return SD_ILLEGAL_CMD;
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );

    rspr1 = SDIO->RESP1;
    if( SD_ALLZERO == ( rspr1 & ( SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED ) ) )
    {
        *prca = ( u16 )( rspr1 >> 16 );
        return errorstatus;
    }
    if( rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR )
    {
        return SD_GENERAL_UNKNOWN_ERROR;
    }
    if( rspr1 & SD_R6_ILLEGAL_CMD )
    {
        return SD_ILLEGAL_CMD;
    }
    if( rspr1 & SD_R6_COM_CRC_FAILED )
    {
        return SD_COM_CRC_FAILED;
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       IsCardProgramming
 *
 * @brief   Check SD card write statue
 *
 * @param   pcardstatus - Current status
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error IsCardProgramming( u8 *pstatus )
{
    vu32 respR1 = 0, status = 0;

    SDIO_CmdInitStructure.SDIO_Argument = ( uint32_t ) RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    status = SDIO->STA;

    while( !( status & ( ( 1 << 0 ) | ( 1 << 6 ) | ( 1 << 2 ) ) ) )
    {
        status = SDIO->STA;
    }

    if( SDIO_GetFlagStatus( SDIO_FLAG_CCRCFAIL ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CCRCFAIL );
        return SD_CMD_CRC_FAIL;
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_CTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_CTIMEOUT );
        return SD_CMD_RSP_TIMEOUT;
    }
    if( SDIO->RESPCMD != SD_CMD_SEND_STATUS )
    {
        return SD_ILLEGAL_CMD;
    }

    SDIO_ClearFlag( SDIO_STATIC_FLAGS );

    respR1 = SDIO->RESP1;

    *pstatus = ( u8 )( ( respR1 >> 9 ) & 0x0000000F );
    return SD_OK;
}

/*********************************************************************
 * @fn       SD_SendStatus
 *
 * @brief   Read SD card write statue
 *
 * @param   pcardstatus - Current status
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_SendStatus( uint32_t *pcardstatus )
{
    SD_Error errorstatus = SD_OK;
    if( pcardstatus == NULL )
    {
        errorstatus = SD_INVALID_PARAMETER;
        return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argument = ( uint32_t ) RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_SEND_STATUS );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    *pcardstatus = SDIO->RESP1;
    return errorstatus;
}

/*********************************************************************
 * @fn      SD_GetState
 *
 * @brief   Get SD card write statue
 *
 * @return   SD_Error -  ERR code
 */
SDCardState SD_GetState( void )
{
    u32 resp1 = 0;
    if( SD_SendStatus( &resp1 ) != SD_OK )
    {
        return SD_CARD_ERROR;
    }
    else
    {
        return ( SDCardState )( ( resp1 >> 9 ) & 0x0F );
    }
}

/*********************************************************************
 * @fn      FindSCR
 *
 * @brief   Find SD card SCR
 *
 * @return   SD_Error -  ERR code
 */
SD_Error FindSCR( u16 rca, u32 *pscr )
{
    u32 index = 0;
    SD_Error errorstatus = SD_OK;
    u32 tempscr[2] = {0, 0};

    SDIO_CmdInitStructure.SDIO_Argument = ( uint32_t )8;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_SET_BLOCKLEN );

    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argument = ( uint32_t ) rca << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_APP_CMD );

    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = 8;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b  ;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig( &SDIO_DataInitStructure );

    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand( &SDIO_CmdInitStructure );

    errorstatus = CmdResp1Error( SD_CMD_SD_APP_SEND_SCR );

    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    while( !( SDIO->STA & ( SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR ) ) )
    {
        if( SDIO_GetFlagStatus( SDIO_FLAG_RXDAVL ) != RESET )
        {
            *( tempscr + index ) = SDIO_ReadData();
            index++;
            if( index >= 2 )
            {
                break;
            }
        }
    }
    if( SDIO_GetFlagStatus( SDIO_FLAG_DTIMEOUT ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_DTIMEOUT );
        return SD_DATA_TIMEOUT;
    }
    else if( SDIO_GetFlagStatus( SDIO_FLAG_DCRCFAIL ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_DCRCFAIL );
        return SD_DATA_CRC_FAIL;
    }
    else if( SDIO_GetFlagStatus( SDIO_FLAG_RXOVERR ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_RXOVERR );
        return SD_RX_OVERRUN;
    }
    else if( SDIO_GetFlagStatus( SDIO_FLAG_STBITERR ) != RESET )
    {
        SDIO_ClearFlag( SDIO_FLAG_STBITERR );
        return SD_START_BIT_ERR;
    }
    SDIO_ClearFlag( SDIO_STATIC_FLAGS );

    *( pscr + 1 ) = ( ( tempscr[0] & SD_0TO7BITS ) << 24 ) | ( ( tempscr[0] & SD_8TO15BITS ) << 8 ) | ( ( tempscr[0] & SD_16TO23BITS ) >> 8 ) | ( ( tempscr[0] & SD_24TO31BITS ) >> 24 );
    *( pscr ) = ( ( tempscr[1] & SD_0TO7BITS ) << 24 ) | ( ( tempscr[1] & SD_8TO15BITS ) << 8 ) | ( ( tempscr[1] & SD_16TO23BITS ) >> 8 ) | ( ( tempscr[1] & SD_24TO31BITS ) >> 24 );
    return errorstatus;
}

/*********************************************************************
 * @fn       convert_from_bytes_to_power_of_two
 *
 * @brief   Get NumberOfBytes value
 *
 * @param   NumberOfBytes - byte number
 *              
 * @return  SD_Error -  ERR code
 */
u8 convert_from_bytes_to_power_of_two( u16 NumberOfBytes )
{
    u8 count = 0;
    while( NumberOfBytes != 1 )
    {
        NumberOfBytes >>= 1;
        count++;
    }
    return count;
}

/*********************************************************************
 * @fn      SD_DMA_Config
 *
 * @brief   configure DMA
 *
 * @param   mbuf - Storage address
 *          bufsize - data size
 *
 * @return  SD_Error -  ERR code
 */
void SD_DMA_Config( u32 *mbuf, u32 bufsize, u32 DMA_DIR )
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2, ENABLE );

    DMA_DeInit( DMA2_Channel4 );
    DMA_Cmd( DMA2_Channel4, DISABLE );

    DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 )&SDIO->FIFO;
    DMA_InitStructure.DMA_MemoryBaseAddr = ( u32 )mbuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR;
    DMA_InitStructure.DMA_BufferSize = bufsize/4;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA2_Channel4, &DMA_InitStructure );

    DMA_Cmd( DMA2_Channel4, ENABLE );
}

/*********************************************************************
 * @fn      SD_ReadDisk
 *
 * @brief   Read eMMC
 *
 * @param   buf - data
 *            sector - sector addr
 *            cnt - sector number
 *
 * @return  SD_Error -  ERR code
 */
u8 SD_ReadDisk( u8 *buf, u32 sector, u8 cnt )
{
    u8 sta = SD_OK;
    long long lsector = sector;
    u8 n;
    lsector <<= 9;
    if( ( u32 )buf % 4 != 0 )
    {
        for( n = 0; n < cnt; n++ )
        {
            sta = SD_ReadBlock( SDIO_DATA_BUFFER, lsector + 512 * n, 512 );
            memcpy( buf, SDIO_DATA_BUFFER, 512 );
            buf += 512;
        }
    }
    else
    {
        if( cnt == 1 )
        {
            sta = SD_ReadBlock( buf, lsector, 512 );
        }
        else
        {
            sta = SD_ReadMultiBlocks( buf, lsector, 512, cnt );
        }
    }
    return sta;
}

/*********************************************************************
 * @fn      SD_WriteDisk
 *
 * @brief   Write eMMC
 *
 * @param   buf - data
 *            sector - sector addr
 *            cnt - sector number
 *
 * @return  SD_Error -  ERR code
 */
u8 SD_WriteDisk( u8 *buf, u32 sector, u8 cnt )
{
    u8 sta = SD_OK;
    u8 n;
    long long lsector = sector;
    lsector <<= 9;
    if( ( u32 )buf % 4 != 0 )
    {
        for( n = 0; n < cnt; n++ ){
            memcpy( SDIO_DATA_BUFFER, buf, 512 );
            sta = SD_WriteBlock( SDIO_DATA_BUFFER, lsector + 512 * n, 512 );
            buf += 512;
        }
    }
    else
    {
        if( cnt == 1 )
        {
            sta = SD_WriteBlock( buf, lsector, 512 );
        }
        else
        {
            sta = SD_WriteMultiBlocks( buf, lsector, 512, cnt );
        }
    }

    return sta;
}


