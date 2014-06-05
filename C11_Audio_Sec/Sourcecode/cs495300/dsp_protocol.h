/*
 *���ļ���������DSP_TASKͨѶ�����ݵ�Э��
 *Э���ʽ:
 * CMD_TYPE CMD_BYTES ... 
 *�������PROTOCOL_MAX_SIZE ��
 */
#ifndef _DSP_PROTOCOL_H
#define _DSP_PROTOCOL_H

enum{
SPK_FRONT = 0,
SPK_SURROUND,
SPK_SURROUND_BACK,
SPK_CENTER,
SPK_LFE,
};

typedef enum{
CMD_POWER = 0x01,
CMD_DELAY,
CMD_CORNER,
CMD_SPK_MODE,
CMD_SIGNAL,
CMD_IRC,
CMD_STREAM = 0x02,
}CMD_TYPES;

typedef struct {
	unsigned char *buffer;
	unsigned char valid_flag;
}DSP_PROTOCOL,*pDSP_PROTOCOL;

#define PROTOCOL_MAX_SIZE 8

#ifdef _USE_DSP_PROTOCOL
extern void DspProcPower(unsigned char *buffer,unsigned char msg_size);
extern void DspProcDelay(unsigned char *buffer,unsigned char msg_size);
extern void DspProcCorner(unsigned char *buffer,unsigned char msg_size);
extern void DspProcSpkMode(unsigned char *buffer,unsigned char msg_size);
extern void DspProcSignal(unsigned char *buffer,unsigned char msg_size);
extern void DspProcIrc(unsigned char *buffer,unsigned char msg_size);
#else
extern void DspProcPower(unsigned char mode);
extern void DspProcDelay(unsigned char channel,unsigned short delay_time);
extern void DspProcCorner(unsigned char channel,unsigned char corner_value);
extern void DspProcSpkMode(unsigned char channel,unsigned char mode);
extern void DspProcSignal(unsigned char signal_type, unsigned char channel, unsigned char freq);
extern void DspProcIrc(unsigned char irc_mode,unsigned char test_channel);
#endif

#endif

