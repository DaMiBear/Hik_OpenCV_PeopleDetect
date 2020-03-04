#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2\opencv.hpp>
#include "cv.h"

#include<QString>
#include<QFileDialog>
#include<QLabel>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<QTimer>
#include<QImage>
#include<QTextEdit>
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include "plaympeg4.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeLightSlot(int);//改变按钮状态
    void openCameraSlot();  //打开摄像头
    void readCameraSlot();  //读取摄像头
    void closeCameraSlot(); //关闭摄像头
    void attendEditSlot(int, int, int);  //添加文本框内容
    void autoControlButtonSlot();   //自动改变按钮状态
    void on_L1Button_clicked();

    void on_L2Button_clicked();

    void on_L3Button_clicked();

    void on_L4Button_clicked();

    void on_L5Button_clicked();

    void on_L6Button_clicked();

    void on_L7Button_clicked();

    void on_L8Button_clicked();

    void on_L9Button_clicked();


    void on_L10Button_clicked();

    void on_L11Button_clicked();

    void on_L12Button_clicked();

    void on_L13Button_clicked();

    void on_L14Button_clicked();

    void on_L15Button_clicked();

    void on_L16Button_clicked();

    void on_L17Button_clicked();

    void on_L18Button_clicked();

    void on_L19Button_clicked();

    void on_L20Button_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
signals:
    void lightNumberSignal(int);    //按钮信号
    void autoControlButtonSignal();  //自动改变按钮状态信号
};

#endif // MAINWINDOW_H
