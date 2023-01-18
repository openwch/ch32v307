/********************************** (C) COPYRIGHT *******************************
* File Name          : VoiceRcg.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        :
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __VOICERCG_H__
#define __VOICERCG_H__

#include "math.h"
#include "debug.h"

#ifndef  NULL
#define  NULL ((void *)0)
#endif

#define ERR_OK     0            //no error
#define ERR_FLASH  1            //error at flash operation
#define ERR_DETEC  2            //error at detecting
#define ERR_PARA   3            //error at calculating parameter
#define ERR_MATCH  0xFFFFFFFF   //error at matching



#define DataSampleFrq       8000                                    //don't change because relate to fft
#define SampleTime          2000                                    //total sample time
#define SampleDataLen       ((DataSampleFrq/1000)*SampleTime)       //total data length
#define SampleNoiseTime     300                                     //noise Sample time, must be a multiple of 30
#define SampleNoiseLen      ((DataSampleFrq/1000)*SampleNoiseTime)  //noise data length


#define stime_energy         9.4                                    //short-time energy rate
#define stime_zerocross      0.2                                    //short-time zero cross


#define FLASH_PAGE_SIZE     256
#define MAGIC_DATA          0x55AA                                  //magic data used to check
#define size_per_chara      (8*1024)                                //sizes of a characteristic parameter
#define page_per_chara      (size_per_chara/FLASH_PAGE_SIZE)        //pages of a characteristic parameter
#define charaml_per_kw      2                                       //characteristic parameters of a key word
#define size_per_kw         (charaml_per_kw*size_per_chara)         //sizes of a key word
#define kw_num              4                                       //number of key words
#define charamls_size       (size_per_kw*kw_num)                    //total sizes

/* we use non-zero waiting period spaces to storage parameters */
#define charamls_start_addr (charamls_end_addr-charamls_size)       //start address at flash
#define charamls_end_addr   0x08070800                              //end address at flash

/* real and image of complex  */
typedef struct
{
    float real;
    float imag;
}vr_compx_d;

typedef struct
{
    uint32_t average;       //short-time average energy
    uint32_t energy;        //short-time energy
    uint16_t noise;         //background noise
    uint16_t zerocross;     //short-time zero cross rate
}vr_dtg_para;

typedef struct
{
    uint16_t *act_start;    //active segment start point
    uint16_t *act_end;      //active segment end point
}vr_act_seg;


typedef struct __attribute__((packed))
{
    uint16_t magic;             //check if overflow
    uint16_t seg_cnt;           //segment cnt
    float    chara_para[120*12];//characteristic parameter
}vr_chara_para ;



/* calculate background noise and shor time parameter */
void environment_noise( uint16_t *noise, uint16_t noise_length, vr_dtg_para *dtg_para );

/* get active segment from data buffer */
void active_segment_detect( uint16_t *data , uint16_t d_len, vr_act_seg *act_seg, vr_dtg_para *dtg_para);

/* calculate mfcc characteristic parameter */
void calc_mfcc_chara_para(vr_act_seg *act_seg, vr_chara_para *chara_para, vr_dtg_para *dtg_para);

/* save mfcc characteristic parameter to flash */
uint8_t save_mfcc_chara_para(vr_chara_para* chara_para, uint32_t addr);

/* calculate distance between input and model */
uint32_t calc_chara_para_match_dis(vr_chara_para *chara_para_src, vr_chara_para *chara_para_mdl);
#endif
