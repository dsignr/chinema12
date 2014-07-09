#include "..\driver\ioport.h"
#include "..\driver\uart.h"
#include "..\driver\timer.h"
#include "..\driver\cs8416.h"
#include "..\driver\Dac.h"
#include "..\driver\vfddriver.h"
#include "..\driver\eeprom.h"
#include "..\driver\ir.h"
#include "..\driver\pin_config.h"
#include "..\driver\Hdmi.h"

#include "debug.h"
#include "task.h"
#include "vfd_disp.h"
#include "message.h"
#include "key_tab.h"
#include "keydefine.h"
#include "macro.h"
#include <stdlib.h>
#include "queue.h"
#include "Radio_task.h"
#include <LPC23xx.H>

#include "..\cs495300\dspdrv.h"

PFV Rs232ProtocolHandler = C11Rs232;

#define TEST_GITHUb 1
/*jian addd this test in 2014-07-09*/
void Systeminit(void)
{
	HardwareInit(); /*Ӳ����ʼ��*/
	UartInit();	     /*���ڳ�ʼ��*/
	Timer0Init();     /*��ʱ����ʼ��*/
	InitRemoteRepeatKey();
	AddRemoteRepeatKey(_IR_VOL_UP_KEY);	/*����+��������������*/
	AddRemoteRepeatKey(_IR_VOL_DOWN_KEY);/*����-��������������*/
}

void ProcDimerOff(KEYT key)
{
	if(sys_flag.system_status == POWER_ON)
		{
		if(key != _REMOTE_BRIGHT_KEY)
			{
			if(sys_flag.system_dimer == 0)
				{
				VfdSetDimerValue(2);	/*off״̬�£�����а���������������������low״̬*/
				temp_dimer_time = 50;
				}
			}
		}
}

extern void debug_message(MSG message);
int main()
{
	MSG message;
	unsigned char status;

	Systeminit(); /*ϵͳ��ʼ�� */

	status=InitMsgQueue();
	assert(status == FALSE);

	//PowerLed(ON);
	LedStandby();

	LoadSystemStartupValue(); //Load the saved start up value
	SetIoStandby();
	InitDSDMode(2);
//        DspMasterBoot();//jian add in 10-09-01
	while(1)
	{
	Rs232ProtocolHandler();

	Timer0_Handler();

	if(GetMessage(&message) == TRUE)
		{
#ifdef _DEBUG
		//debug_message(message);
#endif
		ProcMessage(&message);
		}
//
	DspLoop();

	HDMI_DSD_Loop();

	if(need_refresh_vfd)
		{
		need_refresh_vfd = 0;
		VfdRefresh();
		}
	if(need_refresh_dimer)
		{
		need_refresh_dimer = 0;
		VfdSetDimerValue(sys_flag.system_dimer);
		}

	RadioLoop();
	}
}

#if 0
//Debug --��Сϵͳ
int main()
{
	unsigned char i=0;

	HardwareInit(); /*Ӳ����ʼ��*/
	UartInit();	     /*���ڳ�ʼ��*/
	Timer0Init();     /*��ʱ����ʼ��*/
	PowerLed(ON);

	//SystemPowerOn();

	debug_printf("\n11111111111111111");
	i = EepromReadByte(0);
	debug_printf("PRODUCT_ID_POSITION:%02X\n",i);

	SetTrigerOut(1);
	debug_printf("\n11111111111111111");

	while(1)
		{
		SetTrigerOut(1);
		debug_printf("\n11111111111111111");
		}
}
#endif

