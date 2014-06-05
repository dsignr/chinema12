/* @file 		Dspdrv.c
 * @brief 		DSP Driver File
 *
 * @author 	chenjianming
 * @date 	  	2007-06-27
 * @version 	2.0
 */
 
#include "..\main\macro.h"
#include "..\main\debug.h"
#include "..\main\task.h"
#include "..\main\c11_msg_handler.h"
#include "..\driver\ioport.h"
#include "..\driver\Dsp_Spi.h"
#include "..\driver\Cs8416.h"
#include "..\driver\Volume.h"
#include "..\driver\channel.h"
#include "..\driver\Hdmi.h"
#include "..\driver\Dac.h"
#include "dspdrv.h"
#include "DSP_API.h"
#include "DspInputOutputCfg.h"
#include "OS_Manager.h"
#include "AudioManager.h"
#include "PCM_Manager.h"
#include "DolbyManager.h"
#include "SignalGenerator.h"
#include "BassManager.h"
#include "DolbyPrologicII.h"
#include "DolbyPrologicIIx.h"
#include "Eq_Manager.h"
#include "DelayManager.h"
#include "Irc1_Manager.h"
#include "Irc2_Manager.h"
#include "DTS_ES_Manager.h"
#include "DTS_Manager.h"
#include "DTS_NEO6_Manager.h"
#include "DolbyHeadphone.h"
#include "..\driver\eeprom.h"
//#include "dsp_protocol.h"

#include <string.h>
#include <math.h>

#ifdef _DEBUG
#define _DEBUG_DSPDRV
#endif

#define OUT_MODE 	0x09  /*0x09= 3/4 L/C/R/LS/RS/SBL/SBR*/

unsigned char PreDspMode=8; 
unsigned char compression_mode = 0;
unsigned char enable_eq = 1;
unsigned char enable_bass = 1;
unsigned char enable_delay = 1;

unsigned char system_spk_mode[8] = {0};
unsigned char dsp_buf[DSP_MAX_MSG_SIZE];
unsigned char CurrentSampleFreq = FREQ_48KHZ;
unsigned char CurrentIputPort=Spdif_AutoSwitch;
unsigned char stream_type = STREAM_PCM;
unsigned char dsp_force_change_code = 1;

unsigned char DSDType;
unsigned char PcmType;
unsigned char DolbyType = _DOLBY_5CH;
unsigned char DtsType = _DTS;

extern const float balance_table[];
extern unsigned char Movie_Music_Speaker_Flag; 

unsigned char softmute_flag=0;
unsigned long last_stream=0;
extern unsigned char Pre_TureHD_Type;
extern unsigned char Pre_DTS_HD_Type;
extern unsigned char CD_Pure_Audio;


float ReConvert_8_24_Value(long in_value)
{
	float rtn_value;
	rtn_value = in_value / POW_2_24;
	return rtn_value;
}

float ConvertBalanceValue(float float_value)
{
	short temp_value;
	temp_value = float_value * 10;
	temp_value /= 5;
	float_value = temp_value * 5;
	float_value /= 10;
	return float_value;
}

/*
 * @brief transition the array type to word type
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
void ArrayToDWORD (DWORD *pdwMsg, unsigned char *message_array) 
{
	*pdwMsg = 0;
	*pdwMsg |= message_array[0];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[1];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[2];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[3];
	*pdwMsg <<=8;
	
	*pdwMsg |= message_array[4];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[5];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[6];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[7];	
	return;
}

/*
 * @brief transition the array type to half word type
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
void ArrayToHWORD (DWORD *pdwMsg, unsigned char *message_array) 
{
	*pdwMsg = 0;
	*pdwMsg |= message_array[0];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[1];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[2];
	*pdwMsg <<=8;
	*pdwMsg |= message_array[3];  
	return;
}

/* 
 * @brief Panorama Width Setting
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Width(unsigned char width)
{
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
		{
		DSP_PLII_DECODE_MODE(Music_Mode); /*����Ϊmusic mode*/
		DSP_PLII_CENTER_WIDTH_CONFIGURATION(width);
		}
}

/* 
 * @brief Panorama Dimension Setting
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Dimension(unsigned char Dimension)
{		
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))	
		{
		DSP_PLII_DECODE_MODE(Music_Mode); /*����Ϊmusic mode*/
		DSP_PLII_DIMENSION_CONFIGURATION(Dimension);
		}
}

/* 
 * @brief Panorama Enable Control
 *
 * @author chenjianming
 * @date 2008-2-23
 * @refer 
 */
void Set_Prologic2Panorama_On_Or_Off(unsigned char enable)
{
	if((sys_flag.stream_type==STREAM_AC3)&&(sys_flag.decode_mode.dolby_type==_DOLBY_2CH)&&(sys_flag.decode_mode.dolby_decode_mode==AC3_MODE_PL2))
		{
			DSP_PLII_Control(Enable); /*Prologic2 Enable*/
			DSP_PLII_DECODE_MODE(Music_Mode); /*����Ϊmusic mode*/
			if(enable)
				DSP_PLII_CONTROL_REGISTER(Enable); /*Panorama Enable*/
			else
				DSP_PLII_CONTROL_REGISTER(Disable); /*Panorama Disable*/		
		}
}

/* 
 * @brief Set Speaker Config ���ܺ��� 
 *
 * @author chenjianming
 * @date 2007-12-4
 * @refer chenshiguang C11 code
 */
#ifdef _USE_DSP_PROTOCOL
void DspProcSpkMode(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcSpkMode(unsigned char channel,unsigned char mode)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	assert(msg_size < 2);

	spk_mode = buffer[3];

	switch(buffer[2])
#else
	system_spk_mode[channel] = mode;
#endif

	SetBassManagerSpeaker();
}

/*
 * @brief Set Distance ���ܺ���
 *
 * @author chenjianming
 * @date 2007-12-5
 * @refer chenshiguang C11 code
 */
#ifdef _USE_DSP_PROTOCOL
void DspProcDelay(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcDelay(unsigned char channel,unsigned short delay_time)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned short delay_time = 0;
	assert(msg_size < 3);

	delay_time = buffer[3];
	delay_time <<= 8;
	delay_time |= buffer[4];

	switch(buffer[2])
#else
	switch(channel)
#endif
		{
		case CH_L:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_fl_dtime = delay_time;
			else
				dsp_delay_music_time.spk_fl_dtime=delay_time;
			break;
		case CH_C:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_c_dtime = delay_time;
			else
				dsp_delay_music_time.spk_c_dtime = delay_time;
			break;
		case CH_R:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_fr_dtime = delay_time;
			else
				dsp_delay_music_time.spk_fr_dtime = delay_time;
			break;
		case CH_LS:
			if(Movie_Music_Speaker_Flag==0) 
				dsp_delay_time.spk_ls_dtime = delay_time;
			else
				dsp_delay_music_time.spk_ls_dtime = delay_time;
			break;
		case CH_RS:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_rs_dtime = delay_time;
			else
				dsp_delay_music_time.spk_rs_dtime = delay_time;
			break;
		case CH_LB:
			if(Movie_Music_Speaker_Flag==0)  
				dsp_delay_time.spk_sbl_dtime = delay_time;
			else
				dsp_delay_music_time.spk_sbl_dtime = delay_time;
			break;
		case CH_RB:
			if(Movie_Music_Speaker_Flag==0) 
				dsp_delay_time.spk_sbr_dtime = delay_time;
			else
				dsp_delay_music_time.spk_sbr_dtime = delay_time;
			break;
		case CH_SW:
			if(Movie_Music_Speaker_Flag==0)
				dsp_delay_time.spk_lfe_dtime = delay_time;
			else
				dsp_delay_music_time.spk_lfe_dtime = delay_time;
			break;
		}

#ifdef _DEBUG_DSPDRV
		//debug_printf("\n@@@@@@dsp_delay_time.spk_fl_dtime=%d",dsp_delay_time.spk_fl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_c_dtime=%d",dsp_delay_time.spk_c_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_fr_dtime=%d",dsp_delay_time.spk_fr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_ls_dtime=%d",dsp_delay_time.spk_ls_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_rs_dtime=%d",dsp_delay_time.spk_rs_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_sbl_dtime=%d",dsp_delay_time.spk_sbl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_sbr_dtime=%d",dsp_delay_time.spk_sbr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_time.spk_lfe_dtime=%d",dsp_delay_time.spk_lfe_dtime);

		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_fl_dtime=%d",dsp_delay_music_time.spk_fl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_c_dtime=%d",dsp_delay_music_time.spk_c_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_fr_dtime=%d",dsp_delay_music_time.spk_fr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_ls_dtime=%d",dsp_delay_music_time.spk_ls_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_rs_dtime=%d",dsp_delay_music_time.spk_rs_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_sbl_dtime=%d",dsp_delay_music_time.spk_sbl_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_sbr_dtime=%d",dsp_delay_music_time.spk_sbr_dtime);
		//debug_printf("\n@@@@@@dsp_delay_music_time.spk_lfe_dtime=%d",dsp_delay_music_time.spk_lfe_dtime);	
#endif

	if(dsp_power_flag) 
		{
#ifdef _USE_DSP_PROTOCOL
		switch(buffer[2])
#else
		switch(channel)
#endif
			{
			case CH_L:
				DSP_Delay_Channel_Setting(CH_L,dsp_delay_time.spk_fl_dtime);
				break;
			case CH_C:
				DSP_Delay_Channel_Setting(CH_C,dsp_delay_time.spk_c_dtime);
				break;
			case CH_R:
				DSP_Delay_Channel_Setting(CH_R,dsp_delay_time.spk_fr_dtime);
				break;
			case CH_LS:
				DSP_Delay_Channel_Setting(CH_LS,dsp_delay_time.spk_ls_dtime);
				break;
			case CH_RS:
				DSP_Delay_Channel_Setting(CH_RS,dsp_delay_time.spk_rs_dtime);
				break;
			case CH_LB:
				DSP_Delay_Channel_Setting(CH_LB,dsp_delay_time.spk_sbl_dtime);
				break;
			case CH_RB:
				DSP_Delay_Channel_Setting(CH_RB,dsp_delay_time.spk_sbr_dtime);
				break;
			case CH_SW:
				DSP_Delay_Channel_Setting(CH_SW,dsp_delay_time.spk_lfe_dtime);
				break;
			}
		}
}

