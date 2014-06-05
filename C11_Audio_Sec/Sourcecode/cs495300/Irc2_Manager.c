/* @file   Irc2_Manager.c
 * @brief Irc2 Firmware Manager
 *
 * @author chenjianming
 * @date 2007-11-14
 * @refer AN246PPJ
 *
 *		Write=0x9D00HHHH 0xhhhhhhhh
 *		Read Request=0x9DC0HHHH
 *		Read Response=0x1D0HHHH 0xhhhhhhhh
 *
 *		0xHHHH=index	0xhhhhhhhh=data value
 */

#include "Dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "..\main\Macro.h"
#include "..\main\Debug.h"

#ifdef _DEBUG
//#define _DEBUG_IRC_2_MANAGER
#endif

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/*
 * @brief  IRC2 Control
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note Bit16 0-Enable 1-Disable Ĭ��Ϊ0 Enable passthrough ,
 *           ����Disable,����������������������뵽DSP,�γ�������,Ӱ��IRC���ԡ�
 *           ����˷���յ��������DSP���.
 *           ��ʵ�ʵ�IRC���Թ�����,��������ص�,�����Ӱ����Խ����
 */
void Dsp_IRC2_Control(unsigned char value)
{	
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);		
}

/*
 * @brief  IRC2 Speaker Select
 *
 * @author chenjianming    
 * @date 2007-11-16
 * @note
 */
 void Dsp_IRC2_Speaker_Select(unsigned char value,unsigned char eq_enable)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x01;
	dsp_buf[i++]=0x00;
	if(eq_enable)
		dsp_buf[i++]=0x01;
	else
		dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief  IRC2 Speaker Select
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note
 */
 void Dsp_IRC2_Max_EQ_Fc(unsigned char value)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x04;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=value;

	DspSpiWriteNByte(dsp_buf,i);	
}

/*
 * @brief ��ȡIRC-2 �й̼��Ĵ�����ֵ 
 *
 * @author chenjianming
 * @date 2007-11-16
 * @note Ҫ��ȡ�̼��Ĵ�����ֵ,�����ȷ��Ͷ����������ٶ�ȡDSP �ķ�����Ϣ
 */
unsigned long DspReadIrc2(unsigned char index)
{
	short i=0;

	dsp_buf[i++]=0x9D;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x00;	dsp_buf[i++]=index;

	DspSpiWriteNByte(dsp_buf,i);	/*���Ͷ���������*/
	return ReadDsp();
}

/* 
 * @brief  �ж�IRC ��������Ϣ�Ƿ���Ч
 *
 * @author chenjianming
 * @date 2007-11-16
 * @refer chenshiguang C11 code
 * @note  ��IRC �е�ͨ���������,�̼����ʹ�ܱ�־λ(��4λ)���--��ʾ
 *            IRC����������Ҳ��������Ľ�����Ա���ȡ�ˡ�
 */
int IsIrc2ValidResult(void)
{
	if(DspReadIrc2(0x00)&0x000f)         
		return FALSE;  
	return TRUE;
}

