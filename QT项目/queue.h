/*********************************************************************************************************
*由于图片生成后都是放在内存里面的，而且中间不会进行什么插入、删除操作，所以链队列是最好的选择。
*构建一个存放图片的链队列
*********************************************************************************************************/
#ifndef __Queue_H__
#define __Queue_H__


#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;


//队列节点结构
typedef struct QNode
{
    IplImage * img;
    struct QNode * next;
}QNode, *QueuePtr;

//队列的链表结构
typedef struct
{
    QueuePtr front, rear;   //队头队尾指针
}LinkQueue;

//求队列长度
int QueueLength(LinkQueue * Q);

//构建一个新队列
bool InitQueue(LinkQueue * Q);

//销毁队列
bool DestoryQueue(LinkQueue * Q);

//队列在队尾插入节点，入队操作
//插入元素data为Q的新的队尾元素，插入成功返回true，否则返回false
bool EnQueue(LinkQueue *Q, IplImage * data);

//队列在队头删除节点，出队操作
//若队列不为空，删除Q的队头元素，用data返回该图像的地址，并返回true表示操作成功，否则返回false
bool DeQueue(LinkQueue *Q, IplImage ** data);
//IplImage * DeQueue(LinkQueue *Q);


#endif
