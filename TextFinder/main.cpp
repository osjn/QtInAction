#include "textfinder.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TextFinder textFinder;
    textFinder.show();
    return a.exec();
}
