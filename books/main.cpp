#include "bookwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BookWindow w;
    w.show();
    return a.exec();
}
