/********************************** (C) COPYRIGHT *******************************
 * File Name          : net_filter.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/01/12
 * Description        : MAC address filtering function.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "net_filter.h"

static const uint32_t CRC32Tab[256] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

/*********************************************************************
 * @fn      Calc_CRC32
 *
 * @brief   Calculate the value of CRC32
 *
 * @param   buf    data buff
 *          num    the number of data
 *
 * @return  the value of CRC32.
 */
static uint32_t Calc_CRC32( const uint8_t *buf, uint32_t num)
{
    uint32_t i, CRCVal;
    CRCVal = 0xFFFFFFFF;

    for (i = 0; i < num; i++)
        CRCVal = CRC32Tab[(CRCVal ^ buf[i]) & 0xff] ^ (CRCVal >> 8);

    return CRCVal^0xFFFFFFFF;
}

/*********************************************************************
 * @fn      ETH_HashFilter
 *
 * @brief   hash list destination address filtering
 *
 * @param   MACAddr    MAC address pointer
 *
 * @return  none.
 */
void ETH_HashFilter(const uint8_t *MACAddr)
{
    uint32_t CRCVal;
    uint8_t HashVal = 0;

    CRCVal = Calc_CRC32(MACAddr,6);
    CRCVal &= 0x3f;                         /* get bit[5:0] of value */

    for(uint8_t i = 0; i < 6; i++)          /* reverse bit ,such as 0x1F -> 0xF8 */
    {
        HashVal <<= 1;
        HashVal += CRCVal & 1;
        CRCVal >>= 1;
    }

    if(HashVal & 0x20)
    {
        ETH->MACHTHR |= 1<<(HashVal & 0x1f);
    }
    else {
        ETH->MACHTLR |= 1<<(HashVal & 0x1f);
    }
    if(CHECK_MAC_ISMULTICAST(MACAddr))
    {
        ETH->MACFFR &= ~((1 << 0) | (1 << 3) | (1 << 4) | (1 << 10));
        ETH->MACFFR |= ETH_MulticastFramesFilter_HashTable;
    }
    else {
        ETH->MACFFR &= ~((1 << 0) | (1 << 3) | (1 << 10));
        ETH->MACFFR |= ETH_UnicastFramesFilter_HashTable;
    }
    ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
}

/*********************************************************************
 * @fn      ETH_PerfectFilter
 *
 * @brief   Perfect destination address filtering
 *
 * @param   MACAddr    MAC address pointer
 *
 * @return  the index of MAC address register.
 */
uint8_t ETH_PerfectFilter(const uint8_t *MACAddr)
{
    uint8_t i;
    uint32_t *RegPtr = (uint32_t *)&ETH->MACA0LR;

    for(i = 0; i < 4; i++)
    {
        if(*RegPtr == 0xffffffff)
        {
            *(RegPtr - 1) = (uint32_t)(((MACAddr[5]<<8)) | MACAddr[4]);
            *RegPtr = (uint32_t)(MACAddr[0] | (MACAddr[1]<<8) | (MACAddr[2]<<16) | (MACAddr[3]<<24));
            if(i != 0)
            {
                *(RegPtr - 1) |= 1<<31;
                *(RegPtr - 1) &= ~(1<<30);
            }
            if(CHECK_MAC_ISMULTICAST(MACAddr))
            {
                ETH->MACFFR &= ~((1 << 0) | (1 << 2) | (1 << 3) | (1 << 4));
            }
            else {
                ETH->MACFFR &= ~((1 << 0) | (1 << 1) | (1 << 3));
            }
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
            break;
        }
        RegPtr += 2;
    }
    if(i == 4)
        printf("MAC address register is full !\r\n");
    return i;
}

/*********************************************************************
 * @fn      ETH_ClearMACAddrReg
 *
 * @brief   Clear MAC address register
 *
 * @param   index    the index of MAC address register
 *                   index should be selected from 0, 1, 2, 3
 *
 * @return  none.
 */
void ETH_ClearMACAddrReg(uint8_t index)
{
    uint32_t *RegPtr = (uint32_t *)&ETH->MACA0HR;

    if(index < 4)
    {
        RegPtr += index * 2;
        *RegPtr = 0xffff;
        *(RegPtr + 1) = 0xffffffff;
    }
    else {
        printf("index should be selected from 0, 1, 2, 3\r\n");
    }
}

/*********************************************************************
 * @fn      ETH_HashOrPerfectFilter
 *
 * @brief   Pass when hash filter or perfect filter match
 *
 * @param   mode    UNICAST_HashOrPerfectFilter         1
 *                  MULTICAST_HashOrPerfectFilter       2
 *
 * @return  none.
 */
void ETH_HashOrPerfectFilter(uint8_t mode)
{
    switch (mode)
    {
        case UNICAST_HashOrPerfectFilter:
            ETH->MACFFR &= ~((1 << 0) | (1 << 3));
            ETH->MACFFR |= (1 << 1) | (1 << 10);
            break;

        case MULTICAST_HashOrPerfectFilter:
            ETH->MACFFR &= ~((1 << 0) | (1 << 3) | (1 << 4));
            ETH->MACFFR |= (1 << 2) | (1 << 10);
            break;

        default:
            printf("mode err\r\n");
            break;
    }
    ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
}

/*********************************************************************
 * @fn      ETH_BroadcastFilter
 *
 * @brief   When this function is enabled, broadcast will be filtered out
 *
 * @param   mode    NewState - ENABLE or DISABLE
 *
 * @return  none.
 */
void ETH_BroadcastFilter(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACFFR |= ETH_BroadcastFramesReception_Disable;
        ETH->MACFFR &= ~ETH_PromiscuousMode_Enable;
        ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
    }
    else
    {
        ETH->MACFFR |= ETH_PromiscuousMode_Enable;
        ETH->MACFFR &= ~ETH_BroadcastFramesReception_Disable;
    }
}

/*********************************************************************
 * @fn      ETH_SrcAddrPerfectFilter
 *
 * @brief   Perfect source address filtering
 *
 * @param   MACAddr    MAC address pointer
 *
 * @return  the index of MAC address register.
 */
uint8_t ETH_SrcAddrPerfectFilter(const uint8_t *MACAddr)
{
    uint8_t i = 0;
    uint32_t *RegPtr = (uint32_t *)&ETH->MACA1LR;

    for(i = 1; i < 4; i++)
    {
        if(*RegPtr == 0xffffffff)
        {
            *(RegPtr - 1) = (uint32_t)(((MACAddr[5]<<8)) | MACAddr[4]);
            *RegPtr = (uint32_t)(MACAddr[0] | (MACAddr[1]<<8) | (MACAddr[2]<<16) | (MACAddr[3]<<24));
            *(RegPtr - 1) |= (1<<31) | (1<<30);

            ETH->MACFFR &= ~((1 << 0) | (1 << 8));
            ETH->MACFFR |= 1 << 9;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
            break;
        }
        RegPtr += 2;
    }
    if(i == 4)
        printf("MAC address register is full !\r\n");
    return i;
}
