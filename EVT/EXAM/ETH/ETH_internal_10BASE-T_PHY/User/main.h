/********************************** (C) COPYRIGHT *******************************
* File Name          : main.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the main.
*******************************************************************************/
#ifndef USER_MAIN_H_
#define USER_MAIN_H_

/* MII/MDI interface select */
#define PHY_ADDRESS 0x01

#define USE10BASE_T
#ifndef USE10BASE_T
//    #define USE_GIGA_MAC
    #ifndef USE_GIGA_MAC
        #define USE_FAST_MAC
        //#define USE_RMII
    #endif
#endif

#endif
