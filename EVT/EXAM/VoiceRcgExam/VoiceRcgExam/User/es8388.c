/********************************** (C) COPYRIGHT *******************************
* File Name          : es8388.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "es8388.h"


/*********************************************************************
 * @fn      IIC_Init
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @param   bound
 *          address
 *
 * @return  error code
 */
void IIC_Init( u32 bound , u16 address )
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2C_InitTypeDef I2C_InitTSturcture={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C2, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init( I2C2, &I2C_InitTSturcture );

    I2C_Cmd( I2C2, ENABLE );

    I2C_AcknowledgeConfig( I2C2, ENABLE );
}

/*********************************************************************
 * @fn      ES8388_Contrl_Init
 *
 * @brief   ����I2S��SD���ݷ���
 *
 * @return  none
 */
void ES8388_Contrl_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      ES8388_Init
 *
 * @brief   Initializes the ES8388.
 *
 * @return  0 - success
 *          1 - fail
 */
u8 ES8388_Init(void)
{
    ES8388_Contrl_Init();		//����ģ�⿪�ؿ���SD���ݷ���
    IIC_Init(200000,0x01);                 //��ʼ��IIC�ӿ�

    //����λES8388
    ES8388_Write_Reg(0, 0x80);
    ES8388_Write_Reg(0, 0x00);
    Delay_Ms(100);				//�ȴ���λ

    ES8388_Write_Reg(0x01, 0x58);
    ES8388_Write_Reg(0x01, 0x50);
    ES8388_Write_Reg(0x02, 0xF3);
    ES8388_Write_Reg(0x02, 0xF0);
	
	ES8388_Write_Reg(0x03, 0x09);	//��˷�ƫ�õ�Դ�ر�
	ES8388_Write_Reg(0x00, 0x06);	//ʹ�ܲο�		500K����ʹ��
	ES8388_Write_Reg(0x04, 0x00);	//DAC��Դ�����������κ�ͨ��
	ES8388_Write_Reg(0x08, 0x00);	//MCLK����Ƶ
    ES8388_Write_Reg(0x2B, 0x80);	//DAC����	DACLRC��ADCLRC��ͬ
   
    ES8388_Write_Reg(0x09, 0x88);	//ADC L/R PGA��������Ϊ+24dB
    ES8388_Write_Reg(0x0C, 0x4C);	//ADC	����ѡ��Ϊleft data = left ADC, right data = left ADC 	��Ƶ����Ϊ16bit
    ES8388_Write_Reg(0x0D, 0x12);	//ADC���� MCLK/������=250
    ES8388_Write_Reg(0x10, 0x00);	//ADC�����������ƽ��ź�˥�� L	����Ϊ��С������
    ES8388_Write_Reg(0x11, 0x00);	//ADC�����������ƽ��ź�˥�� R	����Ϊ��С������
	
    ES8388_Write_Reg(0x17, 0x18);	//DAC ��Ƶ����Ϊ16bit
    ES8388_Write_Reg(0x18, 0x02);	//DAC	���� MCLK/������=256
    ES8388_Write_Reg(0x1A, 0x00);	//DAC�����������ƽ��ź�˥�� L	����Ϊ��С������
    ES8388_Write_Reg(0x1B, 0x00);	//DAC�����������ƽ��ź�˥�� R	����Ϊ��С������
    ES8388_Write_Reg(0x27, 0xB8);	//L��Ƶ��
    ES8388_Write_Reg(0x2A, 0xB8);	//R��Ƶ��
    Delay_Ms(100);
    return 0;
}

/*********************************************************************
 * @fn      ES8388_Write_Reg
 *
 * @brief   д���ݵ�ES8388�Ĵ���
 *
 * @param   reg - �Ĵ�����ַ
 *          val - Ҫд��Ĵ�����ֵ
 *
 * @return  0 - success
 *          other - fail
 */
u8 ES8388_Write_Reg(u8 reg, u8 val)
{
    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART( I2C2, ENABLE );

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress(I2C2,((ES8388_ADDR << 1) | 0),I2C_Direction_Transmitter);
    //����������ַ+д����

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );  //�ȴ�Ӧ��
  
    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) ==  RESET );
    I2C_SendData(I2C2,reg); //д�Ĵ�����ַ

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) ); //�ȴ�Ӧ��
  
	while( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) ==  RESET );
    I2C_SendData(I2C2,val & 0XFF); //��������

    I2C_GenerateSTOP( I2C2, ENABLE );
    return 0;
}

