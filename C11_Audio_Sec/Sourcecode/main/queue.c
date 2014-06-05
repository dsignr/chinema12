#include "queue.h"

void InitQueue(pSTRUCT_QUEUE queue_name)
/*��Ϣ���г�ʼ��*/
{
	queue_name->front = -1;
	queue_name->rear = -1;
}

int IsQueueEmpty(pSTRUCT_QUEUE queue_name)
/*���п�,�ö�ͷfront���βrearֵ��ͬʱ�����д��ڿ�״̬*/
{
	if((queue_name->front) % queue_name->queue_max_element == queue_name->rear)
		{
		return QUEUE_EMPTY;
		}
	return QUEUE_NORMAL;
}

int IsQueueFull(pSTRUCT_QUEUE queue_name)
/*����βrear��1֮��ֵ���ͷfront��ͬʱ�����д�����״̬*/
{
	if((queue_name->rear+1) % queue_name->queue_max_element == queue_name->front)
		return QUEUE_FULL;
	return QUEUE_NORMAL;
}

int QueueFront(pSTRUCT_QUEUE queue_name)
/*���ص�ǰ����ͷfrontλ��*/
{
	return queue_name->front;
}

int DeQueue(pSTRUCT_QUEUE queue_name,queue_type *OutValue)
/*�����У�ֵ����*OutValue���ɹ�����0��ʧ�ܷ���1*/
{
	if(IsQueueEmpty(queue_name) == QUEUE_NORMAL)
		{
		queue_name->front = (queue_name->front + 1) % queue_name->queue_max_element;
		*OutValue = queue_name->queue_buf[queue_name->front];
		return 0;
		}
	return 1;
}

int InQueue(pSTRUCT_QUEUE queue_name,queue_type InValue)
/*����У�ֵ����InValue���ɹ�����0��ʧ�ܷ���1,����β���Ԫ��*/
{
	if(IsQueueFull(queue_name) == QUEUE_NORMAL)
		{
		queue_name->rear = (queue_name->rear + 1) % queue_name->queue_max_element;
		queue_name->queue_buf[queue_name->rear] = InValue;
		return 0;
		}
	return 1;
}

int GetQueueElementNum(pSTRUCT_QUEUE queue_name)
/*���ص�ǰ�����е�Ԫ�ظ���*/
{
	if(IsQueueEmpty(queue_name) == QUEUE_EMPTY)
		return 0;
	if(IsQueueFull(queue_name))
		return queue_name->queue_max_element;
	if((queue_name->front) > (queue_name->rear))
		{
		return (queue_name->queue_max_element) - ((queue_name->front) - queue_name->rear);
		}
	else
		return ((queue_name->rear) - (queue_name->front));
}

int GetQueueElementRemain(pSTRUCT_QUEUE queue_name)
/*���ص�ǰ�����е�Ԫ�ظ���*/
{
	return queue_name->queue_max_element - GetQueueElementNum(queue_name);
}

int GetQueueElement(queue_type *buffer,pSTRUCT_QUEUE queue_name,int start_position,int size)
{
	int i;
	for(i=0;i<size;i++)
		{
		*(buffer+i) = queue_name->queue_buf[(start_position+i)%queue_name->queue_max_element];
		}
	return 0;
}