#ifdef _USE_DSP_PROTOCOL
void DspProcCorner(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcCorner(unsigned char channel,unsigned char corner_value)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char corner_value = 0;

	assert(msg_size < 2);

	corner_value = buffer[3];

	switch(buffer[2]) 
#else
	switch(channel)
#endif
		{
		case CH_L:
			dsp_bass_corner.fl_corner = corner_value;
			break;
		case CH_C:
			dsp_bass_corner.cen_corner = corner_value;
			break;
		case CH_R:
			dsp_bass_corner.fr_corner = corner_value;
			break;
		case CH_LS:
			dsp_bass_corner.sl_corner = corner_value;
			break;
		case CH_RS:
			dsp_bass_corner.sr_corner = corner_value;
			break;
		case CH_LB:
			dsp_bass_corner.sbl_corner = corner_value;
			break;
		case CH_RB:
			dsp_bass_corner.sbr_corner = corner_value;
			break;
		case CH_SW:
			dsp_bass_corner.sum1_corner = corner_value;
			dsp_bass_corner.sum2_corner = corner_value;
			dsp_bass_corner.sum3_corner = corner_value;
			dsp_bass_corner.sum4_corner = corner_value;
			dsp_bass_corner.sum5_corner = corner_value;
			break;
		}
	if(dsp_power_flag)
		{
#ifdef _USE_DSP_PROTOCOL
		switch(buffer[2])
#else
		SetBassManagerSpeaker();
#endif
		}
}

#ifdef _USE_DSP_PROTOCOL
void DspProcSignal(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcSignal(unsigned char signal_type, unsigned char channel, unsigned char freq)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned char channel, freq;

	assert(msg_size < 1);
#endif
	if(!dsp_power_flag)	/*dsp not power, return*/
		return;
	
#ifdef _USE_DSP_PROTOCOL
	channel = buffer[3];
	freq = buffer[4];

	switch(buffer[2])
#else
	switch(signal_type)
#endif
		{
		case Signal_White_Noise:		
			DSP_SGEN_CHANNEL_ENABLE(channel);			
			break;	
		case Signal_Stop:
			DSP_SGEN_ENABLE(0);    
			//Change_Input_Audio_Source(CurrentIputPort); /*ͨ���лص���ǰ�����,���ڲ��Էۺ�����ʱ����������л�,�����л�ȥ*/	
			//DelayMs(30);
			//Change_DSP_Decoder_Code(1,ULD_ID_AC3); /*�л��½���̼�*/        			
			break;
		}
}

#define SPK_TEST_TIME 3
#ifdef _USE_DSP_PROTOCOL
void DspProcIrc(unsigned char *buffer,unsigned char msg_size)
#else
void DspProcIrc(unsigned char irc_mode,unsigned char test_channel)
#endif
{
#ifdef _USE_DSP_PROTOCOL
	unsigned long irc_value = 0;
	uchar test_channel;

	test_channel = buffer[3];

	assert(msg_size < 1);
#endif
	if(!dsp_power_flag)
		return;
#ifdef _USE_DSP_PROTOCOL
	switch(buffer[2])
#else
	switch(irc_mode)
#endif
		{
		case IRC_TEST_INIT: /*Load IRC2 code and init */     
			DacMute(1);  /*mute ������ʱ������*/
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_IRC); /*�л�ΪIRC �̼�*/
			DelayMs(100); /*������̼�Kick Start ��,DSP�����ڲ��ȶ�״̬,��ʱ��*/
			DSP_IRC1_Test_Duration(SPK_TEST_TIME); /*IRC Test Time=3*/
			DSP_Rx_Channel(CH_L); /*ѡ��MIC����ͨ��*/		
			InitMicParameter(); /*������˷��������*/  
			DacMute(0);  /*mute ������ʱ������*/
			break;
		case IRC_TEST_AND_SETUP_DELAY: /*Test Speaker Status*/
			DacMute(1);  /*mute ������ʱ������*/
			Dsp_IRC2_Speaker_Select(test_channel,0); /*Speaker ͨ��ѡ��*/			
			Dsp_IRC2_Control(0x01); /*Dealy/Polarity Disable passthrough*/		  		
			DacMute(0);  /*mute ������ʱ������*/
			break;
		case IRC_TEST_AND_SETUP_TRIMS: /*Test Speaker Size and level*/
			DacMute(1);  /*mute ������ʱ������*/
			Dsp_IRC2_Speaker_Select(test_channel,0); /*Speaker ͨ��ѡ��*/
			Dsp_IRC2_Control(0x02); /*Trim mode--Balance ����/Disable passthrough*/
			DacMute(0);  /*mute ������ʱ������*/
			break;
		case IRC_TEST_AND_SETUP_EQ: /*Test EQ*/		
			DacMute(1);  /*mute ������ʱ������*/			
			Dsp_IRC2_Speaker_Select(test_channel,1); /*Speaker ͨ��ѡ�� EQ enable*/  
			Dsp_IRC2_Control(0x08); /*Speaker EQ Calibration /Disable passthrough*/	    
			DacMute(0);  /*mute ������ʱ������*/  
			break;    
		}
}

void SetDh2On(unsigned char on)
{    
//#ifdef _DEBUG_DSPDRV			
	//debug_printf("\nsys_flag.stream_type=%d",sys_flag.stream_type);
	//debug_printf("\nsys_flag.stream_subtype=%d",sys_flag.stream_subtype);
//#endif	

	/*���岻��Headphone ����*/
	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_PLUS)) /*Dolby Plus ��֧��Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_2ch)) /*Dolby Ture HD2.0��֧��Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_5ch)) /*Dolby Ture HD5.1��֧��Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_6ch)) /*Dolby Ture HD6.1��֧��Headphone*/
		{
		return;
		}

	if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD_7ch)) /*Dolby Ture HD7.1��֧��Headphone*/
		{
		return;
		}
		
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_48K)) /*DTS_HD_48K(2.0/5.1/7.1)��֧��Headphone*/
		{
		return;
		}
		
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_MA_96k_192K)) /*DTS_HD_96K(2.0/5.1/7.1)��֧��Headphone*/
		{
		return;
		}
	
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_48K)) /*DTS_HD_HRA_48k��֧��Headphone*/
		{
		return;
		}
	if((stream_type==STREAM_DTS)&&(DtsType==_DTS_HD_HRA_96K)) /*DTS_HD_HRA_48k��֧��Headphone*/
		{
		return;
		}
	 
	if(on)
		{
		Change_DSP_VIRTUALIZER_Code(DH2_MODE_BYPASS,ULD_ID_DH2); /*Load DolbyHeadphone code*/
		DelayMs(30); /*�����ȶ�����������*/
		}
	else 
		{
		Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*Remove DolbyHeadphone code*/
		}
}

void DspChannelOut5_1(void)
{
	OS_OUTPUT_MODE_CONTROL(Disable,Stereo,L_C_R_LS_RS); /* (3/2 L_C_R_LS_RS)*/	
	OS_OUTPUT_MODE_CONTROL_FOR_DSPB(Disable,Stereo,L_C_R_LS_RS); /* (3/2 L_C_R_LS_RS)*/
}

