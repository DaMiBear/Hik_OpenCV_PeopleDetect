#ifndef PROCESSPEOPLE_H
#define PROCESSPEOPLE_H
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"
#include <opencv2\opencv.hpp>
#include "cv.h"
#include "highgui.h"
#include <process.h>
#include "Queue.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>


Point getCenterPoint(Rect rect);
Mat ProcessPeople();
class MySignal:public QObject
{
    Q_OBJECT
public:
    explicit MySignal(QObject *parent = 0):
        QObject(parent)
          {
          }
    void sendSignal(int x,int y,int peopleNumber)
    {
        emit sendPosition(x,y,peopleNumber);
    }
signals:
    void sendPosition(int,int,int);
};
#endif // PROCESSPEOPLE_H


