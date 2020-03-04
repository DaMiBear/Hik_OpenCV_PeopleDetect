/*********************************************************************************************************
*����ͼƬ���ɺ��Ƿ����ڴ�����ģ������м䲻�����ʲô���롢ɾ����������������������õ�ѡ��
*����һ�����ͼƬ��������
*********************************************************************************************************/
#ifndef __Queue_H__
#define __Queue_H__


#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;


//���нڵ�ṹ
typedef struct QNode
{
	IplImage * img;
	struct QNode * next;
}QNode, *QueuePtr;

//���е�����ṹ
typedef struct
{
	QueuePtr front, rear;   //��ͷ��βָ��
}LinkQueue;

//����г���
int QueueLength(LinkQueue * Q);

//����һ���¶���
bool InitQueue(LinkQueue * Q);

//���ٶ���
bool DestoryQueue(LinkQueue * Q);

//�����ڶ�β����ڵ㣬��Ӳ���
//����Ԫ��dataΪQ���µĶ�βԪ�أ�����ɹ�����true�����򷵻�false
bool EnQueue(LinkQueue *Q, IplImage * data);

//�����ڶ�ͷɾ���ڵ㣬���Ӳ���
//�����в�Ϊ�գ�ɾ��Q�Ķ�ͷԪ�أ���data���ظ�ͼ��ĵ�ַ��������true��ʾ�����ɹ������򷵻�false
bool DeQueue(LinkQueue *Q, IplImage ** data);
//IplImage * DeQueue(LinkQueue *Q);


#endif