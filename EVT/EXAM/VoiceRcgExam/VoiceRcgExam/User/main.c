/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
  individual term voice recognition routine:
 
*/

#include "debug.h"
#include "VoiceRcg.h"
#include "Get_Data.h"


/* Global define */
#define Valid_Thl 245

/* Global Variable */
const char  *key_words[]={"up", "down", "left", "right"};

#if USE_ES8388
//ES8388 have two channels,we only need one of them
__attribute__((aligned(4))) uint16_t V_Data[SampleDataLen*2];
#else
__attribute__((aligned(4))) uint16_t V_Data[SampleDataLen];
#endif

vr_dtg_para  dtg_para;
vr_act_seg   active_segment;
__attribute__((aligned(4))) vr_chara_para  chara_para;
volatile uint8_t g_data_ready=0;


void voice_record(void);
uint8_t save_chara_para_mdl(uint16_t *v_dat, uint32_t addr);
uint8_t parameters_practice(void);
void voice_recongition(void);
void key_init(void);



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(921600);	
	printf("SystemClk:%d\r\n", SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	voice_init();
	key_init();
	while(1)
	{
		if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2))
		{
			Delay_Ms(20);
			if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2))
			{
				printf("will practice parameters...\r\n");
				parameters_practice();
			}
		}
		voice_recongition();
		Delay_Ms(1000);
	}
}



/*********************************************************************
 * @fn      voice_record
 *
 * @brief   record voice data
 *
 * @return  none
 */
void voice_record(void)
{
#if USE_ES8388
	I2S_Cmd(SPI2,ENABLE);
	Delay_Ms(SampleDelay); //ES8388 data is empty
	DMA_Rx_Init( DMA1_Channel4, (u32)&SPI2->DATAR, (u32)V_Data, (SampleDataLen*2) );
	DMA_Cmd( DMA1_Channel4, ENABLE );
	Delay_Ms(SampleNoiseTime); //sample noise
	printf("speaking...\r\n");
#else
	TIM_Cmd(TIM1, ENABLE);
	Delay_Ms(SampleNoiseTime);
	printf("speaking...\r\n");
#endif
}

/*********************************************************************
 * @fn      save_chara_para_mdl
 *
 * @brief   save mfcc characteristic parameters
 *
 * @param   v_dat-source data buffer
 *          addr-start address of parameter
 *
 * @return  error code
 */
uint8_t save_chara_para_mdl(uint16_t *v_dat, uint32_t addr)
{
	environment_noise(v_dat,SampleNoiseLen,&dtg_para);
	active_segment_detect(v_dat, SampleDataLen, &active_segment, &dtg_para);
	if(active_segment.act_end== NULL ) return ERR_DETEC;
	calc_mfcc_chara_para(&active_segment,&chara_para,&dtg_para);
	if(chara_para.seg_cnt==0) return ERR_PARA;
	return save_mfcc_chara_para(&chara_para, addr);
}



/*********************************************************************
 * @fn      parameters_practice
 *
 * @brief   practice some key words and save characteristic parameters
 *
 * @return  error code
 */
uint8_t parameters_practice(void)
{
	uint8_t i=0;
	uint8_t retry_cnt=0;
	uint32_t addr=0;
	while(1)
	{
		printf("\r\npractice start...\r\n\r\n");
		for (i = 0; i < kw_num*charaml_per_kw; i++)
		{
			retry:
			printf("please speak:%s \r\n",key_words[i/charaml_per_kw]);
			voice_record();
			addr=charamls_start_addr+(i/charaml_per_kw)*size_per_kw+(i%charaml_per_kw)*size_per_chara;
			printf("addr:%08x\r\n",addr);

			while(!g_data_ready) //waiter for ready
			{
			}

			if(g_data_ready)
			{
				g_data_ready=0;
				if(save_chara_para_mdl(V_Data,addr)==ERR_OK)
				{
					printf("\r\n %s practice success\r\n",key_words[i/charaml_per_kw]);
				}
				else
				{
					printf("%s practice fail !!!\r\n",key_words[i/charaml_per_kw]);
					retry_cnt++;
					if(retry_cnt<5)
					goto retry;
					else break;
				}
			}
			Delay_Ms(1000);
			Delay_Ms(1000);
		}
		printf("practice end!!!\r\n");
		break;
	}
	return 0;
}


/*********************************************************************
 * @fn      voice_recongition
 *
 * @brief   sample data and recognize
 *
 * @return  none
 */
void voice_recongition(void)
{
	uint8_t     i=0,keyword_min=0;
	uint32_t    chara_para_addr=0;
	uint32_t    match_dis=0,cur_dis=0,min_dis=0;
	vr_chara_para *chara_para_mdl;

	voice_record();
	while(!g_data_ready); //wait for data ready
	if(g_data_ready)
	{
		g_data_ready=0;
		environment_noise(V_Data,SampleNoiseLen,&dtg_para);
		active_segment_detect(V_Data, SampleDataLen, &active_segment, &dtg_para);
		if(active_segment.act_end== NULL )
		{
			match_dis=ERR_MATCH;
		}
		calc_mfcc_chara_para(&active_segment,&chara_para,&dtg_para);
		if(chara_para.seg_cnt==0)
		{
			match_dis=ERR_MATCH;
		}

		min_dis=ERR_MATCH;
		for(chara_para_addr=charamls_start_addr; chara_para_addr<charamls_end_addr; chara_para_addr+=size_per_chara)
		{
			chara_para_mdl=(vr_chara_para*)chara_para_addr;
			cur_dis=((chara_para_mdl->magic)==MAGIC_DATA)?calc_chara_para_match_dis(&chara_para,chara_para_mdl):ERR_MATCH;
			if(cur_dis<min_dis)
			{
			   min_dis=cur_dis;
			   keyword_min=i;
			}
			i++;
		}

		keyword_min = keyword_min/charaml_per_kw;
		if(min_dis>Valid_Thl) match_dis=ERR_MATCH;  //unreliable when it is more than Valid_Thl

		if(match_dis==ERR_MATCH)
		{
			match_dis=0;
			printf("recg fail!!!\r\n");
		}
		else
		{
		   printf("recg end,your speaking is: %s\r\n",key_words[keyword_min]);
		}
	}
}


/*********************************************************************
 * @fn      key_init
 *
 * @brief   key initial
 *
 * @return  none
 */
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOE, &GPIO_InitStructure );
}



