#include "platewidget.h"
#include "ui_platewidget.h"
#define I(image,x,y) ((uchar*)(image->imageData + image->widthStep*(y)))[(x)]    //获取像素值
PlateWidget::PlateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlateWidget)
{
    ui->setupUi(this);
     Initialization();
}
void PlateWidget::Initialization()
{
    ui->Partition->clear();
    ui->TopHat_Show->clear();
    ui->GetPlaceOfPlate->clear();
    ui->ShowPlaceOfPlate->clear();
    ui->ShowFindPlate->clear();
    //初始化各个按钮的状态
    ui->Button_Partitioning->setEnabled(false);
    ui->Button_TopHat->setEnabled(false);
    ui->Button_GetPlace->setEnabled(false);
    ui->Button_ShowPlace->setEnabled(false);
    ui->Button_ShowFindPlate->setEnabled(false);
}

void PlateWidget::on_pushButton_clicked()
{
    //打开本地图像文件
    QFileDialog *OpenImageDialog = new QFileDialog();
    QString ImagePath;
    ImagePath = OpenImageDialog->getOpenFileName(this,tr("Open Image"),".",tr("Image Files(*.png *.jpg *.jpeg *.bmp)"));
    if(ImagePath.length() == 0)
    {
        return ;
    }

    Initialization();
    ui->Button_Partitioning->setEnabled(true);

    //将QString转换成String,获取图片
    OriginalImage = cvLoadImage(ImagePath.toStdString().c_str());
    cvSmooth(OriginalImage,OriginalImage,CV_GAUSSIAN);
    //先转换成512*512大小的图像，作为处理的目标 DirImage
    DirImage = cvCreateImage(cvSize(512,512),IPL_DEPTH_8U,3);
    cvResize(OriginalImage,DirImage,CV_INTER_LINEAR);

    //转换成300*300的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(300,300),IPL_DEPTH_8U,3);
    cvResize(OriginalImage,Timage1,CV_INTER_LINEAR);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->label_OpenImage->setPixmap(pixmap1);

}

void PlateWidget::Partitioning()
{
    ui->Button_Partitioning->setEnabled(false);
    IplImage *PartitionImage = cvCloneImage(DirImage);
    //画线8*8
    int temp = 0;
    for(int i=1;i<8;i++)
    {
        cvLine(PartitionImage,cvPoint(63+temp,0),cvPoint(63+temp,511),cvScalar(0,255,255),2,8,0);
        temp+=64;
    }
    temp = 0;
    for(int i=1;i<8;i++)
    {
        cvLine(PartitionImage,cvPoint(0,63+temp),cvPoint(511,63+temp),cvScalar(0,255,255),2,8,0);
        temp+=64;
    }

    //转换成150*150的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(150,150),IPL_DEPTH_8U,3);
    cvResize(PartitionImage,Timage1,CV_INTER_LINEAR);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->Partition->setPixmap(pixmap1);
}

void PlateWidget::on_Button_Partitioning_clicked()
{
    Partitioning();
    ui->Button_TopHat->setEnabled(true);
}

void PlateWidget::on_Button_TopHat_clicked()
{
    ui->Button_TopHat->setEnabled(false);
    ui->Button_GetPlace->setEnabled(true);
    IplImage* Timage3=cvCreateImage(cvGetSize(DirImage),IPL_DEPTH_8U,3);  //下一步寻找区域的图像
    IplImage* tmp=cvCreateImage(cvGetSize(DirImage),IPL_DEPTH_8U,3);  //缓冲图像

    //顶帽变换
    cvMorphologyEx(DirImage,Timage3,tmp,NULL,CV_MOP_TOPHAT,1);
    //赋值高帽变换后的目标图像
    DirImageAfterMorphologyEx = cvCloneImage(Timage3);

    ///显示程序
    //转换成150*150的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(150,150),IPL_DEPTH_8U,3);
    cvResize(Timage3,Timage1,CV_INTER_LINEAR);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->TopHat_Show->setPixmap(pixmap1);
    ///显示程序结束

}

void PlateWidget::on_Button_OneClick_clicked()
{
    on_Button_Partitioning_clicked();
    on_Button_TopHat_clicked();
    on_Button_GetPlace_clicked();
    on_Button_ShowPlace_clicked();
    on_Button_ShowFindPlate_clicked();
}

