#ifndef _GET_DATA_H
#define _GET_DATA_H
#include "debug.h"

#define USE_ES8388 1

#if USE_ES8388
#define SampleDelay         410
#endif





void voice_init(void);
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize );
#endif

