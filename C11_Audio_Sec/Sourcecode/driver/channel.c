/* @file   Channel.c
 * @brief  ͨ���л��м��ӿں���
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
#include "..\main\macro.h"
#include "..\main\task.h"
#include "..\main\debug.h"
#include "..\main\vfd_disp.h"

#include "channel.h"
#include "max335.h"
#include "cs8416.h"
#include "ioport.h"
#include "i2cdrv.h"
#include "eeprom.h"

#include "..\cs495300\Dspdrv.h"

#ifdef _DEBUG  
#define _DEBUG_CHANNEL  
#endif

unsigned char Analog_Input_To_Dsp_Flag; /*add cjm 2007-08-02*/

#define ADDR_DIGI 0x52
IIC_CONFIG const IIC_DIGI = /*lpc922 IICͨѶ�ṹ��*/
{
	ADDR_DIGI,
	set_scl2,
	set_sda2,
	sense_scl2,
	sense_sda2,

	70,   /* tAA */
	70,   /* tBUF */
	50,   /* tHD_STA */
	70,   /* tSU_STA */
	70,   /* tLOW*/
	50,   /* tHIGH*/
	1,   	/* tHD_DAT */
	10,   /* tSU_DAT */
	10,   /* tR*/
	10,   /* tF */  
	70,   /* tSU_STO */
	10,   /* tDH */
	100, /* tWR*/

	100,	/* ack timeout */
};

/*
 * @brief LPC2364��LPC922����I2CͨѶ��������ͨ�����л�( ��LPC922д����)
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 * @note 
 *
 * 	LPC2364��LPC922֮��涨��Э��:
 *		ADDR	DATA	FUNCTION
 *		01		00		TR_OUT_LOW
 *		01		01		TR_OUT_HIGH
 *	
 *		02		0X		ZONE1 DIGI IN SELECT (HDMI SPDIF X=0,COAXIAL X=1 (NO USE),TOSLINK X=2 (NO USE),RADIO X=3)
 *		02		1X		ZONE1 DIGI IN SELECT (COAXIAL X,X=0 TO 7)
 *		02		2X		ZONE1 DIGI IN SELECT (TOSLINK X,X=1 TO 7)
 *
 *		03		0X		ZONE2 DIGI IN SELECT (HDMI SPDIF X=0,COAXIAL X=1 (NO USE),RADIO X=2)
 *		03		1X		ZONE2 DIGI IN SELECT (COAXIAL X,X=0 TO 7)
 *		03		2X		ZONE2 DIGI IN SELECT (TOSLINK X,X=1 TO 7)
 */
void DigiSendByte(unsigned char addr,unsigned char value)
{
	int result;  
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nDigiSendByte(%02X,%02X);",addr,value);
#endif
	result = I2C_bytewrite(IIC_DIGI,addr,value);
#ifdef _DEBUG_CHANNEL
	if(result == -1)
		debug_printf("\nLPC922 NACK\n");
#endif
}

/* 
 * @brief Triger Function Control
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
 //
void SetTrigerOut(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT,1); 
	else 
		DigiSendByte(ADDR_TROUT,0); 
}

/*****jian add in 10-09-13*******/
void SetTrigerOut1(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT1,1); 
	else 
		DigiSendByte(ADDR_TROUT1,0); 
}

void SetTrigerOut2(int high)
{
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nTrigger:%d",high);
#endif
	if(high) 
		DigiSendByte(ADDR_TROUT2,1); 
	else 
		DigiSendByte(ADDR_TROUT2,0); 
}

/*Zone1 Digital channel table*/
const unsigned char Digital_channel_table[] = 
{
0x10, //BALANCE
0x21, //Toslink 2
0x22, //Toslink 3
0x23, //Toslink 4
0x24, //Toslink 5
0x25, //Toslink 6
0x26, //Toslink 7
0x27, //Toslink 8
0x11, //Coaxial 2 
0x12, //Coaxial 3
0x13, //Coaxial 4
0x14, //Coaxial 5
0x15, //Coaxial 6
0x16, //Coaxial 7
0x17, //Coaxial 8
0x00, //HDMI
0x03, //Zone1_HD_Radio
0x02, //Zone2_HD_Radio
};

