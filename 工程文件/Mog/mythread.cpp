#include <MyThread.h>
#include <widget.h>
extern Widget *pw;
void MyThread::run()
{
    pw->ReadAvi();
}

