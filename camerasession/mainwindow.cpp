#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMediaDevices>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    initWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initWindow()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mVideoWidget = new QVideoWidget;
    mainLayout->addWidget(mVideoWidget);
    resize(680, 500);
    QHBoxLayout *controlLayout = new QHBoxLayout;
    QPushButton *start = new QPushButton(tr("开始"));
    QPushButton *stop = new QPushButton(tr("停止"));
    controlLayout->addStretch();
    controlLayout->addWidget(start);
    controlLayout->addWidget(stop);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);
    initCamera();
    connect(start, SIGNAL(clicked()), mCamera, SLOT(start()));
    connect(stop, SIGNAL(clicked()), mCamera, SLOT(stop()));
}

void MainWindow::initCamera()
{
    QCameraDevice cameraDevice = QMediaDevices::defaultVideoInput();
    mCamera = new QCamera(cameraDevice);
    mCaptureSession.setCamera(mCamera);
    mCaptureSession.setVideoOutput(mVideoWidget);
    if (mCamera->cameraFormat().isNull())
    {
        auto formats = cameraDevice.videoFormats();
        if (!formats.isEmpty())
        {
            QCameraFormat bestFormat;
            for (const auto &fmt : formats)
            {
                if (bestFormat.maxFrameRate() < 29 && fmt.maxFrameRate() > bestFormat.maxFrameRate())
                    bestFormat = fmt;
                else if (bestFormat.maxFrameRate() == fmt.maxFrameRate() &&
                         bestFormat.resolution().width() * bestFormat.resolution().height() <
                         fmt.resolution().width() * fmt.resolution().height())
                    bestFormat = fmt;
            }

            mCamera->setCameraFormat(bestFormat);
        }
    }
}