/*
 * @brief  1 ����������ͨ���л�
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void DigitalInSelectChannel(DIGITAL_IN_CHANNEL channel_value)
{
	DigiSendByte(ADDR_ZONE1,Digital_channel_table[channel_value]);
	
}


void DigitalInSelect71Channel(void)
{
	DigiSendByte(ADDR_ZONE1,20);
}
/*Zone2 Digital channel table*/
const unsigned char Zone2_Digital_channel_table[] = 
{
0x10, //BALANCE
0x21, //Toslink 2
0x22, //Toslink 3
0x23, //Toslink 4
0x24, //Toslink 5
0x25, //Toslink 6
0x26, //Toslink 7
0x27, //Toslink 8
0x11, //Coaxial 2 
0x12, //Coaxial 3
0x13, //Coaxial 4
0x14, //Coaxial 5
0x15, //Coaxial 6
0x16, //Coaxial 7
0x17, //Coaxial 8
0x00, //HDMI
0x02, //Zone2_HD_Radio
0x03, //����ص�
};

/*
 * @brief  2 ����������ͨ���л�
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void Zone2DigiSelectChannel(DIGITAL_IN_CHANNEL channel_value)
{
	DigiSendByte(ADDR_ZONE2,Zone2_Digital_channel_table[channel_value]);
}

/*Max335 chip select*/
#define MAX335_CHIP1 0x0000
#define MAX335_CHIP2 0x0100
#define MAX335_CHIP3 0x0200
#define MAX335_CHIP4 0x0300
#define MAX335_CHIP5 0x0400
#define MAX335_CHIP6 0x0500
#define MAX335_CHIP7 0x0600
#define MAX335_CHIP8 0x0700 

/*Max335 chip mission*/
#define INPUT_SELECT_MIC				(MAX335_CHIP8 | 0x80) //BIT7-MIC
#define INPUT_SELECT_2CH_RL			(MAX335_CHIP8 | 0x50) //BIT6/BIT4-2CHL/R
#define INPUT_SELECT_7CH1_RL			(MAX335_CHIP8 | 0x28) //BIT5/BIT3-7.1L/R
#define INPUT_SELECT_Zone2_Downmix   	(MAX335_CHIP8 | 0X06) //BIT2/BIT1-Zone2 Downmix 

#define ANALOG_SELECT_TUNER			(MAX335_CHIP7 | 0x05)  //Zone 1 TUNER(L/R)
#define ANALOG_SELECT_BALANCE 		(MAX335_CHIP7 | 0xA0)  //Zone 1 Balance(L/R)
#define ANALOG_SELECT_ANALOG_2		(MAX335_CHIP6 | 0x05)  //Zone 1 Analog2(L/R)
#define ANALOG_SELECT_ANALOG_3		(MAX335_CHIP6 | 0xA0)  //Zone 1 Analog3(L/R)
#define ANALOG_SELECT_ANALOG_4		(MAX335_CHIP5 | 0x05)  //Zone 1 Analog4(L/R)
#define ANALOG_SELECT_ANALOG_5		(MAX335_CHIP5 | 0xA0)  //Zone 1 Analog5(L/R)
#define ANALOG_SELECT_ANALOG_6		(MAX335_CHIP4 | 0x05)  //Zone 1 Analog6(L/R)
#define ANALOG_SELECT_ANALOG_7		(MAX335_CHIP4 | 0xA0)  //Zone 1 Analog7(L/R)
#define ANALOG_SELECT_ANALOG_8		(MAX335_CHIP3 | 0x05)  //Zone 1 Analog8(L/R)