void PlateWidget::on_Button_GetPlace_clicked()
{
    ui->Button_ShowPlace->setEnabled(true);
    ui->Button_GetPlace->setEnabled(false);
    int number=0;                    //记录横向的长度，找到最长的是车牌的区域
    int number_Max=0;                //最长的车牌横向区域
    int left_top_X=0;                        //记录车牌开始的区域
    int left_top_Y=0;
    int Roi_top=0;                   //矩形的上边
    int Roi_bottom=0;                 //矩形的下边


    //获得灰度图像
    IplImage* DirImageAfterMorphologyEx_Dst = cvCreateImage(cvGetSize(DirImageAfterMorphologyEx),IPL_DEPTH_8U,1);
    cvCvtColor(DirImageAfterMorphologyEx,DirImageAfterMorphologyEx_Dst,CV_RGB2GRAY);

    //赋值标记图像
    DirImageAfterMorphologyEx_Flag = cvCreateImage(cvGetSize(DirImageAfterMorphologyEx_Dst),IPL_DEPTH_8U,3);
    for(int j=0;j<DirImageAfterMorphologyEx_Flag->height;j++)                 //为标志图像付初值
        for(int i=0;i<DirImageAfterMorphologyEx_Flag->width;i++)
        {
            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 0]=0; // B
            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 1]=0;
            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 2]=0;
        }
    ///计算能量
    double Energy[8][8];
    int height = DirImageAfterMorphologyEx_Dst->height/8;
    int width = DirImageAfterMorphologyEx_Dst->width/8;
    double SUM = 0.0;
    double sum = 0.0;
    //计算每一快的能量并存在energy数组之中
    for(int x=0;x<8;x++)
    {
        for(int y=0;y<8;y++)
        {
            for(int j=x*height;j<((x*height)+height);j++)
            {
                for(int i=y*width+1;i<((y*width)+width-1);i++)
                {
                    sum+=pow(2*I(DirImageAfterMorphologyEx_Dst,i,j)-I(DirImageAfterMorphologyEx_Dst,i-1,j)-I(DirImageAfterMorphologyEx_Dst,i+1,j),2);
                }
            }
            SUM+=sum;
            Energy[x][y]=sum;
            sum=0;
        }
    }

    //横向的孤立的变化很大的区域去掉
    for(int x=0;x<8;x++)
        for(int y=1;y<7;y++)
        {
            if(Energy[x][y]>(Energy[x][y-1]*2)&&Energy[x][y]>(Energy[x][y+1]*2))
                Energy[x][y]=0;
        }
    //找到合适的阈值，然后筛选区域
    double E=(SUM/64.0);
    double Th_max = 5*E;              //设定的最大阈值
    double Th_min = 0.5*E;            //设定的最小阈值
    double Th=Th_max;                 //采用自适应阈值的方式，初始阈值为最大值
    bool flag = false;                //标记是否找到合适的车牌位置
    //自适应阈值寻找车牌
    while(!flag&&Th_max>Th_min)
    {
        bool Energy_flag[8][8];          //记录该区域是否符合阈值
        memset(Energy_flag,false,sizeof(Energy_flag));  //赋初值

        for(int x=1;x<7;x++)        //合适的在flag图像中表示出来，不必计算边缘，在边缘的地方很可能出现照不全车牌的情况，自动去掉
            for(int y=1;y<7;y++)
            {
                if(Energy[x][y]>=Th)
                {
                    Energy_flag[x][y]=true;
                    for(int j=x*height;j<((x*height)+height);j++)
                        for(int i=y*width+1;i<((y*width)+width-1);i++)
                        {
                            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 0]=255;
                            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 1]=255;
                            ((uchar *)(DirImageAfterMorphologyEx_Flag->imageData + j*DirImageAfterMorphologyEx_Flag->widthStep))[i*DirImageAfterMorphologyEx_Flag->nChannels + 2]=255;
                        }
                }
            }



        for(int x=1;x<7;x++)                        //将靠边的区域去掉以免出现找不全的情况
            for(int y=1;y<7;y++)
            {
                number=0;
                while(Energy_flag[x][y]&&y<7)
                {
                    y+=1;
                    number++;
                }
                if(number_Max<number)
                {
                    number_Max=number;
                    //左侧的点（x，y）
                    left_top_X=x;
                    left_top_Y=y-number;
                }
            }
        if(number_Max>=2)
        {
            Roi_top=left_top_X;
            Roi_bottom=left_top_X+1;

            for(int x=left_top_X,y=left_top_Y;y<(left_top_Y+number_Max);y++)
            {
                if(Energy_flag[x-1][y])
                    Roi_top=x-1;
                if(Energy_flag[x+1][y])
                    Roi_bottom=x+2;
            }

            result[1]=left_top_Y*64;
            result[3]=(left_top_Y+number_Max)*64;
            result[0]=Roi_top*64;
            result[2]=Roi_bottom*64;
            flag = true;
        }
        else
            Th=Th-0.5*E;
    }

    //图示，找到的矩形框的坐标情况（x，y）
    ///(result[1],result[0])------------
    ///                     |          |
    ///                     |          |
    ///                     ------------(result[3],result[2])

    //cvLine(DirImageAfterMorphologyEx_Flag,cvPoint(result[1],result[0]),cvPoint(result[3],result[2]),cvScalar(0,0,255),2,8,0);

    //给标记图像划线，便于和原始图像比较
    int temp = 0;
    for(int i=1;i<8;i++)
    {
        cvLine(DirImageAfterMorphologyEx_Flag,cvPoint(63+temp,0),cvPoint(63+temp,511),cvScalar(0,255,255),2,8,0);
        temp+=64;
    }
    temp = 0;
    for(int i=1;i<8;i++)
    {
        cvLine(DirImageAfterMorphologyEx_Flag,cvPoint(0,63+temp),cvPoint(511,63+temp),cvScalar(0,255,255),2,8,0);
        temp+=64;
    }
    ///显示程序
    //转换成150*150的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(150,150),IPL_DEPTH_8U,3);
    cvResize(DirImageAfterMorphologyEx_Flag,Timage1,CV_INTER_LINEAR);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->GetPlaceOfPlate->setPixmap(pixmap1);
    ///显示程序结束

}

