#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "camera.h"
#include "processpeople.h"
#include <math.h>
using namespace cv;
using namespace std;
extern MySignal *a; //在processpeople.cpp中声明
int  HandleCoordinate(int x,int y); //把坐标转换为对应的哪一盏灯
int  arrBinToHex(bool arr[]);   //数组形成的二进制转十进制
bool everyLightStateArr[20]={0};//统计每盏灯的状态 9位二进制，每一位代表每一个灯，1为亮，0为灭
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
       ui->setupUi(this);
    //定时器、按钮图片初始化
    timer = new QTimer(this);
    QIcon openIcon("E:\\Pictures\\QImageOpen.png"),closeIcon("E:\\Pictures\\QImageClose.png");  //按钮图片初始化
    /*
     *
     *
     * 按钮设置,图片初始化
     *
     *
     * */
    ui->L1Button->setIcon(closeIcon); ui->L1Button->setIconSize(QSize(35,47));ui->L1Button->setFlat(true);
    ui->L2Button->setIcon(closeIcon); ui->L2Button->setIconSize(QSize(35,47));ui->L2Button->setFlat(true);
    ui->L3Button->setIcon(closeIcon); ui->L3Button->setIconSize(QSize(35,47));ui->L3Button->setFlat(true);
    ui->L4Button->setIcon(closeIcon); ui->L4Button->setIconSize(QSize(35,47));ui->L4Button->setFlat(true);
    ui->L5Button->setIcon(closeIcon); ui->L5Button->setIconSize(QSize(35,47));ui->L5Button->setFlat(true);
    ui->L6Button->setIcon(closeIcon); ui->L6Button->setIconSize(QSize(35,47));ui->L6Button->setFlat(true);
    ui->L7Button->setIcon(closeIcon); ui->L7Button->setIconSize(QSize(35,47));ui->L7Button->setFlat(true);
    ui->L8Button->setIcon(closeIcon); ui->L8Button->setIconSize(QSize(35,47));ui->L8Button->setFlat(true);
    ui->L9Button->setIcon(closeIcon); ui->L9Button->setIconSize(QSize(35,47));ui->L9Button->setFlat(true);
    ui->L10Button->setIcon(closeIcon); ui->L10Button->setIconSize(QSize(35,47));ui->L10Button->setFlat(true);
    ui->L11Button->setIcon(closeIcon); ui->L11Button->setIconSize(QSize(35,47));ui->L11Button->setFlat(true);
    ui->L12Button->setIcon(closeIcon); ui->L12Button->setIconSize(QSize(35,47));ui->L12Button->setFlat(true);
    ui->L13Button->setIcon(closeIcon); ui->L13Button->setIconSize(QSize(35,47));ui->L13Button->setFlat(true);
    ui->L14Button->setIcon(closeIcon); ui->L14Button->setIconSize(QSize(35,47));ui->L14Button->setFlat(true);
    ui->L15Button->setIcon(closeIcon); ui->L15Button->setIconSize(QSize(35,47));ui->L15Button->setFlat(true);
    ui->L16Button->setIcon(closeIcon); ui->L16Button->setIconSize(QSize(35,47));ui->L16Button->setFlat(true);
    ui->L17Button->setIcon(closeIcon); ui->L17Button->setIconSize(QSize(35,47));ui->L17Button->setFlat(true);
    ui->L18Button->setIcon(closeIcon); ui->L18Button->setIconSize(QSize(35,47));ui->L18Button->setFlat(true);
    ui->L19Button->setIcon(closeIcon); ui->L19Button->setIconSize(QSize(35,47));ui->L19Button->setFlat(true);
    ui->L20Button->setIcon(closeIcon); ui->L20Button->setIconSize(QSize(35,47));ui->L20Button->setFlat(true);
    /*
     *
     *
     * 信号槽设置
     *
     *
     * */
    QObject::connect(ui->openButton,SIGNAL(clicked()),this,SLOT(openCameraSlot()));//开 登陆摄像头
    QObject::connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(closeCameraSlot()));//关
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(readCameraSlot()));//定时器刷新Label界面，人体检测
    QObject::connect(a,SIGNAL(sendPosition(int,int,int)),this,SLOT(attendEditSlot(int,int,int)));//传递检测的数据函数 坐标（ x y ）人数
    QObject::connect(this,SIGNAL(lightNumberSignal(int)),this,SLOT(changeLightSlot(int)));//按钮点击
    QObject::connect(this,SIGNAL(autoControlButtonSignal()),this,SLOT(autoControlButtonSlot())); //每次改变文本框信息时发送自动控制按钮状态信号
    /*
     * 控件初始化
     * */
    ui->outEdit->setReadOnly(true);
    ui->outEdit->setStyleSheet("border: 1px solid green; color: green; background: silver;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 *
 *
 * 槽函数
 *
 *
 * */
void MainWindow::openCameraSlot()
{
    Init_Camera();
    timer->start(33);   //开始计时超时则发出timeout信号
}
void MainWindow::closeCameraSlot()
{
    Close_Camera();
}
void MainWindow::readCameraSlot()
{
    Mat frame;

    frame =ProcessPeople();
    cv::cvtColor(frame,frame,CV_BGR2RGB);
    QImage img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,QImage::Format_RGB888);
    ui->imgLabel->setPixmap(QPixmap::fromImage(img));
    ui->imgLabel->resize(ui->imgLabel->pixmap()->size());
}
//增加TextEdit文本
//该槽函数，刚刚检测开始,以及每次检测一个人的时候都会被调用