#define ZONE2_SELECT_TUNER				(MAX335_CHIP7 | 0x0A)  //Zone 2 TUNER(L/R)
#define ZONE2_SELECT_BALANCE			(MAX335_CHIP7 | 0x50)  //Zone 2 Balance(L/R)
#define ZONE2_SELECT_ANALOG_2 		(MAX335_CHIP6 | 0x0A)  //Zone 2 Analog2(L/R)
#define ZONE2_SELECT_ANALOG_3 		(MAX335_CHIP6 | 0x50)  //Zone 2 Analog3(L/R)
#define ZONE2_SELECT_ANALOG_4 		(MAX335_CHIP5 | 0x0A)  //Zone 2 Analog4(L/R)
#define ZONE2_SELECT_ANALOG_5 		(MAX335_CHIP5 | 0x50)  //Zone 2 Analog5(L/R)
#define ZONE2_SELECT_ANALOG_6 		(MAX335_CHIP4 | 0x0A)  //Zone 2 Analog6(L/R)
#define ZONE2_SELECT_ANALOG_7 		(MAX335_CHIP4 | 0x50)  //Zone 2 Analog7(L/R)
#define ZONE2_SELECT_ANALOG_8 		(MAX335_CHIP3 | 0x0A)  //Zone 2 Analog8(L/R)
#define ZONE2_SELECT_7CH1_RL			(MAX335_CHIP3 | 0x30)  //Zone2 7.1 

#define OUTPUT_7CH1_SELECT_2CH		(MAX335_CHIP3 | 0xC0)  //Zone 1 2 CH ByPass(L/R) 
#define OUTPUT_7CH1_SELECT_7CH1		(MAX335_CHIP2 | 0xFF)  //Analog 7.1CH BY PASS
#define OUTPUT_7CH1_SELECT_DSP		(MAX335_CHIP1 | 0xFF)  //D/A ������Ƶİ˸�����

/*Input Select Table*/
const unsigned int input_select_table[]=
{
INPUT_SELECT_MIC,
INPUT_SELECT_2CH_RL,	
INPUT_SELECT_7CH1_RL,
INPUT_SELECT_Zone2_Downmix,
0,
};

/*Zone1 Analog table*/
const unsigned int analog_in_table[] = 
{
ANALOG_SELECT_BALANCE,		//ANALOG_IN_BALANCE
ANALOG_SELECT_ANALOG_2,	//ANALOG 2
ANALOG_SELECT_ANALOG_3,	//ANALOG 3
ANALOG_SELECT_ANALOG_4,	//ANALOG 4
ANALOG_SELECT_ANALOG_5,	//ANALOG 5
ANALOG_SELECT_ANALOG_6,	//ANALOG 6
ANALOG_SELECT_ANALOG_7,	//ANALOG 7
ANALOG_SELECT_ANALOG_8,	//ANALOG 8 
ANALOG_SELECT_TUNER,		//ANALOG_IN_TUNER
0,
};

/*Zone2 Analog table*/
const unsigned int zone2_analog_in_table[] = 
{
ZONE2_SELECT_BALANCE,		//ANALOG_IN_BALANCE
ZONE2_SELECT_ANALOG_2,		//ANALOG 2
ZONE2_SELECT_ANALOG_3,		//ANALOG 3
ZONE2_SELECT_ANALOG_4,		//ANALOG 4
ZONE2_SELECT_ANALOG_5,		//ANALOG 5
ZONE2_SELECT_ANALOG_6,		//ANALOG 6
ZONE2_SELECT_ANALOG_7,		//ANALOG 7
ZONE2_SELECT_ANALOG_8,		//ANALOG 8
ZONE2_SELECT_7CH1_RL,    	//ANALOG 7.1 R/L
ZONE2_SELECT_TUNER,			//ANALOG_IN_TUNER
0,
};

/*
 * @brief  ģ��ͨ���źż��
 *
 * @author chenjianming
 * @date 2008-1-21
 * @refer chenshiguang C11 code
 */
unsigned char CheckAnalogIn(unsigned int ms_value)
{
	uchar retry_time;
	for(retry_time=0;retry_time<10;retry_time++)
		{
		DelayMs(ms_value);
		if(!SenseAnalogSignal()) /*unlock*/
			{
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog In none\n");
#endif  
			return 1;	/*unlock*/			
			}   
		}
	  
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nAnalog In valid\n");  
#endif
	return 0;	/*Lock*/	
}

