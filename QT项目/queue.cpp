#include "queue.h"

//构建一个新队列
//队头和队尾指针都指向同一个节点
bool InitQueue(LinkQueue *Q)
{
    Q->front = Q->rear = new QNode;
    if (!Q->front)
        return false;
    Q->front->next = NULL;
    return true;
}

//销毁队列
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


/* 求队列的长度 */
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


//队列在队尾插入节点，入队操作
//插入元素data为Q的新的队尾元素，插入成功返回true，否则返回false
bool EnQueue(LinkQueue *Q, IplImage * data)
{
    QueuePtr s = new QNode;
    if (!s)   //存储空间分配失败
        return false;
    s->img = data;
    s->next = NULL;
    Q->rear->next = s;
    Q->rear = s;
    return true;
}


//队列在队头删除节点，出队操作
//若队列不为空，删除Q的队头元素，用data返回该图像的地址，并返回true表示操作成功，否则返回false
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