void DspChannelOut7_1(void)
{
	OS_OUTPUT_MODE_CONTROL(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/	
	OS_OUTPUT_MODE_CONTROL_FOR_DSPB(Disable,Stereo,L_C_R_Ls_Rs_Sbl_Sbr); /* (3/4 L/C/R/LS/RS/SBL/SBR)*/
}

void DspChannelOutSetting(unsigned char mode)
{
	switch(mode)
		{
		case PCM_MODE_PL2:
			DspChannelOut5_1();
			break;
		case PCM_MODE_PL2X:
			DspChannelOut7_1();
			break;
		default:
			break;
		}
}

/*DSP�����ɹ�����ز˵�����DSP ��ص�����*/
extern unsigned char DSP_HDMI_Setting_Flag;
void DspLoadDefaultSetting(unsigned char mode)
{
	if(mode==0)
		{  
		//SoftMute(1);      
#ifdef _DEBUG_DSPDRV	
		debug_printf("\n___Before DspLoadDefaultSetting  :%d",PreDspMode);
#endif		
		if(PreDspMode==3)/*Analog 2 channel +NEO6/PL2Xʱû�취����APP,�����������ж�*/
			{
			if((sys_flag.decode_mode.pcm_decode_mode==PCM_MODE_NORMAL))
				{			
				/*add by cjm 2009-4-7*/
				DelayMs(300);  
				DSP_BOOT(configuration_lock,boot_idle);
				DSP_CFG_PPM(ULD_ID_APP);  /*����APP�̼�*/
				DelayMs(30);
				DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/	
				DSP_BOOT(configuration_unlock,change_to_new_config);
				DelayMs(300); 
				DSP_Delay_Firmware_Init(); /*����Delay����ֵ*/
				SetBassManagerSpeaker(); /*���ص�����������ֵ*/
				DSP_EQ_Firmware_Init();		 
				/*add by cjm 2009-4-7*/
				}
			else /*Analog 2 channel +NEO6/PL2Xʱû�취����APP,�����������ж�*/
				{
				DelayMs(200); 
				DSP_Delay_Firmware_Init(); /*����Delay����ֵ*/
				SetBassManagerSpeaker(); /*���ص�����������ֵ*/				
				}
			}
		else /*���˺�ͬ��*/
			{
			DelayMs(200); 
			DSP_Delay_Firmware_Init(); /*����Delay����ֵ*/
			SetBassManagerSpeaker(); /*���ص�����������ֵ*/			
			DSP_EQ_Firmware_Init();  
			}
#ifdef _DEBUG_DSPDRV	
		debug_printf("\n___After DspLoadDefaultSetting");
#endif			
		DSP_AV_Delay(EepromReadByte(AV_SYNC_POSITION)); // jian add in 10-08-05

		SoftMute(0); 
//		if(sys_flag.zone2_channel == INPUT_HDMI)
			Zone2SelectChannel(sys_flag.zone2_channel);
		}
	else
		{	   
		if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
			{
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_CROSSBAR_B); /*HDMI ֻ�ܼ���CROSSBAR_B �����ܼ���CROSSBAR*/
			DSP_HDMI_Setting_Flag=1;
			//DelayMs(300); /*������̼�����ʱ��*/      
			//MsgSetDSPHDMISetting(ZONE1_ID,0); /*���ز˵�����*/
			}
		}  
}
  
extern char dsp_power_flag;
extern unsigned char Pre_AC3_Type; 
extern unsigned char Pre_DTS_Type; 
extern unsigned char Pre_TureHD_Type;
extern unsigned char Pre_DTS_HD_Type;
void DspProcPower(unsigned char mode)
{
	if(mode!=PreDspMode)
		{
#ifdef _DEBUG_DSPDRV	
		debug_printf("\nDiffer Dsp power mode=%d",mode);
#endif				
		PreDspMode=mode;
		}
	else
		{
#ifdef _DEBUG_DSPDRV	
		debug_printf("\nThe same Dsp power mode=%d",mode);
#endif			
		return;
		}

	if(mode==0) /*��λ״̬*/
		{
		DSPHwReset(1);
		dsp_power_flag = 0;

//		if(SUCCESS == DspMasterBoot())
//			dsp_power_flag = 1;	/*boot success */
//		else
//			dsp_power_flag = 0; /*boot fail*/			
		
		return ;
		}
	
	if(SUCCESS == DspMasterBoot())
		dsp_power_flag = 1;	/*boot success */
	else
		dsp_power_flag = 0; /*boot fail*/	
	
	switch(mode)
		{
		case 1: //SPDIF
			Pre_AC3_Type=0; /*ÿ�����������������ǰDolby����Ϣ*/
			Pre_DTS_Type=0; /*ÿ�����������������ǰdts����Ϣ*/
			Change_Input_Audio_Source(Spdif_AutoSwitch); 	
			break;
		case 2: //Multi Analog		  
		case 3: //Multi Analog	
			Change_Input_Audio_Source(Multi_Channel_Analog);	
			break;
		case 4: //HDMI			
			Pre_TureHD_Type=0;
			Pre_DTS_HD_Type=0;
			Change_Input_Audio_Source(HDMI_AutoSwitch);	  
			break;
		default:
			break;
		}  

#if 0	 // jian remote it in 12-03-08
        if(last_stream ==  LINEAR_5_1_PCM_STREAM)
		ReadDsp();
#endif		
}

/*
 * @brief �������е�MIXER ΪStereo
 *
 * ÿ��������������������8���������(L/C/R/LS/RS/SLB/SRB/LFE)
 * ��������������(�������͸����������������͸�������)
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer AN246MPC
 * 		chenshiguang Cll code
 */
void SetMixerAllStereo()
{
#if 0
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x7FFFFFFF); /*FL*/ /*��������ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x3FFFFFFF); /*CEN*/ /*���ø�ȡ1/2*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0);		     /*FR*/ /*������Ҳֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x7FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x7FFFFFFF); /*SL*/ /*���ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0);		     /*SR*/ /*�һ��ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x7FFFFFFF);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x7FFFFFFF); /*SBL*/ /*����ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0);		      /*SBR*/ /*�Һ��ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x7FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x3FFFFFFF); /*��LFE������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x3FFFFFFF);
#endif

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x1FFFFFFF); /*FL*/ /*��������ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x1FFFFFFF); /*CEN*/ /*���ø�ȡ1/2*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0);		     /*FR*/ /*������Ҳֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x1FFFFFFF); /*SL*/ /*���ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0);		     /*SR*/ /*�һ��ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x1FFFFFFF);
	
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x1FFFFFFF); /*SBL*/ /*����ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0);		      /*SBR*/ /*�Һ��ƾ�ֻȡ������*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x1FFFFFFF); /*��LFE������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x81,0x1111); 	     /*��Ƶϵ����HOST*/
}

/*  
 * @brief �������е�MIXER ΪMono
 *
 * ÿ����������������8���������(L/C/R/LS/RS/SLB/SRB/LFE)
 * ��������������1/2(L+R)��L��ͬ��1/2(L+R)��R��
 * 0X7FFFFFFF ��1/2 ����0X3FFFFFFF
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer AN246MPC
 * 		chenshiguang Cll code
 */
void SetMixerAllMono()
{
#if 0 
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x3FFFFFFF); /*��L������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x3FFFFFFF); /*��C������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0x3FFFFFFF); /*��R������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x3FFFFFFF); /*��LS������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0x3FFFFFFF); /*��RS������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x3FFFFFFF); /*��SLB������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0x3FFFFFFF); /*��SRB������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x3FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x3FFFFFFF); /*��LFE������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x3FFFFFFF);
#endif	

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x00,0x1FFFFFFF); /*��L������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x02,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x08,0x1FFFFFFF); /*��C������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x0A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x10,0x1FFFFFFF); /*��R������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x12,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x18,0x1FFFFFFF); /*��LS������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x1A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x20,0x1FFFFFFF); /*��RS������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x22,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x28,0x1FFFFFFF); /*��SLB������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x2A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x30,0x1FFFFFFF); /*��SRB������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x32,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x38,0x1FFFFFFF); /*��LFE������ȡ1/2(L+R)*/
	DSPFirmwareWrite(Crossbar_Manager_Wop,0x3A,0x1FFFFFFF);

	DSPFirmwareWrite(Crossbar_Manager_Wop,0x81,0x1111); 	     /*��Ƶϵ����HOST*/
}

/*
 * @brief ����Crossbar effect
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang Cll code
 */
void DspSetCrossbarEffect(unsigned char mode)
{

#ifdef _DEBUG_DSPDRV	
	debug_printf("\DspSetCrossbarEffect : %d\n",mode);
#endif

	switch(mode)
		{
		case 1: /*CES 7.1 All Stereo*/
			SetMixerAllStereo(); /*������*/
			break;
		case 2:  /*CES 7.1 All Mono*/
			SetMixerAllMono(); /*������*/
			break;
		case 0:		
		default: /*CES 7.1*/
			Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,�л���CROSSBAR*/
			DelayMs(300);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
			Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*mode: 2chs to 7.1 chs,�л���CROSSBAR code*/	
			MsgSetDSPSetting(ZONE1_ID,0); /*�л���̼���APP/SPP �ָ�ΪĬ��ֵ,�����¼���DSPSETTING*/
			break;
		}
}		

