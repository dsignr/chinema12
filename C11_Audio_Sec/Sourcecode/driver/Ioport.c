#include "mcudef.h"
#include "ioport.h"
#include "ir.h"
//#include "ic4094.h"
#include "..\main\debug.h"
#include "..\main\Macro.h"
#include "..\driver\timer.h"
#include "..\driver\Volume.h"
#include "..\driver\Ioport.h" 

#define _USE_SET_IOPORT

/*
 * @brief  �����ĸ���������:
 *		1.����Port0 ��Port1 �ĵ�ƽֵ.
 *		2.��Port0 ��Port1 �ĵ�ƽֵ.
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang Cll Code
 */
#ifdef _USE_SET_IOPORT
void SetIoport0(unsigned long pin, int high)
{
	if(high) { IOSET0 = pin; } else { IOCLR0 = pin; }
}

int SensePort0(unsigned long pin)
{
	return((IOPIN0 & pin)?1:0);
}

void SetIoport1(unsigned long pin, int high)
{
	if(high) { IOSET1 = pin; } else { IOCLR1 = pin; }
}

int SensePort1(unsigned long pin)
{
	return((IOPIN1 & pin)?1:0);
}

void SetIoport2(unsigned long pin, int high)
{
	if(high) {FIO2SET=pin;} else{FIO2CLR=pin;}
}

int SensePort2(unsigned long pin)
{
	return((FIO2PIN & pin)?1:0);
}

void SetIoport3(unsigned long pin, int high)
{
	if(high) {FIO3SET=pin;} else{FIO3CLR=pin;}
}

int SensePort3(unsigned long pin)
{
	return((FIO3PIN & pin)?1:0);
}

void SetIoport4(unsigned long pin, int high)
{
	if(high) {FIO4SET=pin;} else{FIO4CLR=pin;}
}

int SensePort4(unsigned long pin)
{
	return((FIO4PIN & pin)?1:0);
}
#endif

/***************************  Pow and Pow Led *********************************/
/*
 * @brief control the pow switch
 *
 * @author chenjianming
 * @date 2007-12-22
 * @refer 
 */
void SystemPower(unsigned char mode)
{
	if(mode==ON)
		IOCLR0=Pow_Pin;		
	else
		IOSET0=Pow_Pin;
}

/*
 * @brief control the pow led
 *
 * @author chenjianming
 * @date 2007-12-22
 * @refer 
 */
void PowerLed(unsigned char mode)
{
	if(mode==ON)
		IOSET0=Pow_Led_Pin;
	else
		IOCLR0=Pow_Led_Pin;
}

/*
 * @brief LedStandby-�ϵ��Դ�ȶ�
 *
 * @author chenjianming
 * @date 2007-12-22
 * @refer 
 */
void LedStandby(void)
{
	PowerLed(ON);
	DelayMs(1000);
	PowerLed(OFF);
	DelayMs(1000);
	PowerLed(ON);
	DelayMs(1000);
	PowerLed(OFF);
	DelayMs(1000);
	PowerLed(ON);
	DelayMs(1000);
}

/*
 * @brief init power and power led Pin
 *
 * @author chenjianming
 * @date 2007-12-22
 * @refer 
 */
void InitPower(void)
{
	IODIR0 |=Pow_Pin|Pow_Led_Pin; /*OUTPUT*/
}
/***************************  Pow and Pow Led *********************************/

/*************************** HD Radio Power ***********************************/
/*
 * @brief control HD Radio Power
 *
 * @author chenjianming
 * @date 2008-08-06
 * @refer 
 */
void HDRadioPower(unsigned char mode)
{
	if(mode==ON)
		IOSET1=HD_Power;
	else
		IOCLR1=HD_Power;
}

/*
 * @brief init HD Raido Power Pin
 *
 * @author chenjianming
 * @date 2008-08-06
 * @refer 
 */
void InitHDRaidoPower(void)
{
	IODIR1 |=HD_Power; 
}
/*************************** HD Radio Power ***********************************/


/****************************System Reset ************************************/
/*
 * @brief ����RESET_PIN �ĵ�ƽ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void SetResetPin(int high)
{
	if (high) 
		FIO2SET=RESET_PIN;
	else 
		FIO2CLR=RESET_PIN;
}

/*
 * @brief ����RESET_PIN ��I/O �ڷ���
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void InitResetPin(void) 
{
	FIO2DIR |=RESET_PIN;
}

/*
 * @brief System Reset (DAC\CS8416\Flash)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer 
 */
void SystemReset(void)
{
	SetResetPin(0); 
	DelayMs(10);	
	SetResetPin(1);
	DelayMs(10);
}
/******************************* Reset ***************************************/

/******************************  DAC  ****************************************/
/*
 * @brief  �����ĸ������������� DAC �е�Reset\MD\CS\MC ���ŵ�ƽ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 code
 */
 #ifndef _USE_DEFINED_IO
//void SetDacReset(int high)
//{
//#ifdef _USE_SET_IOPORT
//	SetIoport0(DAC_Reset,high);
//#else
//	if(high) { IOSET0 = DAC_Reset; } else { IOCLR0 = DAC_Reset; }
//#endif
//}
void SetDacMd(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport0(DAC_MD ,high);
#else
	if(high) { IOSET0 = DAC_MD; } else { IOCLR0 = DAC_MD; }
#endif
}
 void SetDacMc(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport2(DAC_MC ,high);
#else
	if(high) { FIO2SET = DAC_MC; } else { FIO2CLR = DAC_MC; }
#endif
}
void SetDacCs4(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport0(DAC_CS4 ,high);
#else
	if(high) { IOSET0 = DAC_CS4; } else { IOCLR0 = DAC_CS4; }
#endif
}

void SetDacCs3(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport4(DAC_CS3,high);
#else
	if(high) { FIO4SET = DAC_CS3; } else { FIO4CLR = DAC_CS3; }
#endif
}

void SetDacCs2(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport4(DAC_CS2 ,high);
#else
	if(high) { FIO4SET = DAC_CS2; } else { FIO4CLR = DAC_CS2; }
#endif
}

void SetDacCs1(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport1(DAC_CS1,high);
#else
	if(high) { IOSET1 = DAC_CS1; } else { IOCLR1 = DAC_CS1; }
#endif
}

