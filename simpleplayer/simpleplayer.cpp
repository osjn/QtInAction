#include "simpleplayer.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTime>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    initWindow();
    initMember();
}

Widget::~Widget()
{
}

void Widget::onPlayButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("选择视频"),
        "",
        tr("视频(*.mp4 *.mkv *.avi *.mov)"));
    if (fileName.isEmpty())
        return;

    playVideo(fileName);
    freeResource();
}

void Widget::initWindow()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    QHBoxLayout *controlLayout = new QHBoxLayout;
    QPushButton *playButton = new QPushButton(tr("播放"));
    controlLayout->addWidget(playButton);
    controlLayout->addStretch();
    int videoWidth = this->width();
    int videoHeight = this->height() - playButton->height();
    labelVideo = new QLabel();
    labelVideo->setGeometry(0, 0, videoWidth, videoHeight);
    mainLayout->addWidget(labelVideo);
    mainLayout->addLayout(controlLayout);
    resize(800, 600);

    connect(playButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
}

void Widget::delay(int ms)
{
    QTime stopTime = QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < stopTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void Widget::initMember()
{
    imgConvertCtx = nullptr;
    pFrameRGB = nullptr;
    pFrame = nullptr;
    pCodecCtx = nullptr;
    pFormatCtx = nullptr;
    stopFlag = false;
}

void Widget::freeResource()
{
    if (imgConvertCtx != nullptr)
        sws_freeContext(imgConvertCtx);

    if (pFrameRGB != nullptr)
        av_frame_free(&pFrameRGB);

    if (pFrame != nullptr)
        av_frame_free(&pFrame);

    if (pCodecCtx != nullptr)
        avcodec_free_context(&pCodecCtx);

    if (pFormatCtx != nullptr)
        avformat_close_input(&pFormatCtx);
}

void Widget::playVideo(const QString &filePath)
{
    // 创建AVFormatContext
    pFormatCtx = avformat_alloc_context();
    // 初始化pFormatCtx结构
    if (avformat_open_input(&pFormatCtx, filePath.toStdString().c_str(), NULL, NULL) < 0)
    {
        qDebug("Couldn't open input stream.");
        return;
    }

    // 获取音视频流数据信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        avformat_close_input(&pFormatCtx);
        qDebug("Couldn't find stream information.");
        return;
    }

    // nb_streams视音频流的个数，这里当查找到视频流时就中断了
    // 找到视频流的索引
    int videoIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    //    int audioIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    // 没有视频流就退出
    if (videoIndex < 0)
    {
        avformat_close_input(&pFormatCtx);
        qDebug("Didn't find a video stream.");
        return;
    }

    // 获取视频流编码结构
    pCodecCtx = avcodec_alloc_context3(NULL);
    // 查找解码器
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoIndex]->codecpar);
    const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        qDebug("Codec not found.");
        return;
    }

    // 用于初始化pCodecCtx结构
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        qDebug("Could not open codec.");
        return;
    }

    // 创建帧结构，此函数仅分配基本结构空间，图像数据空间需通过av_malloc分配
    pFrameRGB = av_frame_alloc();
    if (!pFrameRGB)
    {
        return;
    }

    // 创建动态内存,创建存储图像数据的空间
    // av_image_get_buffer_size获取一帧图像需要的大小
    int size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
    unsigned char *outBuffer = (unsigned char *)av_malloc(size);
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, outBuffer,
                         AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1);
    // 初始化img_convert_ctx结构
    imgConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                   pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32,
                                   SWS_BICUBIC, NULL, NULL, NULL);
    AVPacket *packet = av_packet_alloc();
    if (!packet)
    {
        return;
    }

    pFrame = av_frame_alloc();
    if (!pFrame)
    {
        return;
    }

    while (!stopFlag)
    {
        if (av_read_frame(pFormatCtx, packet) == 0)
        {
            // 如果是视频数据
            if (packet->stream_index == videoIndex)
            {
                // 解码一帧视频数据
                int ret = avcodec_send_packet(pCodecCtx, packet);
                if (ret < 0)
                {
                    qDebug("Sending packet Error.");
                    break;
                }

                 while (ret == 0)
                 {
                    ret = avcodec_receive_frame(pCodecCtx, pFrame);
                    if (ret < 0)
                    {
                        qDebug("Receiving frame Error.");
                        break;
                    }

                    sws_scale(imgConvertCtx,
                              (const unsigned char *const *)pFrame->data,
                              pFrame->linesize, 0, pCodecCtx->height,
                              pFrameRGB->data, pFrameRGB->linesize);
                    QImage img((uchar *)pFrameRGB->data[0],
                               pCodecCtx->width,
                               pCodecCtx->height,
                               QImage::Format_RGB32);
                    labelVideo->setPixmap(QPixmap::fromImage(img));
                    delay(40);
                 }
            }

            av_packet_unref(packet);
            av_freep(packet);
        }
    }

    freeResource();
}

void Widget::closeEvent(QCloseEvent *event)
{
    stopFlag = true;
    delay(500);
    freeResource();

    QWidget::closeEvent(event);
}