/* 
 * @brief DSP ��PCM ��  
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
void DspDecodePcm(unsigned char decode_mode)
{	
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nDspDecodePcm\n");
#endif

	if(GetNormalFreq() == FREQ_192KHZ)	/*192K��������Ч��*/
		decode_mode = PCM_MODE_NORMAL;

	switch(PcmType)
		{
		case _PCM_2_1:
			switch(decode_mode)
				{
				case PCM_MODE_PL2:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X);
					DelayMs(300);
					DspChannelOut5_1(); /*��5.1 ������*/ 
					break;
				case PCM_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2X\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;			
				case PCM_MODE_NEO6:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NEO6\n");
#endif
					//Change_DSP_MATRIX_Code(NEO6_MODE_MUSIC,ULD_ID_NEO6);
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*����NEO6������,����PL2X����*/
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
		 			break;
				case PCM_MODE_CES:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_CES\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,�л���CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);	/*��VIRTUALIZER Code�л�Ϊ����,��Ȼ�����ͨ������BUG*/
					break;
				case PCM_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NORMAL\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);  /*0-not mode,ֻ�л���CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					break;  
				}	 
			break;
		case _PCM_5_1:
			switch(decode_mode)
				{
				case PCM_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_PL2X\n");
#endif
					Change_DSP_MATRIX_Code(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;			
				case PCM_MODE_NEO6:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NEO6\n");
#endif
					//Change_DSP_MATRIX_Code(NEO6_MODE_MUSIC,ULD_ID_NEO6);
					Change_DSP_MATRIX_Code(PL2X_MODE_MUSIC,ULD_ID_PL2X); /*����NEO6������,����PL2X����*/
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
		 			break;
				case PCM_MODE_CES:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_CES\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*mode: 5chs to 7.1 chs,�л���CROSSBAR code*/
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID); /*��VIRTUALIZER Code�л�Ϊ����,��Ȼ�����ͨ������BUG*/
					DSP_BOOT(configuration_unlock,change_to_new_config);
					break;
				case PCM_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("PCM_MODE_NORMAL\n");
#endif
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);	
					break;  
				}
			break;
		case _PCM_7_1: /*����Ҫ������*/
			break;
		default:
			break;
		}
}

#if 0
/* 
 * @brief   ����PCM ����Ϣ�仯�������Ӧ�Ĵ���
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer 
 */
char Pcm_Cfg_Change_Deal(void)
{	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nPcm_Cfg_Change_Deal");
#endif
	if(dsp_power_flag==1)
		{    
		if(stream_type==STREAM_PCM) /*���Ѿ���PCM ��,�Ͳ���Ҫ�����ٽ�*/
			return;

		DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);	  
		}	
}
#endif

/* 
 * @brief DSP ��Dolby �� 
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer  chenshiguang C11 code
 */
void DspDecodeAc3(unsigned char Decode_Mode)
{
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nDspDecodeAc3\n");
#endif

	switch(DolbyType)
		{
		case _DOLBY_2CH:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_PL2\n");
#endif				
					DSP_CFG_MATRIX(PL2X_MODE_MUSIC,ULD_ID_PL2X);
					break;
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_PL2X\n");    
#endif							
					DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					break;
				case AC3_MODE_CES_ON:
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_CES_ON\n");
#endif			
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_R_7CH,ULD_ID_CROSSBAR); /*mode: 2chs to 7.1 chs,�л���CROSSBAR*/
					DelayMs(30);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				case AC3_MODE_NORMAL:				
				default:	
#ifdef _DEBUG_DSPDRV	
					debug_printf("Dolby_2Ch_NORMAL\n");
#endif			
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					if(sys_flag.headphone_flag)
						DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
					else
						DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;			
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);
			break;   
		case _DOLBY_5CH:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_PL2X\n");
#endif
					DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X);
					break;
				case AC3_MODE_CES_ON:  
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_CES_ON\n");
#endif
					DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*mode: 5chs to 7.1 chs,�л���CROSSBAR code*/
					DelayMs(30);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				case AC3_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby_5Ch_NORMAL\n");
#endif				
					DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR); 
					DelayMs(30);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					if(sys_flag.headphone_flag)
						DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
					else
						DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);			
			break;
		case _DOLBY_EX:
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				default:
#ifdef _DEBUG_DSPDRV
  				debug_printf("Dolby_EX_Normal\n");    
#endif			
				DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*����crossbar ʹdiscrete ���7.1*/
	//			DSP_CFG_MATRIX(PL2X_MODE_DD_EX,ULD_ID_PL2X); /*�л���PL2X EX mode*/	 jian change in 11-06-22
				if(sys_flag.headphone_flag)
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
				break;
				}
			DelayMs(30);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/	
			DSP_BOOT(configuration_unlock,change_to_new_config);			
			break;  
		//case _DOLBY_PLUS: /*��ǰ�ݲ�������,��������ֻ��PLUS7.1*/
		//	break;
		case _DOLBY_TRUE_HD_2ch: /*TureHD2.0��5.1ֻ��PLIIx����*/
		case _DOLBY_TRUE_HD_5ch:		
		case _DOLBY_TRUE_HD_6ch:		
			DSP_BOOT(configuration_lock,boot_idle);
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
#ifdef _DEBUG_DSPDRV
  					debug_printf("Dolby TureHD + PL IIx\n");    
#endif						
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					break;
				case AC3_MODE_NORMAL:
				default:
#ifdef _DEBUG_DSPDRV
					debug_printf("Dolby TureHD Normal\n");    
#endif
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					break;
				}
			DelayMs(30);
			DSP_BOOT(configuration_unlock,change_to_new_config);
			break;
		//case _DOLBY_TRUE_HD_7ch: /*����Ҫ���⴦��*/
		//	break;
	}
	
	switch(DolbyType)
		{
		case _DOLBY_2CH:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2:
					break;
				case AC3_MODE_PL2X:
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;
				case AC3_MODE_CES_ON:
					break;
				case AC3_MODE_NORMAL:				
				default:						
					break;			
				}
			DelayMs(300); 
			DSP_AC3_DRC_INIT();
			break;   
		case _DOLBY_5CH:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/					
					break;
				case AC3_MODE_CES_ON:  
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}
			DelayMs(300); 
			DSP_AC3_DRC_INIT();
			break;
		case _DOLBY_EX:
			switch(Decode_Mode)
				{
				default:
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/  
					break;
				}
			DelayMs(300);  
			DSP_AC3_DRC_INIT();
			break;  
		//case _DOLBY_PLUS: /*DRC Ĭ��������DSPloop��*/
		//	break;
		case _DOLBY_TRUE_HD_2ch:
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/    
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();
			break;
		case _DOLBY_TRUE_HD_5ch:		
		case _DOLBY_TRUE_HD_6ch:		
			switch(Decode_Mode)
				{
				case AC3_MODE_PL2X:
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/  
					break;
				case AC3_MODE_NORMAL:
				default:
					break;
				}			
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();
			break;
		case _DOLBY_TRUE_HD_7ch:
			DelayMs(300); 
			DSP_TRUEHD_DRC_INIT();  
			break;
		default:
			break;
		}	
}  

/* 
 * @brief   ����AC3 ����Ϣ�仯�������Ӧ��Code �л�
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer
 */
unsigned char Pre_AC3_Type=0; 
char AC3_Cfg_Change_Deal(unsigned char AC3_Type)
{	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nAC3_Cfg_Change_Deal");
	debug_printf("\nAC3_Type=%d",AC3_Type);
	debug_printf("\nPre_AC3_Type=%d",Pre_AC3_Type);  
	debug_printf("\dsp_power_flag=%d",dsp_power_flag);  
#endif
	if(dsp_power_flag==1)
		{    
		if(Pre_AC3_Type==AC3_Type)
			return 0;

		DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);	
		MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/
		Pre_AC3_Type=AC3_Type;
		}	
}

/* 
 * @brief   ����AC3 ��Ϣ����н���
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
extern unsigned char Dolby2ch_Flag; 
void AnalyseAc3Cfg(_AC3_STREAM ac3_stream)  
{	
	unsigned char dolby_type=_DOLBY_5CH;
	
#ifdef _DEBUG_DSPDRV
	//dsp_printf("\nacmod:%X\n",ac3_stream.acmode);	
	//dsp_printf("dsurmod:%X\n",ac3_stream.dsurmod);
	//dsp_printf("bsid:%X\n",ac3_stream.bsid);
	//dsp_printf("lfeon:%X\n",ac3_stream.lfeon);
	//dsp_printf("TIMECOD2:%X\n\n",ac3_stream.timecod2);
#endif

	//if(ac3_stream.bsid == 6) 
	if((ac3_stream.timecod2 == 0x2) && (ac3_stream.bsid == 6)) 
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\nAnalyse AC3:Dolby_EX");
#endif
		dolby_type = _DOLBY_EX;
		}
	else if(ac3_stream.acmode == 2)
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\nAnalyse AC3:Dolby_2CH");
#endif	
		dolby_type = _DOLBY_2CH;
		}	

	DolbyType = dolby_type;
	sys_flag.decode_mode.dolby_type = DolbyType;
	MsgSetStream(stream_type,DolbyType);	
	AC3_Cfg_Change_Deal(dolby_type);
}

/* 
 * @brief   ����Dolby Ture HD ����Ϣ�仯�������Ӧ��Code�л�
 *
 * @author chenjianming
 * @date 2008-12-16
 * @refer 
 */
unsigned char Pre_TureHD_Type=0;
char TureHD_Cfg_Change_Deal(unsigned char TureHD_Type)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nTureHD_Cfg_Change_Deal");
	debug_printf("\nTureHD_Type=%d",TureHD_Type);
	debug_printf("\nPre_TureHD_Type=%d",Pre_TureHD_Type);