void SetDacCs0(int high)
{ 
#ifdef _USE_SET_IOPORT
	SetIoport1(DAC_CS0,high);
#else
	if(high) { IOSET1 = DAC_CS0; } else { IOCLR1 = DAC_CS0; }
#endif
}

void SetCH4RELAY(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport0(CH4_RELAY ,high);
#else
	if(high) { IOSET0 = CH4_RELAY; } else { IOCLR0 = CH4_RELAY; }
#endif
}

void SetCH3RELAY(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport0(CH3_RELAY ,high);
#else
	if(high) { IOSET0 = CH3_RELAY; } else { IOCLR0 = CH3_RELAY; }
#endif
}

void SetCH2RELAY(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport0(CH2_RELAY ,high);
#else
	if(high) { IOSET0 = CH2_RELAY; } else { IOCLR0 = CH2_RELAY; }
#endif
}

void SetCH1RELAY(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport0(CH1_RELAY ,high);
#else
	if(high) { IOSET0 = CH1_RELAY; } else { IOCLR0 = CH1_RELAY; }
#endif
}
#endif

/*
 * @brief  init DAC pin (����DAC���ŵ�I/O�ڷ���)
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 code
 */
void InitDac()
{
	IODIR0 |= DAC_MD|DAC_CS4|CH4_RELAY|CH3_RELAY|CH2_RELAY|CH1_RELAY; /*OUTPUT*/
	FIO2DIR |=DAC_MC;
	FIO4DIR |=DAC_CS3|DAC_CS2;	
	IODIR1 |= DAC_CS1|DAC_CS0;	
	
	SetDacMc(0);
	SetDacMd(0);
	SetDacCs4(0);
	SetDacCs3(0);
	SetDacCs2(0);
	SetDacCs1(0);
}
/******************************  DAC  ******************************************/

/*****************************  MAS9116  ***************************************/
#ifndef _USE_DEFINED_IO
void SetMas9116Clk(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport1(MAS9116_CLK ,high);
#else
	if(high) { IOSET1 = MAS9116_CLK; } else { IOCLR1 = MAS9116_CLK; }
#endif
}

void SetMas9116Data(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport0(MAS9116_DATA ,high);
#else
	if(high) { IOSET0 = MAS9116_DATA; } else { IOCLR0 = MAS9116_DATA; }
#endif
}
#endif

void SetMas9116Cs0(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs1;
	else
		IOCLR1=Mas9116_Cs1;
}

void SetMas9116Cs1(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs2;
	else
		IOCLR1=Mas9116_Cs2;
}

void SetMas9116Cs2(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs3;
	else
		IOCLR1=Mas9116_Cs3;
}

void SetMas9116Cs3(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs4;
	else
		IOCLR1=Mas9116_Cs4;
}

void SetMas9116Cs4(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs5;
	else
		IOCLR1=Mas9116_Cs5;
}

void SetMas9116Cs5(unsigned char chip_select)
{
	if(chip_select==1)
		IOSET1=Mas9116_Cs6;
	else
		IOCLR1=Mas9116_Cs6;
}

void SelectMas9116(unsigned char choose)
{
	switch(choose) /*Mas9116 Ƭѡ�õ�,��ʼд����*/
		{
			case 0: //2Channel Input Volume
				//debug_printf("\n2Channel Input Volume Cs\n");
				SetMas9116Cs0(0);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(1); 
				SetMas9116Cs3(1);
				SetMas9116Cs4(1);
				SetMas9116Cs5(1);		
				break;
			case 1: //Zone2 Output Volume
				//debug_printf("\n2Channel Input Volume Cs\n");
				SetMas9116Cs0(1);
				SetMas9116Cs1(0);
				SetMas9116Cs2(1); 
				SetMas9116Cs3(1);
				SetMas9116Cs4(1);
				SetMas9116Cs5(1);
				break;
			case 2: //7CH F/R
				//debug_printf("\n7CH F/R Volume Cs\n");
				SetMas9116Cs0(1);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(0);
				SetMas9116Cs3(1);
				SetMas9116Cs4(1);
				SetMas9116Cs5(1);
				break;
			case 3://7CH SF/SR
				//debug_printf("\n7CH SF/SR Volume Cs\n");				
				SetMas9116Cs0(1);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(1); 
				SetMas9116Cs3(0);
				SetMas9116Cs4(1);
				SetMas9116Cs5(1);
				break;
			case 4://7CH CEN/SW
				//debug_printf("\n7CH CEN/SW Volume Cs\n");
				SetMas9116Cs0(1);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(1); 
				SetMas9116Cs3(1);
				SetMas9116Cs4(0);	
				SetMas9116Cs5(1);
				break;
			case 5://7CH SBR/SBL
				//debug_printf("\n7CH SBR/SBL Volume Cs\n");
				SetMas9116Cs0(1);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(1); 
				SetMas9116Cs3(1);
				SetMas9116Cs4(1);
				SetMas9116Cs5(0);
				break;
			case 0xff:
			default:				
				//debug_printf("\nRelease All Cs\n");	
				SetMas9116Cs0(1);
				SetMas9116Cs1(1); 
				SetMas9116Cs2(1); 
				SetMas9116Cs3(1);
				SetMas9116Cs4(1);
				SetMas9116Cs5(1);
				break;
		}
}

void InitMas9116(void)
{
	IODIR0 |= MAS9116_DATA; /*OUTPUT*/
	IODIR1 |= MAS9116_CLK; /*OUTPUT*/ /*Mas9116 Mute Init in the InitMuteControl()*/
 
	IODIR1 |=Mas9116_Cs1|Mas9116_Cs2|Mas9116_Cs3|Mas9116_Cs4|Mas9116_Cs5|Mas9116_Cs6; /*OUTPUT*/
	
	SetMas9116Clk(0);
	SetMas9116Data(0);
}
/*****************************  MAS9116  ***************************************/

/*****************************  MAX335  ****************************************/
void SetMax335Data(int high)
{
	if (high) 
		FIO3SET =MAX335_DATA; 
	else 
		FIO3CLR =MAX335_DATA;
}

void SetMax335Clk(int high)
{
	if (high) 
		IOSET0 = MAX335_CLK; 
	else 
		IOCLR0 = MAX335_CLK; 
}

void SetMax335Cs1(unsigned char select) //cs7
{
	if(select==1)
		IOSET1=Max335_Cs1;
	else
		IOCLR1=Max335_Cs1;
}

