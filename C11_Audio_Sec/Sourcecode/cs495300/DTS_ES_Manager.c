/* @file   DTS_ES_Manager.c
 * @brief DTS_ES Manager File
 *
 * @author chenjianming
 * @date 2007-1-9
 * @refer AN246DC
 */

#include "dspdrv.h"
#include "..\driver\Dsp_Spi.h"
#include "DTS_ES_Manager.h"

extern unsigned char dsp_buf[DSP_MAX_MSG_SIZE];

/* @file  
 * @brief DTS_ES ʹ�ܿ���
 *
 * @author chenjianming
 * @date 2007-1-9
 * @refer AN246DC
 *           ����DTS_ES�ǻ���DTS�̼�,����Ҫʹ��DTS_ES,Ҳ����ʹ��DTS.
 *		����bit0 ����Ϊ1����DTS
 */
void DTS_ES_Control(unsigned char Matrix_6dot1,unsigned char Discrete_6dot1_Enalbe)
{
	short i=0;
	unsigned char reg_value;

	switch(Matrix_6dot1)
		{
			case DTS_ES_Matrix_6dot1_OFF:
				reg_value=0x00;
				break;
			case DTS_ES_Matrix_6dot1_ON:
				reg_value=0x20;
				break;
			case DTS_ES_Matrix_6dot1_Autodetect:
				reg_value=0x40;
				break;
			default:
				reg_value=0x00;
				break;
		}

	if(Discrete_6dot1_Enalbe)
		reg_value |=0x10; 

	dsp_buf[i++]=DTS_ES_Wop;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;
	dsp_buf[i++]=0x00;	dsp_buf[i++]=0x00;	dsp_buf[i++]=reg_value;	dsp_buf[i++]=0x01; 
	
	DspSpiWriteNByte(dsp_buf,i);  
}

