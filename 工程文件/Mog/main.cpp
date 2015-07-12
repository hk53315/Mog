#include "widget.h"
#include <QApplication>
Widget *pw;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.setWindowFlags(w.windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowCloseButtonHint);
    pw = &w;
    w.show();

    return a.exec();
}