void SetMax335Cs2(unsigned char select) //cs6
{
	if(select==1)
		IOSET1=Max335_Cs2;
	else
		IOCLR1=Max335_Cs2;		
}

void SetMax335Cs3(unsigned char select) //cs5
{
	if(select==1)
		IOSET1=Max335_Cs3;
	else
		IOCLR1=Max335_Cs3;
}

void SetMax335Cs4(unsigned char select) //cs4
{
	if(select==1)
		IOSET1=Max335_Cs4;
	else
		IOCLR1=Max335_Cs4;
}

void SetMax335Cs5(unsigned char select) //cs3
{
	if(select==1)
		IOSET0=Max335_Cs5;
	else
		IOCLR0=Max335_Cs5;
}

void SetMax335Cs6(unsigned char select) //cs2
{
	if(select==1)
		IOSET0=Max335_Cs6;
	else
		IOCLR0=Max335_Cs6;
}

void SetMax335Cs7(unsigned char select) //cs1
{
	if(select==1)
		IOSET0=Max335_Cs7;
	else
		IOCLR0=Max335_Cs7;
}

void SetMax335Cs8(unsigned char select) //cs8
{
	if(select==1)
		IOSET0=Max335_Cs8;
	else
		IOCLR0=Max335_Cs8;
}

void SelectMax335(unsigned char choose)
{
	SetMax335Cs1(1); /*Max335 ȫ��Ƭѡ�ø�*/
	SetMax335Cs2(1);
	SetMax335Cs3(1);
	SetMax335Cs4(1);
	SetMax335Cs5(1);
	SetMax335Cs6(1);
	SetMax335Cs7(1);
	SetMax335Cs8(1);

	switch(choose)	/*Max335 Ƭѡ�õ�,����д����*/
		{
		case 1:
			SetMax335Cs1(0);
			break;
		case 2:
			SetMax335Cs2(0);
			break;
		case 3:
			SetMax335Cs3(0);
			break;
		case 4:
			SetMax335Cs4(0);
			break;
		case 5:
			SetMax335Cs5(0);
			break;
		case 6:
			SetMax335Cs6(0);
			break;
		case 7:
			SetMax335Cs7(0);
			break;
		case 8:
			SetMax335Cs8(0);
			break;
		default:
			break;
		}
}

void InitMAS335()
{
	IODIR0 |= MAX335_CLK;	/*OUTPUT*/
	FIO3DIR |=MAX335_DATA;	/*OUTPUT*/

	IODIR0 |=Max335_Cs5 |Max335_Cs6|Max335_Cs7|Max335_Cs8; /*OUTPUT*/
	IODIR1 |=Max335_Cs1 |Max335_Cs2|Max335_Cs3|Max335_Cs4; /*OUTPUT*/
	
	SetMax335Clk(0);
	SetMax335Data(0);
}
/*****************************  MAX335  ****************************************/

/****************************Mute Control ***************************************/
void Set7CH1Mute(unsigned char mode)
{
	if(mode==UnMute) 
		IOSET1=ch71_Mute;
	else
		IOCLR1=ch71_Mute;
}

void SetZone2Mute(unsigned char mode)
{
	if(mode==UnMute)
		IOSET0=Zone2_Mute;
	else
		IOCLR0=Zone2_Mute;
}

void SetBalance_Mute(unsigned char mode)
{
	if(mode==UnMute)
		IOSET1=Balance_Mute;
	else
		IOCLR1=Balance_Mute;
}

void SetHeadphoneMute(unsigned char mode)
{
	if(mode==UnMute)
		IOSET0=HP_Mute;
	else
		IOCLR0=HP_Mute;
}

void Zone2Mute(unsigned char mode)
{
	if(mode == ON)
		{
		SetZone2Mute(Mute);	/*Mute on*/
		}
	else
		{
		SetZone2Mute(UnMute);/*Mute off*/
		}
}

void InitMuteControl(void)
{
	IODIR0 |=Zone2_Mute;	/*OUTPUT*/
	IODIR1 |=Balance_Mute|ch71_Mute;/*OUTPUT*/
 	IODIR0 |=HP_Mute;/*OUTPUT*/
	
	Set7CH1Mute(Mute);
	SetZone2Mute(Mute);
	SetBalance_Mute(Mute);
	SetHeadphoneMute(Mute);
}

void PowerOffMuteControl(void)
{
	Set7CH1Mute(Mute);
	SetBalance_Mute(Mute);
	SetHeadphoneMute(Mute);
}
/****************************Mute Control ***************************************/

/*******************************  165  ******************************************/
#ifndef _USE_DEFINED_IO
void set_ic165_ld(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport1(IC165_LD ,high);
#else
	if(high) { IOSET1 = IC165_LD; } else { IOCLR1 = IC165_LD; }
#endif
}

void set_ic165_clk(int high)
{
#ifdef _USE_SET_IOPORT
	SetIoport1(IC165_CLK ,high);
#else
	if(high) { IOSET1 = IC165_CLK; } else { IOCLR1 = IC165_CLK; }
#endif
}

int sense_ic165_data()
{
#ifdef _USE_SET_IOPORT
	return SensePort1(IC165_DATA);
#else
	int i;
	i = (IOPIN1 & IC165_DATA)?1:0;
	return i;
#endif
}
#endif

void Init165()
{
	IODIR1 |= IC165_CLK;	/*OUTPUT*/
	IODIR1 |= IC165_LD;	/*OUTPUT*/	
	
	//IODIR1 &= (~IC165_DATA);	/*INPUT*/
	//set_ic165_clk(0);
	//set_ic165_ld(0);
}
/*******************************  165  ******************************************/

/****************************  encoder switch  ***********************************/
int SenseEncoderSwitchPlus()
{
	return ((IOPIN0 & ENCODER_SWITCH_PLUS)?1:0);
}

int SenseEncoderSwitchMinus()
{
	return ((IOPIN1 & ENCODER_SWITCH_MINUS)?1:0);
}

void InitEncoderSwitch()
{
	IODIR1 &= (~ENCODER_SWITCH_PLUS);	/*INPUT*/
	IODIR0 &= (~ENCODER_SWITCH_MINUS);	/*INPUT*/
}
/****************************  encoder switch  ***********************************/

