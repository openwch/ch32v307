#ifndef __UAC_10_HANDPHONE_H_
#define __UAC_10_HANDPHONE_H_
#include "stdint.h"

/* UAC1.0 5.2 Class-Specific Requests */
#define UAC_SET_CUR                 0x01
#define UAC_GET_CUR                 0x81
#define UAC_SET_MIN                 0x02
#define UAC_GET_MIN                 0x82
#define UAC_SET_MAX                 0x03
#define UAC_GET_MAX                 0X83
#define UAC_SET_RES                 0x04
#define UAC_GET_RES                 0X84
#define UAC_SET_MEM                 0x05
#define UAC_GET_MEM                 0x85
#define UAC_GET_STAT                0xFF

/* UAC1.0 Microphone Feature Unit Control Requests */
#define UAC_CS_MUTE_CONTROL         0x01

/* UAC1.0 Feature Unit Control Requests Define */
#define UAC_FEATURE_VOLUME_MAX      0x7FFF
#define UAC_FEATURE_VOLUME_MIN      0x8001
#define UAC_FEATURE_VOLUME_RES      0x0001

typedef struct
{
    struct
    {
        uint8_t mute;
        int16_t  volume_l;
        int16_t  volume_r;
    } feature_unit;
} uac_microphone_unit_t;

extern uac_microphone_unit_t uac_microphone_unit;

void UART2_Rx_Init(void);
void UAC_UART_Rx_Handle(void);
void UAC_Stream_Receive_Ctrl(uint8_t s);
#endif