void MainWindow::attendEditSlot(int x, int y, int peopleNumber)
{
    static int allowSend = 0;   //统计该函数发送坐标的执行次数
    QIcon openIcon("E:\\Pictures\\QImageOpen.png"),closeIcon("E:\\Pictures\\QImageClose.png");
    if(peopleNumber==0) //如果人数为0，则关闭所有的灯
    {
        ui->L1Button->setIcon(closeIcon);
        ui->L2Button->setIcon(closeIcon);
        ui->L3Button->setIcon(closeIcon);
        ui->L4Button->setIcon(closeIcon);
        ui->L5Button->setIcon(closeIcon);
        ui->L6Button->setIcon(closeIcon);
        ui->L7Button->setIcon(closeIcon);
        ui->L8Button->setIcon(closeIcon);
        ui->L9Button->setIcon(closeIcon);
        ui->L10Button->setIcon(closeIcon);
        ui->L11Button->setIcon(closeIcon);
        ui->L12Button->setIcon(closeIcon);
        ui->L13Button->setIcon(closeIcon);
        ui->L14Button->setIcon(closeIcon);
        ui->L15Button->setIcon(closeIcon);
        ui->L16Button->setIcon(closeIcon);
        ui->L17Button->setIcon(closeIcon);
        ui->L18Button->setIcon(closeIcon);
        ui->L19Button->setIcon(closeIcon);
        ui->L20Button->setIcon(closeIcon);
    }
    if(x==0&&y==0&&peopleNumber!=0)  //当坐标都为0时，表明检测刚刚开始，如果有人数，先显示人数，开始检测人的位置之后坐标才有值
    {
         QString peopleN_s = QString::number(peopleNumber);
         ui->outEdit->append("PeopleNumber:  "+peopleN_s); //追加文本
         for(int i=0;i<20;i++)
         {
             everyLightStateArr[i]=0;
         }
    }
    else if(x!=0&&y!=0)    //当有坐标时(包括误报)，人数肯定不为零，显示检测信息
    {
        allowSend++;    //每输出一次坐标就把执行次数+1
        QString x_s = QString::number(x);
        QString y_s = QString::number(y);
        everyLightStateArr[HandleCoordinate(x,y)-1]=1;   //根据哪一盏灯，把对应位置1
        ui->outEdit->append("x:"+x_s+"y:"+y_s+"     LightNumber"+ QString::number(HandleCoordinate(x,y))); //显示坐标和要打开的灯

        if(allowSend==peopleNumber)
        {
            allowSend = 0;
            //int everyLightStateHex = arrBinToHex(everyLightStateArr);  //把每盏灯的状态转变为十进制传入函数中
            emit this->autoControlButtonSignal();  //根据坐标判断后发送自动控制按钮信号
        }
    }
}
//槽函数  改变按钮图片

