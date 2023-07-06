#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QCamera>


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initWindow();
    void initCamera();

    QVideoWidget *mVideoWidget;
    QMediaCaptureSession mCaptureSession;
    QCamera *mCamera;
};
#endif // MAINWINDOW_H
