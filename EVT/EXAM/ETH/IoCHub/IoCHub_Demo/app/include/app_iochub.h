#ifndef __APP_IOCHUB_H
#define __APP_IOCHUB_H

#include "wchiochub.h"

#ifdef CH32V30X
#include "ch32v30x.h"
#endif
#ifdef CH32V20X
#include "ch32v20x.h"
#endif

#define USART_REC_LEN 128


extern uint8_t localDeviceID[8];
extern uint8_t logindeviceSecret[16];
extern uint8_t wchIochubSerIp[4];//{58, 213, 74, 190};{192, 168, 1, 100};{192, 168, 1, 166};
extern uint16_t wchIochubSerPort; //20001¡¢38089
extern uint16_t wchIochubScrPort;//20000

extern uint8_t IocHub_VER[3];
extern uint8_t IocHub_DeviceID[8];
extern uint8_t IocHub_NATType;
extern uint8_t IocHub_NETType;

#if LAN_SCAN_FUNC_ENABLE
void WCHIOCHUB_lanScanCallBackP(uint8_t num,IOCHUB_LAN_DEVICE_INFO*lanDeviceList);
#endif

void WCHIOCHUB_regCallback(uint16_t state);
void AT_Process(void);
void WCHIOCHUB_TimeIsr(void);
void IoCHub_Init (void);
#endif