/***************************  Input Port Select ***********************************/
/*
 * @brief �������뵽DSP��������AD��CS8416����Դ����HDMI����Դ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer 
 */
void DSP_Input_Stream_Select(unsigned char select)
{
	if(select==HDMI_Input)
		FIO2SET=DSP_Input_Select;
	else
		FIO2CLR=DSP_Input_Select;
}

void Init_Dsp_Input_Select(void)
{
	FIO2DIR |=DSP_Input_Select;  /*OUTPUT*/
}
/***************************  Input Port Select ***********************************/

/***************************  DSD Select ***************************************/
/*
 * @brief DSD �˿���ѡ��
 *
 * @author chenjianming
 * @date 2008-08-06
 * @refer 
 */

 extern unsigned char CD_8416_channel ;
void DSD_Select(unsigned char mode) /*OFF  hdmi1   ON DSD*/
{
	if(mode==OFF)
		{
		FIO2SET=DSD_Selsect;
		FIO2SET=HDMI_Selsect;
		FIO2SET=CD_DSP_Selsect;
		}
	else
		{
		CD_8416_channel = 0;
		FIO2CLR=HDMI_Selsect;
		FIO2SET=DSD_Selsect;
		FIO2SET=CD_DSP_Selsect;
		}
}


void DSP_Select(unsigned char mode) /*OFF  hdmi1   ON DSD*/
{
	if(mode==ON)
		{
		CD_8416_channel = 0;
		FIO2SET=HDMI_Selsect;
		FIO2SET=CD_DSP_Selsect;
		FIO2CLR=DSD_Selsect;
		}
	else
		{
		FIO2SET=DSD_Selsect;
		FIO2SET=HDMI_Selsect;
		FIO2SET=CD_DSP_Selsect;
		}
}

/*JAIN ADD IN 12-02-17**/
void cd_dsp_channle_Select(unsigned char mode)
{
	if(mode==ON)
		{
		CD_8416_channel = 1;
		FIO2CLR=CD_DSP_Selsect;
		FIO2SET=DSD_Selsect;
		FIO2SET=HDMI_Selsect;
		}
	else
		{
		CD_8416_channel = 0;
		FIO2SET=DSD_Selsect;
		FIO2SET=HDMI_Selsect;
		FIO2SET=CD_DSP_Selsect;

		}

}

void Init_DSD_Select(void)
{
	FIO2DIR |=DSD_Selsect|HDMI_Selsect|CD_DSP_Selsect;  /*OUTPUT*/
}
/***************************  DSD Select ***************************************/

/****************************** Analog Signal Detect  *****************************/
/*
 * @brief ģ��ͨ���źż��
 *
 * @author chenjianming
 * @date 2007-12-26
 * @refer 
 */
unsigned char SenseAnalogSignal(void)
{
	return((IOPIN0 & Analog_Signal_Detect)?1:0);
}
/****************************** Analog Signal Detect  *****************************/

/****************************** IEC4094 ***************************************/
void Set4094Cs(int high)
{
	if (high) 
		IOSET0 = CS_4094;  
	else 
		IOCLR0 = CS_4094; 
}

void Set4094OE(int high)
{
	if(high)
		FIO3SET=OE_4094;
	else
		FIO3CLR=OE_4094;
}

void SetData4094(int high)
{
	if (high) 
		FIO3SET = DATA_4094; 
	else 
		FIO3CLR = DATA_4094;
}

void Set4094Clk(int high)
{
	if (high) 
		IOSET0 = CLK_4094; 
	else 
		IOCLR0 = CLK_4094; 
}

void Init4094()
{
	IODIR0 |= CLK_4094 | CS_4094;	/*OUTPUT*/
	FIO3DIR |= OE_4094 | DATA_4094;	/*OUTPUT*/
}
/****************************** IEC4094 ***************************************/

/*********************************  DSP  ***************************************/
/*
 * @brief ����DSP ��λ���ŵ�ƽ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void SetDspReset(int high)
{
	if(high) 
		FIO2SET =DSP_RST;
	else 
		FIO2CLR =DSP_RST;
}

/*
 * @brief ����DSP ��λ����I/O �ڷ���
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void InitDspResetPin()
{
	FIO2DIR |=DSP_RST; /*OUTPUT*/
}

/*
 * @brief DSP hardware reset
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void DSPHwReset(unsigned char mode)
{
	switch(mode)
		{
		case 1:
			SetDspReset(0);
			break;
		case 0:
		default:
			SetDspReset(0);
			//Delay_SPI(1);
			DelayMs(10);
			SetDspReset(1);
			break;
		}
}

/*
 * @brief ���DSP_IRQ ����,���Ϊ�������DSP have data to be read
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet
 */
unsigned char SenseDspIrq(void)
{
	//IODIR0 &= (~DSP_IRQ);	//INPUT//Ĭ����Ϊ����	
#ifdef _USE_SET_IOPORT
	return SensePort0(DSP_IRQ);
#else
	return((IOPIN0 & DSP_IRQ)?1:0);
#endif
}

/*
 * @brief ���DSP_BSY����,���Ϊ�������it cannot receive any more data
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer cs4953xx_datasheet
 */
unsigned char SenseDspBusy(void)
{
	//IODIR0 &= (~DSP_BUSY);  //INPUT//Ĭ����Ϊ����	
#ifdef _USE_SET_IOPORT
	return SensePort0(DSP_BUSY);
#else
	return((IOPIN0 & DSP_BUSY)?1:0);
#endif
}

/*
 * @brief ���º�������DSP SPIͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2008-05-27
 * @refer 
 */
void Set_Spi_Cs(int high)
{
	if(high)
		FIO2SET = SPI_CS;		/*P2.8*/
	else
		FIO2CLR = SPI_CS;		/*P2.8*/
}

void Set_Spi_Clk(int high)
{
	if (high)
		IOSET0 = SPI_CLK;		/*P0.17*/
	else
		IOCLR0 = SPI_CLK;		/*P0.17*/
}

void Set_Spi_MOSI(int high)
{
	if(high)
		FIO2SET = SPI_MOSI;		/*P2.7*/
	else
		FIO2CLR = SPI_MOSI;		/*P2.7*/
}

unsigned char Sense_Spi_MISO(void)
{
	//FIO2DIR &= (~SPI_MISO);	/*Set input Ĭ����Ϊ����*/  
	return((FIO2PIN & SPI_MISO)?1:0);
}