/* 
 * @brief �ر�Zone1 ����ģ������ͨ��
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void AnalogInOff(void)
{
	unsigned char i = 0;

#ifdef _DEBUG_CHANNEL
	//debug_printf("\nAnalogInOff();");
#endif

	while(analog_in_table[i])
		{
		SetMax335Switch(analog_in_table[i],0);	
		i++;
		}
	UpdateMax335();	/*Update MAX335*/
}

/* 
 * @brief �ر�Zone2 ģ������ͨ��
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void Zone2AnalogInOff(void)
{
	unsigned char i=0;
	
#ifdef _DEBUG_CHANNEL
	//debug_printf("\nZone2AnalogInOff();\n");
#endif		

	while(zone2_analog_in_table[i])  
		{
		SetMax335Switch(zone2_analog_in_table[i],0);
		i++;
		}
	UpdateMax335();	/*Update MAX335*/
}

/* 
 * @brief Zone 1 Analog Channel Select
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 */
void AnalogInSelectChannel(ANALOG_IN_CHANNEL channel_value)
{
	static ANALOG_IN_CHANNEL pre_channel;	
	
	if(pre_channel != channel_value)
		{
		SetMax335Switch(analog_in_table[pre_channel],0);	/*�ȹر�analog in ֮ǰ�򿪵Ŀ���*/
		pre_channel = channel_value;
		}
	
	SetMax335Switch(analog_in_table[pre_channel],1);	/*�򿪵�ǰѡ���analog in����*/	
	UpdateMax335();/*Update MAX335*/
}

