#include "Queue.h"

//����һ���¶���
//��ͷ�Ͷ�βָ�붼ָ��ͬһ���ڵ�
bool InitQueue(LinkQueue *Q)
{
	Q->front = Q->rear = new QNode;
	if (!Q->front)
		return false;
	Q->front->next = NULL;
	return true;
}

//���ٶ���
bool DestoryQueue(LinkQueue * Q)
{
	while (Q->front)
	{
		Q->rear = Q->front->next;
		delete Q->front;
		Q->front = Q->rear;
	}
	return true;
}


/* ����еĳ��� */
int QueueLength(LinkQueue * Q)
{
	int i = 0;
	QueuePtr p;
	p = Q->front;
	while (Q->rear != p)
	{
		i++;
		p = p->next;
	}
	return i;
}


//�����ڶ�β����ڵ㣬��Ӳ���
//����Ԫ��dataΪQ���µĶ�βԪ�أ�����ɹ�����true�����򷵻�false
bool EnQueue(LinkQueue *Q, IplImage * data)
{
	QueuePtr s = new QNode;
	if (!s)   //�洢�ռ����ʧ��
		return false;
	s->img = data;
	s->next = NULL;
	Q->rear->next = s;
	Q->rear = s;
	return true;
}


//�����ڶ�ͷɾ���ڵ㣬���Ӳ���
//�����в�Ϊ�գ�ɾ��Q�Ķ�ͷԪ�أ���data���ظ�ͼ��ĵ�ַ��������true��ʾ�����ɹ������򷵻�false
bool DeQueue(LinkQueue *Q, IplImage ** data)
{

	QueuePtr p;
	if (Q->front == Q->rear)
	{
		cout << "The Queue is NULL!" << endl;
		return false;
	}
	p = Q->front->next;
	*data = p->img;
	Q->front->next = p->next;
	if (Q->rear == p)
		Q->rear = Q->front;
	delete p;
	return true;
}