#endif

	if(dsp_power_flag==1)
		{
		if(Pre_TureHD_Type==TureHD_Type)
			return 0 ;
		DspDecodeAc3(sys_flag.decode_mode.dolby_decode_mode);
		Pre_TureHD_Type=TureHD_Type; 
		}
}

/* 
 * @brief   ����Dolby Ture HD ��Ϣ����н���
 *
 * @author chenjianming
 * @date 2008-12-16
 * @refer 
 */
void AnalyseTureHDCfg(unsigned char *uCmdBuf)
{
	unsigned char dolby_type;
	unsigned char TureHD_Channels;

	//DelayMs(100); /*��ʱ��,ʹ�ÿ�����ȷ������������Ĵ�����ֵ*/
	//TureHD_Channels=DspReadTRUEHD(0x0a);

	uCmdBuf+=12;  
	TureHD_Channels=uCmdBuf[3]&0x0F;
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nuCmdBuf[3]=%x",uCmdBuf[3]); 
	debug_printf("\nTureHD_Channels=%d",TureHD_Channels);
#endif

	switch(TureHD_Channels)
		{
		case 2: /*Ture HD 2.0*/
			dolby_type=_DOLBY_TRUE_HD_2ch;
			break;
		case 7: /*Ture HD 5.1*/
			dolby_type=_DOLBY_TRUE_HD_5ch;
			break;
		case 8: /*Ture HD 6.1*/
			dolby_type=_DOLBY_TRUE_HD_6ch;
			break;
		case 9: /*Ture HD 7.1*/
			dolby_type=_DOLBY_TRUE_HD_7ch;
			break;
		}
	
	DolbyType = dolby_type;
	sys_flag.decode_mode.dolby_type = DolbyType;
	MsgSetStream(stream_type,DolbyType);	
	TureHD_Cfg_Change_Deal(dolby_type);
}

/* 
 * @brief  DTS ����Ϣ�仯�������Ӧ��Code �л�
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer 
 */
void DspDecodeDTS(unsigned char decode_mode)
{
	switch(sys_flag.decode_mode.dts_type) /*ֻ��DTS ��DTS 96/24����CES7.1 ON*/
		{
		case _DTS:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS\n");
#endif					
			DSP_BOOT(configuration_lock,boot_idle);
			if(decode_mode==DTS_MODE_CES_ON)
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_CES_ON\n");
#endif					
				Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR);/*mode: 5chs to 7.1 chs,�л���CROSSBAR code*/
				DelayMs(300);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
				Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*��VIRTUALIZER Code�л�Ϊ����,��Ȼ�����BUG*/					
				}
			else
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_NORMAL\n");  
#endif		
				DSP_CFG_DECODER(DTS_ES_MODE_DTS,ULD_ID_DTS_ES);
				DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/
				DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);
				DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/
				if(sys_flag.headphone_flag) 
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);		
				}		
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;
		case _DTS_96_24:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS 96/24\n");
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			if(decode_mode==DTS_MODE_CES_ON)
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_CES_ON\n");
#endif				
				Change_DSP_MATRIX_Code(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR);/*mode: 5chs to 7.1 chs,�л���CROSSBAR code*/
				DelayMs(300);/*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
				Change_DSP_VIRTUALIZER_Code(0,ULD_ID_INVALID); /*��VIRTUALIZER Code�л�Ϊ����,��Ȼ�����BUG*/					
				}
			else
				{
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_MODE_NORMAL\n");  
#endif					
				DSP_CFG_DECODER(DTS_ES_MODE_DTS9624,ULD_ID_DTS_ES);
				DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/
				DSP_CFG_MATRIX(0,ULD_ID_CROSSBAR);
				DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/		
				if(sys_flag.headphone_flag)
					DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
				else
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
				}
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;  
		case _DTS_ES_MATRIX:  
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS-ES Matrix\n");
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			DSP_CFG_DECODER(DTS_ES_MODE_MATRIX,ULD_ID_DTS_ES);
			DelayMs(50); /*�̼����л�ʱ�Ӹ���ʱ*/
		//	DSP_CFG_MATRIX(0,ULD_ID_NEO6); /*Matrix ��6.1��7.1������,������PL2X*/    
			DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*����crossbar ʹdiscrete ���7.1*/
			DelayMs(50); /*�̼����л�ʱ�Ӹ���ʱ*/  
			if(sys_flag.headphone_flag)
				DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);
			else
				DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);  
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;
		case _DTS_ES_DISCRETE:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nDTS-ES Discrete\n");  
#endif			
			DSP_BOOT(configuration_lock,boot_idle);
			DSP_CFG_DECODER(DTS_ES_MODE_DISCRETE,ULD_ID_DTS_ES); 
			DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/ 
			DSP_CFG_MATRIX(CROSSBAR_MODE_COEF_L_C_R_LS_RS_7CH,ULD_ID_CROSSBAR); /*����crossbar ʹdiscrete ���7.1*/
			DelayMs(10); /*�̼����л�ʱ�Ӹ���ʱ*/
			if(sys_flag.headphone_flag)
				DSP_CFG_VIRTUALIZER(DH2_MODE_BYPASS,ULD_ID_DH2);  
			else
				DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
			DSP_CFG_PPM(ULD_ID_APP);
			DSP_CFG_PPM_Mode(1,0); /*���������ļ�����EQ modeΪTrue Parametric*/
			DSP_BOOT(configuration_unlock,change_to_new_config);	 
			break;      
		case _DTS_HD_MA_48K_2ch:
		case _DTS_HD_MA_48K_5ch:
			switch(decode_mode)
				{
				case DTS_MODE_PLIIX:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;
				case DTS_MODE_NORMAL:
				default:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;
				}
			break;
		case _DTS_HD_MA_48K_6ch: /*DTS HD 48k 7.1����Ҫ����*/
		case _DTS_HD_MA_48K_7ch: /*DTS HD 48k 7.1����Ҫ����*/
			break;
		case _DTS_HD_MA_96K_2ch:
		case _DTS_HD_MA_96K_5ch:
			switch(decode_mode)
				{
				case DTS_MODE_PLIIX:
					DSP_BOOT(configuration_lock,boot_idle);
					DSP_CFG_VIRTUALIZER(PL2X_B_MODE_MOVIE,ULD_ID_PL2X_B);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;
				case DTS_MODE_NORMAL:
				default:
					DSP_BOOT(configuration_lock,boot_idle); 
					DSP_CFG_VIRTUALIZER(0,ULD_ID_INVALID);
					DSP_BOOT(configuration_unlock,change_to_new_config);
					DelayMs(300); /*�л���Code�ȴ�DSP �ȶ�*/
					DspChannelOut7_1(); /*��7.1��ͨ��*/
					break;
				}
			break;  
		case _DTS_HD_MA_96K_7ch: /*DTS HD 96k 7.1����Ҫ����*/
			break;
		case _DTS_HD_MA_192K_2ch:
		case _DTS_HD_MA_192K_5ch:
			break;
		case _DTS_HD_MA_192K_7ch:
			break;
		}
}

/* 
 * @brief   ����DTS ����Ϣ�仯�������Ӧ�Ĵ���
 *
 * @author chenjianming
 * @date 2008-07-04
 * @refer  
 */
unsigned char Pre_DTS_Type=0; 
void DTS_Cfg_Change_Deal(unsigned char decode_mode)
{	
	if(dsp_power_flag==1)
		{
#ifdef _DEBUG_DSPDRV		
		debug_printf("\nPre_DTS_Type=%d",Pre_DTS_Type);
		debug_printf("\nsys_flag.decode_mode.dts_type=%d",sys_flag.decode_mode.dts_type);
#endif		
		if(Pre_DTS_Type==sys_flag.decode_mode.dts_type)
			return;
		
		switch(sys_flag.decode_mode.dts_type)
			{	
			case _DTS:
				DspDecodeDTS(decode_mode);
				break;
			case _DTS_96_24:
				DspDecodeDTS(decode_mode);
				break;
			case _DTS_ES_MATRIX: 
				DspDecodeDTS(decode_mode);
				DelayMs(300);
				DspChannelOut7_1(); /*��7.1��ͨ��*/				
				break;		
			case _DTS_ES_DISCRETE:  
				DspDecodeDTS(decode_mode);
				DelayMs(300);
				DspChannelOut7_1(); /*��7.1��ͨ��*/				
				break;
			}
			
		Pre_DTS_Type=sys_flag.decode_mode.dts_type;
		DelayMs(30);
		MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/  
		}	
}  

