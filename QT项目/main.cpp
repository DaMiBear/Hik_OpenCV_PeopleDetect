#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("ZYSong18030",12);//指定显示字体
    a.setFont(font);
    MainWindow w;
    QPalette palette(w.palette());
    palette.setColor(QPalette::Background,QColor::QColor(240,255,240,255));
    w.setPalette(palette);
    w.setWindowTitle("教（实验）室智慧照明控制系统");
    w.show();
    return a.exec();
}