void PlateWidget::on_Button_ShowPlace_clicked()
{
    ui->Button_ShowPlace->setEnabled(false);
    ui->Button_ShowFindPlate->setEnabled(true);
    //已经得到了矩形的位置，由于一般是两块，所以要进行左右上下扩充，
    //确保车牌区域在所找的区域里
    if(result[3]-result[1] == DirImage->width/8*2)
    {
        //左右增加
        result[1] -= DirImage->width/8/3;
        result[3] += DirImage->width/8/3;

    }
    if(result[2]-result[0] == DirImage->height/8)
    {
        //上下增加
        result[0]-=DirImage->height/8/3;
        result[2]+=DirImage->height/8/3;
    }

    IplImage *tmpDirImage = cvCloneImage(DirImage);
    cvLine(tmpDirImage,cvPoint(result[1],result[0]),cvPoint(result[1],result[2]),cvScalar(0,255,0),2,8,0);
    cvLine(tmpDirImage,cvPoint(result[1],result[0]),cvPoint(result[3],result[0]),cvScalar(0,255,0),2,8,0);
    cvLine(tmpDirImage,cvPoint(result[3],result[0]),cvPoint(result[3],result[2]),cvScalar(0,255,0),2,8,0);
    cvLine(tmpDirImage,cvPoint(result[1],result[2]),cvPoint(result[3],result[2]),cvScalar(0,255,0),2,8,0);

    ///显示程序
    //转换成150*150的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(150,150),IPL_DEPTH_8U,3);
    cvResize(tmpDirImage,Timage1,CV_INTER_LINEAR);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->ShowPlaceOfPlate->setPixmap(pixmap1);
    ///显示程序结束
}

void PlateWidget::on_Button_ShowFindPlate_clicked()
{
    //找到得矩形
    CvRect ROI_rect;
    ROI_rect.x=result[1];
    ROI_rect.y=result[0];
    ROI_rect.width=result[3]-result[1];
    ROI_rect.height=result[2]-result[0];

    //将该矩形抠出来
    IplImage *DirImgRect=NULL;
    cvSetImageROI(DirImage,ROI_rect);
    DirImgRect =cvCreateImage(cvSize(ROI_rect.width,ROI_rect.height),IPL_DEPTH_8U,3);
    cvCopy(DirImage,DirImgRect);
    cvResetImageROI(DirImage);

    ///显示程序
    //转换成300*100的图像并显示
    IplImage *Timage1 = cvCreateImage(cvSize(ui->ShowFindPlate->width(),ui->ShowFindPlate->height()),IPL_DEPTH_8U,3); //归一化成409*90
    cvResize(DirImgRect,Timage1,CV_INTER_CUBIC);

    //将文件显示在窗口中,转换成Qt显示图片的方式(BGR->RGB)
    IplImage *Timage2 = cvCreateImage(cvGetSize(Timage1),IPL_DEPTH_8U,3);
    cvConvertImage(Timage1,Timage2, CV_CVTIMG_SWAP_RB);
    QImage image((const uchar *)Timage2->imageData,Timage2->width,Timage2->height,QImage::Format_RGB888);
    QPixmap pixmap1 = QPixmap::fromImage(image);
    ui->ShowFindPlate->setPixmap(pixmap1);
    ///显示程序结束
}

PlateWidget::~PlateWidget()
{
    delete ui;
}
