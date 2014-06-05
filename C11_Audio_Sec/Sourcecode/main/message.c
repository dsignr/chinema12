#include "macro.h"
#include "message.h"
#include "debug.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>

#ifdef _DEBUG
//#define _DEBUG_MESSAGE
#endif

//MSG * pMsgQueue; 		/*MSG �ṹ��ָ��*/ /*Dele cjm 2008-05-08 */
MSG pMsgQueue[200];	/*MSG �ṹ������*/ /*add  cjm 2008-05-08 */
short msg_queue_rear;	/*����β*/
short msg_queue_front;	/*����ͷ*/
#define MSGQUEUE_LENGTH MESSAGE_QUEUE_SIZE

unsigned char InitMsgQueue(void)
{
	if(!pMsgQueue)
		{
		/*����200���ṹ�峤�ȿռ���׵�ַ��pMsgQueue*/
		//pMsgQueue = (MSG *) malloc(MESSAGE_QUEUE_SIZE * sizeof(MSG)); /*sizeof(MSG) = 5*/ /*Dele cjm 2008-05-08 */
		if(!pMsgQueue)
			{
#ifdef _DEBUG_MESSAGE
			debug_printf("out off memory\n");
#endif
			return FALSE;
			}
		}
	msg_queue_rear = msg_queue_front = 0;
	return TRUE;
}

/*�ж���Ϣ�����Ƿ�Ϊ��*/
static unsigned char IsMsgQueueEmpty()
{
	if(msg_queue_rear == msg_queue_front)
		return TRUE;
	return FALSE;
}

/*�ж���Ϣ�����Ƿ�����*/
static unsigned char IsMsgQueueFull()
{
	if((msg_queue_rear+1)%MSGQUEUE_LENGTH == msg_queue_front) 
		{
		return TRUE;
		}
	return FALSE;
}

/*�����ͷ�������*/
unsigned char PostMessage(TASK_ID t_id,unsigned char *buffer)
{
	if(IsMsgQueueFull())
		{
		assert(IsMsgQueueFull());
		return FALSE;
		}
	
	memcpy(pMsgQueue[msg_queue_front].buffer,buffer,MESSAGE_SIZE);
	pMsgQueue[msg_queue_front].t_id = t_id;
	msg_queue_front = (msg_queue_front-1+MSGQUEUE_LENGTH)%MSGQUEUE_LENGTH;
	return TRUE;
}

/*�����β�������*/
unsigned char SendMessage(TASK_ID t_id,unsigned char *buffer)
{
	if(IsMsgQueueFull())
		{
		assert(IsMsgQueueFull());
		return FALSE;
		}

	msg_queue_rear = (msg_queue_rear+1)%MSGQUEUE_LENGTH;
	memcpy(pMsgQueue[msg_queue_rear].buffer,buffer,MESSAGE_SIZE);
	pMsgQueue[msg_queue_rear].t_id = t_id;
	//DispBuffer(pMsgQueue[msg_queue_rear].buffer,MESSAGE_SIZE);
	return TRUE;
}

/*��Ϣ�����У�ͨ��pMsg����
 return:
 	FALSE ���п�
 	TRUE �з���
 */
unsigned char GetMessage(MSG * pMsg)
{
	if(IsMsgQueueEmpty())
		return FALSE;

	msg_queue_front = (msg_queue_front+1)%MSGQUEUE_LENGTH;
	memcpy(pMsg, pMsgQueue + msg_queue_front, sizeof(MSG));
	return TRUE;
}

