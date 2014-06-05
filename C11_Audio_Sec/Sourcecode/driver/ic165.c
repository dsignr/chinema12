#include "..\main\macro.h"
#include "ioport.h"
#include "ic165.h"

/* 
 * @brief  74LS165 ���ǰ����尴����������
 *
 * @author chenjianming
 * @date 2007--
 * @refer 
 */
#define KEY_MASK 0xFFFFFFFF
uint32 read165()  
{
	uint32 rtn_key = 0;
	unsigned char i;

	//set_ic165_ld(1);
	set_ic165_ld(0);
	set_ic165_ld(1);	/*load the parallel data*/ /*LD�ӵ͵�����ס����*/
	//for(i = 0; i < 24; i++) /*��ȡ24λ��������*/
	for(i=0;i<32;i++)
		{
		rtn_key <<= 1;
		set_ic165_clk(0);
		if(sense_ic165_data())
			{
			rtn_key |= 1;
			}
		set_ic165_clk(1);
		}
	rtn_key = ~rtn_key;/*�͵�ƽ��ʾ��������-ȡ�����ߵ�ƽ��ʾ��������*/
	rtn_key &= KEY_MASK;
	
	return rtn_key;
}

