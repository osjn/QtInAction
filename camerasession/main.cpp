#include "mainwindow.h"

#include <QApplication>
#include <QMediaCaptureSession>
#include <QCamera>
#include <QVideoWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    QMediaCaptureSession captureSession;
//    QCamera *camera = new QCamera;
//    captureSession.setCamera(camera);
//    QVideoWidget *viewfinder = new QVideoWidget;
//    captureSession.setVideoOutput(viewfinder);
//    viewfinder->show();

//    camera->start(); // to start the camera

    return a.exec();
}
