#include "httpwindow.h"

#include <QApplication>
#include <QDir>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HttpWindow httpWin;
    const QRect availableSize = httpWin.screen()->availableGeometry();
    httpWin.resize(availableSize.width() / 5, availableSize.height() / 5);
    httpWin.move((availableSize.width() - httpWin.width()) / 2, (availableSize.height() - httpWin.height()) / 2);
    httpWin.show();
    return a.exec();
}
