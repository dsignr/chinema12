/* @file   VFD ����ʾӦ�ú���
 * @brief 
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 code
 */
#include "macro.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "..\driver\vfddriver.h"

#include "vfd_disp.h"
#include "task.h"
#include "debug.h"

#ifdef _DEBUG
//#define _DEBUG_VFD_DISP
#endif

static unsigned char vfd_owner = 1;	/*Normal display*/
static unsigned short temp_display_time = 0;

//const char cary_logo[] = "   Cary Cinema 11   ";
const char Surround_Processor[] = " Surround Processor ";
static char vfd_buffer[] = "                                        ";
static char vfd_temp_disp_buffer[] = "                                        ";

unsigned char need_refresh_vfd = 0;
unsigned char need_refresh_dimer = 0;

/*VFD ��Ļˢ��*/
void VfdRefresh(void)
{
	VfdUpdateBuffer(VFD_UPDATE_LINE_ALL,vfd_buffer);
}

/*�õ�Vfd buffer �е��ַ�*/
char GetVfdbufferChar(uchar position)
{
	return vfd_buffer[position];
}

void SetVfdOwner(uchar owner)
{
	if(owner)
		{
		if(temp_display_time)
			VfdRefresh();
		temp_display_time = 0;
		}
	vfd_owner = owner;
}

void VfdTimerHandler(void)
{
	if(temp_display_time)
		{
		temp_display_time --;
		if(temp_display_time == 0)
			{
			vfd_owner = 1;
			if(sys_flag.system_zone1_status == POWER_ON)
				{
				need_refresh_vfd = 1;
				}
			}
		}
	if(temp_dimer_time)
		{
		temp_dimer_time--;
		if(temp_dimer_time == 0)
			{
			if(sys_flag.system_zone1_status == POWER_ON)
				need_refresh_dimer = 1;
			}
		}
}

/*time �ĵ�λ��0.1��*/
void SetVfdDispMode(unsigned char mode,unsigned short time)
{
	vfd_owner = mode;
	if(mode == 0)
		{
		temp_display_time = time;
		}
}

/* ����VFD��ʾ��������
 * *bufferΪ��Ҫˢ�µ�����
 * start_positionˢ�µ������ڻ������е���ʼλ��
 * lengthΪˢ�µĸ���
 * modeΪˢ��ģʽ�����ΪVFD_UPDATE_NONE����VFD��ˢ�µȴ���һ��ִ��
 */
unsigned char VfdDisplay(unsigned char start_position,unsigned char mode,const char *format,...)
{
	char temp_buffer[64];
	unsigned int length = 0;
	unsigned char rtn_value = VFD_BUFFER_SUCESS;
	va_list arg_ptr; /*��ʼ���ɱ����arg_ptr*/
	va_start (arg_ptr, format);	/*��ʼ������arg_ptr,arg_ptrָ��format����Ĳ���*/
	vsprintf (temp_buffer, format, arg_ptr);  /*��arg_ptr ָ��Ĳ���д��temp_buffer*/
	length = strlen(temp_buffer);

	if((start_position + length) > VFD_MAX_POSITION)
		{
		rtn_value = VFD_BUFFER_LENGTH_ERROR;
		length = VFD_MAX_POSITION - start_position;
		}

	strncpy(vfd_buffer+start_position,temp_buffer,length);/*����temp_buffer ��length ���ַ���vfd_buffer ��*/

	switch(mode)
		{
//		case VFD_UPDATE_NONE:
//			break;
		case VFD_UPDATE_LINE1:
		case VFD_UPDATE_LINE2:
		case VFD_UPDATE_LINE_ALL:
			if(vfd_owner)
				VfdUpdateBuffer(mode,vfd_buffer);
			break;
		default:
			break;
		}
	return rtn_value;
}

void VfdDisplayBlank(unsigned char line,unsigned char update)
{
	unsigned char clear_mode = 0;
	switch(line)
		{
		case VFD_LINE1:
			clear_mode |= 0x01; /*���һ��*/
			break;
		case VFD_LINE2:
			clear_mode |= 0x02; /*��ڶ���*/
			break;
		case VFD_LINE_ALL:
			clear_mode |= 0x03; /*��ȫ������*/
			break;
		}
	if(clear_mode & 0x01)
		memset(vfd_buffer + VFD_LINE1_START_POSITION,' ',VFD_LINE1_LENGTH); /*��vfd_buffer �дӵ�һ�п�ʼǰ20���ַ�����Ϊ�ո�*/
	if(clear_mode & 0x02)
		memset(vfd_buffer + VFD_LINE2_START_POSITION,' ',VFD_LINE2_LENGTH); /*��vfd_buffer �дӵڶ��п�ʼǰ20���ַ�����Ϊ�ո�*/
		
	if(vfd_owner)
		VfdUpdateBuffer(update,vfd_buffer);
}

unsigned char VfdTempDisp(unsigned char start_position,unsigned char mode,unsigned short time, const char *format,...)
{
	char temp_buffer[64];
	unsigned int length = 0;
	va_list arg_ptr;
	va_start (arg_ptr, format);
	vsprintf (temp_buffer, format, arg_ptr);
	length = strlen(temp_buffer);

	if((start_position + length) > VFD_MAX_POSITION)
		return VFD_BUFFER_LENGTH_ERROR;

	strncpy(vfd_temp_disp_buffer+start_position,temp_buffer,length);

	switch(mode)
		{
		//case VFD_UPDATE_NONE:
		//	break;
		case VFD_UPDATE_LINE1:
		case VFD_UPDATE_LINE2:
		case VFD_UPDATE_LINE_ALL:
			VfdUpdateBuffer(mode,vfd_temp_disp_buffer);
			break;
		default:
			break;
		}

	SetVfdDispMode(0,time);

	return VFD_BUFFER_SUCESS;
}

void VfdTempDispBlank(unsigned char line,unsigned char update)
{
	unsigned char clear_mode = 0;

	switch(line)
		{
		case VFD_LINE1:
			clear_mode |= 0x01; 
			break;
		case VFD_LINE2:
			clear_mode |= 0x02;
			break;
		case VFD_LINE_ALL:
			clear_mode |= 0x03;
			break;
		}
	if(clear_mode & 0x01)
		memset(vfd_temp_disp_buffer + VFD_LINE1_START_POSITION,' ',VFD_LINE1_LENGTH);
	if(clear_mode & 0x02)
		memset(vfd_temp_disp_buffer + VFD_LINE2_START_POSITION,' ',VFD_LINE2_LENGTH);

	VfdUpdateBuffer(update,vfd_temp_disp_buffer);
}

#if 0
void test_vfd()
{
	uchar i = 0x20,j = 0;
	do{
		if(j==7)
			{
			VfdDisplay(VFD_LINE1_START_POSITION+j*5,VFD_UPDATE_LINE_ALL,"%02X:%c ",i,i);
			DelayMs(2500);
			}
		else
			{
			VfdDisplay(VFD_LINE1_START_POSITION+j*5,VFD_UPDATE_NONE,"%02X:%c ",i,i);
			}
		i++;
		j++;
		j %= 8;
		}while(i<0xFF);
}
#endif

