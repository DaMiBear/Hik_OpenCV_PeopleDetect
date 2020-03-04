#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T07:30:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpencvTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    camera.cpp \
    queue.cpp \
    processpeople.cpp

HEADERS += \
        mainwindow.h \
    camera.h \
    queue.h \
    processpeople.h

FORMS += \
        mainwindow.ui
INCLUDEPATH += D:/OpenCV/opencv/build/include\
D:/OpenCV/opencv/build/include/opencv\
D:/OpenCV/opencv/build/include/opencv2
LIBS+=D:/OpenCV/opencv/build/x64/vc14/lib/opencv_world341d.lib

INCLUDEPATH +="D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/include"
LIBS+=D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/HCNetSDK.lib\
D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/PlayCtrl.lib\
D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/HCCore.lib\
D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/HCNetSDKCom/HCAlarm.lib\
D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/HCNetSDKCom/HCGeneralCfgMgr.lib\
D:/OpenCV/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/CH-HCNetSDK(Windows64)V5.3.1.22_build20170909/lib/HCNetSDKCom/HCPreview.lib