/* 
 * @brief Zone 2 Channel Select
 *
 * @author chenjianming
 * @date 2008-2-27
 * @refer 
 */
 void Zone2SelectChannel(SYSTEM_CHANNEL channel_value)
{
	unsigned char select_channel;
	//
	if(channel_value!=INPUT_CH71)
		{
#ifdef _DEBUG_CHANNEL
		debug_printf("\nchannel_value=%d",channel_value);  
		debug_printf("\nASSIGNED_INPUT1=%d",EepromReadByte(ASSIGNED_INPUT1+channel_value));
#endif
		select_channel=EepromReadByte(ASSIGNED_INPUT1 + channel_value);
		if(channel_value==INPUT_1)
			{
			select_channel=select_channel+1; /*Input1 ֻ��Digital =0��Analog =1����*/
			}
		
		if(select_channel==2) /*Analog ����ʱ,����Ӧ��ģ�����*/
			{
			Zone2AnalogInOff(); /*Zone2 ������ģ�����붼�ص�*/
			SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);/*downmix���عص�*/
			SetMax335Switch(zone2_analog_in_table[channel_value],1); /*�򿪵�ǰѡ���analog in����*/
			UpdateMax335();	/*Update MAX335*/
			}  
		else /*Toslink ��Coaxial ��������ʱ,��1����2��ѡ������벻һ��ʱ,ֱ�Ӵ�2�����������.��1����2��������ѡ��һ��ʱ,ģ��downmix ����һ�Ҫ�������*/
			{
			Zone2AnalogInOff(); /*Zone2 ������ģ�����붼�ص�*/
			if(sys_flag.zone2_channel !=sys_flag.zone1_channel) /*���1��������ͨ����2���Ĳ�һ��(Bypass)*/
				{
#ifdef _DEBUG_CHANNEL
				//debug_printf("\nZone2 Select Bypass\n");
#endif				
				if((channel_value >= INPUT_2) && (channel_value <= INPUT_8)) /*Input2-Input8 Toslink��Coaxial����*/
					{
					if(EepromReadByte(ASSIGNED_INPUT1 + channel_value)==0)
						Zone2DigiSelectChannel(channel_value); /*Toslink����--ǰ8����ַΪ1��ƽ���ַ+ 7��Toslink ��ַ*/
					else 
						Zone2DigiSelectChannel(channel_value+7); /*coaxial����--�������7����ַΪCoaxial ��ַ*/
					}
				else  if(channel_value == INPUT_1)
					Zone2DigiSelectChannel(0); /*Input1 ƽ������*/		
				else	
					{
					    if(((sys_flag.zone1_channel >= INPUT_HDMI1)&&(sys_flag.zone1_channel <= INPUT_HDMI4))&&(sys_flag.zone2_channel == INPUT_HDMI ||sys_flag.zone2_channel == ARC_CHANNEL))
					    {
						DigiSendByte(ADDR_ZONE2,0x00);
						DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix ʹ��,��DA02_Channel 1/2 ���*/					
						}  // jain add in 12-03-07
					else if(sys_flag.zone2_channel == INPUT_TUNER)  // jian add in 12-04-17
						{
							SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);/*��downmix����*/				
							SetMax335Switch(zone2_analog_in_table[9],1); /*HD Radio analog ����*/
							UpdateMax335();	/*Update MAX335*/
							DigiSendByte(ADDR_ZONE2,0X02); /*Zon2 HD Radio Gigital output*/

						}						
					}
				}
			else  /*���1��������ͨ����2����һ��(Downmix)*/
				{
				if(dsp_power_flag)
					{
#ifdef _DEBUG_CHANNEL
					debug_printf("\nZone2 Select Downmix\n");
#endif					
					//��digital ���
					if((channel_value >= INPUT_2) && (channel_value <= INPUT_8)) /*Input2-Input8 Toslink��Coaxial����*/
						{
						if(EepromReadByte(ASSIGNED_INPUT1 + channel_value)==0)
							Zone2DigiSelectChannel(channel_value); /*Toslink����--ǰ8����ַΪ1��ƽ���ַ+ 7��Toslink ��ַ*/
						else 
							Zone2DigiSelectChannel(channel_value+7); /*coaxial����--�������7����ַΪCoaxial ��ַ*/
						}
					else  if(channel_value == INPUT_1)
						Zone2DigiSelectChannel(0); /*Input1 ƽ������*/		
					else	
						DigiSendByte(ADDR_ZONE2,0x00);
					//ģ��Downmix �����������
					DSPFirmwareWrite(Crossbar_Manager_Wop,0x0080,0x00000101); /*Downmix ʹ��,��DA02_Channel 1/2 ���*/					
					SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);/*��downmix����*/
					UpdateMax335();	/*Update MAX335*/
					}
				}
			}
		}
	else /* ��Input 7.1 ���*/
		{
		Zone2AnalogInOff(); /*Zone2 ��ģ�����붼�ص�*/
		SetMax335Switch(zone2_analog_in_table[channel_value],1); /*�򿪵�ǰѡ���analog in����*/
		UpdateMax335();	/*Update MAX335*/
		}
}

/*
 * @brief ģ����������ѡ��
 *
 * @author chenjianming
 * @date 2008-1-25
 * @refer 
 *	0--MIC_INPUT
 *    1--2CH_RL_INPUT
 *    2--7.1CH_RL_INPUT
 *    3--ZONE2_DOWNMIX_INPUT
 */
void AnalogInputTypeSelect(unsigned char channel_value)
{
	SetMax335Switch(INPUT_SELECT_MIC,0);
	SetMax335Switch(INPUT_SELECT_2CH_RL,0);
	SetMax335Switch(INPUT_SELECT_7CH1_RL,0);
	SetMax335Switch(INPUT_SELECT_Zone2_Downmix,0);
	switch(channel_value)
		{
		case Select_MIC_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog MIC Input");
#endif			
			SetMax335Switch(INPUT_SELECT_MIC,1);
			break;
		case Select_2CH_RL_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog 2 Channel Input");
#endif			
			SetMax335Switch(INPUT_SELECT_2CH_RL,1);
			break;
		case Select_7CH1_RL_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog 7.1 Channel Input");
#endif			
			SetMax335Switch(INPUT_SELECT_7CH1_RL,1);
			break;
		case Select_Zone2_Downmix_INPUT:
#ifdef _DEBUG_CHANNEL
			//debug_printf("\nAnalog Zone2 Downmix Input");
#endif			
			SetMax335Switch(INPUT_SELECT_Zone2_Downmix,1);
			break;
		}
	UpdateMax335();
}