/*********************************************************************
 * @fn      ES8388_Read_Reg
 *
 * @brief   ��ָ����ַ����һ������
 *
 * @param   reg - �Ĵ�����ַ
 *
 * @return  read data
 */
u8 ES8388_Read_Reg(u8 reg)
{
    u8 temp = 0;

    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );
    I2C_GenerateSTART( I2C2, ENABLE );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress(I2C2,(ES8388_ADDR << 1) | 0X00,I2C_Direction_Transmitter); 
    //����������ַ+д����

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );  //�ȴ�Ӧ��
    I2C_SendData(I2C2,reg);        //д�Ĵ�����ַ

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );//�ȴ�Ӧ��

    I2C_GenerateSTART( I2C2, ENABLE );
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );

    I2C_Send7bitAddress(I2C2,((ES8388_ADDR << 1) | 0x01),I2C_Direction_Receiver);//����������ַ+������
    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) ); //�ȴ�Ӧ��

    while(I2C_GetFlagStatus( I2C2, I2C_FLAG_RXNE ) ==  RESET);
    temp = I2C_ReceiveData( I2C2 );

    I2C_GenerateSTOP( I2C2, ENABLE );//����һ��ֹͣ����

    return temp;
}

/*********************************************************************
 * @fn      ES8388_I2S_Cfg
 *
 * @brief   ����I2S����ģʽ
 *
 * @param   fmt - I2S mode
 *            0 - �����ֱ�׼I2S
 *            1 - MSB
 *            2 - LSB
 *            3 - PCM/DSP
 *          address
 *
 * @return  none
 */
void ES8388_I2S_Cfg(u8 fmt, u8 len)
{
    fmt &= 0X03;
    len &= 0X07; //�޶���Χ
    ES8388_Write_Reg(23, (fmt << 1) | (len << 3));	//R23,ES8388����ģʽ����
}

/*********************************************************************
 * @fn      ES8388_Set_Volume
 *
 * @brief   ����������С�������������ӵ����
 *
 * @param   volume - ������С(0-33)
 *                      0 �C -30dB
 *                      1 �C -29dB
 *                      2 �C -28dB
 *                      ��
 *                      30 �C 0dB
 *                      31 �C 1dB
 *                      ��
 *                      33 �C 3dB
 * @return  none
 */
void ES8388_Set_Volume(u8 volume)
{
    for(u8 i = 0; i < volume; i++)
    {
        ES8388_Write_Reg(0x2E, i);
        ES8388_Write_Reg(0x2F, i);
    }
}

/*********************************************************************
 * @fn      ES8388_ADDA_Cfg
 *
 * @brief   ES8388 DAC/ADC����
 *
 * @param   dacen -
 *            0 - dac enable
 *            1 - dac disable
 *          adcen -
 *            0 - adc enable
 *            1 - adc enable
 *
 * @return  none
 */
void ES8388_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u8 res = 0;
	
	res |= (dacen<<0);
	res |= (adcen<<1);
	res |= (dacen<<2);
	res |= (adcen<<3);
	
	ES8388_Write_Reg(0x02, res);
}

/*********************************************************************
 * @fn      ES8388_Output_Cfg
 *
 * @brief   Initializes the IIC peripheral.
 *
 * @param   out -
 *            0 - ͨ��2���
 *            1 - ͨ��1���
 *
 * @return  none
 */
void ES8388_Output_Cfg(u8 out)
{
	ES8388_Write_Reg(0x04, 3<<(out*2+2));
}

/*********************************************************************
 * @fn      ES8388_Input_Cfg
 *
 * @brief   ES8388 ADC���ͨ������
 *
 * @param   in -
 *            0-ͨ��1����
 *            1-ͨ��2����
 *
 * @return  none
 */
void ES8388_Input_Cfg(u8 in)
{
	ES8388_Write_Reg(0x0A,(5*in)<<4);	//ADC1 ����ͨ��ѡ��L/R	INPUT1
}
