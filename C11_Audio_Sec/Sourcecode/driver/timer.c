#include "mcudef.h"
#include "timer.h"
#include "ioport.h"
#include "..\main\task.h"
#include "..\main\vfd_disp.h"
#include "..\main\Radio_task.h"
#include "..\main\Queue.h"

extern void key_task(void);
extern void RefreshHandle(void);
extern void SetupFlashHandler(void);  
extern void GetEncoderSwitchThread(void);
unsigned char psm_8416_time_tick = 0; /*jian add in 12-01-30*/

static short system_delay_timer;
void DelayMs(int delay_time)
{
	if(delay_time > 0)
		{
		system_delay_timer = delay_time;
		while(system_delay_timer);
		}
}

extern unsigned char softmute_flag;
extern unsigned char eint1_timer_cnter;
extern unsigned char eint2_timer_cnter;
extern unsigned char eint3_timer_cnter;
extern unsigned char Tuner_Text_Disp_Flag;
extern unsigned char DSP_HDMI_Setting_Flag;
unsigned short rs232_timer = 0;
unsigned char Tuner_Text_Count=0;
unsigned char Text_Update_Flag=0;
unsigned char HDMI_tick=0;
unsigned char Hdmi_Stream_tick=0;
unsigned char DSD_Stream_tick=0;
unsigned char DSP_Busy_tick=0;
unsigned char radio_time_tick = 0;
unsigned char system_timer_flag = 0;
unsigned char pcm20_delay_timer = 0;
unsigned char pcm20_pill_timer = 0;
void timer0_irq (void) __irq	/*timer 0 tick: 2ms ����һ���ж�*/
{
	static unsigned int t_count = 0; /*Text Display ������ʾ*/
	static unsigned int text_count=0; /*Text Display ʵʱ��ȡ����*/
	static unsigned int hdmi_cnter=0; /*HDMI �Ĵ�������*/
	static unsigned int t_cnter = 0;	
	static unsigned int softmute_cnter=0; /*softmute���ܿ���*/
	static unsigned int HDMI_Setting_cnter=0; /*HDMI ����������*/
	extern uint8 Artist_len ;
	extern uint8 Title_len ;

	T0IR = 1; /* Clear interrupt flag*/
	VICIntEnClr = VIC_ENABLE_TIMER0; /*д��1�����ж�ʹ�ܼĴ����еĶ�Ӧλ����ֹ��Ӧ���ж�����*/

	/*for remote pluse counter to remember timer tick*/
	eint1_timer_cnter++;
	eint2_timer_cnter++;
	eint3_timer_cnter++;

	/*for DelayMs(int)*/
	if(system_delay_timer > 0)	
		system_delay_timer --;

	/*for Flash Rs232*/
	if(rs232_timer)
		rs232_timer --;	
	system_timer_flag = 1 ; // jian add in 10-08-06
	/*HDMI �Ĵ�������*/
	hdmi_cnter++;
	if(hdmi_cnter>500)
		{
		HDMI_tick=1;
		Hdmi_Stream_tick=1;		
		DSD_Stream_tick=1;
		DSP_Busy_tick++;
		hdmi_cnter=0;
		}
  
	t_cnter ++; 
	if(t_cnter >100)  
		{
		t_cnter = 0;  
		psm_8416_time_tick = 1; // jian add in 12-01-30
		radio_time_tick = 1; /*for FM MODE ������ʾ����*/    
		VfdTimerHandler(); /*VFD��ʾ2����Ϣ����������ʾ1������Ϣ*/ /*EQ ON OFF ��ʾ*/
		RefreshHandle(); /*FM������ˢ��*/		
		AD0CR = 0; //jian add in 10-08-05
		}
	
	/*for Text Disply ������ʾ*/
	t_count++;
	if(t_count>250) /*250ms*2=500ms--����һ�ֽ�change  50-- 250*/
		{
		t_count=0;
		if(Tuner_Text_Disp_Flag) /*Text Display ��ʾ�����ѿ���*/
			{
			Tuner_Text_Count++; /*��ʾ���ַ���������һ�����ַ�������ȥ*/
			if(Tuner_Text_Count == (Artist_len+Title_len -19)) /*���20���ֽھͲ�������,���´ӵ�0����ʼ��ʾ*/
				Tuner_Text_Count=0;
			MsgSetVfdShow(RADIO_ID,1); /*mode=1--����Text Display*/
			}
		}

	if(Tuner_Text_Disp_Flag)/*Text Display ��ʾ�����ѿ���*/
		{
		text_count++;
//		if(text_count>15000) /*15000*2=30000ms--����һ��80�ֽ��ܹ���ʱ��*/ 
		if(text_count>(187*(Artist_len+Title_len)))
			{
			text_count=0; 
			Text_Update_Flag=1;
			}
		}

	if(softmute_flag)
		{
		softmute_cnter++;
		if(softmute_cnter>1500) 
			{
			softmute_cnter=0;
			softmute_flag=0;
			MsgSetSoftMute(ZONE1_ID,0);
			}
		}

	if(DSP_HDMI_Setting_Flag)
	{
		HDMI_Setting_cnter++;
		if(HDMI_Setting_cnter>2500) /*2000*2=4000ms*/ 
		{
			HDMI_Setting_cnter=0;
			DSP_HDMI_Setting_Flag=0;
			MsgSetDSPHDMISetting(ZONE1_ID,0); /*���ز˵�����*/
		}
	}
	
	VICIntEnable = VIC_ENABLE_TIMER0; /* 1ʹ���ж����������жϷ���ΪFIQ��IRQ */
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

unsigned char dsp_time_tick = 0;

extern pSTRUCT_QUEUE tuner_queue_pointer;  
//int dsp_setting_timer_cnt = 0;
extern void lpcm_20_config1(void);
void Timer0_Handler(void) /*��ʱ����ŵ���ѭ������,������Ҫ���ڶ�ʱ�жϴ���*/
{
	static unsigned char key_cnt = 0; /*����ɨ��*/
	
	/*get the encoder switch */	
	GetEncoderSwitchThread(); 

	/*for ����ɨ��*/
	key_task(); 
	
	key_cnt ++;
	if(key_cnt >2)
		{
		dsp_time_tick = 1;
		key_cnt = 0;
		}
	
	//jian add in 10-11-03
	if(system_timer_flag)	
		{
		system_timer_flag = 0;
	//	HDMI_secret_loop();
		if(pcm20_delay_timer)
			{
			pcm20_delay_timer--;
			if(pcm20_delay_timer == 0)
				{
				lpcm_20_config();
				pcm20_pill_timer	= 100;	
				}
			
			}
		
		if(pcm20_pill_timer)
			{
			pcm20_pill_timer--;
			if(pcm20_pill_timer==0)
				{
				lpcm_20_config1();
				}
			}
		
		}
	/*for Text Display ʵʱˢ��*/
	if(Text_Update_Flag) /*ÿ30 s ˢ��һ��*/
		{
		Text_Update_Flag=0;
		if(Tuner_Text_Disp_Flag) /*Text Display ��ʾ�����ѿ���*/
			{
			Tuner_Text_Disp_Flag=0; /*���ڶ�PSD ����Ϣʱ,Update text ��text_count ����ʱ,���ڽ��BUG*/
			
	//		Tuner_Reply_Flag=1;	/*׼������PSD CHANGE ��*/		
//			Tuner_Req_Radio_Monitor_Command(0,0,1); /*��HD Radio �����������ñ�־λ��ȡPSD CHANGE ��*/				
			#if 0 // jian remove it in 11-08-18
			do{
				C11_Tuner_PSD_Change();    
				}while(Tuner_Reply_Flag);
			#endif
		//	Tuner_Req_Radio_Monitor_Command(1,0,0); /*���������PSD ����־λ*/			
			Tuner_Text_Count=0; /*ʵʱˢ�¶��ӵ�0����ʼ��ʾ*/
		  	RadioDisplay(system_freq,current_radio_mode,system_position); /*�����ʱ����ʾTextDisplay��������*/    	  		
			Radio_Text_Display();
			DelayMs(30); 
			debug_printf("initqueue \n");
			InitQueue(tuner_queue_pointer);/*�����ʱ����ʾTextDisplay��������*/	      
			
			Tuner_Text_Disp_Flag=1;  /*���ڶ���PSD ����Ϣʱ,Update text ��text_count ��ʼ��ʱ,���ڽ��BUG*/
			}  
		}

	/*�޸�����ͨ���ַ�ʱ,��ʾ"_"*/
	SetupFlashHandler(); 
}

void Timer0Init()
{
	T0MR0 = TIMER0_PERIOD;
	T0MCR = 3;	/*2���ö�ʱ��ֵƥ��ʱ����λ��ʱ��ֵ�����ж�*/
	T0TCR = 1;	/* Timer0 Enable--Timer counter are enabled for counting*/
#if 1
	VICVectAddr4= (unsigned long)timer0_irq; /* set interrupt vector in 0 */
	VICVectCntl4 = VIC_PRIORITY_4;		  /* use it for Timer 0 Interrupt Priority*/
	VICIntEnable = VIC_ENABLE_TIMER0; 	  /*�ж�ʹ��*/ 
#endif
}

