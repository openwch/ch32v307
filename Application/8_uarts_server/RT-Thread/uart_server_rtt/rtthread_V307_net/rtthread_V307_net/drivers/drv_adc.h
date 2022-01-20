
#ifndef DRV_ADC_H__
#define DRV_ADC_H__
typedef struct
{
    ADC_TypeDef                   *Instance;
    ADC_InitTypeDef                Init;
}ADC_HandleTypeDef;


typedef struct
{
    uint32_t     Channel;
    uint32_t     Rank;
    uint32_t     SamplingTime;
    uint32_t     Offset;
}ADC_ChannelConfTypeDef;


#endif