void Init_Dsp_Spi_Pin(void)
{
	FIO2DIR |= SPI_CS; /*Output*/
	IODIR0 |= SPI_CLK; 	
	FIO2DIR |= SPI_MOSI;
	//FIO2DIR &= (~SPI_MISO); /*Set input*/      

	Set_Spi_Cs(1);
	Set_Spi_Clk(0);
	Set_Spi_MOSI(0);
}   

#if 0
/*
 * @brief �������ĸ���������DSP I2CͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void set_sda_dsp(int high)
{
	if (high)
		{
		FIO2SET =I2C_SDA_DSP; 		/*P2.9*/
		FIO2DIR &=(~I2C_SDA_DSP);	/*��������,�������ø�0-input,1-output*/
		}
	else
		{
		FIO2CLR =I2C_SDA_DSP; 		/*P2.9*/
		FIO2DIR |= I2C_SDA_DSP;		/*�������,ǿ������ 0-input,1-output*/
		}
}

void set_scl_dsp(int high)
{
	if (high)
		{
		IOSET0 = I2C_SCL_DSP;		/*P0.17*/
		IODIR0 &= (~I2C_SCL_DSP);	/*��������,�������ø� 0-input,1-output */
		}
	else
		{
		IOCLR0 = I2C_SCL_DSP;		/*P0.17*/
		IODIR0 |= I2C_SCL_DSP;		/*�������,ǿ������ 0-input,1-output*/	
		}
}

int sense_sda_dsp(void)			/*P2.9*/
{
	int i;
	
	unsigned int port_shadow;
	port_shadow = FIO2DIR; 

#if CSG_C11_Bug
//	IODIR1 &= (~I2C_SDA_DSP);	//Set input
#endif

	i = (FIO2PIN & I2C_SDA_DSP)?1:0;
	FIO2DIR = port_shadow;
	return i;
}

int sense_scl_dsp(void)			/*P0.17*/
{
	int i;
	
	unsigned int port_shadow;
	port_shadow = IODIR0;
	
#if CSG_C11_Bug
//	IODIR0 &= (~I2C_SCL_DSP);	//Set input
#endif

	i = (IOPIN0 & I2C_SCL_DSP)?1:0;
	IODIR0 = port_shadow;
	return i;
}
#endif

#if 0
/*********************************  DSP  **************************************/

/*********************************I2C  CS8416*********************************/
/*
 * @brief �������ĸ���������VFD I2CͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 Code
 */
void set_sda(int high)
{
	if (high)
		{
		FIO2SET =I2C_SDA;
		FIO2DIR &= (~I2C_SDA); /*��������,�������ø�*/
		}
	else
		{
		FIO2CLR =I2C_SDA;
		FIO2DIR|= I2C_SDA;	/*�������,ǿ������*/
		}
}

void set_scl(int high)
{
	if (high)
		{
		FIO2SET = I2C_SCL;
		FIO2DIR &= (~I2C_SCL);	/*��������,�������ø�*/
		}
	else
		{
		FIO2CLR =I2C_SCL;
		FIO2DIR |= I2C_SCL;	/*�������,ǿ������*/
		}
}

int sense_sda(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow =FIO2DIR;
	
#if CSG_C11_Bug
	//FIO2DIR &= (~I2C_SDA);	/*Set input*/
#endif

	i= (FIO2PIN & I2C_SDA)?1:0;
	FIO2DIR=port_shadow;
	return i;
}

int sense_scl(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow=FIO2DIR;
	
#if CSG_C11_Bug
	//FIO2DIR &= (~I2C_SCL);	/*Set input*/
#endif

	i=(FIO2PIN & I2C_SCL)?1:0;
	FIO2DIR=port_shadow;
	return i;
}
#endif
/*********************************I2C (CS8416)********************************/

/*********************************I2C  VFD************************************/
/*
 * @brief �������ĸ���������VFD I2CͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-11-26
 * @refer chenshiguang C11 Code
 */

#if 1
void set_vfd_sda(int high)
{
	if (high)
		{
		IOSET1 =I2C_SDA_VFD;
		IODIR1 &= (~I2C_SDA_VFD); /*��������,�������ø�*/
		}
	else
		{
		IOCLR1  =I2C_SDA_VFD;
		IODIR1|= I2C_SDA_VFD;	/*�������,ǿ������*/
		}
}

void set_vfd_scl(int high)
{
	if (high)
		{
		IOSET1 = I2C_SCL_VFD;
		IODIR1 &= (~I2C_SCL_VFD);	/*��������,�������ø�*/
		}
	else
		{
		IOCLR1 =I2C_SCL_VFD;
		IODIR1 |= I2C_SCL_VFD;	/*�������,ǿ������*/
		}
}

int sense_vfd_sda(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow =IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &= (~I2C_SDA_VFD);	/*Set input*/
#endif

	i= (IOPIN1 & I2C_SDA_VFD)?1:0;
	IODIR1=port_shadow;
	return i;
}

int sense_vfd_scl(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow=IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &= (~I2C_SCL_VFD);	/*Set input*/
#endif

	i=(IOPIN1 & I2C_SCL_VFD)?1:0;
	IODIR1=port_shadow;
	return i;
}
#else
void set_vfd_sda(int high)
{
	set_sda(high);
}

void set_vfd_scl(int high)
{
	set_scl(high);  /*jian change it in 12-01-30*/
}

int sense_vfd_sda(void)
{
	return (sense_sda());	/*jian change it in 12-01-30*/
}

int sense_vfd_scl(void)
{
	return (sense_scl());	/*jian change it in 12-01-30*/
}
#endif
/*********************************I2C  VFD************************************/

/*********************************I2C (EEPROM)********************************/
/*
 * @brief �������ĸ���������EEPROM I2CͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void set_sda1(int high)
{
	if (high)
		{
		IOSET1 = I2C_SDA1;
		IODIR1 &= (~I2C_SDA1);	/*��������,�������ø�*/
		}
	else
		{
		IOCLR1 = I2C_SDA1;
		IODIR1 |= I2C_SDA1;	/*�������,ǿ������*/
		}
}

