#include "videothread.h"

#include <QMutex>
#include <QMutexLocker>

VideoThread::VideoThread(QObject *parent) : QThread(parent)
    , stopped(false)
    , playFlag(false)
    , frameFinish(-1)
    , videoIndex(-1)
    , audioIndex(-1)
    , buffer(nullptr)
    , avPacket(nullptr)
    , avFrame(nullptr)
    , avFrame2(nullptr)
    , avFormatContext(nullptr)
    , videoContext(nullptr)
    ,swsContext(nullptr)
    , options(nullptr)
    , videoDecoder(nullptr)
{
    initLib();
}


void VideoThread::run()
{
    while (!stopped) {
        if (playFlag)
        {
            init();
            playFlag = false;
            continue;
        }

        if (av_read_frame(avFormatContext, avPacket) == 0)
        {
            if (avPacket->stream_index == videoIndex)
            {
                int ret = avcodec_send_packet(videoContext, avPacket);
                if (ret < 0)
                    break;


                while (ret == 0) {
                    ret = avcodec_receive_frame(videoContext, avFrame2);
                    if (ret < 0)
                        break;

                    sws_scale(swsContext,
                              (const unsigned char *const *)avFrame2->data,
                              avFrame2->linesize,
                              0,
                              videoHeight,
                              avFrame->data,
                              avFrame->linesize);
                    QImage image((uchar *)avFrame->data[0],
                                 videoWidth,
                                 videoHeight,
                                 QImage::Format_RGB32);
                    if (!image.isNull())
                        emit receiveImage(image);

                    msleep(10);
                }
            }
        }

        av_packet_unref(avPacket);
        av_freep(avPacket);
//        msleep(1);
    }

    free();
    stopped = false;
    playFlag = false;
}

void VideoThread::initLib()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit(false);
    if (!isInit)
    {
        avformat_network_init();
        isInit = true;
    }
}

bool VideoThread::decodeVideo()
{
    videoIndex = av_find_best_stream(avFormatContext,
                                      AVMEDIA_TYPE_VIDEO,
                                      -1,
                                      -1,
                                      &videoDecoder,
                                     0);
    if (videoIndex < 0)
    {
        avformat_close_input(&avFormatContext);
        return false;
    }

    videoContext = avcodec_alloc_context3(NULL);
    AVStream *videoStream = avFormatContext->streams[videoIndex];
    avcodec_parameters_to_context(videoContext,
                                  videoStream->codecpar);
    videoDecoder = avcodec_find_decoder(videoContext->codec_id);
    if (videoDecoder == nullptr)
    {
        avcodec_close(videoContext);
        avformat_close_input(&avFormatContext);
        return false;
    }

    if (avcodec_open2(videoContext, videoDecoder, NULL) < 0)
    {
        avcodec_close(videoContext);
        avformat_close_input(&avFormatContext);
        return false;
    }

    videoWidth = videoContext->width;
    videoHeight = videoContext->height;
    if (videoWidth == 0 || videoHeight == 0)
    {
        avcodec_close(videoContext);
        avformat_close_input(&avFormatContext);
        return false;
    }

    if (lastWidth != videoWidth || lastHeight != videoHeight)
    {
        int size = av_image_get_buffer_size(AV_PIX_FMT_RGB32,
                                            videoWidth,
                                            videoHeight,
                                            1);
        buffer = (unsigned char *)av_malloc(size);
        lastWidth = videoWidth;
        lastHeight = videoHeight;
    }

    avFrame = av_frame_alloc();
    av_image_fill_arrays(avFrame->data,
                         avFrame->linesize,
                         buffer,
                         AV_PIX_FMT_RGB32,
                         videoWidth,
                         videoHeight,
                         1);
    swsContext = sws_getContext(videoWidth, videoHeight, videoContext->pix_fmt,
                                videoWidth, videoHeight, AV_PIX_FMT_RGB32,
                                SWS_BICUBIC, NULL, NULL, NULL);
    if (!swsContext)
        return false;

    avPacket = av_packet_alloc();
    if (!avPacket)
        return false;

    avFrame2 = av_frame_alloc();
    if (!avFrame2)
        return false;

    return true;
}

void VideoThread::setUrl(const QString &url)
{
    videoUrl = url;
}

bool VideoThread::init()
{
    avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext,
                            videoUrl.toStdString().c_str(),
                            nullptr, nullptr) < 0)
        return false;

    if (avformat_find_stream_info(avFormatContext, nullptr) < 0)
    {
        avformat_close_input(&avFormatContext);
        return false;
    }

    return decodeVideo();
}

void VideoThread::free()
{
    if (swsContext != nullptr)
    {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    if (avPacket != nullptr)
    {
        av_packet_unref(avPacket);
        avPacket = nullptr;
    }

    if (avFrame != nullptr)
    {
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }

    if (avFrame2 != nullptr)
    {
        av_frame_free(&avFrame2);
        avFrame2 = nullptr;
    }

    if (videoContext != nullptr)
    {
        avcodec_close(videoContext);
        videoContext = nullptr;
    }

    if (avFormatContext != nullptr)
    {
        avformat_close_input(&avFormatContext);
        avFormatContext = nullptr;
    }
}

void VideoThread::play()
{
    playFlag = true;
}

void VideoThread::stop()
{
    stopped = true;
}
