#include "videowidget.h"

#include <QTimer>
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget{parent}
{
    thread = new VideoThread(this);
    connect(thread, SIGNAL(receiveImage(QImage)), this, SLOT(updateImage(QImage)));
    videoImage = QImage();
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

VideoWidget::~VideoWidget()
{
    close();
}


void VideoWidget::paintEvent(QPaintEvent *event)
{
    if (videoImage.isNull())
    {
        return;
    }

    QPainter painter(this);
    painter.drawImage(rect(), videoImage);
}

void VideoWidget::close()
{
    if (thread->isRunning())
    {
        thread->stop();
        thread->quit();
        thread->wait(500);
    }

    QTimer::singleShot(1, this, SLOT(clear()));
}

void VideoWidget::updateImage(const QImage &image)
{
    videoImage = image;
    update();
}

void VideoWidget::clear()
{
    videoImage = QImage();
    update();
}

void VideoWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void VideoWidget::open()
{
    clear();
    thread->play();
    thread->start();
}