void set_scl1(int high)
{
	if (high)
		{
		IOSET1 = I2C_SCL1;
		IODIR1 &= (~I2C_SCL1);	/*��������,�������ø�*/
		}
	else
		{
		IOCLR1 = I2C_SCL1;
		IODIR1 |= I2C_SCL1;	/*�������,ǿ������*/
		}
}

int sense_sda1()
{
	int i;
	unsigned int port_shadow;
	port_shadow = IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &= (~I2C_SDA1);	/*Set input*/
#endif

	i = (IOPIN1 & I2C_SDA1)?1:0;
	IODIR1 = port_shadow;
	return i;
}

int sense_scl1()
{
	int i;
	unsigned int port_shadow;
	port_shadow = IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &= (~I2C_SCL);	/*Set input*/
#endif

	i = (IOPIN1 & I2C_SCL1)?1:0;
	IODIR1 = port_shadow;
	return i;
}
/*********************************I2C (EEPROM)********************************/

/*********************************I2C (LPC922)********************************/
/*
 * @brief �������ĸ���������LPC922 I2CͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-12-22
 * @refer 
 */
void set_sda2(int high)
{
	if(high)
		{
		IOSET1 =I2C_SDA2;
		IODIR1 &=(~I2C_SDA2); /*��������,�������ø�*/
		}
	else
		{
		IOCLR1 =I2C_SDA2;
		IODIR1 |=I2C_SDA2; /*�������,ǿ������*/
		}
}

void set_scl2(int high)
{
	if(high)
		{
		IOSET1 =I2C_SCL2;
		IODIR1 &=(~I2C_SCL2);
		}
	else
		{
		IOCLR1 =I2C_SCL2;
		IODIR1 |=I2C_SCL2;
		}
}

int sense_sda2()
{
	int i;
	unsigned int port_shadow;
	port_shadow= IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &=(~I2C_SDA2); /*Set input*/
#endif

	i=(IOPIN1 & I2C_SDA2)?1:0;
	IODIR1=port_shadow;
	return i;
}

int sense_scl2()
{
	int i;
	unsigned int port_shadow;
	port_shadow=IODIR1;
	
#if CSG_C11_Bug
	//IODIR1 &=(~I2C_SCL2); /*Set input*/
#endif

	i=(IOPIN1 & I2C_SCL2)?1:0;
	IODIR1=port_shadow;
	return i;
}
/*********************************I2C (LPC922)********************************/

/******************************** I2C (HDMI) *********************************/
/*
 * @brief �������ĸ��������ڸ�HDMI (LPC2364)ͨѶ�ĵ�ƽ���ú͵�ƽ��ȡ
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer 
 */
void set_sda_hdmi(int high)
{
	if(high)
		{
		IOSET0  = HDMI_SDA;
		IODIR0  &=(~HDMI_SDA); /*��������,�������ø�*/
		}
	else
		{
		IOCLR0  =HDMI_SDA;
		IODIR0  |=HDMI_SDA; /*�������,ǿ������*/
		}
}

void set_scl_hdmi(int high)
{
	if(high)
		{
		IOSET0 =HDMI_SCL;
		IODIR0 &=(~HDMI_SCL); /*��������,�������ø�*/
		}
	else
		{
		IOCLR0 =HDMI_SCL;
		IODIR0 |=HDMI_SCL; /*�������,ǿ������*/		
		}
}

int sense_sda_hdmi(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow =IODIR0;
	
#if CSG_C11_Bug
	//IODIR0 &= (~HDMI_SDA); /*Set input*/
#endif

	i= (IOPIN0 & HDMI_SDA)?1:0;
	IODIR0=port_shadow;
	return i;
}

int sense_scl_hdmi(void)
{
	int i;
	unsigned int port_shadow;
	port_shadow=IODIR0;
	
#if CSG_C11_Bug
	//IODIR0 &= (~HDMI_SCL);	 /*Set input*/
#endif

	i=(IOPIN0 & HDMI_SCL)?1:0;
	IODIR0=port_shadow;
	return i;
}
/******************************** I2C (HDMI) *********************************/

/*********************************All I2C Init**********************************/
/*
 * @brief I2C pin init
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void I2cPinInit()
{
	//DSP I2C Pin init  //Dele by cjm 2008-05-27 (becasue use SPI)
	//FIO2DIR|= I2C_SDA_DSP; /*OUTPUT*/
	//IODIR0 |= I2C_SCL_DSP; 	
	//set_sda_dsp(0);
	//set_scl_dsp(1);
	//set_sda_dsp(1);	/*��ʼ����֮����һ��STOP*/

	//EEPROM I2C Pin init
	IODIR1 |= I2C_SDA1|I2C_SCL1; /*OUTPUT*/
	set_sda1(0);
	set_scl1(1);
	set_sda1(1);	/*��ʼ����֮����һ��STOP*/	
#if 0
	//CS8416 I2C Pin init
	FIO2DIR |=I2C_SDA|I2C_SCL; /*OUTPUT*/
	set_sda(0);
	set_scl(1);  
	set_sda(1);	/*��ʼ����֮����һ��STOP*/


#else
	//VFD I2C Pin init
	IODIR1 |=I2C_SDA_VFD|I2C_SCL_VFD; /*OUTPUT*/
	set_vfd_sda(0);
	set_vfd_scl(1);
	set_vfd_sda(1);/*��ʼ����֮����һ��STOP*/
#endif
	//LPC922 I2C Pin init
	IODIR1 |=I2C_SDA2|I2C_SCL2; /*OUTPUT*/
	set_sda2(0);
	set_scl2(1);
	set_sda2(1); /*��ʼ����֮����һ��STOP*/

	//HDMI I2C Pin init
	IODIR0 |=HDMI_SDA|HDMI_SCL; /*OUTPUT*/
	set_sda_hdmi(0);
	set_scl_hdmi(1);
	set_sda_hdmi(1);/*��ʼ����֮����һ��STOP*/
}
/*********************************All I2C Init**********************************/

/********************************* EXT interrupt ********************************/
/*
 * @brief �ⲿ�ж�Enable ��Disable ����
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
void EnableEint0()
{
	VICIntEnable = VIC_ENABLE_EINT0;	/*ʹ��EXINT0�ж�*/
}

void DisableEint0()
{
	VICIntEnClr = VIC_ENABLE_EINT0;	/*��ֹEXINT0�ж�*/
}

