/* @file   	DelayManager.c
 * @brief 		Delay Manager Driver File
 *
 * @author 	chenjianming
 * @date 		2007-11-1
 * @refer 	AN246PPA
 *
 *			Write=0xD900HHHH 0xhhhhhhhh
 *			Read Request=0xD9C0HHHH
 *			Read Response=0x59C0HHHH 0xhhhhhhhh
 *
 *			0xHHHH=index 0xhhhhhhhh=data value
 */
#include "dspdrv.h"
#include "DelayManager.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\Eeprom.h"
#include "..\main\debug.h"

#ifdef _DEBUG
#define _DEBUG_Delay_MANAGER
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

_DELAY_TIME dsp_delay_time = {0};
_DELAY_TIME dsp_delay_music_time={0};
/*
 * @brief Delay ģ��ʹ�ܿ���
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 * @note  ��DataSheet ��Bit0 Ĭ��ΪEnable,��ʵ������Disable.
 *		 ��ʹ��External_Memory_Enable =1 ʱ,��ʱʹ�õ����ⲿSDRAM ��ΪDelay Buffer.
 *		 ��Ϊ0 ʱ��ʹ�õ����ڲ���Memory ��ΪDelay Buffer.
 *		 
 *		�ڲ���Memory ������Delay 115 ms,�ⲿ�ĸ���SDRAM�Ĵ�С����.
 */
void DSP_MAIN_PPM_CONTROL(unsigned char Delay_PPM_Master_Enable,unsigned char MidProc_Delay_Offset_Enable,unsigned char Externnal_Memory_Enable)
{
	short i=0;
	unsigned char value1=0,value2=0;

	if(Delay_PPM_Master_Enable)
		value1|=0x01;
	if(MidProc_Delay_Offset_Enable)
		value1|=0x10;
	if(Externnal_Memory_Enable)  
		value2|=0x01;

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value2;	dsp_buf[i++] =value1;

	DspSpiWriteNByte(dsp_buf,i);   
}

/*
 * @brief  Delay Unit Select (��Ĭ������Millisenconds)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer  AN246PPA 
 */
void DSP_Delay_Unit(unsigned char Dly_Unit_Select)
{
	short i=0;
	unsigned char value;
	
	if(Dly_Unit_Select)
		value=0x01;
	else
		value=0x00;

	dsp_buf[i++] =Delay_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x01;
	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =value;

	DspSpiWriteNByte(dsp_buf,i); 	
}

/*
 * @brief Channels Delay time setting
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 */
 #define MS_UNIT 0x20000	/*0x20000 = 2^17, 15.17 format*/ /*��ʱ1ms �Ĵ������õ�ֵ*/
void DSP_Delay_Channel_Setting(Dsp_Channel channel,unsigned long Delay_Ms)
{
	short i=0;
	unsigned long time;
	unsigned char index_value;

	time = Delay_Ms * MS_UNIT;

	switch(channel)
		{
			case CH_L:		index_value=0x02;			break;
			case CH_C:		index_value=0x03;			break;
			case CH_R:		index_value=0x04;			break;
			case CH_LS:		index_value=0x05;			break;
			case CH_RS:		index_value=0x06;			break;
			case CH_LB:		index_value=0x07;			break;
			case CH_RB:		index_value=0x08;			break;
			case CH_SW:		index_value=0x09;			break;
		}

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;		
	dsp_buf[i++] =(time>>24&0xFF);	dsp_buf[i++] =(time>>16&0xFF);	dsp_buf[i++] =(time>>8&0xFF);	dsp_buf[i++] =(time&0xFF);		

	DspSpiWriteNByte(dsp_buf,i);  		
}

/* 
 * @brief  Channels Max Delay Time Setting 
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA P3
 */
void DSP_Delay_Max_Setting(Dsp_Channel channel,unsigned long Max_Delay_Time)
{
	short i=0;
	unsigned char index_value;

	switch(channel)
		{
			case CH_L:		index_value=0x0a;			break;
			case CH_C:		index_value=0x0b;			break;
			case CH_R:		index_value=0x0c;			break;
			case CH_LS:		index_value=0x0d;			break;
			case CH_RS:		index_value=0x0e;			break;
			case CH_LB:		index_value=0x0f;			break;
			case CH_RB:		index_value=0x10;			break;
			case CH_SW:		index_value=0x11;			break;
		}

	dsp_buf[i++] =Delay_Manager_Wop;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index_value;
	dsp_buf[i++] =(Max_Delay_Time>>24&0xFF);	dsp_buf[i++] =(Max_Delay_Time>>16&0xFF);	dsp_buf[i++] =(Max_Delay_Time>>8&0xFF);	dsp_buf[i++] =(Max_Delay_Time&0xFF);

	DspSpiWriteNByte(dsp_buf,i);			
}

/*
 * @brief Externnal Memory Buffer Address Setting (Address Ĭ�ϴ�0x00000000 ��ʼ)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA p4
 */
void DSP_EXT_BUFF_ADDR(unsigned long Externnal_Buff_Addr)
{
	short i=0;

	dsp_buf[i++] =Delay_Manager_Wop;  dsp_buf[i++] =0x00;	dsp_buf[i++] =0x00;	dsp_buf[i++] =0x12;
	dsp_buf[i++] =(Externnal_Buff_Addr>>24&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr>>16&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr>>8&0xFF);	dsp_buf[i++] =(Externnal_Buff_Addr&0xFF);

	DspSpiWriteNByte(dsp_buf,i);				
}

/* 
 * @brief  Externnal Memory Buffer Size Setting 
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA
 * @note  �üĴ�����ֵΪ�ⲿ�ɴ洢�ֽ���
 *		 �������õ�SDRAM Ϊ64M bits =8M Bytes
 *
 *		8*1024*1024=8388608  
 *		ת����16 ����=0x00800000
 */
