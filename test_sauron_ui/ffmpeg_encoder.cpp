#include "ffmpeg_encoder.h"

#include <QGuiApplication>
#include <QThread>
#include <QWindow>

extern QWindow* windowRef;

ffmpeg_encoder::ffmpeg_encoder():
    file{"C:/dev/rec_app/bullshit.mp4"},
    codec (nullptr),
    c (nullptr),
    screen(QGuiApplication::primaryScreen())
{
    int ret;

    static const char endcodedata[] = {'\x00','\x00','\x01','\xb7'};
    endcode =  QByteArray::fromRawData(endcodedata, sizeof(endcodedata));

    codec = avcodec_find_encoder_by_name("libx264");

    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return;
    }

    //memory allocation
    c = avcodec_alloc_context3(codec);
    if(!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return;
    }

    /* put sample parameters */
    c->bit_rate = 2000000;
    /* resolution must be a multiple of two */
    width = 388;
    height = 268;
    c->width = width;
    c->height = height;

    AVRational r1 = {1, 25};
    AVRational r2 = {25, 1};
    c->time_base = r1;
    c->framerate = r2;

    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P ;

    if (codec->id == AV_CODEC_ID_H264)
    {
        av_opt_set(c->priv_data, "preset", "fast", 0);
    }

    /* open it */
    ret = avcodec_open2(c, codec, nullptr);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec\n");
        return;
    }

    QScreen *screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();
    if (!screen)
        return;
}

ffmpeg_encoder::~ffmpeg_encoder()
{
    avcodec_free_context(&c);
}

void ffmpeg_encoder::encode()
{
    AVFrame *frame;
    AVPacket *pkt;
    int ret, i;

    pkt = av_packet_alloc();
    if (!pkt)
        return;

    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    //frame->format = AV_PIX_FMT_RGB24;
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
        fprintf(stderr, "could not alloc the frame data\n");
        return;
    }

    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());

    struct SwsContext* convertCtx = sws_getContext(image.width(), image.height(),
                                                   AV_PIX_FMT_RGBA, width, height,
                                                   AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR,
                                                   nullptr, nullptr, nullptr);
    for (i = 0; i < 100; i++) {
        fflush(stdout);

//        /* make sure the frame data is writable */

        QThread::msleep(100);

        QPixmap pixmap = screen->grabWindow(0);
        QImage image (pixmap.toImage());

        ret = av_frame_make_writable(frame);
        if (ret < 0)
            return;

        int srcstride[1];
        srcstride[0] = image.width() * 4;
        const uchar* planes[1];
        planes[0] = image.bits();
        sws_scale(convertCtx, planes,
            srcstride, 0, image.height(), frame->data, frame->linesize);

        frame->pts = i;

        /* encode the image */
        this->encode2(frame, pkt, out);
        fprintf(stderr, "Writing payload\n");
    }

    /* flush the encoder */
    this->encode2(nullptr, pkt, out);

    /* add sequence end code to have a real MPEG file */
    // ffs?
    //out << endcode;

    file.close();

    av_frame_free(&frame);
    av_packet_free(&pkt);
}

void ffmpeg_encoder::encode2(AVFrame *frame, AVPacket *pkt, QDataStream &out) {
    int ret;

    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            return;
        }

        out.writeRawData(reinterpret_cast<const char*>(pkt->data), pkt->size);
        av_packet_unref(pkt);
    }
}




//libx264
//libx264rgb
//AV_CODEC_ID_H264
//AV_CODEC_ID_MPEG1VIDEO

//AV_PIX_FMT_RGB24
//AV_PIX_FMT_YUV420P