void EnableEint1()
{
	VICIntEnable=VIC_ENABLE_EINT1;	/*ʹ��EXINT1�ж�*/
}

void DisableEint1()
{
	VICIntEnClr=VIC_ENABLE_EINT1;	/*��ֹEXINT1�ж�*/
}

void EnableEint2()
{
	VICIntEnable = VIC_ENABLE_EINT2;	/*ʹ��EXINT2�ж�*/
}

void DisableEint2()
{
	VICIntEnClr = VIC_ENABLE_EINT2;	/*��ֹEXINT2�ж�*/
}

void EnableEint3()
{
	VICIntEnable = VIC_ENABLE_EINT3;	/*ʹ��EXINT3�ж�*/
}

void DisableEint3()
{
	VICIntEnClr = VIC_ENABLE_EINT3;	/*��ֹEXINT3�ж�*/
}

/*
 * @brief �ⲿ�ж�3 ִ�к���
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
unsigned char eint3_timer_cnter = 0;
void exint3(void) __irq{
	static uint32 prevIRtime; 
	uint32 current_time,tmp1;
	uint8 falling_edge = 0;
	
	EXTINT=FLAG_EINT3; 	/*����ⲿ�ж�3 ��־--�Ա��´ο��Լ��������ж�*/

	if(EXTPOLAR & FLAG_EINT3)
		{
		EXTPOLAR &= (~FLAG_EINT3);
		}
	else
		{
		EXTPOLAR |= FLAG_EINT3;
		falling_edge = 1;	/*falling edge*/
		}
 
	/*Get pluse time value via Timer 0*/
	current_time = T0TC;
	if(eint3_timer_cnter)
		{
		tmp1 = (unsigned long)(current_time + (unsigned long)(eint3_timer_cnter * T0MR0) - prevIRtime);
		eint3_timer_cnter = 0;
		}
	else
		{
		tmp1 = (unsigned long)(current_time - prevIRtime);
		}
	
	prevIRtime = current_time; /*Restore pre Timer value*/
	ir_core_philips(tmp1,falling_edge);
	VICVectAddr = 0;	/* Acknowledge Interrupt*/
}

/*
 * @brief �ⲿ�ж�2 ִ�к���
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
extern unsigned char zone2_ir_mode;
unsigned char eint2_timer_cnter = 0;
void exint2(void) __irq{
	static uint32 prevIRtime;
	uint32 current_time,tmp1;
	uint8 falling_edge = 0;

	EXTINT = FLAG_EINT2;	/*������жϱ�־*/

	if(EXTPOLAR & FLAG_EINT2)
		{
		EXTPOLAR &= (~FLAG_EINT2);
		if(zone2_ir_mode == 1)
			falling_edge = 1;
		}
	else
		{
		EXTPOLAR |= FLAG_EINT2;
		if(zone2_ir_mode == 0)
			falling_edge = 1;
		}
 
	//Get pluse time value via Timer 0
	current_time = T0TC;
	if(eint2_timer_cnter)
		{
		tmp1 = (unsigned long)(current_time + (unsigned long)(eint2_timer_cnter * T0MR0) - prevIRtime);
		eint2_timer_cnter = 0;
		}
	else
		{
		tmp1 = (unsigned long)(current_time - prevIRtime);
		}

	prevIRtime = current_time; /*Restore pre Timer value*/
	ir2_core_philips(tmp1,falling_edge);
	VICVectAddr = 0;	/*Acknowledge Interrupt*/
}

/*
 * @brief �ⲿ�ж�1 ִ�к���
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
extern unsigned char rear_ir_mode;
unsigned char eint1_timer_cnter = 0;
void exint1(void) __irq{
	static uint32 prevIRtime;
	uint32 current_time,tmp1;
	uint8 falling_edge = 0;

	EXTINT = FLAG_EINT1;	/*����жϱ�־*/

	if(EXTPOLAR & FLAG_EINT1)
		{
		EXTPOLAR &= (~FLAG_EINT1);
		if(rear_ir_mode == 1)
			falling_edge = 1;
		}
	else
		{
		EXTPOLAR |= FLAG_EINT1;
		if(rear_ir_mode == 0)
			falling_edge = 1;
		}
 
	//Get pluse time value via Timer 0
	current_time = T0TC;
	if(eint1_timer_cnter)
		{
		tmp1 = (unsigned long)(current_time + (unsigned long)(eint1_timer_cnter * T0MR0) - prevIRtime);
		eint1_timer_cnter = 0;
		}
	else
		{
		tmp1 = (unsigned long)(current_time - prevIRtime);
		}

	prevIRtime = current_time; /*Restore pre Timer value*/
	ir_core_philips(tmp1,falling_edge);
	VICVectAddr = 0;	/* Acknowledge Interrupt*/
}

/*
 * @brief �ⲿ�ж�0 ִ�к���
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
unsigned char eint0_timer_cnter = 0;
void exint0(void) __irq{
	static uint32 prevIRtime; 
	uint32 current_time,tmp1;
	uint8 falling_edge = 0;

	EXTINT = FLAG_EINT0;	/*����ⲿ�ж�0 ��־--�Ա��´ο��Լ��������ж�*/

	/*���ں������ͷ�������ź�һ����һ�����غ�һ��������һ�½���*/
	if(EXTPOLAR & FLAG_EINT0) /*����ʱΪ��������Ч,�´�һ�����½��ع�����Ϊ�½�����Ч*/
		{
		EXTPOLAR &= (~FLAG_EINT0);
		}
	else /*����ʱΪ�½�����Ч,��һ��һ���������ع�����Ϊ��������Ч*/
		{
		EXTPOLAR |= FLAG_EINT0;
		falling_edge = 1;	
		}
 
	//Get pluse time value via Timer 0
	current_time = T0TC;
	if(eint0_timer_cnter)
		{
		tmp1 = (unsigned long)(current_time + (unsigned long)(eint0_timer_cnter * T0MR0) - prevIRtime);
		eint0_timer_cnter = 0;
		}
	else
		{
		tmp1 = (unsigned long)(current_time - prevIRtime);
		}
	
	prevIRtime = current_time; /*Restore pre Timer value*/
	ir_core_philips(tmp1,falling_edge);
	VICVectAddr = 0;	/*Acknowledge Interrupt*/
}

/********************************** EXT interrupt **********************************/