/* 
 * @brief ����DTS ��Ϣ����н��� 
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
void AnalyseDtsCfg(_DTS_STREAM dts_stream)
{
	unsigned char dts_type=_DTS;
	unsigned char ext_audio_id,ext_audio;

	ext_audio_id = dts_stream.ext_audio_id;
	ext_audio=dts_stream.ext_audio & 0x1;

	switch(ext_audio_id)
		{
		case 0: /*XCh*/
			if(ext_audio)
				{
				dts_type = _DTS_ES_DISCRETE;
				break;
				}
			switch(dts_stream.pcmr)
				{
				case 0x01:
				case 0x03:
				case 0x05:
					dts_type = _DTS_ES_MATRIX;
					break;
				}
			break;
		case 2: /*96K*/
			switch(dts_stream.pcmr)
				{
				case 0x01:
				case 0x03:
				case 0x05:
					dts_type = _DTS_ES_MATRIX;
					break;
				case 0x06:
					dts_type = _DTS_96_24;
					break;
				}
			break;
		case 3: /*96K & XCh*/
			dts_type = _DTS_ES_DISCRETE;
			break;
		}

#ifdef _DEBUG_DSPDRV
	//debug_printf("AMODE:%X\n",dts_stream.amode);	
	//debug_printf("LFE:%X\n",dts_stream.lfeon);
	//debug_printf("PCMR:%X\n",dts_stream.pcmr);
	//debug_printf("EXT_AUDIO_ID:%X\n",dts_stream.ext_audio_id);
	//debug_printf("EXT_AUDIO:%X",dts_stream.ext_audio);
	//debug_printf("\ndts_type=%d",dts_type);
#endif

	sys_flag.decode_mode.dts_type = dts_type;
	MsgSetStream(stream_type,sys_flag.decode_mode.dts_type);
	DTS_Cfg_Change_Deal(sys_flag.decode_mode.dts_decode_mode);
}

/* 
 * @brief   ����DTS HD MA ����Ϣ�仯�������Ӧ��Code�л�
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
unsigned char Pre_DTS_HD_Type=0;
char DtsHD_Cfg_Change_Deal(unsigned char Dts_type)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nDtsHD_Cfg_Change_Deal");
	debug_printf("\nDts_type=%d",Dts_type);
	debug_printf("\nPre_DTS_HD_Type=%d",Pre_DTS_HD_Type);
#endif

	if(dsp_power_flag==1)
		{
		if(Pre_DTS_HD_Type==Dts_type)
			return;
		
		DspDecodeDTS(sys_flag.decode_mode.dts_decode_mode);
		Pre_DTS_HD_Type=Dts_type;
		}
}

/*
 * @brief  ��ȡDTS-HD�̼��мĴ�����ֵ
 *
 *		 ����DSP ����һ�� Read Request Message ��DSP�ط���һ��Read Response Message
 *		 Read Request Message=0x9FC0HHHH
 * 		 Read Response Message =0x1FC0HHHH 0xhhhhhhhh
 *
 *		 0xHHHH=index, 0xhhhhhhhh=data value
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer chenshiguang C11 code
 */
 unsigned char DspReadDTSHDChannel()
{
	short i=0;

	dsp_buf[i++]=0xA0;	dsp_buf[i++]=0xC0;	dsp_buf[i++]=0x05;	dsp_buf[i++]=0x86;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
}
/* 
 * @brief   ����DTS_HD_MA ��Ϣ����н���
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
 void AnalyseDtsHDCfg(unsigned char *uCmdBuf)
{
	unsigned char dts_type;
	unsigned char DTS_HD_Channels;
	unsigned char DTS_HD_MA_Freq;
	
	uCmdBuf+=8;  
	DTS_HD_Channels=uCmdBuf[3]&0x0F;
#ifdef _DEBUG_DSPDRV	
	debug_printf("\nuCmdBuf[3]=%x",uCmdBuf[3]);
	debug_printf("\nDTS_HD_Channels=%d",DTS_HD_Channels);
#endif
	uCmdBuf+=4;  
	DTS_HD_MA_Freq=((uCmdBuf[1]>>4)&0x0F); 
#ifdef _DEBUG_DSPDRV		
	debug_printf("\nuCmdBuf[1]=%x",uCmdBuf[1]);
	debug_printf("\nDTS_HD_MA_Freq=%x",DTS_HD_MA_Freq);
#endif

	switch(DtsType)
		{
		case _DTS_HD_MA_48K:
			switch(DTS_HD_Channels)
				{
				case 2: 
					dts_type=_DTS_HD_MA_48K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_MA_48K_5ch;
					break;
				case 7 :
					dts_type=_DTS_HD_MA_48K_6ch; // jian add in 11-10-10
					break;
				case 8:
					dts_type=_DTS_HD_MA_48K_7ch;
					break;
				default:
					break;
				}
			break;
		case _DTS_HD_MA_96k_192K:
			 if(DTS_HD_MA_Freq==0x0E)//192k
			 {
				switch(DTS_HD_Channels)
				{
					case 2:
						dts_type=_DTS_HD_MA_192K_2ch;
						break;
					case 6:
						dts_type=_DTS_HD_MA_192K_5ch;
						break;
					case 8:
						dts_type=_DTS_HD_MA_192K_7ch;
						break;
					default :
						break;
				}			 
			 }
			 else //96K
			 {
				switch(DTS_HD_Channels)
				{
					case 2:
						dts_type=_DTS_HD_MA_96K_2ch;
						break;
					case 6:
						dts_type=_DTS_HD_MA_96K_5ch;
						break;
					case 8:
						dts_type=_DTS_HD_MA_96K_7ch;
						break;
					default :
						break;
				}			 
			 }
			break;
		default:
			break;
		}
	
	sys_flag.decode_mode.dts_type=dts_type;
	MsgSetStream(stream_type,DtsType);
	DtsHD_Cfg_Change_Deal(sys_flag.decode_mode.dts_type);
}


/* 
 * @brief   ����DTS_HD_HRA ��Ϣ����н���
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
void AnalyseDtsHRACfg(unsigned char *uCmdBuf)
{
	unsigned char dts_type;
	unsigned char DTS_HD_HRA_Channels;

	uCmdBuf+=4;  
	DTS_HD_HRA_Channels = ((uCmdBuf[0]>>4)&0x0F); 
#ifdef _DEBUG_DSPDRV	
	debug_printf("\n__uCmdBuf[0]=%x",uCmdBuf[0]); 
	debug_printf("\nDTS_HD_HRA_Channels=%d",DTS_HD_HRA_Channels); 
#endif		

	switch(DtsType)
	{
		case _DTS_HD_HRA_48K:
			switch(DTS_HD_HRA_Channels)
			{
				case 2:
					dts_type=_DTS_HD_HRA_48K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_HRA_48K_6ch;
					break;
				case 7:
					dts_type=_DTS_HD_HRA_48K_7ch;
					break;
				case 8:
					dts_type=_DTS_HD_HRA_48K_8ch;
					break;
				default:
					break;
			}
			break;
		case _DTS_HD_HRA_96K:
			switch(DTS_HD_HRA_Channels)
			{
				case 2:
					dts_type=_DTS_HD_HRA_96K_2ch;
					break;
				case 6:
					dts_type=_DTS_HD_HRA_96K_6ch;
					break;
				case 7:
					dts_type=_DTS_HD_HRA_96K_7ch;
					break;
				case 8:
					dts_type=_DTS_HD_HRA_96K_8ch;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	sys_flag.decode_mode.dts_type=dts_type;
	MsgSetStream(stream_type,DtsType);
}

/* 
 * @brief   DTS-HD HRA �Ĵ�����ȡ
 *
 * @author chenjianming
 * @date 2008-12-17
 * @refer 
 */
unsigned long DSP_DTS_HD_HRA_Read(unsigned char index)
 {
  	short i=0;

	dsp_buf[i++] =0x9F;	dsp_buf[i++] =0xC0;	dsp_buf[i++] =0x00;	dsp_buf[i++] =index;

	DspSpiWriteNByte(dsp_buf,i);
	return ReadDsp();
 }

/* 
 * @brief DSP �Զ������Ϣ����
 *
 * @author chenjianming
 * @date 2007-10-18
 * @refer chenshiguang C11 code
 		 AN288 P23
 */


void lpcm_20_config(void)
{
	SetDspPower(1); /*DSP �л���Spdif ��*/ 
	DigiSendByte(ADDR_ZONE1,0x00); /*���ְ��л���HDMI ���ϵ�Spdif ��*/
	DSP_Input_Stream_Select(AD_Input); /*���뵽DSPԴ--A/D CS8416*/
}

void lpcm_20_config1(void)
{
	unsigned char temp_freq=0;
	stream_type=STREAM_PCM;
	PcmType=_PCM_2_1;

	temp_freq= Cs8416GetFreq();
	if(temp_freq!=0x0c) //0x0c-FREQ_192KHZ ����������ֵ
	{  	


	//	if(CD_Pure_Audio==0)
		{
			DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
			MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/	
		}
	}
	else
	{
		SoftMute(0); 
	} 
	MsgSetStream(stream_type,0); 
	SoftMute(0);

}

char ProcAutodetecResponse(unsigned char *buf)
	
