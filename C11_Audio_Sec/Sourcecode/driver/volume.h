#ifndef _VOLUME_H
#define _VOLUME_H

extern void VolumeInit(void);
extern void SetZone2Volume(float volume);
extern void Set2ChVolume(float volume);
extern void Set7Ch1MainVolume(short volume);
extern void SetBalanceVolume(unsigned char channel, float volume);
extern void AutoSoundSetVolume(short volume);	/*auto sound���Ե�ʱ�򣬱��뽫ƽ��ȫ������Ϊ0�����ҽ�main volume����Ϊ-20 dB,�ô˺���ʵ��*/
extern void SoftMute(uchar on);
#endif