void MainWindow::changeLightSlot(int number)
{
    QIcon openIcon("E:\\Pictures\\QImageOpen.png"),closeIcon("E:\\Pictures\\QImageClose.png");
    static bool lightState[20]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    switch (number)
    {
    case 1:
            if(lightState[number-1]==true)
            {
                ui->L1Button->setIcon(openIcon);
            }
            else
            {
                ui->L1Button->setIcon(closeIcon);
            }
            lightState[number-1]=!lightState[number-1];
        break;
    case 2:
            if(lightState[number-1]==true)
            {
                ui->L2Button->setIcon(openIcon);
            }
            else
            {
                ui->L2Button->setIcon(closeIcon);
            }
            lightState[number-1]=!lightState[number-1];
        break;
    case 3:
            if(lightState[number-1]==true)
            {
                ui->L3Button->setIcon(openIcon);
            }
            else
            {
                ui->L3Button->setIcon(closeIcon);
            }
            lightState[number-1]=!lightState[number-1];
        break;
    case 4:
        if(lightState[number-1]==true)
        {
            ui->L4Button->setIcon(openIcon);
        }
        else
        {
            ui->L4Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 5:
        if(lightState[number-1]==true)
        {
            ui->L5Button->setIcon(openIcon);
        }
        else
        {
            ui->L5Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 6:
        if(lightState[number-1]==true)
        {
            ui->L6Button->setIcon(openIcon);
        }
        else
        {
            ui->L6Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 7:
        if(lightState[number-1]==true)
        {
            ui->L7Button->setIcon(openIcon);
        }
        else
        {
            ui->L7Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 8:
        if(lightState[number-1]==true)
        {
            ui->L8Button->setIcon(openIcon);
        }
        else
        {
            ui->L8Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 9:
        if(lightState[number-1]==true)
        {
            ui->L9Button->setIcon(openIcon);
        }
        else
        {
            ui->L9Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 10:
        if(lightState[number-1]==true)
        {
            ui->L10Button->setIcon(openIcon);
        }
        else
        {
            ui->L10Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 11:
        if(lightState[number-1]==true)
        {
            ui->L11Button->setIcon(openIcon);
        }
        else
        {
            ui->L11Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 12:
        if(lightState[number-1]==true)
        {
            ui->L12Button->setIcon(openIcon);
        }
        else
        {
            ui->L12Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 13:
        if(lightState[number-1]==true)
        {
            ui->L13Button->setIcon(openIcon);
        }
        else
        {
            ui->L13Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 14:
        if(lightState[number-1]==true)
        {
            ui->L14Button->setIcon(openIcon);
        }
        else
        {
            ui->L14Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 15:
        if(lightState[number-1]==true)
        {
            ui->L15Button->setIcon(openIcon);
        }
        else
        {
            ui->L15Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 16:
        if(lightState[number-1]==true)
        {
            ui->L16Button->setIcon(openIcon);
        }
        else
        {
            ui->L16Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 17:
        if(lightState[number-1]==true)
        {
            ui->L17Button->setIcon(openIcon);
        }
        else
        {
            ui->L17Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 18:
        if(lightState[number-1]==true)
        {
            ui->L18Button->setIcon(openIcon);
        }
        else
        {
            ui->L18Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 19:
        if(lightState[number-1]==true)
        {
            ui->L19Button->setIcon(openIcon);
        }
        else
        {
            ui->L19Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;
    case 20:
        if(lightState[number-1]==true)
        {
            ui->L20Button->setIcon(openIcon);
        }
        else
        {
            ui->L20Button->setIcon(closeIcon);
        }
        lightState[number-1]=!lightState[number-1];
    break;

    }
    if(lightState[number-1]==true)
        ui->outEdit->append("Light"+ QString::number(number)+" Close");
    else
        ui->outEdit->append("Light"+ QString::number(number)+" Open");
}
//通过判断坐标后来决定自动改变哪一个按钮（灯）槽函数
void MainWindow::autoControlButtonSlot()
{
    QIcon openIcon("E:\\Pictures\\QImageOpen.png"),closeIcon("E:\\Pictures\\QImageClose.png");
    for(int i=0;i<20;i++)
    {
        int bit = i+1;      //记录当前位数，根据位数来控制按钮状态
        if(everyLightStateArr[i]==1) //依次检测everyLightStateArr中的数，如果当前位数为1，则打开相应位数的灯
        {
            switch(bit)
            {
                case 1:
                    ui->L1Button->setIcon(openIcon);break;
                case 2:
                    ui->L2Button->setIcon(openIcon);break;
                case 3:
                    ui->L3Button->setIcon(openIcon);break;
                case 4:
                    ui->L4Button->setIcon(openIcon);break;
                case 5:
                    ui->L5Button->setIcon(openIcon);break;
                case 6:
                    ui->L6Button->setIcon(openIcon);break;
                case 7:
                    ui->L7Button->setIcon(openIcon);break;
                case 8:
                    ui->L8Button->setIcon(openIcon);break;
                case 9:
                    ui->L9Button->setIcon(openIcon);break;
                case 12:
                    ui->L12Button->setIcon(openIcon);break;
            }
        }
        else    //如果为0，则关闭相应的灯
        {
            switch(bit)
            {
                case 1:
                    ui->L1Button->setIcon(closeIcon);break;
                case 2:
                    ui->L2Button->setIcon(closeIcon);break;
                case 3:
                    ui->L3Button->setIcon(closeIcon);break;
                case 4:
                    ui->L4Button->setIcon(closeIcon);break;
                case 5:
                    ui->L5Button->setIcon(closeIcon);break;
                case 6:
                    ui->L6Button->setIcon(closeIcon);break;
                case 7:
                    ui->L7Button->setIcon(closeIcon);break;
                case 8:
                    ui->L8Button->setIcon(closeIcon);break;
                case 9:
                    ui->L9Button->setIcon(closeIcon);break;
                case 12:
                    ui->L12Button->setIcon(closeIcon);break;
            }
        }
    }
}
//Button点击事件 槽函数
void MainWindow::on_L1Button_clicked()
{
    emit this->changeLightSlot(1);
}

void MainWindow::on_L2Button_clicked()
{
    emit this->changeLightSlot(2);
}

void MainWindow::on_L3Button_clicked()
{
    emit this->changeLightSlot(3);
}

void MainWindow::on_L4Button_clicked()
{
    emit this->changeLightSlot(4);
}

void MainWindow::on_L5Button_clicked()
{
    emit this->changeLightSlot(5);
}

void MainWindow::on_L6Button_clicked()
{
    emit this->changeLightSlot(6);
}

void MainWindow::on_L7Button_clicked()
{
    emit this->changeLightSlot(7);
}

void MainWindow::on_L8Button_clicked()
{
    emit this->changeLightSlot(8);
}

void MainWindow::on_L9Button_clicked()
{
    emit this->changeLightSlot(9);
}

void MainWindow::on_L10Button_clicked()
{
    emit this->changeLightSlot(10);
}

void MainWindow::on_L11Button_clicked()
{
    emit this->changeLightSlot(11);
}

void MainWindow::on_L12Button_clicked()
{
    emit this->changeLightSlot(12);
}

void MainWindow::on_L13Button_clicked()
{
    emit this->changeLightSlot(13);
}

void MainWindow::on_L14Button_clicked()
{
    emit this->changeLightSlot(14);
}

void MainWindow::on_L15Button_clicked()
{
    emit this->changeLightSlot(15);
}

void MainWindow::on_L16Button_clicked()
{
    emit this->changeLightSlot(16);
}

void MainWindow::on_L17Button_clicked()
{
    emit this->changeLightSlot(17);
}

void MainWindow::on_L18Button_clicked()
{
    emit this->changeLightSlot(18);
}

void MainWindow::on_L19Button_clicked()
{
    emit this->changeLightSlot(19);
}

void MainWindow::on_L20Button_clicked()
{
    emit this->changeLightSlot(20);
}
/*
 *
 *  根据监控自动控制按钮开关-----坐标处理
 *
 * */
int  HandleCoordinate(int x,int y)
{
    if(x>=0&&y>=0&&x<=120&&y<=135)
    {
        return 12;
    }
    else if(x>=0&&y>=135&&x<=270&&y<=195)
    {
        return 8;
    }
    else if(x>=0&&y>=195&&x<=390&&y<=360)
    {
        return 4;
    }
    else if(x>=390&&y>=195&&x<=510&&y<=360)
    {
        return 3;
    }
    else if(x>=510&&y>=195&&x<=640&&y<=360)
    {
        return 2;
    }
    else
        return 0;
}
int arrBinToHex(bool arr[])
{
    int hex=0;
    for(int i=0;i<9;i++)    //先设定为9盏灯
    {
        hex+=arr[i]*pow((double)2,(double)i);
    }
    return hex;
}