/********************************** remote *********************** ***************/
/*
 * @brief ң������ʼ������
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer chenshiguang C11 Code 
 */
void remote_config(void)
{
	PINSEL4|=P2_13_EINT3; /*Front IR1*/
	PINSEL4|=P2_12_EINT2; /*IR2*/
	PINSEL4|=P2_11_EINT1; /*Back IR1*/
	
	EXTMODE = 0x0F;	/*ȫ������Ϊ�ش���*/
#if 0
	VICVectAddr3 = (unsigned long)exint2;	/*Zone2 IR*/
	VICVectCntl3 = 0x20 | VIC_CH_EINT2;
	EnableEint2();
#endif
	Zone1SelectIr(0);
}

/*
 * @brief Zone1 ң����ѡ��
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
 unsigned char front_enable_zone1_ir = 1;
void Zone1SelectIr(unsigned char mode)
{
       if(front_enable_zone2_ir ==0)
		DisableEint3(); /*Front IR1*/
	DisableEint1(); /*Back IR1*/
	switch(mode)
		{
		case 0:	/*Front IR*/
			VICVectAddr17 = (unsigned long)exint3;	 
			VICVectCntl17 = 0x20 | VIC_CH_EINT3;  
			EnableEint3();
			front_enable_zone1_ir = 1;
			break;
		case 1:	/*Back IR*/
			VICVectAddr15 = (unsigned long)exint1;	 
			VICVectCntl15 = 0x20 | VIC_CH_EINT1;
			EnableEint1();
			front_enable_zone1_ir = 0;
			break;
		case 2:	/*Front IR and Back IR*/
			front_enable_zone1_ir = 1;
			VICVectAddr17 = (unsigned long)exint3; /*Front IR*/
			VICVectCntl17 = 0x20 | VIC_CH_EINT3;
			EnableEint3();
			
			VICVectAddr15 = (unsigned long)exint1; /*Back IR*/	 
			VICVectCntl15 = 0x20 | VIC_CH_EINT1;
			EnableEint1();
			break;
		}
}

/*
 * @brief Zone2 ң����ѡ��
 *
 * @author chenjianming
 * @date 2007-11-20
 * @refer ��chenshiguang C11 Code ��ֲ����
 */
unsigned char front_enable_zone2_ir = 1;
void Zone2SelectIr(unsigned char mode)
{
	DisableEint2();
	switch(mode)
		{
		case 0:	/*front*/
			front_enable_zone2_ir = 1;
			break;
		case 1:	/*rear*/
			front_enable_zone2_ir = 0;
			VICVectAddr3 = (unsigned long)exint2;	//Zone2 IR
			VICVectCntl3 = 0x20 | VIC_CH_EINT2;
			EnableEint2();
			break;
		case 2:	/*both*/
			front_enable_zone2_ir = 1;
			VICVectAddr3 = (unsigned long)exint2;	//Zone2 IR 
			VICVectCntl3 = 0x20 | VIC_CH_EINT2;
			EnableEint2();
			break;
		}
}
/********************************** remote ***************************************/

/*
 * @brief standby ��IO ��״̬����
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void SetIoStandby(void)
{
	//HDR HDMI �ڹػ���ȫ������Ϊ�͵�ƽ������(5����)	
	SetMax335Data(0);
	SetMax335Clk(0);  
	SetMax335Cs1(0);
	SetMax335Cs2(0);
	SetMax335Cs3(0);
	SetMax335Cs4(0);
	SetMax335Cs5(0);
	SetMax335Cs6(0);
	SetMax335Cs7(0);
	SetMax335Cs8(0);
	
	SetMas9116Data(0);
	SetMas9116Clk(0);
	SetMas9116Cs0(0);
	SetMas9116Cs1(0);
	SetMas9116Cs2(0);
	SetMas9116Cs3(0);
	SetMas9116Cs4(0);
	SetMas9116Cs5(0);

	SetDacMd(0);
	SetDacMc(0);
	SetDacCs4(0);
	SetDacCs3(0);
	SetDacCs2(0);
	SetDacCs1(0);
	
	//set_sda(0);
	//set_scl(0);
	//SetResetPin(0);
	return ;
/*
	set_scl2(0);
	set_sda2(0);
	SetDspReset(0);
*/
}

/*
 * @brief power on IO ��״̬����
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void SetIoPowerOn(void)
{
	return ;	   
	//I2cPinInit();
	//InitMAS335();
	//InitTuner();
	//InitI2sSelect();
}



/*END*/

/*
 * @brief hardware init
 *
 * @author chenjianming
 * @date 2007-06-27
 * @refer chenshiguang C11 Code
 */
void HardwareInit()
{
	//PINSEL2 = 0;		//P1��IO��,���Ҫ������IO����Ϊ�������ܣ���Ҫ����PINSEL
	InitPower();			/*��ʼ��Power IO and PowerLed */		
	SystemPower(OFF); 	/*�ص�POWER ���ڴ���״̬*/

	I2cPinInit();		    	/*��ʼ��I2C*/	
	SetTrigerOut(0);	    	/*Triger out  */ 
	SetTrigerOut1(0);	    	/*Triger out  */ 
	SetTrigerOut2(0);	    	/*Triger out  */ 	
	Init165();		    	/*��ʼ��165*/	
	InitEncoderSwitch();	/*��ʼ�������λ��*/
	remote_config();	       /*��ʼ��ң������*/	
	InitResetPin();           	/*��λ��������Ϊ���*/	
	InitDspResetPin();     	/*DSP ��λ��������Ϊ���*/
	Init_Dsp_Spi_Pin();   /*DSP SPI ͨѶ��������Ϊ���*/ 
	InitDac();	             	/*��ʼ��DAC */	
	InitMas9116();	      	/*��ʼ��MAS9116*/
	InitMAS335(); 	      	/*��ʼ��MAS355*/
	InitMuteControl();      /*��ʼ��Mute IO��*/
	InitHDRaidoPower();  /*��ʼ��HD Radio Power Pin*/
	Init4094();
	
	Init_Dsp_Input_Select(); /*��ʼ��DSP ����Դ*/
	Init_DSD_Select(); /*��ʼ��DSD Select Pin*/
	DSP_Input_Stream_Select(AD_Input); /*ѡ��DSP����ԴΪA/D����*/
	//InitI2sSelect();
}

