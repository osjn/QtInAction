#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include "ffmpeg.h"

#include <QThread>
#include <QImage>

class VideoThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = nullptr);

    // QThread interface
protected:
    void run() override;

private:
    volatile bool stopped;
    volatile bool playFlag;
    int frameFinish;
    int videoWidth;
    int videoHeight;
    int lastWidth;
    int lastHeight;
    int videoIndex;
    int audioIndex;
    unsigned char *buffer;
    QString videoUrl;
    AVPacket *avPacket;
    AVFrame *avFrame;
    AVFrame *avFrame2;
    AVFormatContext *avFormatContext;
    AVCodecContext *videoContext;
    SwsContext *swsContext;
    AVDictionary *options;
    const AVCodec *videoDecoder;

    static void initLib();
    bool decodeVideo();

signals:
    void receiveImage(const QImage &image);

public slots:
    void setUrl(const QString &url);
    bool init();
    void free();
    void play();
    void stop();
};

#endif // VIDEOTHREAD_H