{
	unsigned long current_stream;
	unsigned char HDMI_Stream_Channel;
	unsigned char temp_freq=0;
	extern unsigned char  pcm20_delay_timer;

#ifdef _DEBUG_DSPDRV
	debug_printf("\nProcAutodetecResponse");
#endif

	current_stream=((buf[2]&0xFF)<<8) | (buf[3]&0xFF); /*�õ���ǰ����Ϣ*/	

	/*�������+Pl2x����next�����ܱ���Pl2x���ܵ�����,ͬ����next ������,�������޷���DSP��,����ͨ�����Ա�DSP��*/
	if(!(buf[0] & DECODEABLE_STRM_FLAG))	/*This bit 0/1 = This stream is not/is decodable by the application (no need for new download if 1).*/
		{
		if((current_stream != SILENT_INPUT_DATA) && (current_stream != SILENT_DATA))/*If silent, don't need to reload*/
			{
			Pre_TureHD_Type=0;
			Pre_DTS_HD_Type=0;
			}
		}

	if(last_stream!=current_stream)
		{
		last_stream=current_stream;
		pcm20_delay_timer = 0;
		}
	else /*�ϴ�����Ϣ����ε�һ���Ͳ�����*/
		{	
#ifdef _DEBUG_DSPDRV
		debug_printf("\nThe same stream Information");      
#endif			
		MsgSetStream(stream_type,0); 
		return 0 ;
		}
	
#ifdef _DEBUG_DSPDRV
	debug_printf("\ncurrent_stream:%04X",current_stream);
#endif

	//if(IsCurFocusTid(AUTO_SOUND_ID) == FALSE) /*�����IRC���Թ����У����������µ���*/
	//	return;				

	if(buf[3]&NON_IEC_STFM_FLAG) /*bit5 = 1. Non IEC61937 compressed Stream*/
		{  
		switch(last_stream)
			{
			case SILENT_INPUT_DATA: /*Silent input data*/		
#ifdef _DEBUG_DSPDRV
				debug_printf("\nSilence");  
#endif
				stream_type=STREAM_NONE;  
				MsgSetStream(stream_type,0); 
				break;
			case SILENT_DATA: /*Silent input data*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nNo signal");  
#endif
				if(sys_flag.zone1_sub_channel!=3) //Analog Input ��⵽��No signal ������
				{
					stream_type=STREAM_NONE;
					MsgSetStream(stream_type,0);
				}
				if(sys_flag.zone1_channel != INPUT_CH71) 
				{
					if(sys_flag.zone1_sub_channel!=3) //Analog Input ��⵽��No signal ������
					{
						SoftMute(1);
					}
				}
				break;
			case DTS_FMT16_STREAM: /*DTS FORMAT-16 elementary stram*/
			case DTS_FMT14_STREAM: /*DTS FORMAT-14 elementary stram*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS FMT16 DTS FMT14\n");
#endif
				stream_type=STREAM_DTS;
				DtsType=_DTS;
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/
				break; 
			  case LINEAR_2_1_PCM_STREAM: /*0023*/	
				if(PreDspMode!=3) /*DSP�л���Analog �ڷ�����LPCM2.0��Ϣ������*/
			  	{
#ifdef _DEBUG_DSPDRV  
					debug_printf("\nLINEAR_2_0_PCM_STREAM");
#endif							  			

				if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))				
					{ 					
					 pcm20_delay_timer = 200; // jain add in 10-08-06
                                         break;										 
					}
                                      //
					stream_type=STREAM_PCM;
					PcmType=_PCM_2_1;
					SetDspPower(1); /*DSP �л���Spdif ��*/ 

					temp_freq= Cs8416GetFreq();
					//debug_printf("\ntemp_freq=%d",temp_freq); 
					if(temp_freq!=0x0c) //0x0c-FREQ_192KHZ ����������ֵ
					{  

//					if(CD_Pure_Audio==0)
						{
							DspDecodePcm(sys_flag.decode_mode.pcm_decode_mode);
							MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/	
						}
					}
					else
					{
						SoftMute(0); 
					} 
					MsgSetStream(stream_type,0); 
					SoftMute(0);
				}
				break;  
			case LINEAR_5_1_PCM_STREAM: /*0623*/
#ifdef _DEBUG_DSPDRV			  	
			  	debug_printf("\nLINEAR_5_1_PCM_STREAM");
#endif						
				HDMI_Stream_Channel=HdmiReadByte(5); /*��ȡ��ǰLPCMͨ����*/
#ifdef _DEBUG_DSPDRV			  	
				debug_printf("\nHDMI_Stream_Channel=%d",HDMI_Stream_Channel);
#endif	
				if(HDMI_Stream_Channel==1) //change by cjm 2009-7-10
				{
					stream_type=STREAM_PCM;
					PcmType=_PCM_5_1;
					SetDspPower(3); /*DSP �л���Analog ��*/	
					DSP_Input_Stream_Select(HDMI_Input); /*���뵽DSPԴ--HDMI(I2S)--�ŵ�DSP����������,����DSP����æ״̬*/
					temp_freq= HdmiReadByte(3); 
					//debug_printf("\ntemp_freq=%x",temp_freq);
					if(temp_freq!=0x0e) //����192Kʱ
					{
						MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/	
						MsgSetStream(stream_type,0);  
					}
					DelayMs(50); /*���������Ҫ��ʱ�µ�DSP �ȶ�����*/  
					DspChannelOut7_1(); /*��7.1��ͨ��*/
				}
				break;
			case LINEAR_7_1_PCM_STREAM: /*0823*/
#ifdef _DEBUG_DSPDRV			 
			  	debug_printf("\nLINEAR_7_1_PCM_STREAM\n");  
#endif						
				HDMI_Stream_Channel=HdmiReadByte(5); /*��ȡ��ǰLPCMͨ����*/
#ifdef _DEBUG_DSPDRV			 
				debug_printf("\nHDMI_Stream_Channel=%d",HDMI_Stream_Channel);
#endif					
				if(HDMI_Stream_Channel==1) //change by cjm 2009-7-10
				{
					stream_type=STREAM_PCM; 
					PcmType=_PCM_7_1;  	
					SetDspPower(3); /*DSP �л���Analog ��*/	  
					DSP_Input_Stream_Select(HDMI_Input); /*���뵽DSPԴ--HDMI(I2S)--�ŵ�DSP����������,����DSP����æ״̬*/	
					MsgSetStream(stream_type,0); 
					temp_freq= HdmiReadByte(3); 
					if(temp_freq!=0x0e)
					{
						MsgSetDSPSetting(ZONE1_ID,0); /*����DSPSETTING*/	
					}
					else
					{
						SoftMute(0);
					}
				} 
				break;
			  case HDCD_PCM_Detect_STREAM: /*��Ҫ����HDCD*/
			  case HDCD_PCM_Lost_STREAM:
#ifdef _DEBUG_DSPDRV
				debug_printf("\ndecode HDCD");
#endif
			  	break;
			  default:
			  	break;
			}
		}
	else
		{
		switch(last_stream)
			{
			case NEVER_REPORTED:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nNEVER_REPORTED");
#endif				
				break;
			case AC_3_DATA: /*AC-3 stream*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nAC3 stream");
#endif
				stream_type=STREAM_AC3; 
				DolbyType=_DOLBY_5CH;
				SetDspPower(1); /*DSP �л���spdif ��*/ 	  
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				
				{ 
					DigiSendByte(ADDR_ZONE1,0X00); /*���ְ��л���HDMI ���ϵ�Spdif ��*/
					DSP_Input_Stream_Select(AD_Input); /*���뵽DSPԴ--A/D CS8416*/
				}
				MsgSetStream(stream_type,DolbyType);
				softmute_flag=1;
				break;
			case MPEG1_LAYER1_DATA:		/*MPEG -1 layer 1 data*/
			case MPEG1_LAYER2_3_DATA:	/*MPEG -1 layer 2 or 3 data or MPEG-2 without extension*/
			case MPEG2_DATA:			/*MPEG -2 data with extension*/
			case MPEG2_AAC_ADTS_DATA:  /*MPEG -2 AAC ADTS data*/
			case MPEG2_LAYER1_DATA:		/*MPEG -1 layer 1 data Low sampling frequency*/
			case MPEG2_LAYER2_3_DATA:	/*MPEG -1 layer 2 or 3 data Low sampling frequency*/
				break;
			case DTS_1_DATA:			/*DTS-1 data (512-sample bursts))*/
			case DTS_2_DATA:			/*DTS-2 data (1024-sample bursts))*/
			case DTS_3_DATA:			/*DTS-3 data (2014-sample bursts))*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS_1 DTS_2 DTS_3");
#endif
				stream_type=STREAM_DTS;
				DtsType=_DTS;
				SetDspPower(1); /*DSP �л���spdif ��*/ 	
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{ 
					DigiSendByte(ADDR_ZONE1,0X00); /*���ְ��л���HDMI ���ϵ�Spdif ��*/
					DSP_Input_Stream_Select(AD_Input); /*���뵽DSPԴ--A/D CS8416*/
				}		
				MsgSetStream(stream_type,sys_flag.decode_mode.dts_type); 
				softmute_flag=1; 
				break;
			case MPEG2_AAC_ADTS:  
				break;
			case DOLBY_DIGITAL_PLUS:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDOLBY DIGITAL PLUS");
#endif				
				stream_type=STREAM_AC3;
				DolbyType=_DOLBY_PLUS;
				sys_flag.decode_mode.dolby_type=DolbyType;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))				{
				
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}				
				MsgSetStream(stream_type,DolbyType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/
				//DelayMs(300); 
				//DSP_AC3_DRC_INIT(); 
				break;
			case True_HD:
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDolby True HD");
#endif			
				stream_type=STREAM_AC3; 
				DolbyType=_DOLBY_TRUE_HD; //change by cjm 2009-2-9
				sys_flag.decode_mode.dolby_type=DolbyType;	
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DolbyType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/
				break;			
			case DTS_HD_MA_Stream_48k: /*0x0411 --DTS-HD 48K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD 48K");
#endif				    
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_MA_48K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/ 
				break;							
			case DTS_HD_MA_Stream_96k_192K: /*0x04411 --DTS-HD 96k*/									
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD 96K/192K");
#endif
				stream_type=STREAM_DTS;    
				DtsType=_DTS_HD_MA_96k_192K;    
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetStream(stream_type,DtsType); 
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/  
				break;			
			case DTS_HD_HRA_Stream_48K: /*0x0211 --DTS-HD HRA 48K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD HRA 48K"); 
#endif				
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_HRA_48K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}	
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/
				break;
			case DTS_HD_HRA_Stream_96k:/*0x4211--DTS-HD HRA 96K*/
#ifdef _DEBUG_DSPDRV
				debug_printf("\nDTS HD HRA 96K"); 
#endif							
				stream_type=STREAM_DTS;
				DtsType=_DTS_HD_HRA_96K;
			if((sys_flag.zone1_channel==INPUT_HDMI3)||(sys_flag.zone1_channel==INPUT_HDMI4)||(sys_flag.zone1_channel==INPUT_HDMI1)||(sys_flag.zone1_channel==INPUT_HDMI2))
				{
					SetDspPower(4); /*DSP �л���HDMI ��*/	
					DSP_Input_Stream_Select(HDMI_Input); 
				}
				MsgSetDSPSetting(ZONE1_ID,1); /*����DSPSETTING*/
				break;
			default: 
				break;			
			}
		}