/*
 * @brief ѡ��7.1ͨ�������Դ
 *
 * @author chenjianming
 * @date 2008-1-16
 * @refer chenshiguang C11 code
 //
 */

 #define DIGI_CHECK_TIME 30

void Analog7CH1OutSelectChannel(OUTPUT_7CH1_CHANNEL channel_value)
{
	SetMax335Switch(OUTPUT_7CH1_SELECT_2CH,0);
	SetMax335Switch(OUTPUT_7CH1_SELECT_7CH1,0);
	SetMax335Switch(OUTPUT_7CH1_SELECT_DSP,0);
	switch(channel_value)
		{
		case SOURCE_2CH:	/*ѡ��2CH ͨ��(2CH Bypass)*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_2CH,1);
			break;
		case SOURCE_CH71_IN:/*ѡ��7.1CH ͨ��(7.1CH Bypass)*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_7CH1,1);
			break;
		case SOURCE_DSP:	/*ѡ��DSP����ͨ��*/
			SetMax335Switch(OUTPUT_7CH1_SELECT_DSP,1);
			break;
		}
	UpdateMax335();	/*Set MAX335*/
}

#define ANALOG_CHECK_TIME 50
/* 
 * @brief ����ͨ���źż��
 *
 * @author chenjianming
 * @date 2008--
 * @refer chenshiguang Cll code
 * @note
 *   Ӳ������Ӧ�ñ�֤�л���ͨ��֮��,detect�ĳ�ŵ�ʱ��С��ANALOG_CHECK_TIME,
 *   �����ϵͳ�л�Ҫ��ʱ�������Ļ�,��ô���������ư���ȥ������ԭ�����ж�,
 *   �������Լ��ͨ���л���Ѹ���Լ�ʵʱ��������Ƿ����ź�
 */
