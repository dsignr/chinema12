/* @file  uart.c
 * @brief ��ʼ������,���ò�����
 *
 * @author chenjianming
 * @date 2007-12-20
 * @refer chenshiguang C11 code
 */
#include "mcudef.h"
#include "uart.h"
#include <stdlib.h>
#include "..\main\queue.h"
#include "..\main\debug.h"

#define TUNER_ARRAY_MAX_ELEMENT 		      0x100  
queue_type tuner_array[TUNER_ARRAY_MAX_ELEMENT];
STRUCT_QUEUE tuner_buffer = 
{
	tuner_array,
	-1,  
	-1,
	TUNER_ARRAY_MAX_ELEMENT,
};
pSTRUCT_QUEUE tuner_queue_pointer=&tuner_buffer;  

#define PROTOCOL_ARRAY_MAX_ELEMENT		0x100
queue_type protocol_array[PROTOCOL_ARRAY_MAX_ELEMENT];
STRUCT_QUEUE protocol_buffer=
{
	protocol_array,
	-1,
	-1,
	PROTOCOL_ARRAY_MAX_ELEMENT,
};

pSTRUCT_QUEUE protocol_queue_pointer=&protocol_buffer;
	
void Uart0Isr (void) __irq {
	unsigned char uart0_cache;

	if(0x04==(U0IIR&0x0E)) /*���ڽ������ݿ���*/
		{
		uart0_cache = U0RBR;
		InQueue(protocol_queue_pointer,uart0_cache);
		}
	VICVectAddr = 0;	/* Acknowledge Interrupt */ /*��LPC2300�У�VICVECTADDR ��ʾ����VICAddress*/
}

void SetUart0Baudrate(unsigned long baudrate)
{
	U0LCR = 0x83; /* DLAB = 1�������ò����� */
	U0DLL = ((PCLK * MUL_VALUE) / 16) / baudrate;
	U0DLM = 0x00;
	U0LCR = 0x03;
}

void  Uart0Init(unsigned long baudrate) 
{
	PINSEL0 |= P0_2_TXD0 | P0_3_RXD0; /*����IO��Ϊ����0*/

	SetUart0Baudrate(baudrate); /*����������*/

#ifdef _ENABLE_UART0_INTERRUPT
	U0IER = 0x01; /*ʹ�ܽ����ж�*/
	VICVectAddr6 = (unsigned long)Uart0Isr;	/*�жϷ������*/
	VICVectCntl6 = VIC_CH_UART0; 
	VICIntEnable = VIC_ENABLE_UART0; /*ʹ��uart0�ж�*/

	InitQueue(protocol_queue_pointer);
#endif
}

void Uart0Send(unsigned char value)
{
#if 1
	if (value == '\n')	/*0x0A*/
		{
		U0THR = '\r'; /*�س�*/
		while (!(U0LSR & 0x40));
		}
#endif
	U0THR = value;	/* ��������*/
	while (!(U0LSR & 0x40));	/* �ȴ����ݷ������*/
#if 0
	if (value == '\r')	/*0x0A*/
		{
		U0THR = '\n'; /*����*/
		while (!(U0LSR & 0x40));
		}
#endif
}

//extern unsigned char Tuner_Reply_Flag;
void Uart1Isr (void) __irq {
	unsigned char uart1_cache;

	if(0x04==(U1IIR&0x0E))
		{
		uart1_cache = U1RBR;
		
//		if(Tuner_Reply_Flag==1)
			InQueue(tuner_queue_pointer,uart1_cache);	
		}
	VICVectAddr = 0;	/* Acknowledge Interrupt */
}

void SetUart1Baudrate(unsigned long baudrate)
{
	U1LCR = 0x83; /* DLAB = 1�������ò����� */
	U1DLL = ((PCLK * MUL_VALUE) / 16) / baudrate;
	U1DLM = 0x00;
	U1LCR = 0x03;
}

void  Uart1Init(unsigned long baudrate)
{
	PINSEL0 |=P0_15_TXD1; /*����IO��Ϊ����1*/  
	PINSEL1 |=P0_16_RXD1;

	SetUart1Baudrate(baudrate);

#ifdef _ENABLE_UART1_INTERRUPT
	U1IER = 0x01; /*ʹ�ܽ����ж�*/
	VICVectAddr7 = (unsigned long)Uart1Isr;	/*�жϷ������*/ 
	VICVectCntl7  = 0; /*���ȼ�*/
	VICIntEnable  = VIC_ENABLE_UART1;	/*ʹ��uart1�ж�*/
#endif

	InitQueue(tuner_queue_pointer);	
}

void Uart1Send(unsigned char value)
{
	if (value == '\n')
		{
		U1THR = '\r';
		while (!(U1LSR & 0x40));
		}
	U1THR = value;	/* ��������*/
	while (!(U1LSR & 0x40));	/* �ȴ����ݷ������*/
}

void UartInit()
{
#ifdef _DEBUG
	Uart0Init(115200L); /*Debug*/
#else
	Uart0Init(9600);    /*RS232*/
#endif
	Uart1Init(57600L); /*HD_Radio  jian  add in 11-08-17*/
}