#if 0
	if(sys_flag.headphone_flag == 1) /*����������,����TrueHD��Ҫ����DH2 Heap*/
		{
		if((stream_type==STREAM_AC3)&&(DolbyType==_DOLBY_TRUE_HD))
			{
#ifdef _DEBUG_DSPDRV			
			debug_printf("\nDH2 Mode -- TRUE HD");
#endif
			DH2_HEAP_MODE(1); /*Mode 1 only for Dolby TRUE HD*/
			}
		else  
			{
#ifdef _DEBUG_DSPDRV						
			debug_printf("\nDH2 Mode -- Other stream");
#endif			
			DH2_HEAP_MODE(1); /*Mode 0 for other Stream*/  
			}
		}	
#endif	  
}

/* 
 * @brief DSP ������Ϣ���� 
 *
 * @author chenjianming
 * @date 2007-10-18
 * @refer chenshiguang C11 code
 */
void DspProcUnsolicitedMsg(unsigned char *uCmdBuf)
{
	switch(uCmdBuf[3])
		{
		case AUTODETECT_RESPONSE:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nAutoDetect Response");
#endif
			ProcAutodetecResponse(uCmdBuf+4);
			break;
		case PLL_OUT_OF_CLK:
#ifdef _DEBUG_DSPDRV
			debug_printf("\nPLL OUT OF lock");
#endif
			break;
		default:
			break;
		}
}

/*
 * @brief Check DSP Return Message
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll code
 */
char CheckDspMessage(unsigned long message_value)
{
	signed long dwMsg;
	unsigned char ret;
	unsigned char k; 	

	ret=DspSpiReadNByte(dsp_buf,8); /*��DSP ������Ϣ*/
	ArrayToDWORD(&dwMsg, dsp_buf); /*�������͵���Ϣת��ΪWord ��*/
	
#ifdef _DEBUG_DSPDRV
	debug_printf("\nCheck Message Return\n");
	debug_printf("%02X\n",ret);
	for(k=0;k<8;k++)
	    debug_printf("%02X ",dsp_buf[k]);	 
	debug_printf("\n");
#endif  

	if(dwMsg != message_value)
		return FAILURE;

	return SUCCESS;
}

/*
 * @brief ��DSP ULD �̼�дֵ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer 
 */
void DSPFirmwareWrite(unsigned char wop,unsigned short index,signed long value)
{
	short i=0;
	
	dsp_buf[i++] = wop;					dsp_buf[i++] = 0x00;	
	dsp_buf[i++] = (index >> 8) & 0xFF;	dsp_buf[i++] = index & 0xFF;
	
	dsp_buf[i++] = (value >> 24) & 0xFF;	dsp_buf[i++] = (value >> 16) & 0xFF;	
	dsp_buf[i++] = (value >> 8) & 0xFF;	dsp_buf[i++] = value & 0xFF;

	DspSpiWriteNByte(dsp_buf,i);
}

/*
 * @brief ��ȡDSP Response Message
 *
 * @author chenjianming
 * @date 2007-10-19
 * @refer chenshiguang C11 code
 */
unsigned long ReadDsp(void)
{
	unsigned long rtn_value=0;
	unsigned char time_out_cnter=200;
	
#ifdef _DEBUG_DSPDRV
	unsigned char k;
#endif
	
	do{
		if(!SenseDspIrq())
			break;
		DelayMs(1);
		}while(--time_out_cnter); /*�ж�DSP_IRQ �Ƿ�Ϊ��*/

	if(time_out_cnter)
		{
		DspSpiReadNByte(dsp_buf,8);  /*��ȡ������Ϣ���ŵ�dsp_buf*/		
		
#ifdef _DEBUG_DSPDRV
		debug_printf("\n");
		for(k=0;k<8;k++)
	    		debug_printf("%02X ",dsp_buf[k]);	 
		debug_printf("\n");
#endif

		ArrayToHWORD(&rtn_value,dsp_buf+4); /*�ѷ��ص�data value ת������*/
		}
	else
		{
		rtn_value = 0xFFFFFFFF;
#ifdef _DEBUG_DSPDRV
		debug_printf("ReadDspc Time out\n");
#endif		
		}

	DelayMs(1);
	return rtn_value;
}

/* 
 * @brief ���ص�ǰ������Ƶ��
 *
 * @author chenjianming
 * @date 2007-10-17
 * @refer chenshiguang C11 code
 */
unsigned char GetNormalFreq(void)
{
	return CurrentSampleFreq;
}

/*
 * @brief ң������Late ��������ʵ�� 
 *
 * @author chenjianming
 * @date 2007-12-13
 * @refer chenshiguang C11 code
 */
void SetLate(unsigned char stream,unsigned char mode)
{
	unsigned long set_value;
	
	switch(mode)
		{
		case 0:	/*off*/
			set_value = 0x00;
			break;
		case 1:	/*half*/
			set_value = 0x3FFFFFFF;
			break;
		case 2:	/*full*/
			set_value = 0x7FFFFFFF;
			break;
		}

	if(dsp_power_flag) 
		{
		switch(stream)
			{
			case _DOLBY_2CH:
			case _DOLBY_5CH:
			case _DOLBY_EX:
			case _DOLBY_PLUS:
				DSP_AC3_COMPRESSION_CONTROL();/*Compression mode=Line out mode*/
				DSP_AC3_CUT_X(set_value);
				DSP_AC3_BOOST_Y(set_value);				
				break;
			case _DOLBY_TRUE_HD:
			case _DOLBY_TRUE_HD_2ch:
			case _DOLBY_TRUE_HD_5ch:
			case _DOLBY_TRUE_HD_6ch:
			case _DOLBY_TRUE_HD_7ch:
				DSP_TRUEHD_DRC_Control(); /*Force dynamic range control on*/
				DSP_TRUEHD_Cut_Scalefactor(set_value);
				DSP_TRUEHD_Boost_Scalefactor(set_value);
				break;
			default:
				break;
			}
		}
}

/*
 * @brief Master Boot Program
 *
 * @author chenjianming
 * @date 2008-06-17
 * @refer 49700x_System_Designers_Guide p23
 */
uchar  dsp_repower_flag = 0; // jian add in 10-10-13
char DspMasterBoot(void)
{
#ifdef _DEBUG_DSPDRV
	debug_printf("\nDsp Master Boot");
#endif

	DSPHwReset(0);	/*Hardware reset*/
	do{
		if(!SenseDspIrq())
			break;
#ifdef _DEBUG_DSPDRV		
		else
			debug_printf("\nDSP IRQ is Still hight)");
#endif		
		}while(1); /*�ȴ�IRQ�õ�*/

	if(CheckDspMessage(FLASH_VERIFIED) == FAILURE) /*FLASH_IMAGE_VERIFIED=0xEF000020 00000001*/
		{
#ifdef _DEBUG_DSPDRV
		debug_printf("\ncheck FLASH IMAGE VERIFIED error\n");
#endif
		return FAILURE;
		}	

	DelayMs(10);
	dsp_repower_flag = 1;
	return SUCCESS;
}

