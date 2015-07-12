#-------------------------------------------------
# Project created by QtCreator 2014-04-25T17:08:55
# Made by WangQun
# Problems should be improve
# (1) 高斯背景混合模型的效�?
# (2) 寻找车辆的方�?
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mog
TEMPLATE = app
INCLUDEPATH+=D:\Opencv2.4.1mw4.8\include\opencv\
D:\Opencv2.4.1mw4.8\include\opencv2\
D:\Opencv2.4.1mw4.8\include

LIBS+=D:\Opencv2.4.1mw4.8\lib\libopencv_calib3d243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_contrib243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_core243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_features2d243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_flann243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_gpu243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_highgui243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_imgproc243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_legacy243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_ml243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_objdetect243.dll.a\
D:\Opencv2.4.1mw4.8\lib\libopencv_video243.dll.a

SOURCES += main.cpp\
        widget.cpp \
    mythread.cpp

HEADERS  += widget.h \
    MyThread.h

FORMS    += widget.ui