static unsigned char CheckInputChannel(unsigned char channel,unsigned char sub_channel)
/*Select input channel
 *input:
 	channel: input 1~8, 
 	sub_channel:1->toslink,2->coaxial,3->analog
 *output:
 	0:unlock
 	1:toslink lock
 	2:coaxial lock
 	3:analog lock
 */
{
	unsigned char input_channel;

	if(channel != INPUT_CH71)
		{
		channel = channel << 4;
		channel &= 0xF0;
		channel |= sub_channel;

		switch(channel)
			{
			case ((INPUT_1 << 4) | 1):
			case ((INPUT_1 << 4) | 2):
				input_channel = DIGITAL_IN_BALANCE; /*ƽ����������*/
				break;
			case ((INPUT_1 << 4) | 3):
				input_channel = ANALOG_IN_BALANCE; /*ƽ��ģ������*/
				break;
			case ((INPUT_2 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_2;
				break;
			case ((INPUT_2 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_2;
				break;
			case ((INPUT_2 << 4) | 3):
				input_channel = ANALOG_IN_2;
				break;
			case ((INPUT_3 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_3;
				break;
			case ((INPUT_3 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_3;
				break;
			case ((INPUT_3 << 4) | 3):
				input_channel = ANALOG_IN_3;
				break;
			case ((INPUT_4 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_4;
				break;
			case ((INPUT_4 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_4;
				break;
			case ((INPUT_4 << 4) | 3):
				input_channel = ANALOG_IN_4;
				break;
			case ((INPUT_5 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_5;
				break;
			case ((INPUT_5 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_5;
				break;
			case ((INPUT_5 << 4) | 3):
				input_channel = ANALOG_IN_5;
				break;
			case ((INPUT_6 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_6;
				break;
			case ((INPUT_6 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_6;
				break;
			case ((INPUT_6 << 4) | 3):
				input_channel = ANALOG_IN_6;
				break;
			case ((INPUT_7 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_7;
				break;
			case ((INPUT_7 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_7;
				break;
			case ((INPUT_7 << 4) | 3):
				input_channel = ANALOG_IN_7;
				break;
			case ((INPUT_8 << 4) | 1):
				input_channel = DIGITAL_IN_TOSLINK_8;
				break;
			case ((INPUT_8 << 4) | 2):
				input_channel = DIGITAL_IN_COAXIAL_8;
				break;
			case ((INPUT_8 << 4) | 3):
				input_channel = ANALOG_IN_8;
				break;
			}


//                debug_printf("input_channel1:%x\n",input_channel);
				  
		if(sub_channel == 3)
			{
			AnalogInputTypeSelect(Select_2CH_RL_INPUT);/*����Ϊ2 channel R/L*/ 
			Analog7CH1OutSelectChannel(SOURCE_DSP); /*���ΪD/A 8ͨ�����*/
			AnalogInSelectChannel(input_channel); /*����Ӧ����ͨ����ģ��ͨ��*/
			DigitalInSelectChannel(0); /*add cjm 2007-08-01*/
			Analog_Input_To_Dsp_Flag=1; /*add cjm 2007-08-02*/	
			//if(CheckAnalogIn(ANALOG_CHECK_TIME)==0) //Dele by cjm 2008-12-23
				return sub_channel;
			} 
		else
			{
			
			DigitalInSelectChannel(input_channel); /*�е���Ӧ����ͨ����Toslink����Coaxial*/
		//	  debug_printf("input_channel2:%x\n",input_channel);
			if(CheckCs8416Unlock(DIGI_CHECK_TIME) == 0)	/*lock*/
				return sub_channel;
			}
		}
	else
		{
		DigitalInSelect71Channel();
		AnalogInputTypeSelect(Select_7CH1_RL_INPUT); /*����Ϊ7 channel R/L*/ 
		if(CheckAnalogIn(ANALOG_CHECK_TIME) == 0)
			return 1;
		}
	return 0;
}

/*input:channel,��INPUT_1��INPUT_8���κ�һ��
 �������Զ�������input��Ӧ��toslink,coaxial,analog,
 ������ź����룬�򷵻ص�ǰ��input(toslink,coaxial,analog),���򷵻�0
 */
unsigned char SystemSelectInputChannel(unsigned char channel)
{
	unsigned char i = 1;
	unsigned char status;

	AnalogInOff();	/*�ر����е�ģ��ͨ����ȷ��ģ��ͨ�����������·����ŵ�*/

	if(channel == INPUT_1)
		i = 2;
	
	for(;i<=3;i++)
		{
		status = CheckInputChannel(channel,i);
		if(status)
			return i;
		}
	return 0;
}

/*add cjm 2007-07-30*/
/*input:channel,ֻ������INPUT_1��INPUT_8 Assign �����õ�ͨ��
�������Զ�����ASSIGN ���õ�����ӿ�����
������ź����룬�򷵻ص�ǰ��INPUT(toslink,coaxial,analog),���򷵻�0
*/
unsigned char SystemSearchInputChannel(unsigned char channel)
{
	unsigned char status;
	unsigned char sub_channel;

	AnalogInOff();	/*�ر����е�ģ��ͨ����ȷ��ģ��ͨ�����������·����ŵ�*/
	
	switch(channel)
		{
			case INPUT_HDMI1: /*HDMI1 HDMI2 ֱ�������źż��*/
			case INPUT_HDMI2:
			case INPUT_HDMI3: /*HDMI1 HDMI2 ֱ�������źż��*/
			case INPUT_HDMI4:

				return 0;  
				
			case INPUT_1:  /*Input 1����ֻ��Digital=2 ��Analog=3 ��������*/
				sub_channel=EepromReadByte(ASSIGNED_INPUT1)+2;
				break;
	
			default:
				sub_channel=EepromReadByte(ASSIGNED_INPUT1+channel)+1;
				break;		
		}

	status=CheckInputChannel(channel,sub_channel);
	
	if(status)
		return sub_channel;
	else
		return 0;	
}/*add cjm 2007-07-30*/

unsigned char CheckInput(unsigned char channel,unsigned char sub_channel)
{
	unsigned char status;
	AnalogInOff();
	status = CheckInputChannel(channel,sub_channel);  
	if(status)
		return 1;
	return 0;
}

