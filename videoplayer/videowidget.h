#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "videothread.h"

#include <QWidget>
#include <QImage>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

signals:

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    VideoThread *thread;
    QImage videoImage;

private slots:
    void updateImage(const QImage &image);
    void clear();

public slots:
    void setUrl(const QString &url);
    void open();
    void close();
};

#endif // VIDEOWIDGET_H
