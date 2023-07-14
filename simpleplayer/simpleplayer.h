#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onPlayButtonClicked();

private:
    void initWindow();
    void delay(int ms);
    void initMember();
    void freeResource();
    void playVideo(const QString &filePath);

    QLabel * labelVideo;
    AVFormatContext    *pFormatCtx;
    AVCodecContext    *pCodecCtx;
    AVFrame    *pFrame, *pFrameRGB;
    struct SwsContext *imgConvertCtx;
    bool stopFlag;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // WIDGET_H
