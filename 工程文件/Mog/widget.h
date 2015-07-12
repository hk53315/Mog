#ifndef WIDGET_H
#define WIDGET_H

#include <iostream>
#include <QWidget>
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cvaux.h>
#include <QFileDialog>
#include <QWaitCondition>
#include <qt_windows.h>
#include <MyThread.h>
#include <QThread>
#include <QDir>
#include <QMessageBox>
#include <QTime>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void ReadAvi();
    void FindCar(IplImage *CarImg,IplImage *tmpFrame);
    void Partitioning();
    void Initialization();
    ~Widget();
public slots:
    void on_Button_OpenVideo_clicked();
    void on_pushButton_clicked();
    void on_Button_Partitioning_clicked();
    void on_Button_TopHat_clicked();
    void on_Button_OneClick_clicked();
    void on_Button_GetPlace_clicked();
    void on_Button_ShowPlace_clicked();
    void on_Button_ShowFindPlate_clicked();
    void on_Button_Start_clicked();
    void on_Button_Stop_clicked();
    void on_Button_Exit_clicked();

private slots:
    void on_Button_End_clicked();

private:
    QTime time;
    bool stop;
    bool VideoEnd;
    MyThread thread;
    QString VideoPath;
    int index;
    IplImage *OriginalImage;             //原始图像
    IplImage *DirImage;                  //顶帽变换以前的目标图像
    IplImage *DirImageAfterMorphologyEx; //顶帽变换以后的目标图像
    IplImage *DirImageAfterMorphologyEx_Flag;
    int **Energy;                        //energy数组存灰度照片能量值
    int result[4];
    //图示，找到的矩形框的坐标情况（x，y）
    ///(result[1],result[0])------------
    ///                     |          |
    ///                     |          |
    ///                     ------------(result[3],result[2])
    Ui::Widget *ui;
};

#endif // WIDGET_H