void DSP_EXT_BUFF_SIZE(unsigned long External_Buff_Size)
{
	short i=0;

	dsp_buf[i++]=Delay_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x13;
	dsp_buf[i++]=(External_Buff_Size>>24&0xFF);	dsp_buf[i++]=(External_Buff_Size>>16&0xFF);	dsp_buf[i++]=(External_Buff_Size>>8&0xFF);	dsp_buf[i++]=(External_Buff_Size&0xFF);

	DspSpiWriteNByte(dsp_buf,i);  					
}

/* 
 * @brief  Delay module 0x0014 �Ĵ�������
 *
 * @author chenjianming
 * @date 2007-11-2
 * @refer AN246PPA
 * @note  
 */
void DSP_DLYAUXCNTL(unsigned long reg_value)
{
	short i=0;

	dsp_buf[i++]=Delay_Manager_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x14;
	dsp_buf[i++]=(reg_value>>24&0xFF);	dsp_buf[i++]=(reg_value>>16&0xFF);	dsp_buf[i++]=(reg_value>>8&0xFF);	dsp_buf[i++]=(reg_value&0xFF);

	DspSpiWriteNByte(dsp_buf,i);  					
}

/* 
 * @brief  DSP ��������ز˵��й���Delay������ֵ
 *
 * @author chenjianming
 * @date 2008-9-2
 * @refer AN246PPA
 * @note  
 */
 extern void DelayMs(int delay_time);
void DSP_Delay_Firmware_Init(void)
{
	DelayMs(50); /*�л���Code ʱ��Ҫ����ʱ,��Ȼ������������������*/
	DSP_Delay_Channel_Setting(CH_L,dsp_delay_time.spk_fl_dtime);  
	DSP_Delay_Channel_Setting(CH_C,dsp_delay_time.spk_c_dtime);
	DSP_Delay_Channel_Setting(CH_R,dsp_delay_time.spk_fr_dtime);
	DSP_Delay_Channel_Setting(CH_LS,dsp_delay_time.spk_ls_dtime);
	DSP_Delay_Channel_Setting(CH_RS,dsp_delay_time.spk_rs_dtime);
	DSP_Delay_Channel_Setting(CH_LB,dsp_delay_time.spk_sbl_dtime);
	DSP_Delay_Channel_Setting(CH_RB,dsp_delay_time.spk_sbr_dtime);
	DSP_Delay_Channel_Setting(CH_SW,dsp_delay_time.spk_lfe_dtime);

	DSP_MAIN_PPM_CONTROL(1,1,1); /*Enable Delay*/
}

void DSP_AV_Delay(unsigned char Delay_time)
{
	if(dsp_power_flag) 
		{
		DSP_Delay_Channel_Setting(CH_L,Delay_time);
		DSP_Delay_Channel_Setting(CH_C,Delay_time);
		DSP_Delay_Channel_Setting(CH_R,Delay_time);
		DSP_Delay_Channel_Setting(CH_LS,Delay_time);
		DSP_Delay_Channel_Setting(CH_RS,Delay_time);
		DSP_Delay_Channel_Setting(CH_LB,Delay_time);
		DSP_Delay_Channel_Setting(CH_RB,Delay_time);
		DSP_Delay_Channel_Setting(CH_SW,Delay_time);		
		}  
}

/*
 * @brief ��DSP ���Ͷ����������ȡDSP ���ص������ٴ�ӡ����
 *
 * @author chenjianming
 * @date 2007-11-2
 * @note ��Ҫ������ȡindex (0x0014) ����Ĵ�����BIT12 ��ֵ.
 *		��Ϊ1ʱ,��index(0x0015) �Ĵ�����BIT1 ҲΪ1,���ʾ�Ѿ�
 *		�������ⲿ�洢��������
 */
unsigned long DspReadDelay(unsigned char index)
{
	short i=0;

	dsp_buf[i++]=0xD9;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);	/*���Ͷ���������*/
	return ReadDsp(); /*��ȡDSP ���ص�����*/
}

#if 0
/* 
 * @brief  Dealy Firmware Configuration (����֮ǰ���ص�Delay����ֵ)
 *
 * @author chenjianming
 * @date 2007-11-2
 * @note ���ڼĴ���0x000a-0x0011(ͨ��������ʱ����)
 *		��0x0012-0x13(�ⲿ�洢��ʼ��ַ�ʹ�С)	��Щ�Ĵ��������
 *		����Kick_Start ����֮ǰ����,�����kick_Start ֮������һ��Ҫ��
 *		index (0x0014) �е�BIT4 ��BIT8 ����λ��1.
 *
 */
void DSP_Delay_Firmware_Init(void)
{	
	DSP_MAIN_PPM_CONTROL(Enable,Enable,Enable);
	//DSP_Delay_Unit(0); //default

	DSP_Delay_Max_Setting(CH_L,0x00C80000);  
	DSP_Delay_Max_Setting(CH_C,0x00C80000); 
	DSP_Delay_Max_Setting(CH_R,0x00C80000);     
	DSP_Delay_Max_Setting(CH_LS,0x00C80000);
	DSP_Delay_Max_Setting(CH_RS,0x00C80000);
	DSP_Delay_Max_Setting(CH_SW,0x00C80000);
	DSP_Delay_Max_Setting(CH_LB,0x00C80000);  
	DSP_Delay_Max_Setting(CH_RB,0x00C80000);	
		
	//DSP_EXT_BUFF_ADDR(0x00000000); //default
	DSP_EXT_BUFF_SIZE(0x00800000); 
}
#endif
